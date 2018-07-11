#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>

#include <core/types.h>
#include <core/dsp.h>

#define MIN_RANK 10
#define MAX_RANK 16

namespace lsp
{
    namespace native
    {
        void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        void conv_direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        void packed_direct_fft(float *dst, const float *src, size_t rank);
    }

    namespace sse
    {
        void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        void conv_direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        void packed_direct_fft(float *dst, const float *src, size_t rank);
    }
}

namespace fft_speed_test
{
    using namespace lsp;

    typedef void (* fft_unpacked_t) (float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
    typedef void (* fft_packed_t) (float *dst, const float *src, size_t rank);
    typedef void (* fft_conv_t) (float *dst, const float *src, size_t rank);

    void test_fft(float *fft_re, float *fft_im, const float *sig_re, const float *sig_im, size_t rank, fft_unpacked_t fft, const char *label)
    {
        printf("Testing %s of size %d (rank = %d)...\n", label, 1 << rank, int(rank));
        clock_t start = clock();
        float time = 0.0f;
        size_t iterations = 0;

        do
        {
            // Do 100 iterations
            for (size_t i=0; i<1000; ++i)
                fft(fft_re, fft_im, sig_re, sig_im, rank);

            // Calculate statistics
            iterations     += 1000;
            time            = float(clock() - start) / float(CLOCKS_PER_SEC);
        } while (time < 30.0f);

        printf("Time = %.1f s, iterations = %d, performance = %.1f [i/s], average time = %.5f [ms/i]\n",
            time, int(iterations), iterations / time, (1000.0f * time) / iterations);
    }

    void test_fft(float *dst, const float *src, size_t rank, fft_packed_t fft, const char *label)
    {
        printf("Testing %s of size %d (rank = %d)...\n", label, 1 << rank, int(rank));
        clock_t start = clock();
        float time = 0.0f;
        size_t iterations = 0;

        do
        {
            // Do 100 iterations
            for (size_t i=0; i<1000; ++i)
                fft(dst, src, rank);

            // Calculate statistics
            iterations     += 1000;
            time            = float(clock() - start) / float(CLOCKS_PER_SEC);
        } while (time < 30.0f);

        printf("Time = %.1f s, iterations = %d, performance = %.1f [i/s], average time = %.5f [ms/i]\n",
            time, int(iterations), iterations / time, (1000.0f * time) / iterations);
    }

    void test_conv_fft(float *dst, const float *src, size_t rank, fft_conv_t fft, const char *label)
    {
        printf("Testing %s of size %d (rank = %d)...\n", label, 1 << rank, int(rank));
        clock_t start = clock();
        float time = 0.0f;
        size_t iterations = 0;

        do
        {
            // Do 100 iterations
            for (size_t i=0; i<1000; ++i)
                fft(dst, src, rank);

            // Calculate statistics
            iterations     += 1000;
            time            = float(clock() - start) / float(CLOCKS_PER_SEC);
        } while (time < 30.0f);

        printf("Time = %.1f s, iterations = %d, performance = %.1f [i/s], average time = %.5f [ms/i]\n",
            time, int(iterations), iterations / time, (1000.0f * time) / iterations);
    }

    int test(int argc, const char **argv)
    {
        dsp_context_t ctx;
        dsp::init();
        dsp::start(&ctx);

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

        for (size_t i=0; i < (1 << MAX_RANK); ++i)
        {
            sig_re[i]       = float(rand()) / RAND_MAX;
            sig_im[i]       = 0.0f;
        }

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
//            test_fft(fft_re, fft_im, sig_re, sig_im, i, native::direct_fft, "Native Unpacked FFT");
//            test_fft(fft_re, sig_re, i, native::packed_direct_fft, "Native Packed FFT");
//            test_conv_fft(fft_re, sig_re, i, native::conv_direct_fft, "Native Convolution FFT copy");

            test_fft(fft_re, fft_im, sig_re, sig_im, i, sse::direct_fft, "SSE Unpacked FFT copy");
//            test_fft(fft_re, fft_im, fft_re, fft_im, i, sse::direct_fft, "SSE Unpacked FFT self");
//
            test_fft(fft_re, sig_re, i, sse::packed_direct_fft, "SSE Packed FFT copy");
            test_conv_fft(fft_re, sig_re, i, sse::conv_direct_fft, "SSE Convolution FFT copy");
//            test_fft(fft_re, fft_re, i, sse::packed_direct_fft, "SSE Packed FFT self");
            printf("\n");
        }

        delete [] data;

        dsp::finish(&ctx);

        return 0;
    }
    
}

#undef MIN_RANK
#undef MAX_RANK
