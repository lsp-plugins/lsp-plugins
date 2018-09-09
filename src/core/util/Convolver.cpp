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

#if 1

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

    static void dump(const float *buf, size_t count, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
        printf(": ");

        while (count--)
            printf("%.4f ", *(buf++));

        printf("\n");
    }

    static void dump_fastconv(const float *conv, size_t rank, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
        printf(": ");

        size_t conv_size = 1 << rank;
        float *buf = new float[conv_size * 4];
        dsp::fill_zero(buf, conv_size * 4);
        buf[conv_size] = 1.0f;

        dsp::fastconv_parse_apply(buf, &buf[conv_size*2], conv, conv, rank);
        for(size_t i=0; i<conv_size; ++i)
            printf("%.4f ", buf[i]);

        delete [] buf;

        printf("\n");
    }

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
        allocate               += bins * data_buf_size * 4 + data_buf_size * 4; // Buffer for convolution tail

        float *fptr             = alloc_aligned<float>(vData, allocate);
        if (fptr == NULL)
            return false;

        destroy();
        dsp::fill_zero(fptr, allocate); // Drop all previously used data

        vBufferHead         = fptr;
        fptr               += bins * data_buf_size * 3;
        vBufferTail         = fptr;
        fptr               += (bins + 4) * data_buf_size;
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

        // Prepare first frame
        dsp::copy(vConvFirst, data, nDirectSize);
        dump(vConvFirst, CONVOLVER_SMALL_FRM_SIZE, "vConvFirst");

        // Calculate FFT of first bin
        dsp::fill_zero(vTempBuf, bin_size*2);
        dsp::copy(vTempBuf, data, nDirectSize);
        dsp::fastconv_parse(conv_re, vTempBuf, bin_rank);
        dump(conv_re, 1 << bin_rank, "conv_re[fft]");
        dump_fastconv(conv_re, bin_rank, "conv_re[img]")

        // Move pointers
        data               += frame_size;
        count              -= nDirectSize;
        conv_re            += bin_size * 2;

        while (count > 0)
        {
            size_t to_do        = (count > frame_size) ? frame_size : count;
            nFrameMax           = frame_size;

            // Calculate FFT
            dsp::fill_zero(vTempBuf, bin_size*2);
            dsp::copy(vTempBuf, data, to_do);
            dsp::fastconv_parse(conv_re, vTempBuf, bin_rank);

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
            size_t to_do        = (CONVOLVER_SMALL_FRM_SIZE - (nFrameSize & CONVOLVER_SMALL_FRM_MASK));
            if (to_do > count)
                to_do               = count;

            // Store input data into the frame
            dsp::copy(&vFrame[nFrameSize], src, to_do);

            // Check that we are available to apply convolution via FFT
            if (to_do == CONVOLVER_SMALL_FRM_SIZE)
                dsp::fastconv_parse_apply(vBufferPtr, vTempBuf, vConv, &vFrame[nFrameSize], CONVOLVER_RANK_FFT_SMALL);
            else if (to_do > 0) // We need to do direct convolution
            {
//                for (size_t i=0; i<to_do; ++i)
//                    dsp::scale_add3(&vBufferPtr[i], vConvFirst, src[i], nDirectSize);
                dsp::convolve(vBufferPtr, src, vConvFirst, nDirectSize, to_do);
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
                float *head     = &vFrame[(frame_id << CONVOLVER_RANK_FRM_SMALL)%nFrameMax];
                float *conv_re  = &vConv[CONVOLVER_SMALL_FFT_SIZE*2];

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

                        dsp::fastconv_parse_apply(vBufferPtr, vTempBuf, conv_re, &head[-(step >> 1)], rank);
                    }

                    conv_re            += step*2;
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
                        dsp::fastconv_apply(pTargetPtr, vTempBuf, vTask, pConv, rank);

                        // Update pointers
                        pConv              += step*2;
                        pTargetPtr         += nFrameMax;
                        nBlocksDone        ++;
                    }
                }

                if (nFrameSize >= nFrameMax)
                {
                    if (nBlocks > 0)
                    {
                        // Apply convolution
                        dsp::fastconv_parse(vTask, &head[-(step >> 1)], rank);
                        dsp::fastconv_apply(vBufferPtr, vTempBuf, vTask, conv_re, rank);

                        // Set number of blocks done
                        nBlocksDone     = 1;
                        pConv           = &conv_re[step * 2];
                        pTargetPtr      = &vBufferPtr[step>>1];
                    }

                    // Update frame
                    dsp::copy(&vFrame[-nFrameMax], vFrame, nFrameMax);
                    dsp::fill_zero(vFrame, nFrameMax);
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
//                lsp_trace("dsp::copy %p -> %p x %d", vBufferPtr, dst, int(to_do));
                dsp::copy(dst, vBufferPtr, to_do);
                vBufferPtr         += to_do;
                dst                += to_do;
                count              -= to_do;
            }
        }
    }

} /* namespace lsp */

