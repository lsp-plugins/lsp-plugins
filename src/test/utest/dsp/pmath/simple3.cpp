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

        void    abs_add3(float *dst, const float *src1, const float *src2, size_t count);
        void    abs_sub3(float *dst, const float *src1, const float *src2, size_t count);
        void    abs_mul3(float *dst, const float *src1, const float *src2, size_t count);
        void    abs_div3(float *dst, const float *src1, const float *src2, size_t count);
    }
)

typedef void (* func3)(float *dst, const float *src1, const float *src2, size_t count);

//-----------------------------------------------------------------------------
// Unit test for simple operations
UTEST_BEGIN("dsp.pmath", simple3)

    void call(const char *label, size_t align, func3 func1, func3 func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 999, 0xfff)
        {
            for (size_t mask=0; mask <= 0x07; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer src1(count, align, mask & 0x01);
                FloatBuffer src2(count, align, mask & 0x02);
                FloatBuffer dst1(count, align, mask & 0x04);
                FloatBuffer dst2(dst1);

                // Call functions
                func1(dst1, src1, src2, count);
                func2(dst2, src1, src2, count);

                UTEST_ASSERT_MSG(src1.valid(), "Source buffer 1 corrupted");
                UTEST_ASSERT_MSG(src2.valid(), "Source buffer 2 corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_relative(dst2, 1e-4))
                {
                    src1.dump("src1");
                    src2.dump("src2");
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
        IF_ARCH_X86(call("sse:add3", 16, native::add3, sse::add3));
        IF_ARCH_X86(call("sse:sub3", 16, native::sub3, sse::sub3));
        IF_ARCH_X86(call("sse:mul3", 16, native::mul3, sse::mul3));
        IF_ARCH_X86(call("sse:div3", 16, native::div3, sse::div3));

        IF_ARCH_X86(call("sse:abs_add3", 16, native::abs_add3, sse::abs_add3));
        IF_ARCH_X86(call("sse:abs_sub3", 16, native::abs_sub3, sse::abs_sub3));
        IF_ARCH_X86(call("sse:abs_mul3", 16, native::abs_mul3, sse::abs_mul3));
        IF_ARCH_X86(call("sse:abs_div3", 16, native::abs_div3, sse::abs_div3));

        IF_ARCH_ARM(call("neon_d32:add3", 16, native::add3, neon_d32::add3));
        IF_ARCH_ARM(call("neon_d32:sub3", 16, native::sub3, neon_d32::sub3));
        IF_ARCH_ARM(call("neon_d32:mul3", 16, native::mul3, neon_d32::mul3));
        IF_ARCH_ARM(call("neon_d32:div3", 16, native::div3, neon_d32::div3));

        IF_ARCH_ARM(call("neon_d32:abs_add3", 16, native::abs_add3, neon_d32::abs_add3));
        IF_ARCH_ARM(call("neon_d32:abs_sub3", 16, native::abs_sub3, neon_d32::abs_sub3));
        IF_ARCH_ARM(call("neon_d32:abs_mul3", 16, native::abs_mul3, neon_d32::abs_mul3));
        IF_ARCH_ARM(call("neon_d32:abs_div3", 16, native::abs_div3, neon_d32::abs_div3));
    }
UTEST_END


