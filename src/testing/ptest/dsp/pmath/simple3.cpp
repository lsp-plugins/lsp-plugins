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
    void    add3(float *dst, const float *src1, const float *src2, size_t count);
    void    sub3(float *dst, const float *src1, const float *src2, size_t count);
    void    mul3(float *dst, const float *src1, const float *src2, size_t count);
    void    div3(float *dst, const float *src1, const float *src2, size_t count);

    void    abs_add3(float *dst, const float *src1, const float *src2, size_t count);
    void    abs_sub3(float *dst, const float *src1, const float *src2, size_t count);
    void    abs_mul3(float *dst, const float *src1, const float *src2, size_t count);
    void    abs_div3(float *dst, const float *src1, const float *src2, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void    add3(float *dst, const float *src1, const float *src2, size_t count);
        void    sub3(float *dst, const float *src1, const float *src2, size_t count);
        void    mul3(float *dst, const float *src1, const float *src2, size_t count);
        void    div3(float *dst, const float *src1, const float *src2, size_t count);

        void    abs_add3(float *dst, const float *src1, const float *src2, size_t count);
        void    abs_sub3(float *dst, const float *src1, const float *src2, size_t count);
        void    abs_mul3(float *dst, const float *src1, const float *src2, size_t count);
        void    abs_div3(float *dst, const float *src1, const float *src2, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void    add3(float *dst, const float *src1, const float *src2, size_t count);
        void    sub3(float *dst, const float *src1, const float *src2, size_t count);
        void    mul3(float *dst, const float *src1, const float *src2, size_t count);
        void    div3(float *dst, const float *src1, const float *src2, size_t count);

// TODO
//        void    abs_add3(float *dst, const float *src1, const float *src2, size_t count);
//        void    abs_sub3(float *dst, const float *src1, const float *src2, size_t count);
//        void    abs_mul3(float *dst, const float *src1, const float *src2, size_t count);
//        void    abs_div3(float *dst, const float *src1, const float *src2, size_t count);
    }
)

typedef void (* func3)(float *dst, const float *src1, const float *src2, size_t count);

//-----------------------------------------------------------------------------
// Performance test for complex multiplication
PTEST_BEGIN("dsp.pmath", simple3, 5, 1000)

    void call(const char *label, float *dst, const float *src1, const float *src2, size_t count, func3 func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(dst, src1, src2, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *dst      = alloc_aligned<float>(data, buf_size * 3, 64);
        float *src1     = &dst[buf_size];
        float *src2     = &src1[buf_size];

        for (size_t i=0; i < buf_size; ++i)
        {
            src1[i]             = float(rand()) / RAND_MAX;
            src2[i]             = float(rand()) / RAND_MAX;
        }

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            call("native:add3", dst, src1, src2, count, native::add3);
            call("native:abs_add3", dst, src1, src2, count, native::abs_add3);
            IF_ARCH_X86(call("sse:abs_add3", dst, src1, src2, count, sse::abs_add3));
            IF_ARCH_X86(call("sse:add3", dst, src1, src2, count, sse::add3));
            IF_ARCH_ARM(call("neon_d32:add3", dst, src1, src2, count, neon_d32::add3));
//            IF_ARCH_ARM(call("neon_d32:abs_add3", dst, src1, src2, count, neon_d32::abs_add3)); // TODO
            PTEST_SEPARATOR;

            call("native:sub3", dst, src1, src2, count, native::sub3);
            call("native:abs_sub3", dst, src1, src2, count, native::abs_sub3);
            IF_ARCH_X86(call("sse:sub3", dst, src1, src2, count, sse::sub3));
            IF_ARCH_X86(call("sse:abs_sub3", dst, src1, src2, count, sse::abs_sub3));
            IF_ARCH_ARM(call("neon_d32:sub3", dst, src1, src2, count, neon_d32::sub3));
//            IF_ARCH_ARM(call("neon_d32:abs_sub3", dst, src1, src2, count, neon_d32::abs_sub3)); // TODO
            PTEST_SEPARATOR;

            call("native:mul3", dst, src1, src2, count, native::mul3);
            call("native:abs_mul3", dst, src1, src2, count, native::abs_mul3);
            IF_ARCH_X86(call("sse:mul3", dst, src1, src2, count, sse::mul3));
            IF_ARCH_X86(call("sse:abs_mul3", dst, src1, src2, count, sse::abs_mul3));
            IF_ARCH_ARM(call("neon_d32:mul3", dst, src1, src2, count, neon_d32::mul3));
//            IF_ARCH_ARM(call("neon_d32:abs_mul3", dst, src1, src2, count, neon_d32::abs_mul3)); // TODO
            PTEST_SEPARATOR;

            call("native:div3", dst, src1, src2, count, native::div3);
            call("native:abs_div3", dst, src1, src2, count, native::abs_div3);
            IF_ARCH_X86(call("sse:div3", dst, src1, src2, count, sse::div3));
            IF_ARCH_X86(call("sse:abs_div3", dst, src1, src2, count, sse::abs_div3));
            IF_ARCH_ARM(call("neon_d32:div3", dst, src1, src2, count, neon_d32::div3));
//            IF_ARCH_ARM(call("neon_d32:abs_div3", dst, src1, src2, count, neon_d32::abs_div3)); // TODO
            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END


