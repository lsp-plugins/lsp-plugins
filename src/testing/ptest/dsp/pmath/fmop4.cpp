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

    namespace avx
    {
        void    x64_fmadd4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    x64_fmsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    x64_fmrsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    x64_fmmul4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    x64_fmdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    x64_fmrdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);

        void    x64_fmadd4_fma3(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    x64_fmsub4_fma3(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    x64_fmrsub4_fma3(float *dst, const float *a, const float *b, const float *c, size_t count);
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
            IF_ARCH_X86(CALL(avx::x64_fmadd4));
            IF_ARCH_X86(CALL(avx::x64_fmadd4_fma3));
            PTEST_SEPARATOR;

            CALL(native::fmsub4);
            IF_ARCH_X86(CALL(sse::fmsub4));
            IF_ARCH_X86(CALL(avx::x64_fmsub4));
            IF_ARCH_X86(CALL(avx::x64_fmsub4_fma3));
            PTEST_SEPARATOR;

            CALL(native::fmrsub4);
            IF_ARCH_X86(CALL(sse::fmrsub4));
            IF_ARCH_X86(CALL(avx::x64_fmrsub4));
            IF_ARCH_X86(CALL(avx::x64_fmrsub4_fma3));
            PTEST_SEPARATOR;

            CALL(native::fmmul4);
            IF_ARCH_X86(CALL(sse::fmmul4));
            IF_ARCH_X86(CALL(avx::x64_fmmul4));
            PTEST_SEPARATOR;

            CALL(native::fmdiv4);
            IF_ARCH_X86(CALL(sse::fmdiv4));
            IF_ARCH_X86(CALL(avx::x64_fmdiv4));
            PTEST_SEPARATOR;

            CALL(native::fmrdiv4);
            IF_ARCH_X86(CALL(sse::fmrdiv4));
            IF_ARCH_X86(CALL(avx::x64_fmrdiv4));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }
PTEST_END




