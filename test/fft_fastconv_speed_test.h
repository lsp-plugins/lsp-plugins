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
        void reverse_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        void complex_mul(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count);
        void add2(float *dst, const float *src, size_t count);

        void conv_direct_fft(float *dst, const float *src, size_t rank);
        void packed_complex_mul(float *dst, const float *src1, const float *src2, size_t count);
        void packed_reverse_fft(float *dst, const float *src, size_t rank);
        void packed_complex_add_to_real(float *dst, const float *src, size_t count);

        void fastconv_parse(float *dst, const float *src, size_t rank);
        void fastconv_apply(float *dst, float *tmp, const float *c1, const float *c2, size_t rank);
        void fastconv_parse_apply(float *dst, float *tmp, const float *c, const float *src, size_t rank);


    }

    namespace sse
    {
        void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        void reverse_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        void complex_mul(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count);
        void add2(float *dst, const float *src, size_t count);

        void conv_direct_fft(float *dst, const float *src, size_t rank);
        void packed_complex_mul(float *dst, const float *src1, const float *src2, size_t count);
        void packed_reverse_fft(float *dst, const float *src, size_t rank);
        void packed_complex_add_to_real(float *dst, const float *src, size_t count);

        void fastconv_parse(float *dst, const float *src, size_t rank);
        void fastconv_apply(float *dst, float *tmp, const float *c1, const float *c2, size_t rank);
        void fastconv_parse_apply(float *dst, float *tmp, const float *c, const float *src, size_t rank);


    }
}

namespace fft_fastconv_speed_test
{
    using namespace lsp;

    void test_old_native_convolution(float *out, float *tmp, float *tmp2, float *conv, const float *in, const float *cv, size_t rank, const char *label)
    {
        float time = 0.0f;
        size_t iterations = 0;
        size_t bin_size = 1 << rank;

        // Prepare data
        dsp::fill_zero(out, bin_size);
        native::direct_fft(conv, &conv[bin_size], cv, &cv[bin_size], rank);

        // Start test
        printf("Testing %s of size %d (rank = %d)...\n", label, 1 << rank, int(rank));
        clock_t start = clock();

        do
        {
            // Do 1000 iterations
            for (size_t i=0; i<1000; ++i)
            {
                native::direct_fft(tmp, &tmp[bin_size], in, &in[bin_size], rank);
                native::complex_mul(tmp, &tmp[bin_size], tmp, &tmp[bin_size], conv, &conv[bin_size], bin_size);
                native::reverse_fft(tmp2, &tmp2[bin_size], tmp, &tmp[bin_size], rank);
                native::add2(out, tmp2, bin_size);
            }

            // Calculate statistics
            iterations     += 1000;
            time            = float(clock() - start) / float(CLOCKS_PER_SEC);
        } while (time < 30.0f);

        printf("Time = %.1f s, iterations = %d, performance = %.1f [i/s], average time = %.5f [ms/i]\n",
            time, int(iterations), iterations / time, (1000.0f * time) / iterations);
    }

    void test_old_sse_convolution(float *out, float *tmp, float *tmp2, float *conv, const float *in, const float *cv, size_t rank, const char *label)
    {
        float time = 0.0f;
        size_t iterations = 0;
        size_t bin_size = 1 << rank;

        // Prepare data
        dsp::fill_zero(out, bin_size);
        sse::direct_fft(conv, &conv[bin_size], cv, &cv[bin_size], rank);

        // Start test
        printf("Testing %s of size %d (rank = %d)...\n", label, 1 << rank, int(rank));
        clock_t start = clock();

        do
        {
            // Do 1000 iterations
            for (size_t i=0; i<1000; ++i)
            {
                sse::direct_fft(tmp, &tmp[bin_size], in, &in[bin_size], rank);
                sse::complex_mul(tmp, &tmp[bin_size], tmp, &tmp[bin_size], conv, &conv[bin_size], bin_size);
                sse::reverse_fft(tmp2, &tmp2[bin_size], tmp, &tmp[bin_size], rank);
                sse::add2(out, tmp2, bin_size);
            }

            // Calculate statistics
            iterations     += 1000;
            time            = float(clock() - start) / float(CLOCKS_PER_SEC);
        } while (time < 30.0f);

        printf("Time = %.1f s, iterations = %d, performance = %.1f [i/s], average time = %.5f [ms/i]\n",
            time, int(iterations), iterations / time, (1000.0f * time) / iterations);
    }

    void test_fft_native_convolution(float *out, float *tmp, float *tmp2, float *conv, const float *in, const float *cv, size_t rank, const char *label)
    {
        float time = 0.0f;
        size_t iterations = 0;
        size_t bin_size = 1 << rank;

        // Prepare data
        dsp::fill_zero(out, bin_size);
        native::conv_direct_fft(conv, cv, rank);

        // Start test
        printf("Testing %s of size %d (rank = %d)...\n", label, 1 << rank, int(rank));
        clock_t start = clock();

        do
        {
            // Do 1000 iterations
            for (size_t i=0; i<1000; ++i)
            {
                native::conv_direct_fft(tmp, in, rank);
                native::packed_complex_mul(tmp, tmp, conv, bin_size);
                native::packed_reverse_fft(tmp2, tmp, rank);
                native::packed_complex_add_to_real(out, tmp2, bin_size);
            }

            // Calculate statistics
            iterations     += 1000;
            time            = float(clock() - start) / float(CLOCKS_PER_SEC);
        } while (time < 30.0f);

        printf("Time = %.1f s, iterations = %d, performance = %.1f [i/s], average time = %.5f [ms/i]\n",
            time, int(iterations), iterations / time, (1000.0f * time) / iterations);
    }

