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
//        vfprintf(stderr, fmt, args);
//        va_end(args);
//        fprintf(stderr, ": ");
//
//        while (count--)
//            fprintf(stderr, "%.4f ", *(buf++));
//
//        fprintf(stderr, "\n");
//    }
//
//    static void dump_fastconv(const float *conv, size_t rank, const char *fmt, ...)
//    {
//        va_list args;
//        va_start(args, fmt);
//        vfprintf(stderr, fmt, args);
//        va_end(args);
//        fprintf(stderr, ": ");
//
//        size_t conv_size = 1 << rank;
//        float *buf = new float[conv_size * 4];
//        dsp::fill_zero(buf, conv_size * 4);
//        buf[conv_size] = 1.0f;
//
//        dsp::fastconv_parse_apply(buf, &buf[conv_size*2], conv, &buf[conv_size], rank);
//        for(size_t i=0; i<conv_size; ++i)
//            fprintf(stderr, "%.4f ", buf[i]);
//
//        delete [] buf;
//
//        fprintf(stderr, "\n");
//    }

    bool Convolver::init(const float *data, size_t count, size_t rank, float phase)
    {
        // Check arguments
        if (count <= 0)
        {
            destroy();
            return true;
        }

//        lsp_trace("Initializing convolver this=%p", this);

        // Determine number of buffers
        if (rank < CONVOLVER_RANK_MIN)
            rank    = CONVOLVER_RANK_MIN;
        else if (rank >= CONVOLVER_RANK_MAX)
            rank    = CONVOLVER_RANK_MAX;

        // Determine size of buffer
        size_t fft_buf_size     = 1 << rank;
        size_t data_buf_size    = fft_buf_size >> 1;
        size_t bins             = (count + data_buf_size - 1) >> (rank - 1);

//        lsp_trace("count = 0x%x, rank=%d, phase=%.3f, bins=%d",
//                int(count), int(rank), phase, int(bins));

        size_t allocate         = CONVOLVER_SMALL_FRM_SIZE; // Non-FFT first frame
        allocate               += bins * fft_buf_size * 2; // FFT of the convolution
        allocate               += fft_buf_size * 2; // Temporary buffer (real and imaginary)
        allocate               += fft_buf_size * 2; // Frame buffer (real only, two frames)
        allocate               += fft_buf_size * 2; // Task buffer (real and imaginary)
        allocate               += bins * data_buf_size * 9 + data_buf_size * 6; // Buffer for convolution tail

        uint8_t *pdata          = NULL;
        float *fptr             = alloc_aligned<float>(pdata, allocate);
        lsp_guard_assert(float *save = fptr);
        if (fptr == NULL)
            return false;
//        lsp_trace("Allocated %x floats (%x bytes) at: %p, pdata: %p",
//                int(allocate), int(allocate * sizeof(float)), fptr, pdata);

        // Replace previously used data by new allocated data
        destroy();
        vData               = pdata;
//        lsp_trace("vData    = %p x 0x%x", vData, int(allocate));

        dsp::fill_zero(fptr, allocate); // Drop all previously used data

        vBufferHead         = fptr;
        fptr               += bins * data_buf_size * 8;
//        lsp_trace("vBufferHead = %p x 0x%x", vBufferHead, int(bins * data_buf_size * 8));

        vBufferTail         = fptr;
        fptr               += bins * data_buf_size + data_buf_size * 6;
//        lsp_trace("vBufferTail = %p x 0x%x", vBufferTail, int(bins * data_buf_size * 6));

        vBufferEnd          = fptr;
//        lsp_trace("vBufferEnd = %p", vBufferEnd);

        vBufferPtr          = vBufferHead;
//        lsp_trace("vBufferPtr = %p", vBufferPtr);

        vConvFirst          = fptr;
        fptr               += CONVOLVER_SMALL_FRM_SIZE;
//        lsp_trace("vConvFirst = %p x 0x%x", vConvFirst, CONVOLVER_SMALL_FRM_SIZE);

        vTask               = fptr;
        fptr               += fft_buf_size * 2;

//        lsp_trace("vTask = %p x 0x%x", vTask, int(fft_buf_size * 2));

//        lsp_trace("vFrame(prev) = %p x 0x%x", fptr, int(fft_buf_size));
        fptr               += fft_buf_size; // previous frame (re)
        vFrame              = fptr;
        fptr               += fft_buf_size;
//        lsp_trace("vFrame(curr) = %p x 0x%x", vFrame, int(fft_buf_size));

        vTempBuf            = fptr;
        fptr               += fft_buf_size * 2;
//        lsp_trace("vTempBuf = %p x 0x%x", vTempBuf, int(fft_buf_size * 2));

        vConv               = fptr;
        fptr               += bins * fft_buf_size * 2;
//        lsp_trace("vConv = %p x 0x%x", vConv, int(bins * fft_buf_size * 2));

        lsp_assert(fptr <= &save[allocate]);

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
//        lsp_trace("dsp::copy dst=%p, src=%p, count=0x%x", vConvFirst, data, int(nDirectSize));
//        dump(vConvFirst, CONVOLVER_SMALL_FRM_SIZE, "vConvFirst");

        // Calculate FFT of first bin
        dsp::fill_zero(vTempBuf, bin_size*2);
//        lsp_trace("dsp::fill_zero dst=%p, count=0x%x", vTempBuf, int(bin_size*2));

        dsp::copy(vTempBuf, data, nDirectSize);
//        lsp_trace("dsp::copy dst=%p, src=%p, count=0x%x", vTempBuf, data, int(nDirectSize));

//        dump(vTempBuf, 1 << bin_rank, "conv_tmp[0] (%p)", vTempBuf); // dbg
        dsp::fastconv_parse(conv_re, vTempBuf, bin_rank);
//        lsp_trace("dsp::fastconv_parse dst=%p, src=%p, rank=0x%x", conv_re, vTempBuf, int(bin_rank));
//        dump(conv_re, 1 << bin_rank, "conv_fft[0] (%p)", conv_re); // dbg
//        dump_fastconv(conv_re, bin_rank, "conv_img[0] (%p)", conv_re); // dbg
//        dump_fastconv(conv_re, bin_rank, "conv[0x%x] (%p)", (conv_re - vConv) / CONVOLVER_SMALL_FFT_SIZE, conv_re); // dbg

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
//            lsp_trace("dsp::fill_zero dst=%p, count=0x%x", vTempBuf, int(bin_size*2));
            dsp::copy(vTempBuf, data, to_do);
//            lsp_trace("dsp::copy dst=%p, src=%p, count=0x%x", vTempBuf, data, int(to_do));
//            dump(vTempBuf, 1 << bin_rank, "conv_tmp[%d] (%p)", int(i_dbg), vTempBuf); // dbg
            dsp::fastconv_parse(conv_re, vTempBuf, bin_rank);
//            lsp_trace("dsp::fastconv_parse dst=%p, src=%p, rank=0x%x", conv_re, vTempBuf, int(bin_rank));
//            dump(conv_re, 1 << bin_rank, "conv_fft[%d] (%p)", int(i_dbg), conv_re);
//            dump_fastconv(conv_re, bin_rank, "conv[0x%x] (%p)", (conv_re - vConv) / CONVOLVER_SMALL_FFT_SIZE, conv_re); // dbg

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

//        lsp_assert_msg((conv_re <= &cptr[allocate]),
//                "conv_re (%p) is after end of allocated data (%p)",
//                conv_re, &cptr[allocate]);

        // Initialize frame size
        nFrameSize          = size_t(phase * nFrameMax) & (~CONVOLVER_SMALL_FRM_MASK);
        if (nFrameSize >= nFrameMax)
            nFrameSize          = 0;
        nBlocksDone         = nBlocks;

//        lsp_trace("nSteps   = 0x%x", int(nSteps));
//        lsp_trace("nBlocks  = 0x%x", int(nBlocks));
//        lsp_trace("nFrameSize = 0x%x", int(nFrameSize));
//        lsp_trace("nBlocksDone = 0x%x", int(nBlocksDone));

        return true;
    }

    void Convolver::destroy()
    {
//        lsp_trace("Convolver::destroy this=%p", this);
//        lsp_trace("free_aligned vData=%p", vData);

        free_aligned(vData);

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
        pConv           = NULL;
        pTargetPtr      = NULL;

        nRank           = 0;
        nSteps          = 0;
        nBlocks         = 0;
        nBlocksDone     = 0;
        nDirectSize     = 0;
    }

    void Convolver::process(float *dst, const float *src, size_t count)
    {
        if (vData == NULL)
        {
            dsp::fill_zero(dst, count);
            return;
        }

//        lsp_trace("Start process this=%p, dst=%p, src=%p, count=0x%x, vData=%p",
//                this, dst, src, int(count), vData);

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
//                        lsp_trace("fastconv_parse_apply dst=0x%x, conv=0x%x, fptr=-0x%x, n=%d",
//                                (vBufferPtr-vBufferHead)/CONVOLVER_SMALL_FRM_SIZE,
//                                (conv_re-vConv)/CONVOLVER_SMALL_FFT_SIZE,
//                                (1 << (rank - 1))/CONVOLVER_SMALL_FRM_SIZE,
//                                1 << (rank - CONVOLVER_RANK_FFT_SMALL));

//                        lsp_trace("dsp::fastconv_parse_apply dst=%p, tmp=%p, conv=%p, src=%p, rank=0x%x",
//                                vBufferPtr, vTempBuf, conv_re, fptr, int(rank));
//                        lsp_assert(vBufferEnd >= &vBufferPtr[1 << rank]);
                        dsp::fastconv_parse_apply(vBufferPtr, vTempBuf, conv_re, fptr, rank);
                    }
                    conv_re        += 1 << (rank+1); // Move pointer to next convolution
                    frm_mask      >>= 1;
                    rank           ++;
                }

                // Start of frame and need to perform tail convolution?
                if ((nFrameSize == 0) && (nBlocks > 0))
                {
                    // Apply convolution
//                    lsp_trace("dsp::fastconv_parse dst=%p, src=%p, rank=0x%x",
//                            vTask, vFrame - nFrameMax, int(nRank));
                    dsp::fastconv_parse(vTask, vFrame - nFrameMax, nRank);

                    // Set number of blocks done
                    nBlocksDone     = 0;
                    pConv           = conv_re;
                    pTargetPtr      = vBufferPtr;
                }

                // Do tail convolution
                if (nBlocksDone < nBlocks)
                {
                    size_t tgt_block    = ((nFrameSize + CONVOLVER_SMALL_FRM_SIZE) * nBlocks) / nFrameMax;
                    if (tgt_block > nBlocks)
                        tgt_block           = nBlocks;

                    while (nBlocksDone < tgt_block)
                    {
                        // Do multiplication in frequency domain, reverse FFT and apply it to the history buffer
//                        lsp_trace("fastconv_apply_block dst=0x%x, conv=0x%x, fptr=-0x%x, n=%d %d/%d",
//                                    (pTargetPtr-vBufferHead)/CONVOLVER_SMALL_FRM_SIZE,
//                                    (pConv-vConv)/CONVOLVER_SMALL_FFT_SIZE,
//                                    0, 1 << (nRank - CONVOLVER_RANK_FFT_SMALL),
//                                    nBlocksDone, nBlocks
//                                    );
//                        lsp_trace("dsp::fastconv_apply dst=%p, tmp=%p, conv1=%p, conv2=%p, rank=0x%x",
//                                pTargetPtr, vTempBuf, vTask, pConv, int(nRank));
//                        if (vBufferEnd < &pTargetPtr[1 << nRank])
//                        {
//                            lsp_error("Failed assertion: vBufferEnd >= &pTargetPtr[1 << nRank]");
//                        }
                        dsp::fastconv_apply(pTargetPtr, vTempBuf, vTask, pConv, nRank);

                        // Update pointers
                        pConv              += nFrameMax << 2;
                        pTargetPtr         += nFrameMax;
                        nBlocksDone        ++;

//                        if (pTargetPtr >= vBufferEnd)
//                            lsp_trace("pTargetPtr(%p) >= vBufferEnd(%p)", pTargetPtr, vBufferEnd);
                    }
                }
            }

            // Apply real-time part
            size_t to_do        = CONVOLVER_SMALL_FRM_SIZE - frame_off;
            if (to_do > count)
                to_do               = count;

            // Store input data into the frame
