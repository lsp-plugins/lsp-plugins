/*
 * dynamic.cpp
 *
 *  Created on: 16 окт. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/helpers.h>
#include <core/sugar.h>
#include <test/FloatBuffer.h>

#define BUF_SIZE        1024
#define TOLERANCE       1e-3f

namespace native
{
    void dyn_biquad_process_x1(float *dst, const float *src, float *d, size_t count, const biquad_x1_t *f);
    void dyn_biquad_process_x2(float *dst, const float *src, float *d, size_t count, const biquad_x2_t *f);
    void dyn_biquad_process_x4(float *dst, const float *src, float *d, size_t count, const biquad_x4_t *f);
    void dyn_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const biquad_x8_t *f);
}

IF_ARCH_X86(
    namespace sse
    {
        void dyn_biquad_process_x1(float *dst, const float *src, float *d, size_t count, const biquad_x1_t *f);
        void dyn_biquad_process_x2(float *dst, const float *src, float *d, size_t count, const biquad_x2_t *f);
        void dyn_biquad_process_x4(float *dst, const float *src, float *d, size_t count, const biquad_x4_t *f);
        void dyn_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const biquad_x8_t *f);
    }

    IF_ARCH_X86_64(
        namespace sse3
        {
            void x64_dyn_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const biquad_x8_t *f);
        }

        namespace avx
        {
            void x64_dyn_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const biquad_x8_t *f);
            void x64_dyn_biquad_process_x8_fma3(float *dst, const float *src, float *d, size_t count, const biquad_x8_t *f);
        }
    )
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void dyn_biquad_process_x1(float *dst, const float *src, float *d, size_t count, const biquad_x1_t *f);
        void dyn_biquad_process_x2(float *dst, const float *src, float *d, size_t count, const biquad_x2_t *f);
        void dyn_biquad_process_x4(float *dst, const float *src, float *d, size_t count, const biquad_x4_t *f);
        void dyn_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const biquad_x8_t *f);
    }
)

typedef void (* dyn_biquad_process_x1_t)(float *dst, const float *src, float *d, size_t count, const biquad_x1_t *f);
typedef void (* dyn_biquad_process_x2_t)(float *dst, const float *src, float *d, size_t count, const biquad_x2_t *f);
typedef void (* dyn_biquad_process_x4_t)(float *dst, const float *src, float *d, size_t count, const biquad_x4_t *f);
typedef void (* dyn_biquad_process_x8_t)(float *dst, const float *src, float *d, size_t count, const biquad_x8_t *f);

static biquad_x1_t bq_normal =
{
    { 0.992303491f, 0.992303491f, -1.98460698f, 0.992303491f },
    { 1.98398674f, -0.985227287f, 0.0f, 0.0f }
};

UTEST_BEGIN("dsp.filters", dynamic)

    void call(const char *label, dyn_biquad_process_x1_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        float d[BIQUAD_D_ITEMS];

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 0x1f, 0x40, 0x1ff)
        {
            printf("Testing %s on input buffer size=%d...\n", label, int(count));

            FloatBuffer src(count);
            FloatBuffer dst1(count);
            FloatBuffer dst2(count);
            src.randomize_sign();

            // Initialize filters
            void *p1 = NULL, *p2 = NULL;
            biquad_x1_t *f1 = alloc_aligned<biquad_x1_t>(p1, count, 64);
            biquad_x1_t *f2 = alloc_aligned<biquad_x1_t>(p2, count, 64);
            UTEST_ASSERT_MSG(f1 != NULL, "Out of memory while allocating f1");
            UTEST_ASSERT_MSG(f2 != NULL, "Out of memory while allocating f2");

            for (size_t i=0; i<count; ++i)
                f1[i]       = bq_normal;
            for (size_t i=0; i<count; ++i)
                f2[i]       = bq_normal;

            // Apply processing
            dsp::fill_zero(d, BIQUAD_D_ITEMS);
            native::dyn_biquad_process_x1(dst1, src, d, count, f1);

            dsp::fill_zero(d, BIQUAD_D_ITEMS);
            func(dst2, src, d, count, f2);

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

            free_aligned(p1);
            free_aligned(p2);
        }
    }

    void call(const char *label, dyn_biquad_process_x2_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        float d[BIQUAD_D_ITEMS];

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 0x1f, 0x40, 0x1ff)
        {
            printf("Testing %s on input buffer size=%d...\n", label, int(count));

            FloatBuffer src(count);
            FloatBuffer dst1(count);
            FloatBuffer dst2(count);
            src.randomize_sign();

            // Initialize filters
            void *p1 = NULL, *p2 = NULL;
            biquad_x1_t *f1 = alloc_aligned<biquad_x1_t>(p1, count, 64);
            biquad_x2_t *f2 = alloc_aligned<biquad_x2_t>(p2, count+1, 64);
            UTEST_ASSERT_MSG(f1 != NULL, "Out of memory while allocating f1");
            UTEST_ASSERT_MSG(f2 != NULL, "Out of memory while allocating f2");

            for (size_t i=0; i<count; ++i)
                f1[i]       = bq_normal;
            for (size_t i=0; i<(count+1); ++i)
            {
                f2[i].a[0]  = bq_normal.a[0];
                f2[i].a[1]  = bq_normal.a[1];
                f2[i].a[2]  = bq_normal.a[2];
                f2[i].a[3]  = bq_normal.a[3];
                f2[i].a[4]  = bq_normal.a[0];
                f2[i].a[5]  = bq_normal.a[1];
                f2[i].a[6]  = bq_normal.a[2];
                f2[i].a[7]  = bq_normal.a[3];

                f2[i].b[0]  = bq_normal.b[0];
                f2[i].b[1]  = bq_normal.b[1];
                f2[i].b[2]  = bq_normal.b[2];
                f2[i].b[3]  = bq_normal.b[3];
                f2[i].b[4]  = bq_normal.b[0];
                f2[i].b[5]  = bq_normal.b[1];
                f2[i].b[6]  = bq_normal.b[2];
                f2[i].b[7]  = bq_normal.b[3];
            }

            // Apply processing
            dsp::fill_zero(d, BIQUAD_D_ITEMS);
            native::dyn_biquad_process_x1(dst1, src, d, count, f1);
            dsp::fill_zero(d, BIQUAD_D_ITEMS);
            native::dyn_biquad_process_x1(dst1, dst1, d, count, f1);

            dsp::fill_zero(d, BIQUAD_D_ITEMS);
            func(dst2, src, d, count, f2);

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

            free_aligned(p1);
            free_aligned(p2);
        }
    }

    void call(const char *label, dyn_biquad_process_x4_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        float d[BIQUAD_D_ITEMS];

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 0x1f, 0x40, 0x1ff)
        {
            printf("Testing %s on input buffer size=%d...\n", label, int(count));

            FloatBuffer src(count);
            FloatBuffer dst1(count);
            FloatBuffer dst2(count);
            src.randomize_sign();

            // Initialize filters
            void *p1 = NULL, *p2 = NULL;
            biquad_x1_t *f1 = alloc_aligned<biquad_x1_t>(p1, count, 64);
            biquad_x4_t *f2 = alloc_aligned<biquad_x4_t>(p2, count+3, 64);
            UTEST_ASSERT_MSG(f1 != NULL, "Out of memory while allocating f1");
            UTEST_ASSERT_MSG(f2 != NULL, "Out of memory while allocating f2");

            for (size_t i=0; i<count; ++i)
                f1[i]       = bq_normal;
            for (size_t i=0; i<(count+3); ++i)
            {
                f2[i].a0[0]  = f2[i].a0[1] = f2[i].a0[2] = f2[i].a0[3] = bq_normal.a[1];
                f2[i].a1[0]  = f2[i].a1[1] = f2[i].a1[2] = f2[i].a1[3] = bq_normal.a[2];
                f2[i].a2[0]  = f2[i].a2[1] = f2[i].a2[2] = f2[i].a2[3] = bq_normal.a[3];
                f2[i].b1[0]  = f2[i].b1[1] = f2[i].b1[2] = f2[i].b1[3] = bq_normal.b[0];
                f2[i].b2[0]  = f2[i].b2[1] = f2[i].b2[2] = f2[i].b2[3] = bq_normal.b[1];
            }

            // Apply processing
            dsp::fill_zero(d, BIQUAD_D_ITEMS);
            native::dyn_biquad_process_x1(dst1, src, d, count, f1);
            dsp::fill_zero(d, BIQUAD_D_ITEMS);
            native::dyn_biquad_process_x1(dst1, dst1, d, count, f1);
            dsp::fill_zero(d, BIQUAD_D_ITEMS);
            native::dyn_biquad_process_x1(dst1, dst1, d, count, f1);
            dsp::fill_zero(d, BIQUAD_D_ITEMS);
            native::dyn_biquad_process_x1(dst1, dst1, d, count, f1);

            dsp::fill_zero(d, BIQUAD_D_ITEMS);
            func(dst2, src, d, count, f2);

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

            free_aligned(p1);
            free_aligned(p2);
        }
    }

    void call(const char *label, dyn_biquad_process_x8_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        float d[BIQUAD_D_ITEMS];

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 0x1f, 0x40, 0x1ff)
        {
            printf("Testing %s on input buffer size=%d...\n", label, int(count));

            FloatBuffer src(count);
            FloatBuffer dst1(count);
            FloatBuffer dst2(count);
            src.randomize_sign();

            // Initialize filters
            void *p1 = NULL, *p2 = NULL;
            biquad_x1_t *f1 = alloc_aligned<biquad_x1_t>(p1, count, 64);
            biquad_x8_t *f2 = alloc_aligned<biquad_x8_t>(p2, count+7, 64);
            UTEST_ASSERT_MSG(f1 != NULL, "Out of memory while allocating f1");
            UTEST_ASSERT_MSG(f2 != NULL, "Out of memory while allocating f2");

            for (size_t i=0; i<count; ++i)
                f1[i]       = bq_normal;
            for (size_t i=0; i<(count+7); ++i)
            {
                f2[i].a0[0]  = f2[i].a0[1] = f2[i].a0[2] = f2[i].a0[3] =
                f2[i].a0[4]  = f2[i].a0[5] = f2[i].a0[6] = f2[i].a0[7] = bq_normal.a[1];
                f2[i].a1[0]  = f2[i].a1[1] = f2[i].a1[2] = f2[i].a1[3] =
                f2[i].a1[4]  = f2[i].a1[5] = f2[i].a1[6] = f2[i].a1[7] = bq_normal.a[2];
                f2[i].a2[0]  = f2[i].a2[1] = f2[i].a2[2] = f2[i].a2[3] =
                f2[i].a2[4]  = f2[i].a2[5] = f2[i].a2[6] = f2[i].a2[7] = bq_normal.a[3];
                f2[i].b1[0]  = f2[i].b1[1] = f2[i].b1[2] = f2[i].b1[3] =
                f2[i].b1[4]  = f2[i].b1[5] = f2[i].b1[6] = f2[i].b1[7] = bq_normal.b[0];
                f2[i].b2[0]  = f2[i].b2[1] = f2[i].b2[2] = f2[i].b2[3] =
                f2[i].b2[4]  = f2[i].b2[5] = f2[i].b2[6] = f2[i].b2[7] = bq_normal.b[1];
            }

            // Apply processing
            dsp::fill_zero(d, BIQUAD_D_ITEMS);
            native::dyn_biquad_process_x1(dst1, src, d, count, f1);
            dsp::fill_zero(d, BIQUAD_D_ITEMS);
            native::dyn_biquad_process_x1(dst1, dst1, d, count, f1);
            dsp::fill_zero(d, BIQUAD_D_ITEMS);
            native::dyn_biquad_process_x1(dst1, dst1, d, count, f1);
            dsp::fill_zero(d, BIQUAD_D_ITEMS);
            native::dyn_biquad_process_x1(dst1, dst1, d, count, f1);
            dsp::fill_zero(d, BIQUAD_D_ITEMS);
            native::dyn_biquad_process_x1(dst1, dst1, d, count, f1);
            dsp::fill_zero(d, BIQUAD_D_ITEMS);
            native::dyn_biquad_process_x1(dst1, dst1, d, count, f1);
            dsp::fill_zero(d, BIQUAD_D_ITEMS);
            native::dyn_biquad_process_x1(dst1, dst1, d, count, f1);
            dsp::fill_zero(d, BIQUAD_D_ITEMS);
            native::dyn_biquad_process_x1(dst1, dst1, d, count, f1);

            dsp::fill_zero(d, BIQUAD_D_ITEMS);
            func(dst2, src, d, count, f2);

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

            free_aligned(p1);
            free_aligned(p2);
        }
    }

    UTEST_MAIN
    {
        // Do overall check
        call("native::dyn_biquad_process_x1", native::dyn_biquad_process_x1);
        IF_ARCH_X86(call("sse::dyn_biquad_process_x1", sse::dyn_biquad_process_x1));
        IF_ARCH_ARM(call("neon_d32::biquad_process_x1", neon_d32::dyn_biquad_process_x1));

        call("native::dyn_biquad_process_x2", native::dyn_biquad_process_x2);
        IF_ARCH_X86(call("sse::dyn_biquad_process_x2", sse::dyn_biquad_process_x2));
        IF_ARCH_ARM(call("neon_d32::dyn_biquad_process_x2", neon_d32::dyn_biquad_process_x2));

        call("native::dyn_biquad_process_x4", native::dyn_biquad_process_x4);
        IF_ARCH_X86(call("sse::dyn_biquad_process_x4", sse::dyn_biquad_process_x4));
        IF_ARCH_ARM(call("neon_d32::dyn_biquad_process_x4", neon_d32::dyn_biquad_process_x4));

        call("native::dyn_biquad_process_x8", native::dyn_biquad_process_x8);
        IF_ARCH_X86(call("sse::dyn_biquad_process_x8", sse::dyn_biquad_process_x8));
        IF_ARCH_X86_64(call("sse3::x64_dyn_biquad_process_x8", sse3::x64_dyn_biquad_process_x8));
        IF_ARCH_X86_64(call("avx::x64_dyn_biquad_process_x8", avx::x64_dyn_biquad_process_x8));
        IF_ARCH_X86_64(call("avx::x64_dyn_biquad_process_x8_fma3", avx::x64_dyn_biquad_process_x8_fma3));
        IF_ARCH_ARM(call("neon_d32::dyn_biquad_process_x8", neon_d32::dyn_biquad_process_x8));
    }

UTEST_END
