/*
 * abs_op3.cpp
 *
 *  Created on: 19 нояб. 2019 г.
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
    void    abs_add3(float *dst, const float *src1, const float *src2, size_t count);
    void    abs_sub3(float *dst, const float *src1, const float *src2, size_t count);
    void    abs_mul3(float *dst, const float *src1, const float *src2, size_t count);
    void    abs_div3(float *dst, const float *src1, const float *src2, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void    abs_add3(float *dst, const float *src1, const float *src2, size_t count);
        void    abs_sub3(float *dst, const float *src1, const float *src2, size_t count);
        void    abs_mul3(float *dst, const float *src1, const float *src2, size_t count);
        void    abs_div3(float *dst, const float *src1, const float *src2, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
//        void    abs_add3(float *dst, const float *src1, const float *src2, size_t count);
//        void    abs_sub3(float *dst, const float *src1, const float *src2, size_t count);
//        void    abs_mul3(float *dst, const float *src1, const float *src2, size_t count);
//        void    abs_div3(float *dst, const float *src1, const float *src2, size_t count);
    }
)

typedef void (* func3)(float *dst, const float *src1, const float *src2, size_t count);

//-----------------------------------------------------------------------------
// Performance test for complex multiplication
PTEST_BEGIN("dsp.pmath", abs_op3, 5, 1000)

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

        randomize_sign(src1, buf_size);
        randomize_sign(src2, buf_size);

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            call("native::abs_add3", dst, src1, src2, count, native::abs_add3);
            IF_ARCH_X86(call("sse::abs_add3", dst, src1, src2, count, sse::abs_add3));
//            IF_ARCH_ARM(call("neon_d32::abs_add3", dst, src1, src2, count, neon_d32::abs_add3)); // TODO
            PTEST_SEPARATOR;

            call("native::abs_sub3", dst, src1, src2, count, native::abs_sub3);
            IF_ARCH_X86(call("sse::abs_sub3", dst, src1, src2, count, sse::abs_sub3));
//            IF_ARCH_ARM(call("neon_d32:abs_sub3", dst, src1, src2, count, neon_d32::abs_sub3)); // TODO
            PTEST_SEPARATOR;

            call("native::abs_mul3", dst, src1, src2, count, native::abs_mul3);
            IF_ARCH_X86(call("sse::abs_mul3", dst, src1, src2, count, sse::abs_mul3));
//            IF_ARCH_ARM(call("neon_d32:abs_mul3", dst, src1, src2, count, neon_d32::abs_mul3)); // TODO
            PTEST_SEPARATOR;

            call("native::abs_div3", dst, src1, src2, count, native::abs_div3);
            IF_ARCH_X86(call("sse::abs_div3", dst, src1, src2, count, sse::abs_div3));
//            IF_ARCH_ARM(call("neon_d32:abs_div3", dst, src1, src2, count, neon_d32::abs_div3)); // TODO
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }
PTEST_END