#else

#define CONVOLVER_FRMRANK_SMALLEST      (CONVOLVER_RANK_SMALLEST-1)
#define CONVOLVER_BIN_SMALLEST          (1 << CONVOLVER_RANK_SMALLEST)
#define CONVOLVER_FRM_SMALLEST          (1 << CONVOLVER_FRMRANK_SMALLEST)
#define CONVOLVER_BIN_MIN               (1 << CONVOLVER_RANK_SMALLEST)
#define CONVOLVER_FRM_MIN               (1 << (CONVOLVER_RANK_SMALLEST-1))

namespace lsp
{
    Convolver::Convolver()
    {
        vSmallConv      = NULL;
        vSmallFft       = NULL;
        vFrame          = NULL;

        vFftBuf         = NULL;
        vTmpBuf         = NULL;

        vBufferHead     = NULL;
        pBufferPtr      = NULL;
        pBufferThresh   = NULL;
        pBufferEnd      = NULL;

        vTaskBuf        = NULL;
        pTaskPtr        = NULL;
        nTaskDone       = 0;

        vData           = NULL;

        nFrameSize      = 0;
        nFrameMax       = 0;
        nRank           = 0;
        nDump           = 0;
        fInitPhase      = 0.0f;

        for (size_t i=0; i<CONVOLVER_PLANS; ++i)
        {
            vPlans[i].nCount    = 0;
            vPlans[i].pConv     = NULL;
        }
    }

    Convolver::~Convolver()
    {
        destroy();
    }

    inline uint32_t checksum(const void *ptr, size_t count)
    {
        uint32_t result = 0;
        if (ptr == NULL)
            return result;

        const uint8_t *p = reinterpret_cast<const uint8_t *>(ptr);
        while (count--)
        {
            result      += *(p++);
            result       = (result << 1) | (result >> (sizeof(uint32_t) * 8 - 1));
        }
        return result;
    }

//    static void print_complex(const char *str, const float *ptr, size_t items)
//    {
//        printf("%s [RE]: ", str);
//        const float *p = ptr;
//        for (size_t i=0; i<items; ++i, p += 2)
//            printf("%.3f ", *p);
//
//        printf("\n%s [IM]: ", str);
//        p = &ptr[1];
//        for (size_t i=0; i<items; ++i, p += 2)
//            printf("%.3f ", *p);
//        printf("\n");
//    }
//
//    static void print_real(const char *str, const float *ptr, size_t items)
//    {
//        printf("%s: ", str);
//        const float *p = ptr;
//        for (size_t i=0; i<items; ++i, ++p)
//            printf("%.3f ", *p);
//        printf("\n");
//    }

