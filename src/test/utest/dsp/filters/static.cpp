/*
 * bq_static.cpp
 *
 *  Created on: 12 окт. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/helpers.h>
#include <test/FloatBuffer.h>

#define BUF_SIZE        1024
#define TOLERANCE       1e-4f

namespace native
{
    void biquad_process_x1(float *dst, const float *src, size_t count, biquad_t *f);
    void biquad_process_x2(float *dst, const float *src, size_t count, biquad_t *f);
    void biquad_process_x4(float *dst, const float *src, size_t count, biquad_t *f);
    void biquad_process_x8(float *dst, const float *src, size_t count, biquad_t *f);
}

IF_ARCH_X86(
    namespace sse
    {
        void biquad_process_x1(float *dst, const float *src, size_t count, biquad_t *f);
        void biquad_process_x2(float *dst, const float *src, size_t count, biquad_t *f);
        void biquad_process_x4(float *dst, const float *src, size_t count, biquad_t *f);
        void biquad_process_x8(float *dst, const float *src, size_t count, biquad_t *f);
    }

    IF_ARCH_X86_64(
        namespace sse3
        {
            void x64_biquad_process_x2(float *dst, const float *src, size_t count, biquad_t *f);
            void x64_biquad_process_x8(float *dst, const float *src, size_t count, biquad_t *f);
        }

        namespace avx
        {
            void x64_biquad_process_x8(float *dst, const float *src, size_t count, biquad_t *f);
            void x64_biquad_process_x8_fma3(float *dst, const float *src, size_t count, biquad_t *f);
        }
    )
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void biquad_process_x1(float *dst, const float *src, size_t count, biquad_t *f);
        void biquad_process_x2(float *dst, const float *src, size_t count, biquad_t *f);
//        void biquad_process_x4(float *dst, const float *src, size_t count, biquad_t *f);
//        void biquad_process_x8(float *dst, const float *src, size_t count, biquad_t *f);
    }
)

typedef void (* biquad_process_t)(float *dst, const float *src, size_t count, biquad_t *f);

UTEST_BEGIN("dsp.filters", static)

    void call(const char *label, const biquad_t *bq, biquad_process_t func1, biquad_process_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        printf("Testing %s on buffer size %d...\n", label, BUF_SIZE);

        biquad_t f1 = *bq, f2 = *bq;

        FloatBuffer src(BUF_SIZE);
        FloatBuffer dst1(BUF_SIZE);
        FloatBuffer dst2(BUF_SIZE);

        func1(dst1, src, BUF_SIZE, &f1);
        func2(dst2, src, BUF_SIZE, &f2);

        UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
        UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
        UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");
        if (!dst1.equals_absolute(dst2, TOLERANCE))
        {
            src.dump("src");
            dst1.dump("dst1");
            dst2.dump("dst2");
            UTEST_FAIL_MSG("Output of functions for test '%s' differs at sample %d: %.6f vs %.6f",
                    label, int(dst1.last_diff()), dst1.get_diff(), dst2.get_diff());
        }

        for (size_t i=0; i<BIQUAD_D_ITEMS; ++i)
        {
            if (float_equals_absolute(f1.d[i], f2.d[i], TOLERANCE))
                continue;
            UTEST_FAIL_MSG("Filter memory items #%d for test '%s' differ: %.6f vs %.6f",
                    int(i), label, f1.d[i], f2.d[i]);
        }
    }


    UTEST_MAIN
    {
        biquad_t bq __lsp_aligned64;
        dsp::fill_zero(bq.d, BIQUAD_D_ITEMS);

        // Prepare simple 2 zero, 2 pole hi-pass filter
        biquad_x1_t *x1 = &bq.x1;
        x1->a[0]    = 0.992303491f;
        x1->a[1]    = 0.992303491f;
        x1->a[2]    = -1.98460698f;
        x1->a[3]    = 0.992303491f;

        x1->b[0]    = 1.98398674f;
        x1->b[1]    = -0.985227287f;
        x1->b[2]    = 0.0f;
        x1->b[3]    = 0.0f;

        IF_ARCH_X86(call("sse::biquad_process_x1", &bq, native::biquad_process_x1, sse::biquad_process_x1));
        IF_ARCH_ARM(call("neon_d32::biquad_process_x1", &bq, native::biquad_process_x1, neon_d32::biquad_process_x1));

        // Prepare simple 4 zero, 4 pole shelving filter
        biquad_x2_t *x2 = &bq.x2;
        x2->a[0]    = 0.346979439f;
        x2->a[1]    = 0.346979439f;
        x2->a[2]    = -0.683136344f;
        x2->a[3]    = 0.337956876f;
        x2->a[4]    = 0.704830527f;
        x2->a[5]    = 0.704830527f;
        x2->a[6]    = -1.38767684f;
        x2->a[7]    = 0.686502695f;

        x2->b[0]    = 1.97910678;
        x2->b[1]    = -0.981672168;
        x2->b[2]    = 0.0f;
        x2->b[3]    = 0.0f;
        x2->b[4]    = 1.97910678;
        x2->b[5]    = -0.981672168;
        x2->b[6]    = 0.0f;
        x2->b[7]    = 0.0f;

        IF_ARCH_X86(call("sse::biquad_process_x2", &bq, native::biquad_process_x2, sse::biquad_process_x2));
        IF_ARCH_X86_64(call("sse3::x64_biquad_process_x2", &bq, native::biquad_process_x2, sse3::x64_biquad_process_x2));
        IF_ARCH_ARM(call("neon_d32::biquad_process_x2", &bq, native::biquad_process_x2, neon_d32::biquad_process_x2));

        // TODO
        // Prepare simple 8 zero, 8 pole shelving filter
        // a0 = 0.515558779, 0.878978848, 0.878978848, 0.878978848
        // a1 = -0.994858623, -1.69613969, -1.69613969, -1.69613969
        // a2 = 0.481613606, 0.821105599, 0.821105599, 0.821105599
        // b1 = 1.93867457, 1.93867457, 1.93867457, 1.93867457
        // b2 = -0.942126751, -0.942126751, -0.942126751, -0.942126751

        // Prepare simple 16 zero, 16 pole filter
        // a0 = 1.79906213, 1.16191483, 1.13150513, 1.11161804, 1.79906213, 1.16191483, 1.13150513, 1.11161804
        // a1 = -3.38381839, -2.20469999, -2.18261695, -2.19184852, -3.38381839, -2.20469999, -2.18261695, -2.19184852
        // a2 = 1.59139514, 1.04720736, 1.05562544, 1.08485937, 1.59139514, 1.04720736, 1.05562544, 1.08485937
        // b1 = 1.8580488, 1.88010871, 1.91898823, 1.96808743, 1.8580488, 1.88010871, 1.91898823, 1.96808743
        // b2 = -0.863286555, -0.88529253, -0.924120247, -0.97324127, -0.863286555, -0.88529253, -0.924120247, -0.97324127
    }

UTEST_END
