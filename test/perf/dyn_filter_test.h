#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <math.h>

#include <core/types.h>
#include <dsp/dsp.h>

#define FTEST_BUF_SIZE  0x200
#define ITERATIONS      10000

namespace dyn_filter_test
{
    using namespace lsp;

    static biquad_x1_t bq_normal = {
        { 1.0, 1.0, 2.0, 1.0 },
        {-2.0, -1.0, 0.0, 0.0 }
    };

    void dyn_biquad_process_x1(float *dst, const float *src, float *d, size_t count, const biquad_x1_t *f)
    {
        while (count--)
        {
            float s     = *(src++);
            //   a: a0 a0 a1 a2
            //   b: b1 b2 0  0
            float s2    = f->a[0]*s + d[0];
            float p1    = f->a[2]*s + f->b[0]*s2;
            float p2    = f->a[3]*s + f->b[1]*s2;

            // Shift buffer
            d[0]        = d[1] + p1;
            d[1]        = p2;

            // Store result
            *(dst++)    = s2;
            f++;
        }
    }

    void test_dyn_biquad(float *out, const float *in, size_t count)
    {
        printf("Testing DYN_BIQUAD X1 NATIVE x8 FILTER on buffer size %d ...\n", int(count));

        float d[16];
        for (size_t i=0; i<16; ++i)
            d[i]     = 0.0;

        void *ptr = NULL;
        biquad_x1_t *f = alloc_aligned<biquad_x1_t>(ptr, count);
        for (size_t i=0; i<count; ++i)
            f[i]        = bq_normal;

        clock_t start = clock();
        float time = 0.0f;
        size_t iterations = 0;

        do
        {
            // Do 100 iterations
            for (size_t i=0; i<ITERATIONS; ++i)
            {
                dyn_biquad_process_x1(out, in, &d[0], count, f);
                dyn_biquad_process_x1(out, out, &d[2], count, f);
                dyn_biquad_process_x1(out, out, &d[4], count, f);
                dyn_biquad_process_x1(out, out, &d[6], count, f);
                dyn_biquad_process_x1(out, out, &d[8], count, f);
                dyn_biquad_process_x1(out, out, &d[10], count, f);
                dyn_biquad_process_x1(out, out, &d[12], count, f);
                dyn_biquad_process_x1(out, out, &d[14], count, f);
            }

            // Calculate statistics
            iterations     += ITERATIONS;
            time            = float(clock() - start) / float(CLOCKS_PER_SEC);
        } while (time < 30.0f);

        printf("Time = %.1f s, iterations = %d, performance = %.1f [i/s], average time = %.6f [ms/i]\n",
            time, int(iterations), iterations / time, (1000.0f * time) / iterations);

        free_aligned(ptr);
    }

    void test_dyn_biquad_x1(float *out, const float *in, size_t count)
    {
        printf("Testing DYN_BIQUAD X1 x8 FILTER on buffer size %d ...\n", int(count));

        float d[16];
        for (size_t i=0; i<16; ++i)
            d[i]     = 0.0;

        void *ptr = NULL;
        biquad_x1_t *f = alloc_aligned<biquad_x1_t>(ptr, count);
        for (size_t i=0; i<count; ++i)
            f[i]        = bq_normal;

        clock_t start = clock();
        float time = 0.0f;
        size_t iterations = 0;

        do
        {
            // Do 100 iterations
            for (size_t i=0; i<ITERATIONS; ++i)
            {
                dsp::dyn_biquad_process_x1(out, in, &d[0], count, f);
                dsp::dyn_biquad_process_x1(out, out, &d[2], count, f);
                dsp::dyn_biquad_process_x1(out, out, &d[4], count, f);
                dsp::dyn_biquad_process_x1(out, out, &d[6], count, f);
                dsp::dyn_biquad_process_x1(out, out, &d[8], count, f);
                dsp::dyn_biquad_process_x1(out, out, &d[10], count, f);
                dsp::dyn_biquad_process_x1(out, out, &d[12], count, f);
                dsp::dyn_biquad_process_x1(out, out, &d[14], count, f);
            }

            // Calculate statistics
            iterations     += ITERATIONS;
            time            = float(clock() - start) / float(CLOCKS_PER_SEC);
        } while (time < 30.0f);

        printf("Time = %.1f s, iterations = %d, performance = %.1f [i/s], average time = %.6f [ms/i]\n",
            time, int(iterations), iterations / time, (1000.0f * time) / iterations);

        free_aligned(ptr);
    }

