#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stddef.h>

#include <core/types.h>
#include <core/dsp.h>

#define FFT_CONV_BUF_SIZE 16
#define FFT_CONV_BLOCK_SIZE 4

namespace fft_conv_test
{
    using namespace lsp;

    float convolution[] =
    {
        1.0, 2.0, 3.0, 4.0,
        5.0, 6.0, 7.0, 8.0
    };

    const float signal[] =
    {
        0, 0, 0, 0,
        1, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0
    };

    void dump_data(const float *data, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            if (i > 0)
                printf(", ");
            printf("%.3f", data[i]);
        }
        printf("\n");
    }

    float vFftRe[FFT_CONV_BUF_SIZE], vFftIm[FFT_CONV_BUF_SIZE], vConvRe[FFT_CONV_BUF_SIZE], vConvIm[FFT_CONV_BUF_SIZE], vBuffer[FFT_CONV_BUF_SIZE];
    size_t nBufSize, nConvSize, nFftRank;

    void process(float *out, const float *in, size_t samples)
    {
        while (samples > 0)
        {
            if (nBufSize >= nConvSize)
            {
                // Perform the direct FFT of the input signal
                dsp::fill_zero(&vFftRe[nConvSize], nConvSize);
                dsp::fill_zero(vFftIm, nConvSize*2);
                dsp::direct_fft(vFftRe, vFftIm, vFftRe, vFftIm, nFftRank + 1);

                // Perform convolution
                dsp::complex_mul(vFftRe, vFftIm, vFftRe, vFftIm, vConvRe, vConvIm, nConvSize*2);

                // Perform the reverse FFT
                dsp::reverse_fft(vFftRe, vFftIm, vFftRe, vFftIm, nFftRank + 1);

                // Apply previous convolution tail and update buffer
                dsp::add2(vFftRe, &vBuffer[nConvSize], nConvSize);
                dsp::copy(vBuffer, vFftRe, nConvSize * 2);

                // Reset the buffer size
                nBufSize    = 0;
            }

            // Determine number of samples to process
            size_t to_process = nConvSize - nBufSize;
            if (to_process > samples)
                to_process      = samples;

            // Push new data for processing and emit processed data
            dsp::copy(&vFftRe[nBufSize], in, to_process);
            dsp::copy(out, &vBuffer[nBufSize], to_process);

            // Update pointers and counters
            nBufSize       += to_process;
            out            += to_process;
            in             += to_process;
            samples        -= to_process;
        }
    }

    int test(int argc, const char **argv)
    {
        dsp::init();

        printf("Convolution: \n");
        dump_data(convolution, sizeof(convolution)/sizeof(float));

        // Init convolution
        nConvSize   = FFT_CONV_BUF_SIZE >> 1;
        nFftRank    = 3;
        nBufSize    = 0;
        dsp::copy(vConvRe, convolution, nConvSize);
        dsp::fill_zero(&vConvRe[nConvSize], nConvSize);
        dsp::fill_zero(&vConvIm[nConvSize], nConvSize*2);
        dsp::direct_fft(vConvRe, vConvIm, vConvRe, vConvIm, nFftRank + 1);

        printf("Convolution: \n");
        dump_data(vConvRe, nConvSize * 2);
        dump_data(vConvIm, nConvSize * 2);

        printf("Input signal: \n");
        dump_data(signal, sizeof(signal)/sizeof(float));

        // Clear buffers
        dsp::fill_zero(vFftRe, nConvSize*2);
        dsp::fill_zero(vFftIm, nConvSize*2);
        dsp::fill_zero(vBuffer, nConvSize*2);

        // Process with small blocks
        size_t count        = sizeof(signal)/sizeof(float);
        float *buf          = new float[count];
        dsp::copy(buf, signal, count);
        float *ptr          = buf;

        while (count > 0)
        {
            printf("Input block: \n");
            dump_data(ptr, FFT_CONV_BLOCK_SIZE);

            process(ptr, ptr, FFT_CONV_BLOCK_SIZE);

            printf("Output block: \n");
            dump_data(ptr, FFT_CONV_BLOCK_SIZE);

            ptr     += FFT_CONV_BLOCK_SIZE;
            count   -= FFT_CONV_BLOCK_SIZE;
        }

        delete [] buf;

        return 0;
    }
    
}
