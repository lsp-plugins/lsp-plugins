/*
 * transfer.cpp
 *
 *  Created on: 2 янв. 2020 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/helpers.h>
#include <test/FloatBuffer.h>

#define FREQ_MIN        10.0f
#define FREQ_MAX        24000.0f
#define TOLERANCE       1e-4

namespace native
{
    void filter_transfer_calc_ri(float *re, float *im, const f_cascade_t *c, const float *freq, size_t count);
    void filter_transfer_apply_ri(float *re, float *im, const f_cascade_t *c, const float *freq, size_t count);
    void filter_transfer_calc_pc(float *dst, const f_cascade_t *c, const float *freq, size_t count);
    void filter_transfer_apply_pc(float *dst, const f_cascade_t *c, const float *freq, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void filter_transfer_calc_ri(float *re, float *im, const f_cascade_t *c, const float *freq, size_t count);
        void filter_transfer_apply_ri(float *re, float *im, const f_cascade_t *c, const float *freq, size_t count);
        void filter_transfer_calc_pc(float *dst, const f_cascade_t *c, const float *freq, size_t count);
        void filter_transfer_apply_pc(float *dst, const f_cascade_t *c, const float *freq, size_t count);
    }

    namespace avx
    {
        void filter_transfer_calc_ri(float *re, float *im, const f_cascade_t *c, const float *freq, size_t count);
        void filter_transfer_apply_ri(float *re, float *im, const f_cascade_t *c, const float *freq, size_t count);
        void filter_transfer_calc_pc(float *dst, const f_cascade_t *c, const float *freq, size_t count);
        void filter_transfer_apply_pc(float *dst, const f_cascade_t *c, const float *freq, size_t count);

        void filter_transfer_calc_ri_fma3(float *re, float *im, const f_cascade_t *c, const float *freq, size_t count);
        void filter_transfer_apply_ri_fma3(float *re, float *im, const f_cascade_t *c, const float *freq, size_t count);
        void filter_transfer_calc_pc_fma3(float *dst, const f_cascade_t *c, const float *freq, size_t count);
        void filter_transfer_apply_pc_fma3(float *dst, const f_cascade_t *c, const float *freq, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void filter_transfer_calc_ri(float *re, float *im, const f_cascade_t *c, const float *freq, size_t count);
        void filter_transfer_apply_ri(float *re, float *im, const f_cascade_t *c, const float *freq, size_t count);
        void filter_transfer_calc_pc(float *dst, const f_cascade_t *c, const float *freq, size_t count);
        void filter_transfer_apply_pc(float *dst, const f_cascade_t *c, const float *freq, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void filter_transfer_calc_ri(float *re, float *im, const f_cascade_t *c, const float *freq, size_t count);
        void filter_transfer_apply_ri(float *re, float *im, const f_cascade_t *c, const float *freq, size_t count);
        void filter_transfer_calc_pc(float *dst, const f_cascade_t *c, const float *freq, size_t count);
        void filter_transfer_apply_pc(float *dst, const f_cascade_t *c, const float *freq, size_t count);
    }
)

typedef void (* filter_transfer_calc_ri_t)(float *re, float *im, const f_cascade_t *c, const float *freq, size_t count);
typedef void (* filter_transfer_calc_pc_t)(float *dst, const f_cascade_t *c, const float *freq, size_t count);

UTEST_BEGIN("dsp.filters", transfer)

    void call(const char *label, filter_transfer_calc_ri_t func1, filter_transfer_calc_ri_t func2, const f_cascade_t *c, size_t align)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 0x1f, 0x40, 0x1ff)
        {
            for (size_t mask=0; mask <= 0x07; ++mask)
            {
                FloatBuffer src(count, align, mask & 0x01);
                FloatBuffer dst1_re(count, align, mask & 0x02);
                FloatBuffer dst1_im(count, align, mask & 0x04);
                dst1_re.randomize_sign();
                dst1_im.randomize_sign();
                FloatBuffer dst2_re(dst1_re);
                FloatBuffer dst2_im(dst1_im);

                printf("Testing %s on input buffer size=%d, mask=0x%x...\n", label, int(count), int(mask));

                // Generate set of frequencies
                float *ptr = src.data();
                float f0    = logf(FREQ_MIN);
                float delta = logf(FREQ_MAX/FREQ_MIN) / count;
                for (size_t i=0; i<count; ++i)
                    ptr[i] = expf(f0 + delta * i);

                func1(dst1_re, dst1_im, c, src, count);
                func2(dst2_re, dst2_im, c, src, count);

                // Perform validation
                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
                UTEST_ASSERT_MSG(dst1_re.valid(), "dst1_re corrupted");
                UTEST_ASSERT_MSG(dst1_im.valid(), "dst1_im corrupted");
                UTEST_ASSERT_MSG(dst2_re.valid(), "dst2_re corrupted");
                UTEST_ASSERT_MSG(dst2_im.valid(), "dst2_im corrupted");

                if ((!dst1_re.equals_adaptive(dst2_re, TOLERANCE)) ||
                    (!dst1_im.equals_adaptive(dst2_im, TOLERANCE)))
                {
                    src.dump("src");
                    dst1_re.dump("re1");
                    dst1_im.dump("im1");
                    dst2_re.dump("re2");
                    dst2_im.dump("im2");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differ", label);
                }
            }
        }
    }

    void call(const char *label, filter_transfer_calc_pc_t func1, filter_transfer_calc_pc_t func2, const f_cascade_t *c, size_t align)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 0x1f, 0x40, 0x1ff)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                FloatBuffer src(count, align, mask & 0x01);
                FloatBuffer dst1(count*2, align, mask & 0x02);
                dst1.randomize_sign();
                FloatBuffer dst2(dst1);

                printf("Testing %s on input buffer size=%d, mask=0x%x...\n", label, int(count), int(mask));

                // Generate set of frequencies
                float *ptr  = src.data();
                float f0    = logf(FREQ_MIN);
                float delta = logf(FREQ_MAX/FREQ_MIN) / count;
                for (size_t i=0; i<count; ++i)
                    ptr[i] = expf(f0 + delta * i);

                func1(dst1, c, src, count);
                func2(dst2, c, src, count);

                // Perform validation
                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "dst1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "dst2 corrupted");

                if (!dst1.equals_adaptive(dst2, TOLERANCE))
                {
                    src.dump("src ");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differ", label);
                }
            }
        }
    }

    UTEST_MAIN
    {
        // H[p]=(2+0.001*p+0.000001*p^2)/(1.5+0.01*p+0.0001*p^2);
        f_cascade_t fc;
        fc.t[0] = 2.0f;
        fc.t[1] = 1e-3f;
        fc.t[2] = 1e-6f;
        fc.t[3] = 0.0f;
        fc.b[0] = 1.5f;
        fc.b[1] = 1e-2f;
        fc.b[2] = 1e-4f;
        fc.b[3] = 0.0f;

        #define CALL(native, func, align) \
            call(#func, native, func, &fc, align)

        IF_ARCH_X86(CALL(native::filter_transfer_calc_ri, sse::filter_transfer_calc_ri, 16));
        IF_ARCH_X86(CALL(native::filter_transfer_apply_ri, sse::filter_transfer_apply_ri, 16));
        IF_ARCH_X86(CALL(native::filter_transfer_calc_pc, sse::filter_transfer_calc_pc, 16));
        IF_ARCH_X86(CALL(native::filter_transfer_apply_pc, sse::filter_transfer_apply_pc, 16));

        IF_ARCH_X86(CALL(native::filter_transfer_calc_ri, avx::filter_transfer_calc_ri, 32));
        IF_ARCH_X86(CALL(native::filter_transfer_apply_ri, avx::filter_transfer_apply_ri, 32));
        IF_ARCH_X86(CALL(native::filter_transfer_calc_pc, avx::filter_transfer_calc_pc, 32));
        IF_ARCH_X86(CALL(native::filter_transfer_apply_pc, avx::filter_transfer_apply_pc, 32));

        IF_ARCH_X86(CALL(native::filter_transfer_calc_ri, avx::filter_transfer_calc_ri_fma3, 32));
        IF_ARCH_X86(CALL(native::filter_transfer_apply_ri, avx::filter_transfer_apply_ri_fma3, 32));
        IF_ARCH_X86(CALL(native::filter_transfer_calc_pc, avx::filter_transfer_calc_pc_fma3, 32));
        IF_ARCH_X86(CALL(native::filter_transfer_apply_pc, avx::filter_transfer_apply_pc_fma3, 32));

        IF_ARCH_ARM(CALL(native::filter_transfer_calc_ri, neon_d32::filter_transfer_calc_ri, 16));
        IF_ARCH_ARM(CALL(native::filter_transfer_apply_ri, neon_d32::filter_transfer_apply_ri, 16));
        IF_ARCH_ARM(CALL(native::filter_transfer_calc_pc, neon_d32::filter_transfer_calc_pc, 16));
        IF_ARCH_ARM(CALL(native::filter_transfer_apply_pc, neon_d32::filter_transfer_apply_pc, 16));

        IF_ARCH_AARCH64(CALL(native::filter_transfer_calc_ri, asimd::filter_transfer_calc_ri, 16));
        IF_ARCH_AARCH64(CALL(native::filter_transfer_apply_ri, asimd::filter_transfer_apply_ri, 16));
        IF_ARCH_AARCH64(CALL(native::filter_transfer_calc_pc, asimd::filter_transfer_calc_pc, 16));
        IF_ARCH_AARCH64(CALL(native::filter_transfer_apply_pc, asimd::filter_transfer_apply_pc, 16));
    }

UTEST_END



