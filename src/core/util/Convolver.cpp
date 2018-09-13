/*
 * Convolver.cpp
 *
 *  Created on: 29 янв. 2016 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <core/debug.h>
#include <core/util/Convolver.h>
#include <stdarg.h>

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

        vFrame          = NULL;
        vTempBuf        = NULL;
        vConv           = NULL;
        vBufferHead     = NULL;
        vBufferTail     = NULL;
        vBufferPtr      = NULL;
        vBufferEnd      = NULL;
        vConvFirst      = NULL;
        vTask           = NULL;

        nRank           = 0;
        nSteps          = 0;
        nBlocks         = 0;
        nBlocksDone     = 0;
        nDirectSize     = CONVOLVER_SMALL_FRM_SIZE;
        pConv           = NULL;
        pTargetPtr      = NULL;
        vData           = NULL;
    }

    Convolver::~Convolver()
    {
        destroy();
    }

//    static void dump(const float *buf, size_t count, const char *fmt, ...)
//    {
//        va_list args;
//        va_start(args, fmt);
//        vprintf(fmt, args);
//        va_end(args);
//        printf(": ");
//
//        while (count--)
//            printf("%.4f ", *(buf++));
//
//        printf("\n");
//    }

//    static void dump_fastconv(const float *conv, size_t rank, const char *fmt, ...)
//    {
//        va_list args;
//        va_start(args, fmt);
//        vprintf(fmt, args);
//        va_end(args);
//        printf(": ");
//
//        size_t conv_size = 1 << rank;
//        float *buf = new float[conv_size * 4];
//        dsp::fill_zero(buf, conv_size * 4);
//        buf[conv_size] = 1.0f;
//
//        dsp::fastconv_parse_apply(buf, &buf[conv_size*2], conv, &buf[conv_size], rank);
//        for(size_t i=0; i<conv_size; ++i)
//            printf("%.4f ", buf[i]);
//
//        delete [] buf;
//
//        printf("\n");
//    }

    bool Convolver::init(const float *data, size_t count, size_t rank, float phase)
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
        allocate               += fft_buf_size * 2; // Temporary buffer (real and imaginary)
        allocate               += fft_buf_size * 2; // Frame buffer (real only, two frames)
        allocate               += fft_buf_size * 2; // Task buffer (real and imaginary)
        allocate               += bins * data_buf_size * 9 + data_buf_size * 4; // Buffer for convolution tail

        uint8_t *pdata          = NULL;
        float *fptr             = alloc_aligned<float>(pdata, allocate);
        if (fptr == NULL)
            return false;

        // Replace previously used data by new allocated data
        destroy();
        vData               = pdata;

        dsp::fill_zero(fptr, allocate); // Drop all previously used data

        vBufferHead         = fptr;
        fptr               += bins * data_buf_size * 8;
        vBufferTail         = fptr;
        fptr               += bins * data_buf_size + data_buf_size * 4;
        vBufferEnd          = fptr;
        vBufferPtr          = vBufferHead;

        vConvFirst          = fptr;
        fptr               += CONVOLVER_SMALL_FRM_SIZE;

        vTask               = fptr;
        fptr               += fft_buf_size * 2;

        fptr               += fft_buf_size; // previous frame (re)
        vFrame              = fptr;
        fptr               += fft_buf_size;

        vTempBuf            = fptr;
        fptr               += fft_buf_size * 2;

        vConv               = fptr;
        fptr               += bins * fft_buf_size * 2;

        /* Calculate convolutions

            Conv buffer layout:
            +---+---+------+------------+------------------------+
            |FFT|FFT|FFT x2|   FFT x4   |       FFT x5           |  . . .
            +---+---+------+------------+------------------------+
         */
        float *conv_re      = vConv;
        size_t bin_rank     = CONVOLVER_RANK_FFT_SMALL;
        size_t bin_size     = 1 << bin_rank;
        size_t frame_size   = bin_size >> 1;
        nRank               = rank;
        nSteps              = 0;
        nBlocks             = 0;
        nFrameMax           = frame_size;
        nDirectSize         = (count > frame_size) ? frame_size : count;

//        dump(data, count, "DATA");

        // Prepare first frame
        dsp::copy(vConvFirst, data, nDirectSize);
//        dump(vConvFirst, CONVOLVER_SMALL_FRM_SIZE, "vConvFirst");

        // Calculate FFT of first bin
        dsp::fill_zero(vTempBuf, bin_size*2);
        dsp::copy(vTempBuf, data, nDirectSize);
//        dump(vTempBuf, 1 << bin_rank, "conv_tmp[0] (%p)", vTempBuf); // dbg
        dsp::fastconv_parse(conv_re, vTempBuf, bin_rank);