    void test_dyn_biquad_x2(float *out, const float *in, size_t count)
    {
        printf("Testing DYN_BIQUAD X2 x4 FILTER on buffer size %d ...\n", int(count));

        float d[16];
        for (size_t i=0; i<16; ++i)
            d[i]     = 0.0;

        void *ptr = NULL;
        biquad_x2_t *f = alloc_aligned<biquad_x2_t>(ptr, count+1);
        for (size_t i=0; i<(count+1); ++i)
        {
            f[i].a[0]   = bq_normal.a[0];
            f[i].a[1]   = bq_normal.a[1];
            f[i].a[2]   = bq_normal.a[2];
            f[i].a[3]   = bq_normal.a[3];
            f[i].a[4]   = bq_normal.a[0];
            f[i].a[5]   = bq_normal.a[1];
            f[i].a[6]   = bq_normal.a[2];
            f[i].a[7]   = bq_normal.a[3];

            f[i].b[0]   = bq_normal.b[0];
            f[i].b[1]   = bq_normal.b[1];
            f[i].b[2]   = bq_normal.b[2];
            f[i].b[3]   = bq_normal.b[3];
            f[i].b[4]   = bq_normal.b[0];
            f[i].b[5]   = bq_normal.b[1];
            f[i].b[6]   = bq_normal.b[2];
            f[i].b[7]   = bq_normal.b[3];
        }

        clock_t start = clock();
        float time = 0.0f;
        size_t iterations = 0;

        do
        {
            // Do 100 iterations
            for (size_t i=0; i<ITERATIONS; ++i)
            {
                dsp::dyn_biquad_process_x2(out, in, &d[0], count, f);
                dsp::dyn_biquad_process_x2(out, out, &d[4], count, f);
                dsp::dyn_biquad_process_x2(out, out, &d[8], count, f);
                dsp::dyn_biquad_process_x2(out, out, &d[12], count, f);
            }

            // Calculate statistics
            iterations     += ITERATIONS;
            time            = float(clock() - start) / float(CLOCKS_PER_SEC);
        } while (time < 30.0f);

        printf("Time = %.1f s, iterations = %d, performance = %.1f [i/s], average time = %.6f [ms/i]\n",
            time, int(iterations), iterations / time, (1000.0f * time) / iterations);

        free_aligned(ptr);
    }

    void test_dyn_biquad_x4(float *out, const float *in, size_t count)
    {
        printf("Testing DYN_BIQUAD X4 x2 FILTER on buffer size %d ...\n", int(count));

        float d[16];
        for (size_t i=0; i<16; ++i)
            d[i]     = 0.0;

        void *ptr = NULL;
        biquad_x4_t *f = alloc_aligned<biquad_x4_t>(ptr, count+3);
        for (size_t i=0; i<(count+3); ++i)
        {
            f[i].a0[0]  = f[i].a0[1] = f[i].a0[2] = f[i].a0[3] = bq_normal.a[0];
            f[i].a1[0]  = f[i].a1[1] = f[i].a1[2] = f[i].a1[3] = bq_normal.a[1];
            f[i].a2[0]  = f[i].a2[1] = f[i].a2[2] = f[i].a2[3] = bq_normal.a[2];
            f[i].b1[0]  = f[i].b1[1] = f[i].b1[2] = f[i].b1[3] = bq_normal.b[1];
            f[i].b2[0]  = f[i].b2[1] = f[i].b2[2] = f[i].b2[3] = bq_normal.b[2];
        }

        clock_t start = clock();
        float time = 0.0f;
        size_t iterations = 0;

        do
        {
            // Do 100 iterations
            for (size_t i=0; i<ITERATIONS; ++i)
            {
                dsp::dyn_biquad_process_x4(out, in, &d[0], count, f);
                dsp::dyn_biquad_process_x4(out, out, &d[8], count, f);
            }

            // Calculate statistics
            iterations     += ITERATIONS;
            time            = float(clock() - start) / float(CLOCKS_PER_SEC);
        } while (time < 30.0f);

        printf("Time = %.1f s, iterations = %d, performance = %.1f [i/s], average time = %.6f [ms/i]\n",
            time, int(iterations), iterations / time, (1000.0f * time) / iterations);

        free_aligned(ptr);
    }


