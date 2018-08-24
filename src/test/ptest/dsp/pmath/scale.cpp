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
    void    scale2(float *dst, float k, size_t count);
    void    scale3(float *dst, const float *src, float k, size_t count);

    void    scale_add3(float *dst, const float *src, float k, size_t count);
    void    scale_sub3(float *dst, const float *src, float k, size_t count);
    void    scale_mul3(float *dst, const float *src, float k, size_t count);
    void    scale_div3(float *dst, const float *src, float k, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void    scale2(float *dst, float k, size_t count);
        void    scale3(float *dst, const float *src, float k, size_t count);

        void    scale_add3(float *dst, const float *src, float k, size_t count);
        void    scale_sub3(float *dst, const float *src, float k, size_t count);
        void    scale_mul3(float *dst, const float *src, float k, size_t count);
        void    scale_div3(float *dst, const float *src, float k, size_t count);
    }
)

typedef void (* scale2)(float *dst, float k, size_t count);
typedef void (* scale3)(float *dst, const float *src, float k, size_t count);

//-----------------------------------------------------------------------------
// Performance test for complex multiplication
PTEST_BEGIN("dsp.pmath", scale, 5, 10000)

    void call(const char *label, float *dst, const float *src, size_t count, scale2 func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            dsp::copy(dst, src, count);
            func(dst, 0.5f, count);
        );
    }

    void call(const char *label, float *dst, const float *src, size_t count, scale3 func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(dst, src, 0.5f, count);
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

            call("scale2 native", dst, src, count, native::scale2);
            IF_ARCH_X86(call("scale2 sse", dst, src, count, sse::scale2));

            call("scale3 native", dst, src, count, native::scale3);
            IF_ARCH_X86(call("scale3 sse", dst, src, count, sse::scale3));

            call("scale_add3 native", dst, src, count, native::scale_add3);
            IF_ARCH_X86(call("scale_add3 sse", dst, src, count, sse::scale_add3));

            call("scale_sub3 native", dst, src, count, native::scale_sub3);
            IF_ARCH_X86(call("scale_sub3 sse", dst, src, count, sse::scale_sub3));

            call("scale_mul3 native", dst, src, count, native::scale_mul3);
            IF_ARCH_X86(call("scale_mul3 sse", dst, src, count, sse::scale_mul3));

            call("scale_div3 native", dst, src, count, native::scale_div3);
            IF_ARCH_X86(call("scale_div3 sse", dst, src, count, sse::scale_div3));

            PTEST_SEPARATOR;

            printf("\n");
        }

        free_aligned(data);
    }
PTEST_END


