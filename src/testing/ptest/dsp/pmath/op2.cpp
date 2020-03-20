/*
 * op2.cpp
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
    void    add2(float *dst, const float *src, size_t count);
    void    sub2(float *dst, const float *src, size_t count);
    void    rsub2(float *dst, const float *src, size_t count);
    void    mul2(float *dst, const float *src, size_t count);
    void    div2(float *dst, const float *src, size_t count);
    void    rdiv2(float *dst, const float *src, size_t count);
    void    mod2(float *dst, const float *src, size_t count);
    void    rmod2(float *dst, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void    add2(float *dst, const float *src, size_t count);
        void    sub2(float *dst, const float *src, size_t count);
        void    rsub2(float *dst, const float *src, size_t count);
        void    mul2(float *dst, const float *src, size_t count);
        void    div2(float *dst, const float *src, size_t count);
        void    rdiv2(float *dst, const float *src, size_t count);
    }

    namespace sse2
    {
        void    mod2(float *dst, const float *src, size_t count);
        void    rmod2(float *dst, const float *src, size_t count);
    }

    namespace avx
    {
        void    add2(float *dst, const float *src, size_t count);
        void    sub2(float *dst, const float *src, size_t count);
        void    rsub2(float *dst, const float *src, size_t count);
        void    mul2(float *dst, const float *src, size_t count);
        void    div2(float *dst, const float *src, size_t count);
        void    rdiv2(float *dst, const float *src, size_t count);
        void    mod2(float *dst, const float *src, size_t count);
        void    rmod2(float *dst, const float *src, size_t count);
        void    mod2_fma3(float *dst, const float *src, size_t count);
        void    rmod2_fma3(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void    add2(float *dst, const float *src, size_t count);
        void    sub2(float *dst, const float *src, size_t count);
        void    rsub2(float *dst, const float *src, size_t count);
        void    mul2(float *dst, const float *src, size_t count);
        void    div2(float *dst, const float *src, size_t count);
        void    rdiv2(float *dst, const float *src, size_t count);
        void    mod2(float *dst, const float *src, size_t count);
        void    rmod2(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void    add2(float *dst, const float *src, size_t count);
        void    sub2(float *dst, const float *src, size_t count);
        void    rsub2(float *dst, const float *src, size_t count);
        void    mul2(float *dst, const float *src, size_t count);
        void    div2(float *dst, const float *src, size_t count);
        void    rdiv2(float *dst, const float *src, size_t count);
        void    mod2(float *dst, const float *src, size_t count);
        void    rmod2(float *dst, const float *src, size_t count);
    }
)

typedef void (* func2)(float *dst, const float *src, size_t count);

//-----------------------------------------------------------------------------
PTEST_BEGIN("dsp.pmath", op2, 5, 1000)

    void call(const char *label, float *dst, const float *src, size_t count, func2 func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            dsp::copy(dst, src, count);
            func(dst, src, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *dst      = alloc_aligned<float>(data, buf_size * 2, 64);
        float *src      = &dst[buf_size];

        randomize_sign(src, buf_size);

        #define CALL(func) \
            call(#func, dst, src, count, func)

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(native::add2);
            IF_ARCH_X86(CALL(sse::add2));
            IF_ARCH_X86(CALL(avx::add2));
            IF_ARCH_ARM(CALL(neon_d32::add2));
            IF_ARCH_AARCH64(CALL(asimd::add2));
            PTEST_SEPARATOR;

            CALL(native::sub2);
            IF_ARCH_X86(CALL(sse::sub2));
            IF_ARCH_X86(CALL(avx::sub2));
            IF_ARCH_ARM(CALL(neon_d32::sub2));
            IF_ARCH_AARCH64(CALL(asimd::sub2));
            PTEST_SEPARATOR;

            CALL(native::rsub2);
            IF_ARCH_X86(CALL(sse::rsub2));
            IF_ARCH_X86(CALL(avx::rsub2));
            IF_ARCH_ARM(CALL(neon_d32::rsub2));
            IF_ARCH_AARCH64(CALL(asimd::rsub2));
            PTEST_SEPARATOR;

            CALL(native::mul2);
            IF_ARCH_X86(CALL(sse::mul2));
            IF_ARCH_X86(CALL(avx::mul2));
            IF_ARCH_ARM(CALL(neon_d32::mul2));
            IF_ARCH_AARCH64(CALL(asimd::mul2));
            PTEST_SEPARATOR;

            CALL(native::div2);
            IF_ARCH_X86(CALL(sse::div2));
            IF_ARCH_X86(CALL(avx::div2));
            IF_ARCH_ARM(CALL(neon_d32::div2));
            IF_ARCH_AARCH64(CALL(asimd::div2));
            PTEST_SEPARATOR;

            CALL(native::rdiv2);
            IF_ARCH_X86(CALL(sse::rdiv2));
            IF_ARCH_X86(CALL(avx::rdiv2));
            IF_ARCH_ARM(CALL(neon_d32::rdiv2));
            IF_ARCH_AARCH64(CALL(asimd::rdiv2));
            PTEST_SEPARATOR;

            CALL(native::mod2);
            IF_ARCH_X86(CALL(sse2::mod2));
            IF_ARCH_X86(CALL(avx::mod2));
            IF_ARCH_X86(CALL(avx::mod2_fma3));
            IF_ARCH_ARM(CALL(neon_d32::mod2));
            IF_ARCH_AARCH64(CALL(asimd::mod2));
            PTEST_SEPARATOR;

            CALL(native::rmod2);
            IF_ARCH_X86(CALL(sse2::rmod2));
            IF_ARCH_X86(CALL(avx::rmod2));
            IF_ARCH_X86(CALL(avx::rmod2_fma3));
            IF_ARCH_ARM(CALL(neon_d32::rmod2));
            IF_ARCH_AARCH64(CALL(asimd::rmod2));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }
PTEST_END


