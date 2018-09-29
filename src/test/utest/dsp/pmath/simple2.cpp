/*
 * simple2.cpp
 *
 *  Created on: 23 авг. 2018 г.
 *      Author: sadko
 */


#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/FloatBuffer.h>
#include <core/sugar.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace native
{
    void    add2(float *dst, const float *src, size_t count);
    void    sub2(float *dst, const float *src, size_t count);
    void    mul2(float *dst, const float *src, size_t count);
    void    div2(float *dst, const float *src, size_t count);

    void    abs_add2(float *dst, const float *src, size_t count);
    void    abs_sub2(float *dst, const float *src, size_t count);
    void    abs_mul2(float *dst, const float *src, size_t count);
    void    abs_div2(float *dst, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void    add2(float *dst, const float *src, size_t count);
        void    sub2(float *dst, const float *src, size_t count);
        void    mul2(float *dst, const float *src, size_t count);
        void    div2(float *dst, const float *src, size_t count);

        void    abs_add2(float *dst, const float *src, size_t count);
        void    abs_sub2(float *dst, const float *src, size_t count);
        void    abs_mul2(float *dst, const float *src, size_t count);
        void    abs_div2(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void    add2(float *dst, const float *src, size_t count);
        void    sub2(float *dst, const float *src, size_t count);
        void    mul2(float *dst, const float *src, size_t count);
        void    div2(float *dst, const float *src, size_t count);

        void    abs_add2(float *dst, const float *src, size_t count);
        void    abs_sub2(float *dst, const float *src, size_t count);
        void    abs_mul2(float *dst, const float *src, size_t count);
        void    abs_div2(float *dst, const float *src, size_t count);
    }
)

typedef void (* func2)(float *dst, const float *src, size_t count);

//-----------------------------------------------------------------------------
// Unit test for simple operations
UTEST_BEGIN("dsp.pmath", simple2)

    void call(const char *label, size_t align, func2 func1, func2 func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 999, 0xfff)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer src(count, align, mask & 0x01);
                FloatBuffer dst1(count, align, mask & 0x02);
                FloatBuffer dst2(dst1);

                // Call functions
                func1(dst1, src, count);
                func2(dst2, src, count);

                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_relative(dst2, 1e-4))
                {
                    src.dump("src");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    printf("index=%d, %.6f vs %.6f\n", dst1.last_diff(), dst1.get_diff(), dst2.get_diff());
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    UTEST_MAIN
    {
        IF_ARCH_X86(call("sse:add2", 16, native::add2, sse::add2));
        IF_ARCH_X86(call("sse:sub2", 16, native::sub2, sse::sub2));
        IF_ARCH_X86(call("sse:mul2", 16, native::mul2, sse::mul2));
        IF_ARCH_X86(call("sse:div2", 16, native::div2, sse::div2));

        IF_ARCH_X86(call("sse:abs_add2", 16, native::abs_add2, sse::abs_add2));
        IF_ARCH_X86(call("sse:abs_sub2", 16, native::abs_sub2, sse::abs_sub2));
        IF_ARCH_X86(call("sse:abs_mul2", 16, native::abs_mul2, sse::abs_mul2));
        IF_ARCH_X86(call("sse:abs_div2", 16, native::abs_div2, sse::abs_div2));

        IF_ARCH_ARM(call("neon_d32:add2", 16, native::add2, neon_d32::add2));
        IF_ARCH_ARM(call("neon_d32:sub2", 16, native::sub2, neon_d32::sub2));
        IF_ARCH_ARM(call("neon_d32:mul2", 16, native::mul2, neon_d32::mul2));
        IF_ARCH_ARM(call("neon_d32:div2", 16, native::div2, neon_d32::div2));

        IF_ARCH_ARM(call("neon_d32:abs_add2", 16, native::abs_add2, neon_d32::abs_add2));
        IF_ARCH_ARM(call("neon_d32:abs_sub2", 16, native::abs_sub2, neon_d32::abs_sub2));
        IF_ARCH_ARM(call("neon_d32:abs_mul2", 16, native::abs_mul2, neon_d32::abs_mul2));
        IF_ARCH_ARM(call("neon_d32:abs_div2", 16, native::abs_div2, neon_d32::abs_div2));
    }
UTEST_END


