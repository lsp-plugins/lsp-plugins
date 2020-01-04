/*
 * transfer.cpp
 *
 *  Created on: 2 янв. 2020 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>
#include <test/helpers.h>

#define MIN_RANK        6
#define MAX_RANK        12

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

//-----------------------------------------------------------------------------
// Performance test for static biquad processing
PTEST_BEGIN("dsp.filters", transfer, 5, 1000)

    void call(const char *text, float *re, float *im, const float *in, const f_cascade_t *fc, filter_transfer_calc_ri_t func, size_t count)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        printf("Testing %s static filters on input buffer of %d samples ...\n", text, int(count));

        PTEST_LOOP(text,
            func(re, im, fc, in, count);
        );
    }

    void call(const char *text, float *out, const float *in, const f_cascade_t *fc, filter_transfer_calc_pc_t func, size_t count)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        printf("Testing %s static filters on input buffer of %d samples ...\n", text, int(count));

        PTEST_LOOP(text,
            func(out, fc, in, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *dst      = alloc_aligned<float>(data, buf_size * 5, 64);
        float *src      = &dst[buf_size*2];
        float *backup   = &src[buf_size];

        randomize_sign(backup, buf_size*2);
        float step      = logf(FREQ_MAX/FREQ_MIN) / buf_size;
        for (size_t i=0; i<buf_size; ++i)
            src[i] = FREQ_MIN * expf( i * step );

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

        #define CALL1(func) \
            dsp::copy(dst, backup, buf_size); \
            call(#func, dst, &dst[buf_size], src, &fc, func, count);

        #define CALL2(func) \
            dsp::copy(dst, backup, buf_size); \
            call(#func, dst, src, &fc, func, count);

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL1(native::filter_transfer_calc_ri);
            IF_ARCH_X86(CALL1(sse::filter_transfer_calc_ri));
            IF_ARCH_X86(CALL1(avx::filter_transfer_calc_ri));
            IF_ARCH_X86(CALL1(avx::filter_transfer_calc_ri_fma3));
            IF_ARCH_ARM(CALL1(neon_d32::filter_transfer_calc_ri));
            IF_ARCH_AARCH64(CALL1(asimd::filter_transfer_calc_ri));
            PTEST_SEPARATOR;

            CALL1(native::filter_transfer_apply_ri);
            IF_ARCH_X86(CALL1(sse::filter_transfer_apply_ri));
            IF_ARCH_X86(CALL1(avx::filter_transfer_apply_ri));
            IF_ARCH_X86(CALL1(avx::filter_transfer_apply_ri_fma3));
            IF_ARCH_ARM(CALL1(neon_d32::filter_transfer_apply_ri));
            IF_ARCH_AARCH64(CALL1(asimd::filter_transfer_apply_ri));
            PTEST_SEPARATOR;

            CALL2(native::filter_transfer_calc_pc);
            IF_ARCH_X86(CALL2(sse::filter_transfer_calc_pc));
            IF_ARCH_X86(CALL2(avx::filter_transfer_calc_pc));
            IF_ARCH_X86(CALL2(avx::filter_transfer_calc_pc_fma3));
            IF_ARCH_ARM(CALL2(neon_d32::filter_transfer_calc_pc));
            IF_ARCH_AARCH64(CALL2(asimd::filter_transfer_calc_pc));
            PTEST_SEPARATOR;

            CALL2(native::filter_transfer_apply_pc);
            IF_ARCH_X86(CALL2(sse::filter_transfer_apply_pc));
            IF_ARCH_X86(CALL2(avx::filter_transfer_apply_pc));
            IF_ARCH_X86(CALL2(avx::filter_transfer_apply_pc_fma3));
            IF_ARCH_ARM(CALL2(neon_d32::filter_transfer_apply_pc));
            IF_ARCH_AARCH64(CALL2(asimd::filter_transfer_apply_pc));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }

PTEST_END


