#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>

#include <core/types.h>
#include <core/dsp.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace fft_speed_test
{
    using namespace lsp;

    void test_fft(float *fft_re, float *fft_im, const float *sig_re, const float *sig_im, size_t rank)
    {
        printf("Testing FFT of size %d (rank = %d)...\n", 1 << rank, int(rank));
        clock_t start = clock();
        float time = 0.0f;
        size_t iterations = 0;

        do
        {
            // Do 100 iterations
            for (size_t i=0; i<1000; ++i)
                dsp::direct_fft(fft_re, fft_im, sig_re, sig_im, rank);

            // Calculate statistics
            iterations     += 1000;
            time            = float(clock() - start) / float(CLOCKS_PER_SEC);
        } while (time < 30.0f);

        printf("Time = %.1f s, iterations = %d, performance = %.1f [i/s], average time = %.5f [ms/i]\n",
            time, int(iterations), iterations / time, (1000.0f * time) / iterations);
    }

    int test(int argc, const char **argv)
    {
        dsp::init();

        size_t fft_size = 1 << MAX_RANK;

        uint8_t *data   = new uint8_t[fft_size*4*sizeof(float) + DEFAULT_ALIGN];
        float *ptr      = reinterpret_cast<float *>(ALIGN_PTR(data, DEFAULT_ALIGN));

        float *sig_re   = ptr;
        ptr            += fft_size;
        float *sig_im   = ptr;
        ptr            += fft_size;
        float *fft_re   = ptr;
        ptr            += fft_size;
        float *fft_im   = ptr;
        ptr            += fft_size;

        for (size_t i=0; i < 1 << MAX_RANK; ++i)
        {
            sig_re[i]       = float(rand()) / RAND_MAX;
            sig_im[i]       = 0.0f;
        }

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
            test_fft(fft_re, fft_im, sig_re, sig_im, i);

//        delete [] sig_im;
//        delete [] sig_re;
        delete [] data;

        return 0;
    }
    
}
