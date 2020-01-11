/*
 * fmop_k4.cpp
 *
 *  Created on: 23 авг. 2018 г.
 *      Author: sadko
 */


#include <dsp/dsp.h>
#include <test/ptest.h>
#include <test/helpers.h>
#include <core/sugar.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace native
{
    void    fmadd_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
    void    fmsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
    void    fmrsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
    void    fmmul_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
    void    fmdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
    void    fmrdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
    void    fmmod_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
    void    fmrmod_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void    fmadd_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmrsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmmul_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmrdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
    }

    namespace sse2
    {
        void    fmmod_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmrmod_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
    }

    namespace avx
    {
        void    fmadd_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmrsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmmul_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmrdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmmod_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmrmod_k4(float *dst, const float *src1, const float *src2, float k, size_t count);

        void    fmadd_k4_fma3(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmsub_k4_fma3(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmrsub_k4_fma3(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmmod_k4_fma3(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmrmod_k4_fma3(float *dst, const float *src1, const float *src2, float k, size_t count);
    }

    namespace avx2
    {
        void    fmadd_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmrsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmmul_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmrdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmmod_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmrmod_k4(float *dst, const float *src1, const float *src2, float k, size_t count);

        void    fmadd_k4_fma3(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmsub_k4_fma3(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmrsub_k4_fma3(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmmod_k4_fma3(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmrmod_k4_fma3(float *dst, const float *src1, const float *src2, float k, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void    fmadd_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmrsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmmul_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmrdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmmod_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmrmod_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void    fmadd_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmrsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmmul_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmrdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmmod_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    fmrmod_k4(float *dst, const float *src1, const float *src2, float k, size_t count);
    }
)

typedef void (* fmop_k4_t)(float *dst, const float *src1, const float *src2, float k, size_t count);

//-----------------------------------------------------------------------------
PTEST_BEGIN("dsp.pmath", fmop_k4, 5, 1000)

    void call(const char *label, float *dst, const float *src1, const float *src2, size_t count, fmop_k4_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(dst, src1, src2, 0.5f, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *dst      = alloc_aligned<float>(data, buf_size * 6, 64);
        float *src1     = &dst[buf_size];
        float *src2     = &src1[buf_size];
        float *backup   = &src2[buf_size];

        randomize_sign(dst, buf_size*3);
        dsp::copy(backup, dst, buf_size*3);

        #define CALL(func) \
            dsp::copy(dst, backup, buf_size*3); \
            call(#func, dst, src1, src2, count, func);

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(native::fmadd_k4);
            IF_ARCH_X86(CALL(sse::fmadd_k4));
            IF_ARCH_X86(CALL(avx::fmadd_k4));
            IF_ARCH_X86(CALL(avx::fmadd_k4_fma3));
            IF_ARCH_X86(CALL(avx2::fmadd_k4));
            IF_ARCH_X86(CALL(avx2::fmadd_k4_fma3));
            IF_ARCH_ARM(CALL(neon_d32::fmadd_k4));
            IF_ARCH_AARCH64(CALL(asimd::fmadd_k4));
            PTEST_SEPARATOR;

            CALL(native::fmsub_k4);
            IF_ARCH_X86(CALL(sse::fmsub_k4));
            IF_ARCH_X86(CALL(avx::fmsub_k4));
            IF_ARCH_X86(CALL(avx::fmsub_k4_fma3));
            IF_ARCH_X86(CALL(avx2::fmsub_k4));
            IF_ARCH_X86(CALL(avx2::fmsub_k4_fma3));
            IF_ARCH_ARM(CALL(neon_d32::fmsub_k4));
            IF_ARCH_AARCH64(CALL(asimd::fmsub_k4));
            PTEST_SEPARATOR;

            CALL(native::fmrsub_k4);
            IF_ARCH_X86(CALL(sse::fmrsub_k4));
            IF_ARCH_X86(CALL(avx::fmrsub_k4));
            IF_ARCH_X86(CALL(avx::fmrsub_k4_fma3));
            IF_ARCH_X86(CALL(avx2::fmrsub_k4));
            IF_ARCH_X86(CALL(avx2::fmrsub_k4_fma3));
            IF_ARCH_ARM(CALL(neon_d32::fmrsub_k4));
            IF_ARCH_AARCH64(CALL(asimd::fmrsub_k4));
            PTEST_SEPARATOR;

            CALL(native::fmmul_k4);
            IF_ARCH_X86(CALL(sse::fmmul_k4));
            IF_ARCH_X86(CALL(avx::fmmul_k4));
            IF_ARCH_X86(CALL(avx2::fmmul_k4));
            IF_ARCH_ARM(CALL(neon_d32::fmmul_k4));
            IF_ARCH_AARCH64(CALL(asimd::fmmul_k4));
            PTEST_SEPARATOR;

            CALL(native::fmdiv_k4);
            IF_ARCH_X86(CALL(sse::fmdiv_k4));
            IF_ARCH_X86(CALL(avx::fmdiv_k4));
            IF_ARCH_X86(CALL(avx2::fmdiv_k4));
            IF_ARCH_ARM(CALL(neon_d32::fmdiv_k4));
            IF_ARCH_AARCH64(CALL(asimd::fmdiv_k4));
            PTEST_SEPARATOR;

            CALL(native::fmrdiv_k4);
            IF_ARCH_X86(CALL(sse::fmrdiv_k4));
            IF_ARCH_X86(CALL(avx::fmrdiv_k4));
            IF_ARCH_X86(CALL(avx2::fmrdiv_k4));
            IF_ARCH_ARM(CALL(neon_d32::fmrdiv_k4));
            IF_ARCH_AARCH64(CALL(asimd::fmrdiv_k4));
            PTEST_SEPARATOR;

            CALL(native::fmmod_k4);
            IF_ARCH_X86(CALL(sse2::fmmod_k4));
            IF_ARCH_X86(CALL(avx::fmmod_k4));
            IF_ARCH_X86(CALL(avx::fmmod_k4_fma3));
            IF_ARCH_X86(CALL(avx2::fmmod_k4));
            IF_ARCH_X86(CALL(avx2::fmmod_k4_fma3));
            IF_ARCH_ARM(CALL(neon_d32::fmmod_k4));
            IF_ARCH_AARCH64(CALL(asimd::fmmod_k4));
            PTEST_SEPARATOR;

            CALL(native::fmrmod_k4);
            IF_ARCH_X86(CALL(sse2::fmrmod_k4));
            IF_ARCH_X86(CALL(avx::fmrmod_k4));
            IF_ARCH_X86(CALL(avx::fmrmod_k4_fma3));
            IF_ARCH_X86(CALL(avx2::fmrmod_k4));
            IF_ARCH_X86(CALL(avx2::fmrmod_k4_fma3));
            IF_ARCH_ARM(CALL(neon_d32::fmrmod_k4));
            IF_ARCH_AARCH64(CALL(asimd::fmrmod_k4));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }
PTEST_END


