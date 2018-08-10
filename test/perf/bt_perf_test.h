#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <math.h>

#include <core/types.h>
#include <dsp/dsp.h>

#define PERF_BUF_SIZE   0x200
#define ITERATIONS      10000

namespace lsp
{
    namespace native
    {
        void bilinear_transform_x1(biquad_x1_t *bf, const f_cascade_t *bc, float kf, size_t count);
    }
}

namespace bt_perf_test
{
    using namespace lsp;

    static const f_cascade_t test_c =
    {
        1, 2, 1, 0,
        1, -2, 1, 0
    };

    void test_native_bilinear_x1(size_t count)
    {
        printf("Testing BILINEAR NATIVE X1 x8 TRANSFORM on buffer size %d ...\n", int(count));

        void *p1 = NULL, *p2 = NULL;
        biquad_x1_t *dst = alloc_aligned<biquad_x1_t>(p1, count, 32);
        f_cascade_t *src = alloc_aligned<f_cascade_t>(p2, count, 32);

        for (size_t i=0; i<count; ++i)
            src[i]  = test_c;

        clock_t start = clock();
        float time = 0.0f;
        size_t iterations = 0;

        do
        {
            // Do 100 iterations
            for (size_t i=0; i<ITERATIONS; ++i)
            {
                native::bilinear_transform_x1(dst, src, 1.0f, count);
                native::bilinear_transform_x1(dst, src, 1.0f, count);
                native::bilinear_transform_x1(dst, src, 1.0f, count);
                native::bilinear_transform_x1(dst, src, 1.0f, count);
                native::bilinear_transform_x1(dst, src, 1.0f, count);
                native::bilinear_transform_x1(dst, src, 1.0f, count);
                native::bilinear_transform_x1(dst, src, 1.0f, count);
                native::bilinear_transform_x1(dst, src, 1.0f, count);
            }

            // Calculate statistics
            iterations     += ITERATIONS;
            time            = float(clock() - start) / float(CLOCKS_PER_SEC);
        } while (time < 30.0f);

        printf("Time = %.1f s, iterations = %d, performance = %.1f [i/s], average time = %.6f [ms/i]\n",
            time, int(iterations), iterations / time, (1000.0f * time) / iterations);

        free_aligned(p1);
        free_aligned(p2);
    }

    void test_bilinear_x1(size_t count)
    {
        printf("Testing BILINEAR X1 x8 TRANSFORM on buffer size %d ...\n", int(count));

        void *p1 = NULL, *p2 = NULL;
        biquad_x1_t *dst = alloc_aligned<biquad_x1_t>(p1, count, 32);
        f_cascade_t *src = alloc_aligned<f_cascade_t>(p2, count, 32);

        for (size_t i=0; i<count; ++i)
            src[i]  = test_c;

        clock_t start = clock();
        float time = 0.0f;
        size_t iterations = 0;

        do
        {
            // Do 100 iterations
            for (size_t i=0; i<ITERATIONS; ++i)
            {
                dsp::bilinear_transform_x1(dst, src, 1.0f, count);
                dsp::bilinear_transform_x1(dst, src, 1.0f, count);
                dsp::bilinear_transform_x1(dst, src, 1.0f, count);
                dsp::bilinear_transform_x1(dst, src, 1.0f, count);
                dsp::bilinear_transform_x1(dst, src, 1.0f, count);
                dsp::bilinear_transform_x1(dst, src, 1.0f, count);
                dsp::bilinear_transform_x1(dst, src, 1.0f, count);
                dsp::bilinear_transform_x1(dst, src, 1.0f, count);
            }

            // Calculate statistics
            iterations     += ITERATIONS;
            time            = float(clock() - start) / float(CLOCKS_PER_SEC);
        } while (time < 30.0f);

        printf("Time = %.1f s, iterations = %d, performance = %.1f [i/s], average time = %.6f [ms/i]\n",
            time, int(iterations), iterations / time, (1000.0f * time) / iterations);

        free_aligned(p1);
        free_aligned(p2);
    }

