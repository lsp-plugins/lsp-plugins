#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <math.h>

#include <core/types.h>
#include <dsp/dsp.h>

#define PERF_BUF_SIZE   0x200
#define ITERATIONS      10000

#define KF              100.0f
#define TD              (2*M_PI/48000.0)

namespace lsp
{
    namespace native
    {
        void matched_transform_x1(biquad_x1_t *bf, f_cascade_t *bc, float kf, float td, size_t count);
    }
}

namespace mt_perf_test
{
    using namespace lsp;

    static const f_cascade_t test_c =
    {
        1, 2, 1, 0,
        1, -2, 1, 0
    };

    void test_native_matched_x1(size_t count)
    {
        printf("Testing MATCHED NATIVE X1 x8 TRANSFORM on buffer size %d ...\n", int(count));

        void *p1 = NULL, *p2 = NULL;
        biquad_x1_t *dst = alloc_aligned<biquad_x1_t>(p1, count, 32);
        f_cascade_t *src = alloc_aligned<f_cascade_t>(p2, count, 32);
        f_cascade_t *tmp = alloc_aligned<f_cascade_t>(p2, count, 32);

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
                dsp::copy(tmp->t, src->t, count * (sizeof(f_cascade_t) / sizeof(float)));
                native::matched_transform_x1(dst, tmp, KF, TD, count);
                dsp::copy(tmp->t, src->t, count * (sizeof(f_cascade_t) / sizeof(float)));
                native::matched_transform_x1(dst, tmp, KF, TD, count);
                dsp::copy(tmp->t, src->t, count * (sizeof(f_cascade_t) / sizeof(float)));
                native::matched_transform_x1(dst, tmp, KF, TD, count);
                dsp::copy(tmp->t, src->t, count * (sizeof(f_cascade_t) / sizeof(float)));
                native::matched_transform_x1(dst, tmp, KF, TD, count);
                dsp::copy(tmp->t, src->t, count * (sizeof(f_cascade_t) / sizeof(float)));
                native::matched_transform_x1(dst, tmp, KF, TD, count);
                dsp::copy(tmp->t, src->t, count * (sizeof(f_cascade_t) / sizeof(float)));
                native::matched_transform_x1(dst, tmp, KF, TD, count);
                dsp::copy(tmp->t, src->t, count * (sizeof(f_cascade_t) / sizeof(float)));
                native::matched_transform_x1(dst, tmp, KF, TD, count);
                dsp::copy(tmp->t, src->t, count * (sizeof(f_cascade_t) / sizeof(float)));
                native::matched_transform_x1(dst, tmp, KF, TD, count);
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

    void test_matched_x1(size_t count)
    {
        printf("Testing MATCHED  X1 x8 TRANSFORM on buffer size %d ...\n", int(count));

        void *p1 = NULL, *p2 = NULL;
        biquad_x1_t *dst = alloc_aligned<biquad_x1_t>(p1, count, 32);
        f_cascade_t *src = alloc_aligned<f_cascade_t>(p2, count, 32);
        f_cascade_t *tmp = alloc_aligned<f_cascade_t>(p2, count, 32);

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
                dsp::copy(tmp->t, src->t, count * (sizeof(f_cascade_t) / sizeof(float)));
                dsp::matched_transform_x1(dst, tmp, KF, TD, count);
                dsp::copy(tmp->t, src->t, count * (sizeof(f_cascade_t) / sizeof(float)));
                dsp::matched_transform_x1(dst, tmp, KF, TD, count);
                dsp::copy(tmp->t, src->t, count * (sizeof(f_cascade_t) / sizeof(float)));
                dsp::matched_transform_x1(dst, tmp, KF, TD, count);
                dsp::copy(tmp->t, src->t, count * (sizeof(f_cascade_t) / sizeof(float)));
                dsp::matched_transform_x1(dst, tmp, KF, TD, count);
                dsp::copy(tmp->t, src->t, count * (sizeof(f_cascade_t) / sizeof(float)));
                dsp::matched_transform_x1(dst, tmp, KF, TD, count);
                dsp::copy(tmp->t, src->t, count * (sizeof(f_cascade_t) / sizeof(float)));
                dsp::matched_transform_x1(dst, tmp, KF, TD, count);
                dsp::copy(tmp->t, src->t, count * (sizeof(f_cascade_t) / sizeof(float)));
                dsp::matched_transform_x1(dst, tmp, KF, TD, count);
                dsp::copy(tmp->t, src->t, count * (sizeof(f_cascade_t) / sizeof(float)));
                dsp::matched_transform_x1(dst, tmp, KF, TD, count);
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

    int test(int argc, const char **argv)
    {
        dsp::context_t ctx;

        dsp::init();
        dsp::start(&ctx);

        test_native_matched_x1(PERF_BUF_SIZE);
        test_matched_x1(PERF_BUF_SIZE);

        dsp::finish(&ctx);

        return 0;
    }

}

#undef TD
#undef KF

#undef PERF_BUF_SIZE
#undef ITERATIONS