    bool Convolver::init(const float *data, size_t count, size_t rank, float phase)
    {
        // Check arguments
        destroy();
        if (count <= 0)
            return true;

        // Determine number of buffers
        if (rank < CONVOLVER_RANK_MIN)
            rank    = CONVOLVER_RANK_MIN;
        else if (rank >= CONVOLVER_RANK_MAX)
            rank    = CONVOLVER_RANK_MAX;

        // Estimate plan
        ssize_t c_bins      = (count - 1) >> CONVOLVER_FRMRANK_SMALLEST;
        size_t conv_size    = (c_bins + 1) << CONVOLVER_FRMRANK_SMALLEST;
        lsp_trace("Processing data size=%d, samples=%d, minimal bin size=%d, minimal bins=%d ",
            int(count), int(count), int(CONVOLVER_FRM_SMALLEST), int(c_bins+1));
        for (size_t i=0; i < CONVOLVER_PLANS; ++i)
        {
            vPlans[i].nCount    = 0;
            vPlans[i].pConv     = NULL;
        }

        vPlans[0].nCount    = 0;

        for (size_t i=CONVOLVER_RANK_SMALLEST; i<=rank; ++i)
        {
            ssize_t n           = 1 << (i - CONVOLVER_RANK_SMALLEST);
            if (n > c_bins)
                break;
            vPlans[i-CONVOLVER_RANK_SMALLEST].nCount    ++;
            c_bins             -= n;
        }

        for (size_t i=CONVOLVER_RANK_SMALLEST; i<rank; ++i, c_bins >>= 1)
        {
            if (!(c_bins & 1))
                continue;
            vPlans[i-CONVOLVER_RANK_SMALLEST].nCount    ++;
        }

        lsp_assert(c_bins >= 0);
        vPlans[rank-CONVOLVER_RANK_SMALLEST].nCount    += c_bins;

        // Adjust rank
        while ((rank >= CONVOLVER_RANK_SMALLEST) && (vPlans[rank-CONVOLVER_RANK_SMALLEST].nCount <= 0))
            rank --;
        nRank               = rank;
        nFrameMax           = (nRank >= CONVOLVER_RANK_SMALLEST) ? 1 << (nRank - 1) : CONVOLVER_FRM_SMALLEST;
        nFrameSize          = (size_t(phase * nFrameMax) % nFrameMax) & (~(CONVOLVER_FRM_SMALLEST-1));

        lsp_trace("Initial frame size: %d", int(nFrameSize));

        #ifdef LSP_TRACE
        lsp_trace("Estimated plan for %d samples: ", int(count));
        for (size_t i=0; i<CONVOLVER_PLANS; ++i)
        {
            int count = vPlans[i].nCount;
            if (i == 0)
                count ++;
            lsp_trace("  %d bins of %d size (%d samples)",
                count, int(1 << (i + CONVOLVER_RANK_SMALLEST)), int(1 << (i + CONVOLVER_RANK_SMALLEST - 1)));
        }
        #endif /* LSP_TRACE */

        // We have estimated plan, now we can allocate data
        size_t conv_buf_size    = 0; // 2 floats per sample
        for (size_t i=CONVOLVER_RANK_SMALLEST; i<=rank; ++i)
            conv_buf_size          += (vPlans[i - CONVOLVER_RANK_SMALLEST].nCount << (i + 1)); // 2 floats per sample

        lsp_trace("Convolution FFT image size = %d floats", int(conv_buf_size));

        size_t fft_buf_size     = 1 << (rank + 1);
        lsp_trace("FFT buffer size = %d floats", int(fft_buf_size));

        size_t allocate         = CONVOLVER_FRM_SMALLEST + CONVOLVER_BIN_SMALLEST*2; // First realtime frame/bin
        allocate               += conv_buf_size;                    // Buffer required by FFT image
        allocate               += nFrameMax * 2;                    // Buffer required for current frame and history
        allocate               += conv_size*4 + (1 << (rank+2));    // Buffer for convolution result
        allocate               += fft_buf_size;                     // Buffer for FFT calculation
        allocate               += fft_buf_size;                     // Temporary buffer
        allocate               += fft_buf_size;                     // Buffer required by FFT task image

        // Allocate data and fill with zeros
        vData                   = new uint8_t[allocate * sizeof(float) + DEFAULT_ALIGN];
        if (vData == NULL)
            return false;
        float *fptr         = reinterpret_cast<float *>(ALIGN_PTR(vData, DEFAULT_ALIGN));
        dsp::fill_zero(fptr, allocate);

        // Initialize pointers
        vSmallConv              = fptr;
        fptr                   += CONVOLVER_FRM_SMALLEST;
        vSmallFft               = fptr;
        fptr                   += CONVOLVER_BIN_SMALLEST*2;

        for (size_t i=CONVOLVER_RANK_SMALLEST; i<=rank; ++i)
        {
            vPlans[i-CONVOLVER_RANK_SMALLEST].pConv      = fptr;
            fptr               += (vPlans[i - CONVOLVER_RANK_SMALLEST].nCount << (i + 1)); // 2 floats per sample
        }

        // Initialize buffer
        vBufferHead         = fptr;
        pBufferPtr          = fptr;
        pTaskPtr            = fptr;
        fptr               += conv_size*3;
        pBufferThresh       = fptr;
        fptr               += conv_size + (1 << (rank+2));
        pBufferEnd          = fptr;

        // Two frames, store pointer to the second frame
        fptr               += nFrameMax;
        vFrame              = fptr;
        fptr               += nFrameMax;

        vFftBuf             = fptr;
        fptr               += fft_buf_size;
        vTmpBuf             = fptr;
        fptr               += fft_buf_size;
        vTaskBuf            = fptr;
        fptr               += fft_buf_size;

        nTaskDone           = -1;
        nDump               = 0;
        fInitPhase          = phase;

        // Prepare convolution data
        const float *conv   = data;

        size_t to_do        = (count > CONVOLVER_FRM_SMALLEST) ? CONVOLVER_FRM_SMALLEST : count;

        dsp::copy(vSmallConv, conv, to_do);
        dsp::fastconv_parse(vSmallFft, conv, CONVOLVER_RANK_SMALLEST);

        conv               += CONVOLVER_FRM_SMALLEST;
        count              -= to_do;

        for (size_t r=CONVOLVER_RANK_SMALLEST; r<=rank; ++r)
        {
            float *fft          = vPlans[r-CONVOLVER_RANK_SMALLEST].pConv;
            size_t n            = vPlans[r-CONVOLVER_RANK_SMALLEST].nCount;

            while (n--)
            {
                to_do               = (count > size_t(1 << (r-1))) ? (1 << (r-1)) : count;
                dsp::fastconv_parse(fft, conv, r);

                count              -= to_do;
                conv               += 1 << (r-1);
                fft                += 1 << (r+1); // 2 floats
            }

        }

        // Calculate checksums
        for (size_t r=CONVOLVER_RANK_SMALLEST; r<=rank; ++r)
        {
            plan_t *plan        = &vPlans[r-CONVOLVER_RANK_SMALLEST];
            plan->nCkSum        = checksum(plan->pConv, (plan->nCount << (r + 1)) * sizeof(float));
        }

        lsp_trace("fptr = %p, end = %p", fptr, &vData[allocate * sizeof(float) + DEFAULT_ALIGN]);
        lsp_assert(reinterpret_cast<uint8_t *>(fptr) <= &vData[allocate * sizeof(float) + DEFAULT_ALIGN]);

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

        nDump += count;
        if (nDump >= 5 * 48000)
        {
            printf("--------------------\n");
            printf("vSmallConv      = %p\n", vSmallConv);
            printf("vSmallFft       = %p\n", vSmallFft);
            printf("vFrame          = %p\n", vFrame);
            printf("vFftBuf         = %p\n", vFftBuf);
            printf("vTmpBuf         = %p\n", vTmpBuf);
            printf("vBufferHead     = %p\n", vBufferHead);
            printf("pBufferPtr      = %p\n", pBufferPtr);
            printf("pBufferThresh   = %p\n", pBufferThresh);
            printf("pBufferEnd      = %p\n", pBufferEnd);
            printf("vTaskBuf        = %p\n", vTaskBuf);
            printf("pTaskPtr        = %p\n", pTaskPtr);
            printf("nTaskDone       = %d\n", int(nTaskDone));
            printf("vData           = %p\n", vData);
            printf("nFrameSize      = %d\n", int(nFrameSize));
            printf("nFrameMax       = %d\n", int(nFrameMax));
            printf("nRank           = %d\n", int(nRank));
            printf("nDump           = %d\n", int(nDump));
            printf("fInitPhase      = %f\n", fInitPhase);

            for (size_t i=0; i<CONVOLVER_PLANS; ++i)
            {
                plan_t *plan        = &vPlans[i];
                uint32_t cksum      = checksum(plan->pConv, (plan->nCount << (i + CONVOLVER_RANK_SMALLEST + 1)) * sizeof(float));
                printf("  Plan %d nCount=%d, pConv=%p, nCkSum=%lx, check=%lx\n",
                        int(i), int(vPlans[i].nCount), vPlans[i].pConv, long(plan->nCkSum), long(cksum));
            }
            printf("--------------------\n");

            nDump   -= 5 * 48000;
        }

        // Check input data
        for (size_t i=0; i<count; ++i)
        {
            if ((src[i] > 100.0f) || (src[i] < -100.0f))
                printf("Unexpected input: sample %d, value %f\n", int(i), src[i]);
        }

        while (count > 0)
        {
            lsp_assert(nFrameSize <= nFrameMax);

            // Check if we need to execute tasks first
            if ((!(nFrameSize & (CONVOLVER_FRM_SMALLEST-1))) && (nRank >= CONVOLVER_RANK_SMALLEST))
            {
                // Here, rank == nRank
                plan_t *plan    = &vPlans[nRank - CONVOLVER_RANK_SMALLEST];
                if ((plan->nCount > 0) && (nTaskDone < plan->nCount))
                {
                    size_t bin_size     = 1 << nRank;

                    size_t tgt_block    = (nFrameSize * (plan->nCount + 1)) / nFrameMax;
                    if (tgt_block < 1)
                        tgt_block           = 1;
                    else if (tgt_block > plan->nCount)
                        tgt_block           = plan->nCount;

                    float  *fft         = &plan->pConv[nTaskDone << (nRank + 1)];
                    while (nTaskDone < tgt_block)
                    {
                        lsp_assert((pTaskPtr + bin_size) <= pBufferEnd);
                        dsp::fastconv_apply(pTaskPtr, vTmpBuf, vTaskBuf, fft, nRank);

                        nTaskDone          ++;
                        fft                += bin_size << 1;
                        pTaskPtr           += bin_size >> 1;
                    }
                }
            }

            // Determine how many samples to process
            size_t to_do        = CONVOLVER_FRM_SMALLEST - (nFrameSize & (CONVOLVER_FRM_SMALLEST-1));
            if (to_do > count)
                to_do               = count;

            // Copy data to the frame buffer
            float *frm          = &vFrame[nFrameSize];
            dsp::copy(frm, src, to_do);

            for (size_t i=0; i<count; ++i)

            // Do first-order convolution
            if (to_do == CONVOLVER_FRM_SMALLEST)
                dsp::fastconv_parse_apply(pBufferPtr, vTmpBuf, vSmallFft, frm, CONVOLVER_RANK_SMALLEST);
            else
            {
                lsp_trace("doing raw convolution");
                for (size_t i=0; i<to_do; ++i)
                    dsp::scale_add3(pBufferPtr, vSmallConv, src[i], CONVOLVER_FRM_SMALLEST);
            }

            // Copy rendering buffer to output and update pointers
            dsp::copy(dst, pBufferPtr, to_do);

            for (size_t i=0; i<to_do; ++i)
            {
                if ((dst[i] > 100.0f) || (dst[i] < -100.0f))
                    printf("Unexpected output: sample %d, value %f\n", int(i), dst[i]);
            }

            // Update frame size and source pointer
            nFrameSize         += to_do;
            pBufferPtr         += to_do;
            dst                += to_do;
            src                += to_do;
            count              -= to_do;

            lsp_assert(nFrameSize <= nFrameMax);

            // Check that we have processed the smallest frame
            if (!(nFrameSize & (CONVOLVER_FRM_SMALLEST-1)))
            {
                // Move the frame back
                if (nRank >= CONVOLVER_RANK_SMALLEST)
                {
                    size_t mask         = (nFrameSize-1) >> CONVOLVER_FRMRANK_SMALLEST;
                    mask                = mask ^ (mask - 1);
                    size_t bin_size     = CONVOLVER_BIN_SMALLEST;
                    float *bptr         = pBufferPtr;

                    // We need to apply all FFT plans for the future samples
                    for (size_t rank = CONVOLVER_RANK_SMALLEST; rank<nRank; ++rank)
                    {
                        plan_t *plan        = &vPlans[rank - CONVOLVER_RANK_SMALLEST];

                        if ((mask & 1) && (plan->nCount > 0))
                        {
                            float  *fft         = plan->pConv;

                            if (plan->nCount > 1)
                            {
                                dsp::fastconv_parse(vFftBuf, frm, rank);

                                for (ssize_t j=plan->nCount; j>0; --j)
                                {
                                    lsp_assert((bptr + bin_size) <= pBufferEnd);
                                    dsp::fastconv_apply(bptr, vTmpBuf, vFftBuf, fft, rank);

                                    fft            += bin_size << 1;
                                    bptr           += bin_size >> 1;
                                }
                                bptr           -= bin_size >> 1;
                            }
                            else
                            {
                                // Parse and apply
                                lsp_assert((bptr + bin_size) <= pBufferEnd);
                                dsp::fastconv_parse_apply(bptr, vTmpBuf, fft, frm, rank);
                                fft            += bin_size << 1;
                            }
                        } // if

                        frm            -= bin_size >> 1; // Move the frame back
                        bin_size      <<= 1;
                        mask          >>= 1;
                    } // Rank


                    // Here, rank == nRank, create task if possible
                    if (mask & 1)
                    {
                        dsp::fastconv_parse(vTaskBuf, frm, nRank);
                        pTaskPtr    = bptr;
                        nTaskDone   = 0;

                        lsp_assert((pTaskPtr + bin_size) <= pBufferEnd);
                    }
                }

                if (nFrameSize >= nFrameMax)
                {
                    // Shift delay buffer
                    dsp::move(&vFrame[-nFrameMax], vFrame, nFrameMax);  // switch frame
                    dsp::fill_zero(vFrame, nFrameMax);                  // clear frame
                    nFrameSize -= nFrameMax;

                    // Check that buffer head is not out of range
                    if (pBufferPtr >= pBufferThresh)
                    {
                        lsp_trace("Buffer shift end=%p, head=%p, ptr=%p, thresh=%p",
                                pBufferEnd, vBufferHead, pBufferPtr, pBufferThresh);
                        size_t buf_size     = pBufferEnd - vBufferHead;
                        size_t hist_size    = pBufferEnd - pBufferPtr;
                        lsp_trace("buf_size=%d, hist_size=%d", int(buf_size), int(hist_size));
                        dsp::move(vBufferHead, pBufferPtr, hist_size);
                        dsp::fill_zero(&vBufferHead[hist_size], buf_size - hist_size);
                        lsp_trace("new task_ptr=%p buf_ptr=%p", pTaskPtr, pBufferPtr);
                        pTaskPtr           -= (pBufferPtr - vBufferHead); // Move task pointer
                        pBufferPtr          = vBufferHead;
                    }
                }
            } // Frame size
        } // while (count > 0)
    }

} /* namespace lsp */

#endif
