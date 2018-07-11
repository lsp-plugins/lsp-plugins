#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <math.h>

#include <core/types.h>
#include <core/dsp.h>

#define RTEST_BUF_SIZE  0x1000
#define ITERATIONS      10000

namespace lsp
{
    namespace native
    {
        void downsample_2x(float *dst, const float *src, size_t count);
        void downsample_3x(float *dst, const float *src, size_t count);
        void downsample_4x(float *dst, const float *src, size_t count);
        void downsample_6x(float *dst, const float *src, size_t count);
        void downsample_8x(float *dst, const float *src, size_t count);
    }

    namespace sse
    {
        void downsample_2x(float *dst, const float *src, size_t count);
        void downsample_3x(float *dst, const float *src, size_t count);
        void downsample_4x(float *dst, const float *src, size_t count);
        void downsample_6x(float *dst, const float *src, size_t count);
        void downsample_8x(float *dst, const float *src, size_t count);
    }
}

namespace downsampling_test
{
    using namespace lsp;

    void dump_buffer(const char *text, const float *buf, size_t count)
    {
        printf("  dump of buffer %s:\n    ", text);
        while (count--)
            printf("%.5f ", *(buf++));
        printf("\n");
    }

    void test_downsampling(float *out, const float *in, size_t count, size_t times, const char *text, resampling_function_t func)
    {
        printf("Testing %s downsampling on input buffer size %d, output buffer size %d ...\n", text, int(count*times), int(count));

        clock_t start = clock();
        float time = 0.0f;
        size_t iterations = 0;

        do
        {
            // Do 100 iterations
            for (size_t i=0; i<ITERATIONS; ++i)
                func(out, in, count);

            // Calculate statistics
            iterations     += ITERATIONS;
            time            = float(clock() - start) / float(CLOCKS_PER_SEC);
        } while (time < 30.0f);

        printf("Time = %.1f s, iterations = %d, performance = %.1f [i/s], average time = %.6f [ms/i]\n",
            time, int(iterations), iterations / time, (1000.0f * time) / iterations);
    }

    void call_downsampling(float *out, float *in, size_t times, const char *text, resampling_function_t func)
    {
        printf("\nTesting %s downsampling...\n", text);

        dsp::fill_minus_one(in, times * 64);
        for (size_t i=0; i<=63; i++)
            in[i*times]     = i;

        func(out, in, 63);
        dump_buffer("in", in, 63*times);
        dump_buffer("out", out, 63);
        printf("\n");
    }

    int test(int argc, const char **argv)
    {
        dsp::init();

        float *out          = new float[RTEST_BUF_SIZE];
        float *in           = new float[RTEST_BUF_SIZE*8];

        call_downsampling(out, in, 2, "2x native", native::downsample_2x);
        call_downsampling(out, in, 2, "2x sse", sse::downsample_2x);
        call_downsampling(out, in, 3, "3x native", native::downsample_3x);
        call_downsampling(out, in, 3, "3x sse", sse::downsample_3x);
        call_downsampling(out, in, 4, "4x native", native::downsample_4x);
        call_downsampling(out, in, 4, "4x sse", sse::downsample_4x);
        call_downsampling(out, in, 6, "6x native", native::downsample_6x);
        call_downsampling(out, in, 6, "6x sse", sse::downsample_6x);
        call_downsampling(out, in, 8, "8x native", native::downsample_8x);
        call_downsampling(out, in, 8, "8x sse", sse::downsample_8x);

        for (size_t i=0; i<RTEST_BUF_SIZE*4; ++i)
            in[i]               = (i % 1) ? 1.0f : -1.0f;

        printf("\n\n");

        test_downsampling(out, in, RTEST_BUF_SIZE, 2, "2x native", native::downsample_2x);
        test_downsampling(out, in, RTEST_BUF_SIZE, 2, "2x sse", sse::downsample_2x);
        test_downsampling(out, in, RTEST_BUF_SIZE, 3, "3x native", native::downsample_3x);
        test_downsampling(out, in, RTEST_BUF_SIZE, 3, "3x sse", sse::downsample_3x);
        test_downsampling(out, in, RTEST_BUF_SIZE, 4, "4x native", native::downsample_4x);
        test_downsampling(out, in, RTEST_BUF_SIZE, 4, "4x sse", sse::downsample_4x);
        test_downsampling(out, in, RTEST_BUF_SIZE, 6, "6x native", native::downsample_6x);
        test_downsampling(out, in, RTEST_BUF_SIZE, 6, "6x sse", sse::downsample_6x);
        test_downsampling(out, in, RTEST_BUF_SIZE, 8, "8x native", native::downsample_8x);
        test_downsampling(out, in, RTEST_BUF_SIZE, 8, "8x sse", sse::downsample_8x);

        delete [] out;
        delete [] in;

        return 0;
    }

}

#undef RTEST_BUF_SIZE
#undef ITERATIONS
