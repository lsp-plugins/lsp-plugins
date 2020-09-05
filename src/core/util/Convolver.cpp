/*
 * Convolver.cpp
 *
 *  Created on: 29 янв. 2016 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <core/debug.h>
#include <core/util/Convolver.h>
#include <core/sugar.h>
#include <stdarg.h>

#define CONVOLVER_MIN_CONV_BUF_SIZE         (1 << (CONVOLVER_RANK_MIN))
#define CONVOLVER_MIN_DATA_BUF_SIZE         (1 << (CONVOLVER_RANK_MIN - 1))
#define CONVOLVER_MIN_FFT_BUF_SIZE          (1 << (CONVOLVER_RANK_MIN + 1))

#define CONVOLVER_DATA_ALIGN                0x40

namespace lsp
{
    Convolver::Convolver()
    {
        construct();
    }

    Convolver::~Convolver()
    {
        destroy();
    }

    void Convolver::construct()
    {
        vDataBuffer         = NULL;
        vFrame              = NULL;
        vConvBuffer         = NULL;
        vTaskData           = NULL;
        vDirectData         = NULL;
        vConvData           = NULL;

        nDataBufferSize     = 0;
        nDirectSize         = 0;
        nFrameSize          = 0;
        nFrameOff           = 0;
        nConvSize           = 0;
        nLevels             = 0;
        nBlocks             = 0;
        nBlocksDone         = 0;
        nRank               = 0;

        vData               = NULL;
    }

    void Convolver::destroy()
    {
        free_aligned(vData);
        construct();
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

        // Determine number of buffers
        rank                    = lsp_limit(ssize_t(rank), CONVOLVER_RANK_MIN, CONVOLVER_RANK_MAX);

        // Determine size of buffer
        size_t data_buf_size    = 1 << (rank - 1);
        size_t fft_buf_size     = 1 << (rank + 1);
        size_t direct_buf_size  = lsp_max(CONVOLVER_MIN_DATA_BUF_SIZE, int(CONVOLVER_DATA_ALIGN/sizeof(float)));
        size_t bins             = (count + data_buf_size - 1) >> (rank - 1);

        size_t allocate         = (bins + 1) * data_buf_size;       // Size of data buffer (convolutio tail)
        allocate               += data_buf_size * 2;                // Input data frame
        allocate               += fft_buf_size;                     // Convolution buffer
        allocate               += fft_buf_size;                     // Task data for tail convolution
        allocate               += bins * fft_buf_size;              // FFT convolution data
        allocate               += direct_buf_size;                  // Direct convolution data

        // Allocate buffer and clear
        uint8_t *pdata          = NULL;
        float *fptr             = alloc_aligned<float>(pdata, allocate, CONVOLVER_DATA_ALIGN);
        lsp_guard_assert(float *save = fptr);
        if (fptr == NULL)
            return false;

        destroy();
        vData                   = pdata;
        dsp::fill_zero(fptr, allocate);                             // Cleanup all buffer data

        // Perform initialization
        vDataBuffer             = fptr;
        fptr                   += (bins + 1) * data_buf_size;

        // Input data frame
        fptr                   += data_buf_size;
        vFrame                  = fptr;
        fptr                   += data_buf_size;

        // Convolution buffer
        vConvBuffer             = fptr;
        fptr                   += fft_buf_size;

        // Task data for tail convolution
        vTaskData               = fptr;
        fptr                   += fft_buf_size;

        // FFT convolution data
        vConvData               = fptr;
        fptr                   += bins * fft_buf_size;

        // Direct convolution data
        vDirectData             = fptr;
        fptr                   += direct_buf_size;

        // Validate allocation
        lsp_assert(fptr == &save[allocate]);

        // Initialize simple values
        nDataBufferSize         = (bins + 1) * data_buf_size;
        nFrameSize              = data_buf_size;
        nFrameOff               = size_t(phase * nFrameSize) % nFrameSize;
        nDirectSize             = lsp_min(count, size_t(CONVOLVER_MIN_DATA_BUF_SIZE));
        nConvSize               = count;

        /* Calculate convolutions

            Conv buffer layout:
            +---+---+------+------------+------------------------+
            |FFT|FFT|FFT x2|   FFT x4   |       FFT x8           |  . . .
            +---+---+------+------------+------------------------+
         */

        float *conv             = vConvData;
        size_t brank            = CONVOLVER_RANK_MIN;

        // Process direct convolution data
        dsp::copy(vDirectData, data, nDirectSize);
        dsp::fill_zero(vConvBuffer, fft_buf_size);
        dsp::copy(vConvBuffer, data, nDirectSize);
        dsp::fastconv_parse(conv, vConvBuffer, brank);

        data                   += nDirectSize;
        conv                   += (1 << (brank + 1));
        count                  -= nDirectSize;

        // Prepare raising levels
        nLevels                 = 0;
        for (; (count > 0) && (brank < rank); ++brank)
        {
            size_t n                = lsp_min(count, size_t(1 << (brank - 1)));

            // Prepare raising convolution
            dsp::fill_zero(vConvBuffer, fft_buf_size);
            dsp::copy(vConvBuffer, data, n);
            dsp::fastconv_parse(conv, vConvBuffer, brank);

            data                   += n;
            conv                   += (1 << (brank + 1));
            count                  -= n;
            nLevels                 ++;             // Increment number of raising levels
        }

        // Prepare constant part
        nBlocks                 = 0;
        while (count > 0)
        {
            size_t n            = lsp_min(count, data_buf_size);

            // Prepare raising convolution
            dsp::fill_zero(vConvBuffer, fft_buf_size);
            dsp::copy(vConvBuffer, data, n);
            dsp::fastconv_parse(conv, vConvBuffer, rank);

            data                   += n;
            conv                   += fft_buf_size;
            count                  -= n;
            nBlocks                 ++;             // Increment number of constant-size blocks
        }

        lsp_assert(conv <= vDirectData);

        nBlocksDone             = nBlocks;
        ssize_t steps           = data_buf_size >> (CONVOLVER_RANK_MIN - 1);
        if (steps <= 1)
        {
            nBlkInit                = nBlocks;
            fBlkCoef                = 0.0f;
        }
        else
        {
            nBlkInit                = 1;
            fBlkCoef                = (float(nBlocks) + 1e-3f) / (float(steps) - 1.0f);
        }

        nRank                   = rank;

        return true;
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
            size_t sub_off      = nFrameOff & (CONVOLVER_MIN_DATA_BUF_SIZE - 1);        // Determine sub-offset in the frame

            // We are strictly at the boundary of the frame?
            if (sub_off == 0)
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

                size_t sub_id       = nFrameOff >> (CONVOLVER_RANK_MIN - 1);
                size_t mask         = ((sub_id-1) ^ sub_id);
                size_t rank         = CONVOLVER_RANK_MIN;
                const float *conv   = &vConvData[CONVOLVER_MIN_FFT_BUF_SIZE];

                // Apply convolution with raising level
                for (size_t i=0; i<nLevels; ++i)
                {
                    if (mask & 1)
                    {
                        const float *fptr   = vFrame + nFrameOff - (1 << (rank - 1));
                        dsp::fastconv_parse_apply(&vDataBuffer[nFrameOff], vConvBuffer, conv, fptr, rank);
                    }

                    ++rank;
                    conv               += (1 << rank);
                    mask              >>= 1;
                }

                // Need to apply long tail?
                if (nBlocks > 0)
                {
                    // Need to reset tasks?
                    if (mask & 1)
                    {
                        dsp::fastconv_parse(vTaskData, vFrame - nFrameSize, nRank);
                        nBlocksDone         = 0;
                    }

                    // Need to execute tasks?
                    size_t target_blk   = lsp_min(nBlocks, size_t(nBlkInit + fBlkCoef * sub_id));
                    size_t fft_step     = 1 << (nRank + 1);
                    conv                = &vConvData[(nBlocksDone + 1) * fft_step];     // Source convolution
                    float *xdst         = &vDataBuffer[nBlocksDone << (nRank - 1)];     // Offset to store the block

                    for ( ; nBlocksDone < target_blk; ++nBlocksDone)
                    {
                        dsp::fastconv_apply(xdst, vConvBuffer, conv, vTaskData, rank);
                        xdst               += (fft_step >> 2);
                        conv               += fft_step;
                    }
                }
            }

            // Apply direct convolution
            size_t to_do        = lsp_min(count, size_t(CONVOLVER_MIN_DATA_BUF_SIZE - sub_off));
            dsp::copy(&vFrame[nFrameOff], src, to_do);      // Store data to frame
            if (to_do == CONVOLVER_MIN_DATA_BUF_SIZE)
                dsp::fastconv_parse_apply(&vDataBuffer[nFrameOff], vConvBuffer, vConvData, src, CONVOLVER_RANK_MIN);
            else
                dsp::convolve(&vDataBuffer[nFrameOff], src, vDirectData, nDirectSize, to_do);
            dsp::copy(dst, &vDataBuffer[nFrameOff], to_do); // Output result

            // Update counters/pointers
            nFrameOff          += to_do;
            src                += to_do;
            dst                += to_do;
            count              -= to_do;

            // Check that we are out of the frame and need to shift the data and convolution tail
            if (nFrameOff >= nFrameSize)
            {
                nFrameOff          -= nFrameSize;
                dsp::move(vFrame - nFrameSize, vFrame, nFrameSize);
                dsp::move(vDataBuffer, &vDataBuffer[nFrameSize], nDataBufferSize - nFrameSize);
                dsp::fill_zero(&vDataBuffer[nDataBufferSize - nFrameSize], nFrameSize);
            }
        }
    }

    void Convolver::dump(IStateDumper *v) const
    {
        v->write("pDataBuffer", vDataBuffer);
        v->write("vFrame", vFrame);
        v->write("vConvBuffer", vConvBuffer);
        v->write("vTaskData", vTaskData);
        v->write("vConvData", vConvData);
        v->write("vDirectData", vDirectData);

        v->write("nDataBufferSize", nDataBufferSize);
        v->write("nDirectSize", nDirectSize);
        v->write("nFrameSize", nFrameSize);
        v->write("nFrameOff", nFrameOff);
        v->write("nConvSize", nConvSize);
        v->write("nLevels", nLevels);
        v->write("nBlocks", nBlocks);
        v->write("nBlocksDone", nBlocksDone);
        v->write("nRank", nRank);
        v->write("nBlkInit", nBlkInit);
        v->write("fBlkCoef", fBlkCoef);

        v->write("vData", vData);
    }

} /* namespace lsp */