    void test_fft_sse_convolution(float *out, float *tmp, float *tmp2, float *conv, const float *in, const float *cv, size_t rank, const char *label)
    {
        float time = 0.0f;
        size_t iterations = 0;
        size_t bin_size = 1 << rank;

        // Prepare data
        dsp::fill_zero(out, bin_size);
        sse::conv_direct_fft(conv, cv, rank);

        // Start test
        printf("Testing %s of size %d (rank = %d)...\n", label, 1 << rank, int(rank));
        clock_t start = clock();

        do
        {
            // Do 1000 iterations
            for (size_t i=0; i<1000; ++i)
            {
                sse::conv_direct_fft(tmp, in, rank);
                sse::packed_complex_mul(tmp, tmp, conv, bin_size);
                sse::packed_reverse_fft(tmp2, tmp, rank);
                sse::packed_complex_add_to_real(out, tmp2, bin_size);
            }

            // Calculate statistics
            iterations     += 1000;
            time            = float(clock() - start) / float(CLOCKS_PER_SEC);
        } while (time < 30.0f);

        printf("Time = %.1f s, iterations = %d, performance = %.1f [i/s], average time = %.5f [ms/i]\n",
            time, int(iterations), iterations / time, (1000.0f * time) / iterations);
    }

    void test_fast_native_convolution(float *out, float *tmp, float *conv, const float *in, const float *cv, size_t rank, const char *label)
    {
        float time = 0.0f;
        size_t iterations = 0;

        // Prepare data
        dsp::fill_zero(out, 1 << rank);
        native::fastconv_parse(conv, cv, rank);

        // Start test
        printf("Testing %s of size %d (rank = %d)...\n", label, 1 << rank, int(rank));
        clock_t start = clock();

        do
        {
            // Do 1000 iterations
            for (size_t i=0; i<1000; ++i)
                native::fastconv_parse_apply(out, tmp, conv, in, rank);

            // Calculate statistics
            iterations     += 1000;
            time            = float(clock() - start) / float(CLOCKS_PER_SEC);
        } while (time < 30.0f);

        printf("Time = %.1f s, iterations = %d, performance = %.1f [i/s], average time = %.5f [ms/i]\n",
            time, int(iterations), iterations / time, (1000.0f * time) / iterations);
    }

    void test_fast_sse_convolution(float *out, float *tmp, float *conv, const float *in, const float *cv, size_t rank, const char *label)
    {
        float time = 0.0f;
        size_t iterations = 0;

        // Prepare data
        dsp::fill_zero(out, 1 << rank);
        sse::fastconv_parse(conv, cv, rank);

        // Start test
        printf("Testing %s of size %d (rank = %d)...\n", label, 1 << rank, int(rank));
        clock_t start = clock();

        do
        {
            // Do 1000 iterations
            for (size_t i=0; i<1000; ++i)
                sse::fastconv_parse_apply(out, tmp, conv, in, rank);

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
        size_t alloc    = fft_size * 2 * 2 // in + cv size
                        + fft_size * 2 // conv size
                        + fft_size * 2 // tmp size
                        + fft_size * 2 // tmp2 size
                        + fft_size;     // out size

        uint8_t *data   = new uint8_t[alloc*sizeof(float) + DEFAULT_ALIGN];
        float *ptr      = reinterpret_cast<float *>(ALIGN_PTR(data, DEFAULT_ALIGN));
        dsp::fill_zero(ptr, alloc);

        float *in       = ptr;
        ptr            += fft_size * 2;
        float *cv       = ptr;
        ptr            += fft_size * 2;
        float *conv     = ptr;
        ptr            += fft_size * 2;
        float *tmp      = ptr;
        ptr            += fft_size * 2;
        float *tmp2     = ptr;
        ptr            += fft_size * 2;
        float *out      = ptr;
        ptr            += fft_size;

        for (size_t i=0; i < (fft_size / 2); ++i)
        {
            in[i]       = float(rand()) / RAND_MAX;
            cv[i]       = float(rand()) / RAND_MAX;
        }

        printf("\n");
        for (size_t rank=MIN_RANK; rank <= MAX_RANK; ++rank)
        {
            test_old_native_convolution(out, tmp, tmp2, conv, in, cv, rank, "OLD Native Convolution");
            test_fft_native_convolution(out, tmp, tmp2, conv, in, cv, rank, "FFT Native Convolution");
            test_fast_native_convolution(out, tmp, conv, in, cv, rank, "Fast Native Convolution");

            test_old_sse_convolution(out, tmp, tmp2, conv, in, cv, rank, "OLD SSE Convolution");
            test_fft_sse_convolution(out, tmp, tmp2, conv, in, cv, rank, "FFT SSE Convolution");
            test_fast_sse_convolution(out, tmp, conv, in, cv, rank, "Fast SSE Convolution");

            printf("\n");
        }

        delete [] data;

        dsp::finish(&ctx);

        return 0;
    }
    
}

#undef MIN_RANK
#undef MAX_RANK