    void test_bilinear_x2(size_t count)
    {
        printf("Testing BILINEAR X2 x4 TRANSFORM on buffer size %d ...\n", int(count));

        count++;
        void *p1 = NULL, *p2 = NULL;
        biquad_x2_t *dst = alloc_aligned<biquad_x2_t>(p1, count, 32);
        f_cascade_t *src = alloc_aligned<f_cascade_t>(p2, count*2, 32);

        for (size_t i=0; i<count*2; ++i)
            src[i]  = test_c;

        clock_t start = clock();
        float time = 0.0f;
        size_t iterations = 0;

        do
        {
            // Do 100 iterations
            for (size_t i=0; i<ITERATIONS; ++i)
            {
                dsp::bilinear_transform_x2(dst, src, 1.0f, count);
                dsp::bilinear_transform_x2(dst, src, 1.0f, count);
                dsp::bilinear_transform_x2(dst, src, 1.0f, count);
                dsp::bilinear_transform_x2(dst, src, 1.0f, count);
            }

            // Calculate statistics
            iterations     += ITERATIONS;
            time            = float(clock() - start) / float(CLOCKS_PER_SEC);
        }
        while (time < 30.0f);

        printf("Time = %.1f s, iterations = %d, performance = %.1f [i/s], average time = %.6f [ms/i]\n",
            time, int(iterations), iterations / time, (1000.0f * time) / iterations);

        free_aligned(p1);
        free_aligned(p2);
    }

    void test_bilinear_x4(size_t count)
    {
        printf("Testing BILINEAR X4 x2 TRANSFORM on buffer size %d ...\n", int(count));

        count += 3;
        void *p1 = NULL, *p2 = NULL;
        biquad_x4_t *dst = alloc_aligned<biquad_x4_t>(p1, count, 32);
        f_cascade_t *src = alloc_aligned<f_cascade_t>(p2, count*4, 32);

        for (size_t i=0; i<count*4; ++i)
            src[i]  = test_c;

        clock_t start = clock();
        float time = 0.0f;
        size_t iterations = 0;

        do
        {
            // Do 100 iterations
            for (size_t i=0; i<ITERATIONS; ++i)
            {
                dsp::bilinear_transform_x4(dst, src, 1.0f, count);
                dsp::bilinear_transform_x4(dst, src, 1.0f, count);
            }

            // Calculate statistics
            iterations     += ITERATIONS;
            time            = float(clock() - start) / float(CLOCKS_PER_SEC);
        } while (time < 30.0f);

        printf("Time = %.1f s, iterations = %d, performance = %.1f [i/s], average time = %.6f [ms/i]\n",
            time, int(iterations), iterations / time, (1000.0f * time) / iterations);

        free_aligned(p1);
        free_aligned(p2);
    }

    void test_bilinear_x8(size_t count)
    {
        printf("Testing BILINEAR X8 x1 TRANSFORM on buffer size %d ...\n", int(count));

        count += 7;
        void *p1 = NULL, *p2 = NULL;
        biquad_x8_t *dst = alloc_aligned<biquad_x8_t>(p1, count, 32);
        f_cascade_t *src = alloc_aligned<f_cascade_t>(p2, count*8, 32);

        for (size_t i=0; i<count*8; ++i)
            src[i]  = test_c;

        clock_t start = clock();
        float time = 0.0f;
        size_t iterations = 0;

        do
        {
            // Do 100 iterations
            for (size_t i=0; i<ITERATIONS; ++i)
                dsp::bilinear_transform_x8(dst, src, 1.0f, count);

            // Calculate statistics
            iterations     += ITERATIONS;
            time            = float(clock() - start) / float(CLOCKS_PER_SEC);
        } while (time < 30.0f);

        printf("Time = %.1f s, iterations = %d, performance = %.1f [i/s], average time = %.6f [ms/i]\n",
            time, int(iterations), iterations / time, (1000.0f * time) / iterations);

        free_aligned(p1);
        free_aligned(p2);
    }


    int test(int argc, const char **argv)
    {
        dsp_context_t ctx;

        dsp::init();
        dsp::start(&ctx);

        test_native_bilinear_x1(PERF_BUF_SIZE);
        test_bilinear_x1(PERF_BUF_SIZE);
        test_bilinear_x2(PERF_BUF_SIZE);
        test_bilinear_x4(PERF_BUF_SIZE);
        test_bilinear_x8(PERF_BUF_SIZE);

        dsp::finish(&ctx);

        return 0;
    }

}

#undef PERF_BUF_SIZE
#undef ITERATIONS