//            lsp_trace("dsp::copy dst=%p, src=%p, count=0x%x", &vFrame[nFrameSize], src, int(to_do));
            dsp::copy(&vFrame[nFrameSize], src, to_do);

            // Check that we are available to apply convolution via FFT
            if (to_do == CONVOLVER_SMALL_FRM_SIZE)
            {
//                lsp_trace("dsp::fastconv_parse_apply dst=%p, temp=%p, conv=%p, src=%p, count=0x%x",
//                        vBufferPtr, vTempBuf, vConv, &vFrame[nFrameSize], CONVOLVER_RANK_FFT_SMALL);
//                lsp_assert(vBufferEnd >= &vBufferPtr[1 << CONVOLVER_RANK_FFT_SMALL]);
                dsp::fastconv_parse_apply(vBufferPtr, vTempBuf, vConv, &vFrame[nFrameSize], CONVOLVER_RANK_FFT_SMALL);
            }
            else // We need to do direct convolution
            {
//                lsp_trace("dsp::convolve dst=%p, src=%p, conv=%p, conv_size=0x%x, count=0x%x",
//                        vBufferPtr, src, vConvFirst, int(nDirectSize), int(to_do));
//                lsp_assert(vBufferEnd >= &vBufferPtr[nDirectSize + to_do]);
                dsp::convolve(vBufferPtr, src, vConvFirst, nDirectSize, to_do);
            }

            // Update frame pointer
            nFrameSize         += to_do;
            if (nFrameSize >= nFrameMax)
            {
                // Update frame
//                lsp_trace("switch_frame");
//                lsp_trace("dsp::copy dst=%p src=%p, count=0x%x", vFrame-nFrameMax, vFrame, int(nFrameMax));
                dsp::copy(vFrame-nFrameMax, vFrame, nFrameMax);
//                lsp_trace("dsp::fill_zero dst=%p, count=0x%x", vFrame, int(nFrameMax));
                dsp::fill_zero(vFrame, nFrameMax);
                nFrameSize -= nFrameMax;
            }

            // Copy data to output and update buffer pointers
