#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <math.h>

#include <core/types.h>
#include <core/dsp.h>

#define FTEST_BUF_SIZE  0x200
#define ITERATIONS      10000

namespace filter_test
{
    void biquad_process_multi(float *dst, const float *src, size_t count, float *buf, const float *ir)
    {
        for (size_t i=0; i<count; ++i)
        {
            float s         = src[i];

            // Calculate sample
            float result    =
                buf[0] * ir[0] +
                buf[1] * ir[1] +
                buf[2] * ir[2] +
                buf[3] * ir[3] +
                s      * ir[4];

            // Shift buffer
            buf[3]  = buf[1];
            buf[2]  = buf[0];
            buf[1]  = s;
            buf[0]  = result;

            // Store sample
            dst[i]  = result;
        }
    }

    using namespace lsp;

    void test_biquad(float *out, const float *in, size_t count)
    {
        printf("Testing BIQUAD OLD x8 FILTER on buffer size %d ...\n", int(count));
        float ir[8]     = { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f };
        float buf[4]    = { 0.0f, 0.0f, 0.0f, 0.0f };

        clock_t start = clock();
        float time = 0.0f;
        size_t iterations = 0;

        do
        {
            // Do 100 iterations
            for (size_t i=0; i<ITERATIONS; ++i)
            {
                biquad_process_multi(out, in, count, buf, ir);
                biquad_process_multi(out, out, count, buf, ir);
                biquad_process_multi(out, out, count, buf, ir);
                biquad_process_multi(out, out, count, buf, ir);
                biquad_process_multi(out, out, count, buf, ir);
                biquad_process_multi(out, out, count, buf, ir);
                biquad_process_multi(out, out, count, buf, ir);
                biquad_process_multi(out, out, count, buf, ir);
            }

            // Calculate statistics
            iterations     += ITERATIONS;
            time            = float(clock() - start) / float(CLOCKS_PER_SEC);
        } while (time < 30.0f);

        printf("Time = %.1f s, iterations = %d, performance = %.1f [i/s], average time = %.6f [ms/i]\n",
            time, int(iterations), iterations / time, (1000.0f * time) / iterations);
    }

    void test_biquad_x1(float *out, const float *in, size_t count)
    {
        printf("Testing BIQUAD X1 x8 FILTER on buffer size %d ...\n", int(count));
        biquad_t f __lsp_aligned64;

        f.x1.a[0]      = 1.0f;
        f.x1.a[1]      = 1.0f;
        f.x1.a[2]      = 0.0f;
        f.x1.a[3]      = 0.0f;
        f.x1.b[0]      = 0.0f;
        f.x1.b[1]      = 0.0f;
        f.x1.b[2]      = 0.0f;
        f.x1.b[3]      = 0.0f;

        for (size_t i=0; i<8; ++i)
            f.d[i]          = 0.0f;

        clock_t start = clock();
        float time = 0.0f;
        size_t iterations = 0;

        do
        {
            // Do 100 iterations
            for (size_t i=0; i<ITERATIONS; ++i)
            {
                dsp::biquad_process_x1(out, in, count, &f);
                dsp::biquad_process_x1(out, out, count, &f);
                dsp::biquad_process_x1(out, out, count, &f);
                dsp::biquad_process_x1(out, out, count, &f);
                dsp::biquad_process_x1(out, out, count, &f);
                dsp::biquad_process_x1(out, out, count, &f);
                dsp::biquad_process_x1(out, out, count, &f);
                dsp::biquad_process_x1(out, out, count, &f);
            }

            // Calculate statistics
            iterations     += ITERATIONS;
            time            = float(clock() - start) / float(CLOCKS_PER_SEC);
        } while (time < 30.0f);

        printf("Time = %.1f s, iterations = %d, performance = %.1f [i/s], average time = %.6f [ms/i]\n",
            time, int(iterations), iterations / time, (1000.0f * time) / iterations);
    }

    void test_biquad_x2(float *out, const float *in, size_t count)
    {
        printf("Testing BIQUAD X2 x4 FILTER on buffer size %d ...\n", int(count));
        biquad_t f __lsp_aligned64;

        // Filter 1
        f.x2.a[0]      = 1.0f;
        f.x2.a[1]      = 1.0f;
        f.x2.a[2]      = 0.0f;
        f.x2.a[3]      = 0.0f;
        f.x2.b[0]      = 0.0f;
        f.x2.b[1]      = 0.0f;
        f.x2.b[2]      = 0.0f;
        f.x2.b[3]      = 0.0f;

        // Filter 2
        f.x2.a[4]      = 1.0f;
        f.x2.a[5]      = 1.0f;
        f.x2.a[6]      = 0.0f;
        f.x2.a[7]      = 0.0f;
        f.x2.b[4]      = 0.0f;
        f.x2.b[5]      = 0.0f;
        f.x2.b[6]      = 0.0f;
        f.x2.b[7]      = 0.0f;

        for (size_t i=0; i<8; ++i)
            f.d[i]          = 0.0f;

        clock_t start = clock();
        float time = 0.0f;
        size_t iterations = 0;

        do
        {
            // Do 100 iterations
            for (size_t i=0; i<ITERATIONS; ++i)
            {
                dsp::biquad_process_x2(out, in, count, &f);
                dsp::biquad_process_x2(out, out, count, &f);
                dsp::biquad_process_x2(out, out, count, &f);
                dsp::biquad_process_x2(out, out, count, &f);
            }

            // Calculate statistics
            iterations     += ITERATIONS;
            time            = float(clock() - start) / float(CLOCKS_PER_SEC);
        } while (time < 30.0f);

        printf("Time = %.1f s, iterations = %d, performance = %.1f [i/s], average time = %.6f [ms/i]\n",
            time, int(iterations), iterations / time, (1000.0f * time) / iterations);
    }

