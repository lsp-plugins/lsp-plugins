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

    namespace sse3
    {
        void x64_dyn_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const biquad_x8_t *f);
    }

    namespace avx
    {
        void dyn_biquad_process_x1(float *dst, const float *src, float *d, size_t count, const biquad_x1_t *f);
        void dyn_biquad_process_x1_fma3(float *dst, const float *src, float *d, size_t count, const biquad_x1_t *f);

        void dyn_biquad_process_x2(float *dst, const float *src, float *d, size_t count, const biquad_x2_t *f);
        void dyn_biquad_process_x2_fma3(float *dst, const float *src, float *d, size_t count, const biquad_x2_t *f);

        void dyn_biquad_process_x4(float *dst, const float *src, float *d, size_t count, const biquad_x4_t *f);
        void dyn_biquad_process_x4_fma3(float *dst, const float *src, float *d, size_t count, const biquad_x4_t *f);

        void x64_dyn_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const biquad_x8_t *f);
        void dyn_biquad_process_x8_fma3(float *dst, const float *src, float *d, size_t count, const biquad_x8_t *f);
    }
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

IF_ARCH_AARCH64(
    namespace asimd
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
    0.992303491f, -1.98460698f, 0.992303491f, // a0 - a2
    1.98398674f, -0.985227287f, // b1-b2
    0.0f, 0.0f, 0.0f // padding
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
                for (size_t j=0; j<2; ++j)
                {
                    f2[i].b0[j] = bq_normal.b0;
                    f2[i].b1[j] = bq_normal.b1;
                    f2[i].b2[j] = bq_normal.b2;
                    f2[i].a1[j] = bq_normal.a1;
                    f2[i].a2[j] = bq_normal.a2;
                    f2[i].p[j]  = 0.0f;
                }
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
                for (size_t j=0; j<4; ++j)
                {
                    f2[i].b0[j] = bq_normal.b0;
                    f2[i].b1[j] = bq_normal.b1;
                    f2[i].b2[j] = bq_normal.b2;
                    f2[i].a1[j] = bq_normal.a1;
                    f2[i].a2[j] = bq_normal.a2;
                }
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
                for (size_t j=0; j<8; ++j)
                {
                    f2[i].b0[j] = bq_normal.b0;
                    f2[i].b1[j] = bq_normal.b1;
                    f2[i].b2[j] = bq_normal.b2;
                    f2[i].a1[j] = bq_normal.a1;
                    f2[i].a2[j] = bq_normal.a2;
                }
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
        #define CALL(func) \
            call(#func, func)

        // Do overall check
        CALL(native::dyn_biquad_process_x1);
        IF_ARCH_X86(CALL(sse::dyn_biquad_process_x1));
        IF_ARCH_X86(CALL(avx::dyn_biquad_process_x1));
        IF_ARCH_X86(CALL(avx::dyn_biquad_process_x1_fma3));
        IF_ARCH_ARM(CALL(neon_d32::dyn_biquad_process_x1));
        IF_ARCH_AARCH64(CALL(asimd::dyn_biquad_process_x1));

        CALL( native::dyn_biquad_process_x2);
        IF_ARCH_X86(CALL(sse::dyn_biquad_process_x2));
        IF_ARCH_X86(CALL(avx::dyn_biquad_process_x2));
        IF_ARCH_X86(CALL(avx::dyn_biquad_process_x2_fma3));
        IF_ARCH_ARM(CALL(neon_d32::dyn_biquad_process_x2));
        IF_ARCH_AARCH64(CALL(asimd::dyn_biquad_process_x2));

        CALL( native::dyn_biquad_process_x4);
        IF_ARCH_X86(CALL(sse::dyn_biquad_process_x4));
        IF_ARCH_X86(CALL(avx::dyn_biquad_process_x4));
        IF_ARCH_X86(CALL(avx::dyn_biquad_process_x4_fma3));
        IF_ARCH_ARM(CALL(neon_d32::dyn_biquad_process_x4));
        IF_ARCH_AARCH64(CALL(asimd::dyn_biquad_process_x4));

        CALL(native::dyn_biquad_process_x8);
        IF_ARCH_X86(CALL(sse::dyn_biquad_process_x8));
        IF_ARCH_X86(CALL(sse3::x64_dyn_biquad_process_x8));
        IF_ARCH_X86(CALL(avx::x64_dyn_biquad_process_x8));
        IF_ARCH_X86(CALL(avx::dyn_biquad_process_x8_fma3));
        IF_ARCH_ARM(CALL(neon_d32::dyn_biquad_process_x8));
        IF_ARCH_AARCH64(CALL(asimd::dyn_biquad_process_x8));
    }

UTEST_END
