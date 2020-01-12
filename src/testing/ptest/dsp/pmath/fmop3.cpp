/*
 * fmop3.cpp
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
    void    fmadd3(float *dst, const float *a, const float *b, size_t count);
    void    fmsub3(float *dst, const float *a, const float *b, size_t count);
    void    fmrsub3(float *dst, const float *a, const float *b, size_t count);
    void    fmmul3(float *dst, const float *a, const float *b, size_t count);
    void    fmdiv3(float *dst, const float *a, const float *b, size_t count);
    void    fmrdiv3(float *dst, const float *a, const float *b, size_t count);
    void    fmmod3(float *dst, const float *a, const float *b, size_t count);
    void    fmrmod3(float *dst, const float *a, const float *b, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void    fmadd3(float *dst, const float *a, const float *b, size_t count);
        void    fmsub3(float *dst, const float *a, const float *b, size_t count);
        void    fmrsub3(float *dst, const float *a, const float *b, size_t count);
        void    fmmul3(float *dst, const float *a, const float *b, size_t count);
        void    fmdiv3(float *dst, const float *a, const float *b, size_t count);
        void    fmrdiv3(float *dst, const float *a, const float *b, size_t count);
    }

    namespace sse2
    {
        void    fmmod3(float *dst, const float *a, const float *b, size_t count);
        void    fmrmod3(float *dst, const float *a, const float *b, size_t count);
    }

    namespace avx
    {
        void    fmadd3(float *dst, const float *a, const float *b, size_t count);
        void    fmsub3(float *dst, const float *a, const float *b, size_t count);
        void    fmrsub3(float *dst, const float *a, const float *b, size_t count);
        void    fmmul3(float *dst, const float *a, const float *b, size_t count);
        void    fmdiv3(float *dst, const float *a, const float *b, size_t count);
        void    fmrdiv3(float *dst, const float *a, const float *b, size_t count);
        void    fmmod3(float *dst, const float *a, const float *b, size_t count);
        void    fmrmod3(float *dst, const float *a, const float *b, size_t count);

        void    fmadd3_fma3(float *dst, const float *a, const float *b, size_t count);
        void    fmsub3_fma3(float *dst, const float *a, const float *b, size_t count);
        void    fmrsub3_fma3(float *dst, const float *a, const float *b, size_t count);
        void    fmmod3_fma3(float *dst, const float *a, const float *b, size_t count);
        void    fmrmod3_fma3(float *dst, const float *a, const float *b, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void    fmadd3(float *dst, const float *a, const float *b, size_t count);
        void    fmsub3(float *dst, const float *a, const float *b, size_t count);
        void    fmrsub3(float *dst, const float *a, const float *b, size_t count);
        void    fmmul3(float *dst, const float *a, const float *b, size_t count);
        void    fmdiv3(float *dst, const float *a, const float *b, size_t count);
        void    fmrdiv3(float *dst, const float *a, const float *b, size_t count);
        void    fmmod3(float *dst, const float *a, const float *b, size_t count);
        void    fmrmod3(float *dst, const float *a, const float *b, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void    fmadd3(float *dst, const float *a, const float *b, size_t count);
        void    fmsub3(float *dst, const float *a, const float *b, size_t count);
        void    fmrsub3(float *dst, const float *a, const float *b, size_t count);
        void    fmmul3(float *dst, const float *a, const float *b, size_t count);
        void    fmdiv3(float *dst, const float *a, const float *b, size_t count);
        void    fmrdiv3(float *dst, const float *a, const float *b, size_t count);
        void    fmmod3(float *dst, const float *a, const float *b, size_t count);
        void    fmrmod3(float *dst, const float *a, const float *b, size_t count);
    }
)

typedef void (* fmop3_t)(float *dst, const float *a, const float *b, size_t count);

//-----------------------------------------------------------------------------
PTEST_BEGIN("dsp.pmath", fmop3, 5, 1000)

    void call(const char *label, float *dst, const float *a, const float *b, size_t count, fmop3_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(dst, a, b, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *dst      = alloc_aligned<float>(data, buf_size * 6, 64);
        float *a        = &dst[buf_size];
        float *b        = &a[buf_size];
        float *backup   = &b[buf_size];

        randomize_sign(dst, buf_size*3);
        dsp::copy(backup, dst, buf_size*3);

        #define CALL(method) \
            dsp::copy(dst, backup, buf_size*3); \
            call(#method, dst, a, b, count, method);

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(native::fmadd3);
            IF_ARCH_X86(CALL(sse::fmadd3));
            IF_ARCH_X86(CALL(avx::fmadd3));
            IF_ARCH_X86(CALL(avx::fmadd3_fma3));
            IF_ARCH_ARM(CALL(neon_d32::fmadd3));
            IF_ARCH_AARCH64(CALL(asimd::fmadd3));
            PTEST_SEPARATOR;

            CALL(native::fmsub3);
            IF_ARCH_X86(CALL(sse::fmsub3));
            IF_ARCH_X86(CALL(avx::fmsub3));
            IF_ARCH_X86(CALL(avx::fmsub3_fma3));
            IF_ARCH_ARM(CALL(neon_d32::fmsub3));
            IF_ARCH_AARCH64(CALL(asimd::fmsub3));
            PTEST_SEPARATOR;

            CALL(native::fmrsub3);
            IF_ARCH_X86(CALL(sse::fmrsub3));
            IF_ARCH_X86(CALL(avx::fmrsub3));
            IF_ARCH_X86(CALL(avx::fmrsub3_fma3));
            IF_ARCH_ARM(CALL(neon_d32::fmrsub3));
            IF_ARCH_AARCH64(CALL(asimd::fmrsub3));
            PTEST_SEPARATOR;

            CALL(native::fmmul3);
            IF_ARCH_X86(CALL(sse::fmmul3));
            IF_ARCH_X86(CALL(avx::fmmul3));
            IF_ARCH_ARM(CALL(neon_d32::fmmul3));
            IF_ARCH_AARCH64(CALL(asimd::fmmul3));
            PTEST_SEPARATOR;

            CALL(native::fmdiv3);
            IF_ARCH_X86(CALL(sse::fmdiv3));
            IF_ARCH_X86(CALL(avx::fmdiv3));
            IF_ARCH_ARM(CALL(neon_d32::fmdiv3));
            IF_ARCH_AARCH64(CALL(asimd::fmdiv3));
            PTEST_SEPARATOR;

            CALL(native::fmrdiv3);
            IF_ARCH_X86(CALL(sse::fmrdiv3));
            IF_ARCH_X86(CALL(avx::fmrdiv3));
            IF_ARCH_ARM(CALL(neon_d32::fmrdiv3));
            IF_ARCH_AARCH64(CALL(asimd::fmrdiv3));
            PTEST_SEPARATOR;

            CALL(native::fmmod3);
            IF_ARCH_X86(CALL(sse2::fmmod3));
            IF_ARCH_X86(CALL(avx::fmmod3));
            IF_ARCH_X86(CALL(avx::fmmod3_fma3));
            IF_ARCH_ARM(CALL(neon_d32::fmmod3));
            IF_ARCH_AARCH64(CALL(asimd::fmmod3));
            PTEST_SEPARATOR;

            CALL(native::fmrmod3);
            IF_ARCH_X86(CALL(sse2::fmrmod3));
            IF_ARCH_X86(CALL(avx::fmrmod3));
            IF_ARCH_X86(CALL(avx::fmrmod3_fma3));
            IF_ARCH_ARM(CALL(neon_d32::fmrmod3));
            IF_ARCH_AARCH64(CALL(asimd::fmrmod3));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }
PTEST_END