//        dump(conv_re, 1 << bin_rank, "conv_fft[0] (%p)", conv_re); // dbg
//        dump_fastconv(conv_re, bin_rank, "conv_img[0] (%p)", conv_re); // dbg

        // Move pointers
        data               += frame_size;
        count              -= nDirectSize;
        conv_re            += bin_size * 2;

        size_t i_dbg        = 1;
        while (count > 0)
        {
            size_t to_do        = (count > frame_size) ? frame_size : count;
            nFrameMax           = frame_size;

            // Calculate FFT
            dsp::fill_zero(vTempBuf, bin_size*2);
            dsp::copy(vTempBuf, data, to_do);
//            dump(vTempBuf, 1 << bin_rank, "conv_tmp[%d] (%p)", int(i_dbg), vTempBuf); // dbg
            dsp::fastconv_parse(conv_re, vTempBuf, bin_rank);
//            dump(conv_re, 1 << bin_rank, "conv_fft[%d] (%p)", int(i_dbg), conv_re);
//            dump_fastconv(conv_re, bin_rank, "conv_img[%d] (%p)", int(i_dbg), conv_re); // dbg

            // Move pointers
            data               += frame_size;
            count              -= to_do;
            conv_re            += bin_size * 2;

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
            i_dbg++;
        }

        // Initialize frame size
        nFrameSize          = size_t(phase * nFrameMax) & (~CONVOLVER_SMALL_FRM_MASK);
        if (nFrameSize >= nFrameMax)
            nFrameSize          = 0;
        nBlocksDone         = nBlocks;

        return true;
    }

    void Convolver::destroy()
    {
        if (vData != NULL)
            free_aligned(vData);
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
            // Determine current offset relative to frame
            size_t frame_off    = nFrameSize & CONVOLVER_SMALL_FRM_MASK;

            // Check that frame part is full
            if (!frame_off)
            {
                /*
                     Calculate convolution mask:
                         prev curr | trigger
                         ----------+---------
                           0    0  |    0
                           0    1  |    1
                           1    0  |    1
                           1    1  |    0

                    Formula: trigger = prev ^ curr

                 */

                // Apply higher-order convolutions
                size_t frame_id     = nFrameSize >> CONVOLVER_RANK_FRM_SMALL;
                size_t frm_mask     = ((frame_id-1) ^ frame_id);
                float *conv_re      = &vConv[CONVOLVER_SMALL_FFT_SIZE*2];
                size_t rank         = CONVOLVER_RANK_FFT_SMALL;

                for (size_t i=0; i<nSteps; ++i)
                {
                    float *fptr     = vFrame + nFrameSize - (1 << (rank - 1));
                    if (frm_mask & 1)
                    {
//                        printf("apply_history vFrame=%p, head=%p, diff=%d\n", vFrame, head, int(head - vFrame));
//                        dump(vBufferHead, 0x80, "pbhptr(%p)", vBufferHead);
//                        dump(vBufferPtr, (1 << rank), "bbptr(%p)", vBufferPtr);
//                        dump(head, (1 << rank), "head(%p)", head);
//                        dump_fastconv(conv_re, CONVOLVER_RANK_FFT_SMALL, "conv_re(%p)", conv_re);
                        lsp_trace("fastconv_parse_apply dst=0x%x, conv=0x%x, fptr=-0x%x, n=%d",
                                (vBufferPtr-vBufferHead)/CONVOLVER_SMALL_FRM_SIZE,
                                (conv_re-vConv)/CONVOLVER_SMALL_FFT_SIZE,
                                (1 << (rank - 1))/CONVOLVER_SMALL_FRM_SIZE,
                                1 << (rank - CONVOLVER_RANK_FFT_SMALL));
                        dsp::fastconv_parse_apply(vBufferPtr, vTempBuf, conv_re, fptr, rank);
//                        dump(vBufferPtr, (1 << rank), "abptr(%p)", vBufferPtr);
//                        dump(vBufferHead, 0x80, "abhptr(%p)", vBufferHead);
                    }
                    conv_re        += 1 << (rank+1);
                    frm_mask      >>= 1;
                    rank           ++;
                }

                if (nBlocksDone < nBlocks)
                {
                    size_t tgt_block    = ((nFrameSize + CONVOLVER_SMALL_FRM_SIZE) * nBlocks) / nFrameMax;
                    if (tgt_block > nBlocks)
                        tgt_block           = nBlocks;

                    while (nBlocksDone < tgt_block)
                    {
                        // Do multiplication in frequency domain, reverse FFT and apply it to the history buffer
                        lsp_trace("fastconv_apply_block dst=0x%x, fptr=-0x%x, n=%d %d/%d",
                                    (pTargetPtr-vBufferHead)/CONVOLVER_SMALL_FRM_SIZE,
                                    0, 1 << (nRank - CONVOLVER_RANK_FFT_SMALL),
                                    nBlocksDone, nBlocks
                                    );
                        dsp::fastconv_apply(pTargetPtr, vTempBuf, vTask, pConv, nRank);

                        // Update pointers
                        pConv              += nFrameMax << 1;
                        pTargetPtr         += nFrameMax;
                        nBlocksDone        ++;
                    }
                }

                // Frame reached it's size?
                if ((nFrameSize == 0) && (nBlocks > 0))
                {
                    // Apply convolution
                    lsp_trace("fastconv_parse+apply dst=0x%x, conv=0x%x, fptr=-0x%x, n=%d 0/%d",
                            (vBufferPtr-vBufferHead)/CONVOLVER_SMALL_FRM_SIZE,
                            (conv_re-vConv)/CONVOLVER_SMALL_FFT_SIZE,
                            nFrameMax/CONVOLVER_SMALL_FRM_SIZE,
                            1 << (nRank - CONVOLVER_RANK_FFT_SMALL),
                            nBlocks);
                    dsp::fastconv_parse(vTask, vFrame - nFrameMax, nRank);
                    dsp::fastconv_apply(vBufferPtr, vTempBuf, vTask, conv_re, nRank);

                    // Set number of blocks done
                    nBlocksDone     = 1;
                    pConv           = conv_re;
                    pTargetPtr      = vBufferPtr + nFrameMax;
                }
            }

            // Apply real-time part
            size_t to_do        = CONVOLVER_SMALL_FRM_SIZE - frame_off;
            if (to_do > count)
                to_do               = count;

            // Store input data into the frame
            dsp::copy(&vFrame[nFrameSize], src, to_do);

            // Check that we are available to apply convolution via FFT
            if (to_do == CONVOLVER_SMALL_FRM_SIZE)
            {
//                printf("fastconv_parse_apply\n");
//                dump(vBufferHead, 0x80, "pbufhead(%p)", vBufferHead);
//                dump(vBufferPtr, CONVOLVER_SMALL_FFT_SIZE, "bbufptr(%p)", vBufferPtr);
//                dump(&vFrame[nFrameSize], CONVOLVER_SMALL_FFT_SIZE, "frame[%x](%p)", int(nFrameSize), &vFrame[nFrameSize]);
//                dump_fastconv(vConv, CONVOLVER_RANK_FFT_SMALL, "conv(%p)", vConv);
                dsp::fastconv_parse_apply(vBufferPtr, vTempBuf, vConv, &vFrame[nFrameSize], CONVOLVER_RANK_FFT_SMALL);
//                dump(vBufferPtr, CONVOLVER_SMALL_FFT_SIZE, "abufptr(%p)", vBufferPtr);
//                dump(vBufferHead, 0x80, "abufhead(%p)", vBufferHead);
            }
            else // We need to do direct convolution
            {
//                for (size_t i=0; i<to_do; ++i)
//                    dsp::scale_add3(&vBufferPtr[i], vConvFirst, src[i], nDirectSize);
//                printf("direct_convolve\n");
//                dump(vBufferHead, 0x80, "pbufhead(%p)", vBufferHead);
                dsp::convolve(vBufferPtr, src, vConvFirst, nDirectSize, to_do);
//                dump(vBufferPtr, CONVOLVER_SMALL_FFT_SIZE, "abufptr(%p)", vBufferPtr);
//                dump(vBufferHead, 0x80, "abufhead(%p)", vBufferHead);
            }

            // Update frame pointer
            nFrameSize         += to_do;
            if (nFrameSize >= nFrameMax)
            {
                // Update frame
                lsp_trace("switch_frame");
                dsp::copy(vFrame-nFrameMax, vFrame, nFrameMax);
                dsp::fill_zero(vFrame, nFrameMax);
                nFrameSize -= nFrameMax;
            }

            // Copy data to output and update buffer pointers
            dsp::copy(dst, vBufferPtr, to_do);
            vBufferPtr         += to_do;
            src                += to_do;
            dst                += to_do;
            count              -= to_do;

            // Check that buffer head is required to be moved
            if (vBufferPtr >= vBufferTail)
            {
                lsp_trace("buffer_shift");
                size_t hist_size    = vBufferEnd - vBufferPtr;
                size_t free_size    = vBufferPtr - vBufferHead;
                dsp::move(vBufferHead, vBufferPtr, hist_size);
                dsp::fill_zero(&vBufferHead[hist_size], vBufferPtr - vBufferHead);
                pTargetPtr         -= free_size;
                vBufferPtr          = vBufferHead;
            }
        }
    }

} /* namespace lsp */