    void test_dyn_biquad_x8(float *out, const float *in, size_t count)
    {
        printf("Testing DYN_BIQUAD X8 x1 FILTER on buffer size %d ...\n", int(count));

        float d[16];
        for (size_t i=0; i<16; ++i)
            d[i]     = 0.0;

        void *ptr = NULL;
        biquad_x8_t *f = alloc_aligned<biquad_x8_t>(ptr, count+7);
        for (size_t i=0; i<(count+7); ++i)
        {
            f[i].a0[0]  = f[i].a0[1] = f[i].a0[2] = f[i].a0[3] =
            f[i].a0[4]  = f[i].a0[5] = f[i].a0[6] = f[i].a0[7] = bq_normal.a[0];
            f[i].a1[0]  = f[i].a1[1] = f[i].a1[2] = f[i].a1[3] =
            f[i].a1[4]  = f[i].a1[5] = f[i].a1[6] = f[i].a1[7] = bq_normal.a[1];
            f[i].a2[0]  = f[i].a2[1] = f[i].a2[2] = f[i].a2[3] =
            f[i].a2[4]  = f[i].a2[5] = f[i].a2[6] = f[i].a2[7] = bq_normal.a[2];
            f[i].b1[0]  = f[i].b1[1] = f[i].b1[2] = f[i].b1[3] =
            f[i].b1[4]  = f[i].b1[5] = f[i].b1[6] = f[i].b1[7] = bq_normal.b[1];
            f[i].b2[0]  = f[i].b2[1] = f[i].b2[2] = f[i].b2[3] =
            f[i].b2[4]  = f[i].b2[5] = f[i].b2[6] = f[i].b2[7] = bq_normal.b[2];
        }

        clock_t start = clock();
        float time = 0.0f;
        size_t iterations = 0;

        do
        {
            // Do 100 iterations
            for (size_t i=0; i<ITERATIONS; ++i)
                dsp::dyn_biquad_process_x8(out, in, d, count, f);

            // Calculate statistics
            iterations     += ITERATIONS;
            time            = float(clock() - start) / float(CLOCKS_PER_SEC);
        } while (time < 30.0f);

        printf("Time = %.1f s, iterations = %d, performance = %.1f [i/s], average time = %.6f [ms/i]\n",
            time, int(iterations), iterations / time, (1000.0f * time) / iterations);

        free_aligned(ptr);
    }


    int test(int argc, const char **argv)
    {
        dsp::context_t ctx;

        dsp::init();
        dsp::start(&ctx);


        float *out          = new float[FTEST_BUF_SIZE];
        float *in           = new float[FTEST_BUF_SIZE];

        for (size_t i=0; i<FTEST_BUF_SIZE; ++i)
        {
            in[i]               = (i % 1) ? 1.0f : -1.0f;
            out[i]              = 0.0f;
        }

        test_dyn_biquad(out, in, FTEST_BUF_SIZE);
        test_dyn_biquad_x1(out, in, FTEST_BUF_SIZE);
        test_dyn_biquad_x2(out, in, FTEST_BUF_SIZE);
        test_dyn_biquad_x4(out, in, FTEST_BUF_SIZE);
        test_dyn_biquad_x8(out, in, FTEST_BUF_SIZE);

        delete [] out;
        delete [] in;

        dsp::finish(&ctx);

        return 0;
    }

}

#undef FTEST_BUF_SIZE
#undef ITERATIONS
