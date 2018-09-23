/*
 * scale3.cpp
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
    void    scale3(float *dst, const float *src, float k, size_t count);
    void    scale_add3(float *dst, const float *src, float k, size_t count);
    void    scale_sub3(float *dst, const float *src, float k, size_t count);
    void    scale_mul3(float *dst, const float *src, float k, size_t count);
    void    scale_div3(float *dst, const float *src, float k, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void    scale3(float *dst, const float *src, float k, size_t count);
        void    scale_add3(float *dst, const float *src, float k, size_t count);
        void    scale_sub3(float *dst, const float *src, float k, size_t count);
        void    scale_mul3(float *dst, const float *src, float k, size_t count);
        void    scale_div3(float *dst, const float *src, float k, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void    scale3(float *dst, const float *src, float k, size_t count);
        void    scale_add3(float *dst, const float *src, float k, size_t count);
        void    scale_sub3(float *dst, const float *src, float k, size_t count);
        void    scale_mul3(float *dst, const float *src, float k, size_t count);
        void    scale_div3(float *dst, const float *src, float k, size_t count);
    }
)

typedef void (* scale3_t)(float *dst, const float *src, float k, size_t count);

//-----------------------------------------------------------------------------
// Performance test for complex multiplication
PTEST_BEGIN("dsp.pmath", scale3, 5, 1000)

    void call(const char *label, float *dst, const float *src, size_t count, scale3_t func)
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
        float *dst      = alloc_aligned<float>(data, buf_size * 4, 64);
        float *src      = &dst[buf_size];
        float *backup   = &src[buf_size];

        for (size_t i=0; i < buf_size*2; ++i)
            dst[i]          = float(rand()) / RAND_MAX;
        dsp::copy(backup, dst, buf_size*2);

        #define CALL(...) \
            dsp::copy(dst, backup, buf_size*2); \
            call(__VA_ARGS__);

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL("native:scale3 native", dst, src, count, native::scale3);
            IF_ARCH_X86(CALL("sse:scale3", dst, src, count, sse::scale3));
            IF_ARCH_ARM(CALL("neon_d32:scale3", dst, src, count, neon_d32::scale3));
            PTEST_SEPARATOR;

            CALL("native:scale_add3 native", dst, src, count, native::scale_add3);
            IF_ARCH_X86(CALL("sse:scale_add3", dst, src, count, sse::scale_add3));
            IF_ARCH_ARM(CALL("neon_d32:scale_add3", dst, src, count, neon_d32::scale_add3));
            PTEST_SEPARATOR;

            CALL("native:scale_sub3 native", dst, src, count, native::scale_sub3);
            IF_ARCH_X86(CALL("sse:scale_sub3", dst, src, count, sse::scale_sub3));
            IF_ARCH_ARM(CALL("neon_d32:scale_sub3", dst, src, count, neon_d32::scale_sub3));
            PTEST_SEPARATOR;

            CALL("native:scale_mul3 native", dst, src, count, native::scale_mul3);
            IF_ARCH_X86(CALL("sse:scale_mul3", dst, src, count, sse::scale_mul3));
            IF_ARCH_ARM(CALL("neon_d32:scale_mul3", dst, src, count, neon_d32::scale_mul3));
            PTEST_SEPARATOR;

            CALL("native:scale_div3 native", dst, src, count, native::scale_div3);
            IF_ARCH_X86(CALL("sse:scale_div3", dst, src, count, sse::scale_div3));
            IF_ARCH_ARM(CALL("neon_d32:scale_div3", dst, src, count, neon_d32::scale_div3));
            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END


