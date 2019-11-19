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
}

IF_ARCH_X86(
    namespace sse
    {
        void    add2(float *dst, const float *src, size_t count);
        void    sub2(float *dst, const float *src, size_t count);
//        void    rsub2(float *dst, const float *src, size_t count);
        void    mul2(float *dst, const float *src, size_t count);
        void    div2(float *dst, const float *src, size_t count);
//        void    rdiv2(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void    add2(float *dst, const float *src, size_t count);
        void    sub2(float *dst, const float *src, size_t count);
//        void    rsub2(float *dst, const float *src, size_t count);
        void    mul2(float *dst, const float *src, size_t count);
        void    div2(float *dst, const float *src, size_t count);
//        void    rdiv2(float *dst, const float *src, size_t count);
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
    }
)

typedef void (* func2)(float *dst, const float *src, size_t count);

//-----------------------------------------------------------------------------
// Performance test for complex multiplication
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

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            call("native::add2", dst, src, count, native::add2);
            IF_ARCH_X86(call("sse::add2", dst, src, count, sse::add2));
            IF_ARCH_ARM(call("neon_d32::add2", dst, src, count, neon_d32::add2));
            IF_ARCH_AARCH64(call("asimd::add2", dst, src, count, asimd::add2));
            PTEST_SEPARATOR;

            call("native::sub2", dst, src, count, native::sub2);
            IF_ARCH_X86(call("sse::sub2", dst, src, count, sse::sub2));
            IF_ARCH_ARM(call("neon_d32::sub2", dst, src, count, neon_d32::sub2));
            IF_ARCH_AARCH64(call("asimd::sub2", dst, src, count, asimd::sub2));
            PTEST_SEPARATOR;

            call("native::rsub2", dst, src, count, native::rsub2);
//            IF_ARCH_X86(call("sse::rsub2", dst, src, count, sse::rsub2));
//            IF_ARCH_ARM(call("neon_d32::rsub2", dst, src, count, neon_d32::rsub2));
            IF_ARCH_AARCH64(call("asimd::rsub2", dst, src, count, asimd::rsub2));
            PTEST_SEPARATOR;

            call("native::mul2", dst, src, count, native::mul2);
            IF_ARCH_X86(call("sse::mul2", dst, src, count, sse::mul2));
            IF_ARCH_ARM(call("neon_d32::mul2", dst, src, count, neon_d32::mul2));
            IF_ARCH_AARCH64(call("asimd::mul2", dst, src, count, asimd::mul2));
            PTEST_SEPARATOR;

            call("native::div2", dst, src, count, native::div2);
            IF_ARCH_X86(call("sse::div2", dst, src, count, sse::div2));
            IF_ARCH_ARM(call("neon_d32::div2", dst, src, count, neon_d32::div2));
            IF_ARCH_AARCH64(call("asimd::div2", dst, src, count, asimd::div2));
            PTEST_SEPARATOR;

            call("native::rdiv2", dst, src, count, native::rdiv2);
//            IF_ARCH_X86(call("sse::rdiv2", dst, src, count, sse::rdiv2));
//            IF_ARCH_ARM(call("neon_d32::rdiv2", dst, src, count, neon_d32::rdiv2));
            IF_ARCH_AARCH64(call("asimd::rdiv2", dst, src, count, asimd::rdiv2));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }
PTEST_END