//            lsp_trace("dsp::copy dst=%p src=%p, count=0x%x", dst, vBufferPtr, int(to_do));
            dsp::copy(dst, vBufferPtr, to_do);
            vBufferPtr         += to_do;
            src                += to_do;
            dst                += to_do;
            count              -= to_do;

            // Check that buffer head is required to be moved
            if (vBufferPtr >= vBufferTail)
            {
                float *sptr = ((pTargetPtr == NULL) || (pTargetPtr >= vBufferPtr)) ?
                        vBufferPtr : pTargetPtr;

                size_t hist_size    = vBufferEnd - sptr;
                size_t free_size    = sptr - vBufferHead;

//                lsp_trace("dsp::move dst=%p src=%p, count=0x%x", vBufferHead, vBufferPtr, int(hist_size));
                dsp::move(vBufferHead, sptr, hist_size);
//                lsp_trace("dsp::fill_zero dst=%p, count=0x%x", &vBufferHead[hist_size], int(vBufferPtr - vBufferHead));
                dsp::fill_zero(&vBufferHead[hist_size], free_size);
                if (pTargetPtr != NULL)
                    pTargetPtr         -= free_size;
                vBufferPtr         -= free_size;

//                if ((vBufferPtr < vBufferHead) || (pTargetPtr < vBufferHead))
//                {
//                    lsp_error("Failed assertion: (vBufferPtr >= vBufferHead) && (pTargetPtr >= vBufferHead)");
//                }
            }
        }

//        lsp_trace("End process this=%p, dst=%p, src=%p, count=0x%x, vData=%p",
//                        this, dst, src, int(count), vData);
    }

} /* namespace lsp */
