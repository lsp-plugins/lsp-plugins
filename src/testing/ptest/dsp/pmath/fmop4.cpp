/*
 * fmop4.cpp
 *
 *  Created on: 22 нояб. 2019 г.
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
    void    fmadd4(float *dst, const float *a, const float *b, const float *c, size_t count);
    void    fmsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
    void    fmrsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
    void    fmmul4(float *dst, const float *a, const float *b, const float *c, size_t count);
    void    fmdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
    void    fmrdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
    void    fmmod4(float *dst, const float *a, const float *b, const float *c, size_t count);
    void    fmrmod4(float *dst, const float *a, const float *b, const float *c, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void    fmadd4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmrsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmmul4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmrdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
    }

    namespace sse2
    {
        void    fmmod4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmrmod4(float *dst, const float *a, const float *b, const float *c, size_t count);
    }

    namespace avx
    {
        void    fmadd4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmrsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmmul4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmrdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmmod4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmrmod4(float *dst, const float *a, const float *b, const float *c, size_t count);

        void    fmadd4_fma3(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmsub4_fma3(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmrsub4_fma3(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmmod4_fma3(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmrmod4_fma3(float *dst, const float *a, const float *b, const float *c, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void    fmadd4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmrsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmmul4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmrdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmmod4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmrmod4(float *dst, const float *a, const float *b, const float *c, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void    fmadd4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmrsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmmul4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmrdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmmod4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmrmod4(float *dst, const float *a, const float *b, const float *c, size_t count);
    }
)

typedef void (* fmop4_t)(float *dst, const float *a, const float *b, const float *c, size_t count);

//-----------------------------------------------------------------------------
PTEST_BEGIN("dsp.pmath", fmop4, 5, 1000)

    void call(const char *label, float *dst, const float *a, const float *b, const float *c, size_t count, fmop4_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(dst, a, b, c, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *dst      = alloc_aligned<float>(data, buf_size * 4, 64);
        float *a        = &dst[buf_size];
        float *b        = &a[buf_size];
        float *c        = &b[buf_size];

        randomize_sign(dst, buf_size*4);

        #define CALL(method) \
            call(#method, dst, a, b, c, count, method);

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(native::fmadd4);
            IF_ARCH_X86(CALL(sse::fmadd4));
            IF_ARCH_X86(CALL(avx::fmadd4));
            IF_ARCH_X86(CALL(avx::fmadd4_fma3));
            IF_ARCH_ARM(CALL(neon_d32::fmadd4));
            IF_ARCH_AARCH64(CALL(asimd::fmadd4));
            PTEST_SEPARATOR;

            CALL(native::fmsub4);
            IF_ARCH_X86(CALL(sse::fmsub4));
            IF_ARCH_X86(CALL(avx::fmsub4));
            IF_ARCH_X86(CALL(avx::fmsub4_fma3));
            IF_ARCH_ARM(CALL(neon_d32::fmsub4));
            IF_ARCH_AARCH64(CALL(asimd::fmsub4));
            PTEST_SEPARATOR;

            CALL(native::fmrsub4);
            IF_ARCH_X86(CALL(sse::fmrsub4));
            IF_ARCH_X86(CALL(avx::fmrsub4));
            IF_ARCH_X86(CALL(avx::fmrsub4_fma3));
            IF_ARCH_ARM(CALL(neon_d32::fmrsub4));
            IF_ARCH_AARCH64(CALL(asimd::fmrsub4));
            PTEST_SEPARATOR;

            CALL(native::fmmul4);
            IF_ARCH_X86(CALL(sse::fmmul4));
            IF_ARCH_X86(CALL(avx::fmmul4));
            IF_ARCH_ARM(CALL(neon_d32::fmmul4));
            IF_ARCH_AARCH64(CALL(asimd::fmmul4));
            PTEST_SEPARATOR;

            CALL(native::fmdiv4);
            IF_ARCH_X86(CALL(sse::fmdiv4));
            IF_ARCH_X86(CALL(avx::fmdiv4));
            IF_ARCH_ARM(CALL(neon_d32::fmdiv4));
            IF_ARCH_AARCH64(CALL(asimd::fmdiv4));
            PTEST_SEPARATOR;

            CALL(native::fmrdiv4);
            IF_ARCH_X86(CALL(sse::fmrdiv4));
            IF_ARCH_X86(CALL(avx::fmrdiv4));
            IF_ARCH_ARM(CALL(neon_d32::fmrdiv4));
            IF_ARCH_AARCH64(CALL(asimd::fmrdiv4));
            PTEST_SEPARATOR;

            CALL(native::fmmod4);
            IF_ARCH_X86(CALL(sse2::fmmod4));
            IF_ARCH_X86(CALL(avx::fmmod4));
            IF_ARCH_X86(CALL(avx::fmmod4_fma3));
            IF_ARCH_ARM(CALL(neon_d32::fmmod4));
            IF_ARCH_AARCH64(CALL(asimd::fmmod4));
            PTEST_SEPARATOR;

            CALL(native::fmrmod4);
            IF_ARCH_X86(CALL(sse2::fmrmod4));
            IF_ARCH_X86(CALL(avx::fmrmod4));
            IF_ARCH_X86(CALL(avx::fmrmod4_fma3));
            IF_ARCH_ARM(CALL(neon_d32::fmrmod4));
            IF_ARCH_AARCH64(CALL(asimd::fmrmod4));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }
PTEST_END