    void test_biquad_x4(float *out, const float *in, size_t count)
    {
        printf("Testing BIQUAD X4 x2 FILTER on buffer size %d ...\n", int(count));
        biquad_t f __lsp_aligned64;

        // Filters x 4
        f.x4.a0[0]     = 1.0f;
        f.x4.a0[1]     = 1.0f;
        f.x4.a0[2]     = 1.0f;
        f.x4.a0[3]     = 1.0f;

        f.x4.a1[0]     = 0.0f;
        f.x4.a1[1]     = 0.0f;
        f.x4.a1[2]     = 0.0f;
        f.x4.a1[3]     = 0.0f;

        f.x4.a2[0]     = 0.0f;
        f.x4.a2[1]     = 0.0f;
        f.x4.a2[2]     = 0.0f;
        f.x4.a2[3]     = 0.0f;

        f.x4.b1[0]     = 0.0f;
        f.x4.b1[1]     = 0.0f;
        f.x4.b1[2]     = 0.0f;
        f.x4.b1[3]     = 0.0f;

        f.x4.b2[0]     = 0.0f;
        f.x4.b2[1]     = 0.0f;
        f.x4.b2[2]     = 0.0f;
        f.x4.b2[3]     = 0.0f;

        for (size_t i=0; i<8; ++i)
            f.d[i]          = 0.0f;

        clock_t start = clock();
        float time = 0.0f;
        size_t iterations = 0;

        do
        {
            // Do 100 iterations
            for (size_t i=0; i<ITERATIONS; ++i)
            {
                dsp::biquad_process_x4(out, in, count, &f);
                dsp::biquad_process_x4(out, in, count, &f);
            }

            // Calculate statistics
            iterations     += ITERATIONS;
            time            = float(clock() - start) / float(CLOCKS_PER_SEC);
        } while (time < 30.0f);

        printf("Time = %.1f s, iterations = %d, performance = %.1f [i/s], average time = %.6f [ms/i]\n",
            time, int(iterations), iterations / time, (1000.0f * time) / iterations);
    }

    void test_biquad_x8(float *out, const float *in, size_t count)
    {
        printf("Testing BIQUAD X8 x1 FILTER on buffer size %d ...\n", int(count));
        biquad_t f __lsp_aligned64;

        bzero(&f, sizeof(biquad_t));

        // Filters x 8
        f.x8.a0[0]     = 1.0f;
        f.x8.a0[1]     = 1.0f;
        f.x8.a0[2]     = 1.0f;
        f.x8.a0[3]     = 1.0f;
        f.x8.a0[4]     = 1.0f;
        f.x8.a0[5]     = 1.0f;
        f.x8.a0[6]     = 1.0f;
        f.x8.a0[7]     = 1.0f;

        clock_t start = clock();
        float time = 0.0f;
        size_t iterations = 0;

        do
        {
            // Do 100 iterations
            for (size_t i=0; i<ITERATIONS; ++i)
                dsp::biquad_process_x8(out, in, count, &f);

            // Calculate statistics
            iterations     += ITERATIONS;
            time            = float(clock() - start) / float(CLOCKS_PER_SEC);
        } while (time < 30.0f);

        printf("Time = %.1f s, iterations = %d, performance = %.1f [i/s], average time = %.6f [ms/i]\n",
            time, int(iterations), iterations / time, (1000.0f * time) / iterations);
    }


    int test(int argc, const char **argv)
    {
        dsp::init();

        float *out          = new float[FTEST_BUF_SIZE];
        float *in           = new float[FTEST_BUF_SIZE];

        for (size_t i=0; i<FTEST_BUF_SIZE; ++i)
        {
            in[i]               = (i % 1) ? 1.0f : -1.0f;
            out[i]              = 0.0f;
        }

//        test_biquad(out, in, FTEST_BUF_SIZE);
//        test_biquad_x1(out, in, FTEST_BUF_SIZE);
//        test_biquad_x2(out, in, FTEST_BUF_SIZE);
//        test_biquad_x4(out, in, FTEST_BUF_SIZE);
        test_biquad_x8(out, in, FTEST_BUF_SIZE);

        delete [] out;
        delete [] in;

        return 0;
    }

}

#undef FTEST_BUF_SIZE
#undef ITERATIONS
