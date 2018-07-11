/*
 * Convolver.cpp
 *
 *  Created on: 29 янв. 2016 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <core/dsp.h>
#include <core/debug.h>
#include <core/Convolver.h>

#define CONVOLVER_RANK_FRM_SMALL    (CONVOLVER_RANK_FFT_SMALL - 1)
#define CONVOLVER_SMALL_FFT_SIZE    (1 << CONVOLVER_RANK_FFT_SMALL)
#define CONVOLVER_SMALL_FRM_SIZE    (1 << CONVOLVER_RANK_FRM_SMALL)
#define CONVOLVER_SMALL_FFT_MASK    (CONVOLVER_SMALL_FFT_SIZE - 1)
#define CONVOLVER_SMALL_FRM_MASK    (CONVOLVER_SMALL_FRM_SIZE - 1)

namespace lsp
{
    Convolver::Convolver()
    {
        nFrameSize      = 0;
        nFrameMax       = 0;

        vFrameRe        = NULL;
        vFrameIm        = NULL;
        vFftRe          = NULL;
        vFftIm          = NULL;
        vTempRe         = NULL;
        vTempIm         = NULL;
        vConvRe         = NULL;
        vConvIm         = NULL;
        vBufferHead     = NULL;
        vBufferTail     = NULL;
        vBufferPtr      = NULL;
        vBufferEnd      = NULL;
        vConvFirst      = NULL;
        vTaskRe         = NULL;
        vTaskIm         = NULL;

        nRank           = 0;
        nSteps          = 0;
        nBlocks         = 0;
        nBlocksDone     = 0;
        pConvRe         = NULL;
        pConvIm         = NULL;
        pTargetPtr      = NULL;
        vData           = NULL;
    }

    Convolver::~Convolver()
    {
        destroy();
    }

    bool Convolver::init(const float *data, size_t count, size_t rank)
    {
        // Check arguments
        if (count <= 0)
        {
            destroy();
            return true;
        }

        // Determine number of buffers
        if (rank < CONVOLVER_RANK_MIN)
            rank    = CONVOLVER_RANK_MIN;
        else if (rank >= CONVOLVER_RANK_MAX)
            rank    = CONVOLVER_RANK_MAX;

        // Determine size of buffer
        size_t fft_buf_size     = 1 << rank;
        size_t data_buf_size    = fft_buf_size >> 1;
        size_t bins             = (count + data_buf_size - 1) >> (rank - 1);

        size_t allocate         = CONVOLVER_SMALL_FRM_SIZE; // Non-FFT first frame
        allocate               += bins * fft_buf_size * 2; // FFT of the convolution
        allocate               += fft_buf_size * 2; // FFT buffer (real and imaginary)
        allocate               += fft_buf_size * 2; // Temporary buffer (real and imaginary)
        allocate               += fft_buf_size * 2 * 2; // Frame buffer (real and imaginary)
        allocate               += fft_buf_size * 2; // Task buffer (real and imaginary)
        allocate               += bins * data_buf_size * 4 + data_buf_size * 4; // Buffer for convolution tail

        uint8_t *ptr            = new uint8_t[allocate * sizeof(float) + DEFAULT_ALIGN];
        if (ptr == NULL)
            return false;

        // Drop all previously used data
        destroy();
        vData               = ptr;
        float *fptr         = reinterpret_cast<float *>(ALIGN_PTR(ptr, DEFAULT_ALIGN));
        dsp::fill_zero(fptr, allocate);

        vConvFirst          = fptr;
        fptr               += CONVOLVER_SMALL_FRM_SIZE;

        vTaskRe             = fptr;
        fptr               += fft_buf_size;
        vTaskIm             = fptr;
        fptr               += fft_buf_size;

        fptr               += fft_buf_size; // previous frame (re)
        vFrameRe            = fptr;
        fptr               += fft_buf_size;
        fptr               += fft_buf_size; // previous frame (im)
        vFrameIm            = fptr;
        fptr               += fft_buf_size;

        vFftRe              = fptr;
        fptr               += fft_buf_size;
        vFftIm              = fptr;
        fptr               += fft_buf_size;

        vTempRe             = fptr;
        fptr               += fft_buf_size;
        vTempIm             = fptr;
        fptr               += fft_buf_size;

        vConvRe             = fptr;
        fptr               += bins * fft_buf_size;
        vConvIm             = fptr;
        fptr               += bins * fft_buf_size;

        vBufferHead         = fptr;
        fptr               += bins * data_buf_size * 3;
        vBufferTail         = fptr;
        fptr               += (bins + 4) * data_buf_size;
        vBufferEnd          = fptr;

        nFrameSize          = 0;
        vBufferPtr          = vBufferHead;

        /* Calculate convolutions

            Conv buffer layout:
            +---+---+------+------------+------------------------+
            |FFT|FFT|FFT x2|   FFT x4   |       FFT x5           |  . . .
            +---+---+------+------------+------------------------+
         */
        float *conv_re      = vConvRe;
        float *conv_im      = vConvIm;
        size_t bin_rank     = CONVOLVER_RANK_FFT_SMALL;
        size_t bin_size     = 1 << bin_rank;
        size_t frame_size   = bin_size >> 1;
        nRank               = rank;
        nSteps              = 0;
        nBlocks             = 0;
        nFrameMax           = frame_size;
        size_t to_do        = (count > frame_size) ? frame_size : count;

        // Prepare first frame
        dsp::copy(vConvFirst, data, to_do);

        // Calculate FFT of first bin
        dsp::copy(conv_re, data, to_do);
        dsp::direct_fft(conv_re, conv_im, conv_re, conv_im, bin_rank);

        // Move pointers
        data               += frame_size;
        count              -= frame_size;
        conv_re            += bin_size;
        conv_im            += bin_size;

        while (count > 0)
        {
            to_do               = (count > frame_size) ? frame_size : count;
            nFrameMax           = frame_size;

            // Calculate FFT
            dsp::copy(conv_re, data, to_do);
            dsp::direct_fft(conv_re, conv_im, conv_re, conv_im, bin_rank);

            // Move pointers
            data               += frame_size;
            count              -= to_do;
            conv_re            += bin_size;
            conv_im            += bin_size;

            // Update size of bin
            if (bin_rank < rank)
            {
                nSteps         ++;
                bin_rank       ++;
                bin_size      <<= 1;
                frame_size    <<= 1;
            }
            else
                nBlocks        ++;
        }

        nBlocksDone         = nBlocks;

        return true;
    }

    void Convolver::destroy()
    {
        if (vData != NULL)
        {
            delete[] vData;
            vData       = NULL;
        }
    }

    void Convolver::process(float *dst, const float *src, size_t count)
    {
        if (vData == NULL)
        {
            dsp::fill_zero(dst, count);
            return;
        }

        while (count > 0)
        {
            size_t to_do        = (CONVOLVER_SMALL_FRM_SIZE - (nFrameSize & CONVOLVER_SMALL_FRM_MASK));
            if (to_do > count)
                to_do               = count;

            // Check that we are available to apply convolution via FFT
            if (to_do == CONVOLVER_SMALL_FRM_SIZE)
            {
                // Apply convolution
                dsp::copy(&vFrameRe[nFrameSize], src, CONVOLVER_SMALL_FRM_SIZE);
                dsp::direct_fft(vFftRe, vFftIm, &vFrameRe[nFrameSize], vFrameIm, CONVOLVER_RANK_FFT_SMALL);
                dsp::complex_mul(vFftRe, vFftIm, vFftRe, vFftIm, vConvRe, vConvIm, CONVOLVER_SMALL_FFT_SIZE);
                dsp::reverse_fft(vFftRe, vFftIm, vFftRe, vFftIm, CONVOLVER_RANK_FFT_SMALL);
                dsp::add(vBufferPtr, vFftRe, CONVOLVER_SMALL_FFT_SIZE);
            }
            else if (to_do > 0) // We need to do direct convolution
            {
                // Do direct convolution
                dsp::copy(&vFrameRe[nFrameSize], src, to_do); // Store input data into the frame

                for (size_t i=0; i<to_do; ++i)
                    dsp::add_multiplied(vBufferPtr, vConvFirst, src[i], CONVOLVER_SMALL_FRM_SIZE);
            }

            // Update frame size and source pointer
            nFrameSize         += to_do;
            src                += to_do;

            // Check that frame part is full
            if (!(nFrameSize & CONVOLVER_SMALL_FRM_MASK))
            {
                /*
                     Calculate mask:
                         prev curr | trigger
                         ----------+---------
                           0    0  |    0
                           0    1  |    1
                           1    0  |    1
                           1    1  |    0

                    Formula: trigger = prev ^ curr

                 */
                size_t frame_id = (nFrameSize-1) >> CONVOLVER_RANK_FRM_SMALL;
                size_t frm_mask = ((frame_id-1) ^ frame_id);
                size_t step     = CONVOLVER_SMALL_FFT_SIZE;
                size_t rank     = CONVOLVER_RANK_FFT_SMALL;
                float *head     = &vFrameRe[(frame_id << CONVOLVER_RANK_FRM_SMALL)%nFrameMax];
                float *conv_re  = &vConvRe[CONVOLVER_SMALL_FFT_SIZE];
                float *conv_im  = &vConvIm[CONVOLVER_SMALL_FFT_SIZE];

//                lsp_trace("frame_id = %d, mask=0x%x, size=%d, max_size=%d",
//                        int(frame_id), int(frm_mask), int(nFrameSize), int(nFrameMax));

                for (size_t i=0; i<nSteps; ++i)
                {
                    if (frm_mask & 1)
                    {
                        // Apply higher-order convolutions
//                        lsp_trace("Convolve: off=%d, blk1=%d, blk2=%d\n",
//                            int(frame_id),
//                            int(&head[-(step >> 1)] - head) / CONVOLVER_SMALL_FRM_SIZE,
//                            int(conv_re - vConvRe) / CONVOLVER_SMALL_FFT_SIZE);

                        dsp::copy(vTempRe, &head[-(step >> 1)], step >> 1);
                        dsp::fill_zero(&vTempRe[step >> 1], (step >> 1));
                        dsp::direct_fft(vFftRe, vFftIm, vTempRe, vFrameIm, rank);
                        dsp::complex_mul(vTempRe, vTempIm, vFftRe, vFftIm, conv_re, conv_im, step);
                        dsp::reverse_fft(vFftRe, vFftIm, vTempRe, vTempIm, rank);
                        dsp::add(vBufferPtr, vFftRe, step);
                    }

                    conv_re            += step;
                    conv_im            += step;
                    rank               ++;
                    step              <<= 1;
                    frm_mask          >>= 1;
                }

                if (nBlocksDone < nBlocks)
                {
                    size_t tgt_block    = (nFrameSize * (nBlocks + 1)) / nFrameMax;
                    if (tgt_block > nBlocks)
                        tgt_block           = nBlocks;

                    while (nBlocksDone < tgt_block)
                    {
                        // Do multiplication in frequency domain, reverse FFT and apply it to the history buffer
                        dsp::complex_mul(vFftRe, vFftIm, vTaskRe, vTaskIm, pConvRe, pConvIm, (1 << nRank));
                        dsp::reverse_fft(vTempRe, vTempIm, vFftRe, vFftIm, rank);
                        dsp::add(pTargetPtr, vTempRe, step);

                        // Update pointers
                        pConvRe            += (1 << nRank);
                        pConvIm            += (1 << nRank);
                        pTargetPtr         += nFrameMax;
                        nBlocksDone        ++;
                    }
                }

                if (nFrameSize >= nFrameMax)
                {
                    if (nBlocks > 0)
                    {
                        // Do FFT of the frame into task buffer
                        dsp::direct_fft(vTaskRe, vTaskIm, &head[-(step >> 1)], vFrameIm, rank);

                        // Do multiplication in frequency domain, reverse FFT and apply it to the history buffer
                        dsp::complex_mul(vFftRe, vFftIm, vTaskRe, vTaskIm, conv_re, conv_im, step);
                        dsp::reverse_fft(vTempRe, vTempIm, vFftRe, vFftIm, rank);
                        dsp::add(vBufferPtr, vTempRe, step);

                        // Set number of blocks done
                        nBlocksDone     = 1;
                        pConvRe         = &conv_re[step];
                        pConvIm         = &conv_im[step];
                        pTargetPtr      = &vBufferPtr[step>>1];
                    }

                    // Update frame
                    dsp::copy(&vFrameRe[-nFrameMax], vFrameRe, nFrameMax);
                    dsp::fill_zero(vFrameRe, nFrameMax);
                    nFrameSize -= nFrameMax;

                    // Check that buffer head is not out of range
                    if (vBufferPtr >= vBufferTail)
                    {
//                        lsp_trace("Buffer shift");
                        size_t buf_size     = vBufferEnd - vBufferHead;
                        size_t hist_size    = vBufferEnd - vBufferPtr;
                        dsp::move(vBufferHead, vBufferPtr, hist_size);
                        dsp::fill_zero(&vBufferHead[hist_size], buf_size - hist_size);
                        pTargetPtr         -= (vBufferPtr - vBufferHead); // Move target pointer
                        vBufferPtr          = vBufferHead;
                    }
                }
            }

            // Copy data to output and update buffer pointers
            if (to_do > 0)
            {
                dsp::copy(dst, vBufferPtr, to_do);
                vBufferPtr         += to_do;
                dst                += to_do;
                count              -= to_do;
            }
        }

//        while (count > 0)
//        {
//            // Determine current frame ID
//            size_t frame_off    = nFrameSize & CONVOLVER_SMALL_FRM_MASK;
//            size_t to_do        = (CONVOLVER_SMALL_FRM_SIZE - frame_off);
//            if (to_do > count)
//                to_do               = count;
//
//            // Check that we are available to apply convolution via FFT
//            if (to_do == CONVOLVER_SMALL_FRM_SIZE)
//            {
//                // We can do FFT convolution
//                // Do FFT of the small frame into temporary buffer
//                dsp::direct_fft(vTmpBuf, &vTmpBuf[CONVOLVER_SMALL_FFT_SIZE], &vFrame[nFrameSize], vZeroFrame, CONVOLVER_RANK_FFT_SMALL);
//
//                // Now we can calculate convolutions with all small parts
//                float *fft_re   = &vTmpBuf[CONVOLVER_SMALL_FFT_SIZE*2];
//                float *fft_im   = &vTmpBuf[CONVOLVER_SMALL_FFT_SIZE*3];
//                float *c_re     = vConvRe;
//                float *c_im     = vConvIm;
//                float *tgt      = pBufHead;
//
//                for (size_t i=0; i<nSmallParts; ++i)
//                {
//                    // Do multiplication in frequency domain, reverse FFT and apply it to the history buffer
//                    dsp::complex_mul(fft_re, fft_im, c_re, c_im, vTmpBuf, &vTmpBuf[CONVOLVER_SMALL_FFT_SIZE], CONVOLVER_SMALL_FFT_SIZE);
//                    dsp::reverse_fft(fft_re, fft_im, fft_re, fft_im, CONVOLVER_RANK_FFT_SMALL);
//                    dsp::add(tgt, fft_re, CONVOLVER_SMALL_FFT_SIZE);
//
//                    // Update pointers
//                    c_re           += CONVOLVER_SMALL_FFT_SIZE;
//                    c_im           += CONVOLVER_SMALL_FFT_SIZE;
//                    tgt            += CONVOLVER_SMALL_FRM_SIZE;
//                }
//
//                // Copy data to output buffer
//                dsp::copy(dst, pBufHead, to_do);
//
//                // Update pointers
//                nFrameSize             += to_do;
//                pBufHead               += to_do;
//                src                    += to_do;
//                dst                    += to_do;
//                count                  -= to_do;
//            }
//            else if (to_do > 0) // We need to do direct convolution
//            {
//                // Do direct convolution
//                dsp::copy(&vFrame[nFrameSize], src, to_do); // Store input data into the frame
//                frame_off              += to_do;
//                nFrameSize             += to_do;
//                count                  -= to_do;
//
//                while (to_do--)
//                {
//                    dsp::add_multiplied(pBufHead, vFirstPart, *(src++), CONVOLVER_SMALL_FRM_SIZE);
//                    *(dst++)            = *(pBufHead++);
//                }
//
//                // Check if small frame is full
//                if ((frame_off >= CONVOLVER_SMALL_FRM_SIZE) && (nSmallParts > 1))
//                {
//                    // Do FFT of the small frame into temporary buffer
//                    dsp::direct_fft(vTmpBuf, &vTmpBuf[CONVOLVER_SMALL_FFT_SIZE], &vFrame[nFrameSize - CONVOLVER_SMALL_FRM_SIZE], vZeroFrame, CONVOLVER_RANK_FFT_SMALL);
//
//                    // Now we can calculate convolutions with all small parts
//                    float *fft_re   = &vTmpBuf[CONVOLVER_SMALL_FFT_SIZE*2];
//                    float *fft_im   = &vTmpBuf[CONVOLVER_SMALL_FFT_SIZE*3];
//                    float *c_re     = &vConvRe[CONVOLVER_SMALL_FFT_SIZE];
//                    float *c_im     = &vConvIm[CONVOLVER_SMALL_FFT_SIZE];
//                    float *tgt      = pBufHead;
//
//                    for (size_t i=1; i<nSmallParts; ++i)
//                    {
//                        // Do multiplication in frequency domain, reverse FFT and apply it to the history buffer
//                        dsp::complex_mul(fft_re, fft_im, c_re, c_im, vTmpBuf, &vTmpBuf[CONVOLVER_SMALL_FFT_SIZE], CONVOLVER_SMALL_FFT_SIZE);
//                        dsp::reverse_fft(fft_re, fft_im, fft_re, fft_im, CONVOLVER_RANK_FFT_SMALL);
//                        dsp::add(tgt, fft_re, CONVOLVER_SMALL_FFT_SIZE);
//
//                        // Update pointers
//                        c_re           += CONVOLVER_SMALL_FFT_SIZE;
//                        c_im           += CONVOLVER_SMALL_FFT_SIZE;
//                        tgt            += CONVOLVER_SMALL_FRM_SIZE;
//                    }
//                } // if (frame_off >= CONVOLVER_SMALL_FRM_SIZE)
//            } // to_do > 0
//
//            // Check if large frame is full, compute large tail
//            if (nFrameSize >= nFrameCapacity)
//            {
//                nFrameSize = 0;
//
//                if (nLargeParts > 0)
//                {
//                    // Do FFT of the frame into temporary buffer
//                    dsp::direct_fft(vTmpBuf, &vTmpBuf[nLargeFFT], vFrame, vZeroFrame, nRank);
//
//                    // Now we can calculate convolutions with all large parts
//                    float *fft_re   = &vTmpBuf[nLargeFFT*2];
//                    float *fft_im   = &vTmpBuf[nLargeFFT*3];
//                    float *c_re     = &vConvRe[nLargeFFT];
//                    float *c_im     = &vConvIm[nLargeFFT];
//                    float *tgt      = pBufHead;
//
//                    for (size_t i=0; i<nLargeParts; ++i)
//                    {
//                        // Do multiplication in frequency domain, reverse FFT and apply it to the history buffer
//                        dsp::complex_mul(fft_re, fft_im, c_re, c_im, vTmpBuf, &vTmpBuf[nLargeFFT], nLargeFFT);
//                        dsp::reverse_fft(fft_re, fft_im, fft_re, fft_im, nRank);
//                        dsp::add(tgt, fft_re, nLargeFFT);
//
//                        // Update pointers
//                        c_re           += nLargeFFT;
//                        c_im           += nLargeFFT;
//                        tgt            += nFrameCapacity;
//                    }
//                }
//                dsp::fill_zero(vFrame, nLargeFFT);
//
//                // Validate that buffer head is not out of range
//                if (pBufHead >= pBufThresh)
//                {
//                    size_t buf_size     = vTmpBuf - vBuffer;
//                    size_t hist_size    = vTmpBuf - pBufHead;
//                    dsp::move(vBuffer, pBufHead, hist_size);
//                    dsp::fill_zero(&vBuffer[hist_size], buf_size - hist_size);
//                    pBufHead    = vBuffer;
//                }
//            } // (nFrameSize >= nFrameCapacity)
//        }
    }

} /* namespace lsp */
