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
#define TOLERANCE       1e-3f

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
        void biquad_process_x4(float *dst, const float *src, size_t count, biquad_t *f);
        void biquad_process_x8(float *dst, const float *src, size_t count, biquad_t *f);
    }
)

typedef void (* biquad_process_t)(float *dst, const float *src, size_t count, biquad_t *f);

UTEST_BEGIN("dsp.filters", static)

    void call(const char *label, biquad_process_t func, size_t n)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        biquad_t f1, f2;

        // Initialize biquad filter
        biquad_x1_t *x1 = &f1.x1;
        x1->a[0]    = 0.992303491f;
        x1->a[1]    = 0.992303491f;
        x1->a[2]    = -1.98460698f;
        x1->a[3]    = 0.992303491f;

        x1->b[0]    = 1.98398674f;
        x1->b[1]    = -0.985227287f;
        x1->b[2]    = 0.0f;
        x1->b[3]    = 0.0f;

        if (n == 1)
            f2.x1       = f1.x1;
        else if (n == 2)
        {
            biquad_x2_t *x2 = &f2.x2;
            dsp::copy(x2->a, x1->a, 4);
            dsp::copy(&x2->a[4], x1->a, 4);
            dsp::copy(x2->b, x1->b, 4);
            dsp::copy(&x2->b[4], x1->b, 4);
        }
        else if (n == 4)
        {
            biquad_x4_t *x4 = &f2.x4;
            for (size_t i=0; i<n; ++i)
            {
                x4->a0[i]   = x1->a[1];
                x4->a1[i]   = x1->a[2];
                x4->a2[i]   = x1->a[3];
                x4->b1[i]   = x1->b[0];
                x4->b2[i]   = x1->b[1];
            }
        }
        else if (n == 8)
        {
            biquad_x8_t *x8 = &f2.x8;
            for (size_t i=0; i<n; ++i)
            {
                x8->a0[i]   = x1->a[1];
                x8->a1[i]   = x1->a[2];
                x8->a2[i]   = x1->a[3];
                x8->b1[i]   = x1->b[0];
                x8->b2[i]   = x1->b[1];
            }
        }

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 0x1f, 0x40, 0x1ff)
//        size_t count = 4;
        {
            FloatBuffer src(count);
            FloatBuffer dst1(count);
            FloatBuffer dst2(count);
            src.randomize_sign();

            printf("Testing %s on input buffer size=%d...\n", label, int(count));

            // Apply processing
            float *ptr = src.data();
            for (size_t i=0; i<n; ++i)
            {
                dsp::fill_zero(f1.d, BIQUAD_D_ITEMS);
                native::biquad_process_x1(dst1, ptr, count, &f1);
                ptr = dst1.data();
            }

            dsp::fill_zero(f2.d, BIQUAD_D_ITEMS);
            func(dst2, src, count, &f2);

            // Perform validation
            UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
            UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
            UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

            if (!dst1.equals_adaptive(dst2, TOLERANCE))
            {
                src.dump("src");
                dst1.dump("dst1");
                dst2.dump("dst2");
                UTEST_FAIL_MSG("Output of functions for test '%s' differs at sample %d: %.6f vs %.6f",
                        label, int(dst1.last_diff()), dst1.get_diff(), dst2.get_diff());
            }
        }
    }

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
        if (!dst1.equals_adaptive(dst2, TOLERANCE))
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
        // PART 1, overall check
        call("native::biquad_process_x1", native::biquad_process_x1, 1);
        IF_ARCH_X86(call("sse::biquad_process_x1", sse::biquad_process_x1, 1));
        IF_ARCH_ARM(call("neon_d32::biquad_process_x1", neon_d32::biquad_process_x1, 1));

        call("native::biquad_process_x2", native::biquad_process_x2, 2);
        IF_ARCH_X86(call("sse::biquad_process_x2", sse::biquad_process_x2, 2));
        IF_ARCH_X86_64(call("sse3::x64_biquad_process_x2", sse3::x64_biquad_process_x2, 2));
        IF_ARCH_ARM(call("neon_d32::biquad_process_x2", neon_d32::biquad_process_x2, 2));

        call("native::biquad_process_x4", native::biquad_process_x4, 4);
        IF_ARCH_X86(call("sse::biquad_process_x4", sse::biquad_process_x4, 4));
        IF_ARCH_ARM(call("neon_d32::biquad_process_x4", neon_d32::biquad_process_x4, 4));

        call("native::biquad_process_x8", native::biquad_process_x8, 8);
        IF_ARCH_X86(call("sse::biquad_process_x8", sse::biquad_process_x8, 8));
        IF_ARCH_X86_64(call("sse3::x64_biquad_process_x8", sse3::x64_biquad_process_x8, 8));
        IF_ARCH_X86_64(call("avx::x64_biquad_process_x8", avx::x64_biquad_process_x8, 8));
        IF_ARCH_X86_64(call("avx::x64_biquad_process_x8_fma3", avx::x64_biquad_process_x8_fma3, 8));
        IF_ARCH_ARM(call("neon_d32::biquad_process_x8", neon_d32::biquad_process_x8, 8));

        // PART 2
        biquad_t bq __lsp_aligned64;
        dsp::fill_zero(bq.d, BIQUAD_D_ITEMS);

        // Prepare 2 zero, 2 pole hi-pass filter
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

        // Prepare 4 zero, 4 pole shelving filter
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

        // Prepare 8 zero, 8 pole shelving filter
        biquad_x4_t *x4 = &bq.x4;

        x4->a0[0]   = 0.515558779;
        x4->a0[1]   = 0.878978848;
        x4->a0[2]   = 0.878978848;
        x4->a0[3]   = 0.878978848;

        x4->a1[0]   = -0.994858623;
        x4->a1[1]   = -1.69613969;
        x4->a1[2]   = -1.69613969;
        x4->a1[3]   = -1.69613969;

        x4->a2[0]   = 0.481613606;
        x4->a2[1]   = 0.821105599;
        x4->a2[2]   = 0.821105599;
        x4->a2[3]   = 0.821105599;

        x4->b1[0]   = 1.93867457;
        x4->b1[1]   = 1.93867457;
        x4->b1[2]   = 1.93867457;
        x4->b1[3]   = 1.93867457;

        x4->b2[0]   = -0.942126751;
        x4->b2[1]   = -0.942126751;
        x4->b2[2]   = -0.942126751;
        x4->b2[3]   = -0.942126751;

        IF_ARCH_X86(call("sse::biquad_process_x4", &bq, native::biquad_process_x4, sse::biquad_process_x4));
        IF_ARCH_ARM(call("neon_d32::biquad_process_x4", &bq, native::biquad_process_x4, neon_d32::biquad_process_x4));

        // Prepare simple 16 zero, 16 pole filter
        biquad_x8_t *x8 = &bq.x8;

        x8->a0[0]   = 1.79906213f;
        x8->a0[1]   = 1.16191483f;
        x8->a0[2]   = 1.13150513f;
        x8->a0[3]   = 1.11161804f;
        x8->a0[4]   = 1.79906213f;
        x8->a0[5]   = 1.16191483f;
        x8->a0[6]   = 1.13150513f;
        x8->a0[7]   = 1.11161804f;

        x8->a1[0]   = -3.38381839f;
        x8->a1[1]   = -2.20469999f;
        x8->a1[2]   = -2.18261695f;
        x8->a1[3]   = -2.19184852f;
        x8->a1[4]   = -3.38381839f;
        x8->a1[5]   = -2.20469999f;
        x8->a1[6]   = -2.18261695f;
        x8->a1[7]   = -2.19184852f;

        x8->a2[0]   = 1.59139514f;
        x8->a2[1]   = 1.04720736f;
        x8->a2[2]   = 1.05562544f;
        x8->a2[3]   = 1.08485937f;
        x8->a2[4]   = 1.59139514f;
        x8->a2[5]   = 1.04720736f;
        x8->a2[6]   = 1.05562544f;
        x8->a2[7]   = 1.08485937f;

        x8->b1[0]   = 1.8580488f;
        x8->b1[1]   = 1.88010871f;
        x8->b1[2]   = 1.91898823f;
        x8->b1[3]   = 1.96808743f;
        x8->b1[4]   = 1.8580488f;
        x8->b1[5]   = 1.88010871f;
        x8->b1[6]   = 1.91898823f;
        x8->b1[7]   = 1.96808743f;

        x8->b2[0]   = -0.863286555f;
        x8->b2[1]   = -0.88529253f;
        x8->b2[2]   = -0.924120247f;
        x8->b2[3]   = -0.97324127f;
        x8->b2[4]   = -0.863286555f;
        x8->b2[5]   = -0.88529253f;
        x8->b2[6]   = -0.924120247f;
        x8->b2[7]   = -0.97324127f;

        IF_ARCH_X86(call("sse::biquad_process_x8", &bq, native::biquad_process_x8, sse::biquad_process_x8));
        IF_ARCH_X86_64(call("sse3::x64_biquad_process_x8", &bq, native::biquad_process_x8, sse3::x64_biquad_process_x8));
        IF_ARCH_X86_64(call("avx::x64_biquad_process_x8", &bq, native::biquad_process_x8, avx::x64_biquad_process_x8));
        IF_ARCH_X86_64(call("avx::x64_biquad_process_x8_fma3", &bq, native::biquad_process_x8, avx::x64_biquad_process_x8_fma3));
        IF_ARCH_ARM(call("neon_d32::biquad_process_x8", &bq, native::biquad_process_x8, neon_d32::biquad_process_x8));
    }

UTEST_END
