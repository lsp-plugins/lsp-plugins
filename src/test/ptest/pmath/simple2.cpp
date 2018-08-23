/*
 * simple2.cpp
 *
 *  Created on: 23 авг. 2018 г.
 *      Author: sadko
 */


#include <dsp/dsp.h>
#include <test/ptest.h>
#include <core/sugar.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace native
{
    void    add2(float *dst, const float *src, size_t count);
    void    sub2(float *dst, const float *src, size_t count);
    void    mul2(float *dst, const float *src, size_t count);
    void    div2(float *dst, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void    add2(float *dst, const float *src, size_t count);
        void    sub2(float *dst, const float *src, size_t count);
        void    mul2(float *dst, const float *src, size_t count);
        void    div2(float *dst, const float *src, size_t count);
    }
)

typedef void (* func2)(float *dst, const float *src, size_t count);

//-----------------------------------------------------------------------------
// Performance test for complex multiplication
PTEST_BEGIN("dsp.pmath", simple2, 5, 10000)

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

        for (size_t i=0; i < buf_size; ++i)
            src[i]          = float(rand()) / RAND_MAX;

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            call("add2 native", dst, src, count, native::add2);
            IF_ARCH_X86(call("add2 sse", dst, src, count, sse::add2));

            call("sub2 native", dst, src, count, native::sub2);
            IF_ARCH_X86(call("sub2 sse", dst, src, count, sse::sub2));

            call("mul2 native", dst, src, count, native::mul2);
            IF_ARCH_X86(call("mul2 sse", dst, src, count, sse::mul2));

            call("div2 native", dst, src, count, native::div2);
            IF_ARCH_X86(call("div2 sse", dst, src, count, sse::div2));

            PTEST_SEPARATOR;

            printf("\n");
        }

        free_aligned(data);
    }
PTEST_END


