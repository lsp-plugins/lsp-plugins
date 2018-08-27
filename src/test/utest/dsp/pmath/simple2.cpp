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
UTEST_BEGIN("dsp.pmath", simple2)

    void call(const char *label, size_t align, func2 func1, func2 func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 999)
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
                if (!dst1.equals_absolute(dst2, 1e-5))
                {
                    src.dump("src");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    UTEST_MAIN
    {
        IF_ARCH_X86(call("add2 sse", 16, native::add2, sse::add2));
        IF_ARCH_X86(call("sub2 sse", 16, native::sub2, sse::sub2));
        IF_ARCH_X86(call("mul2 sse", 16, native::mul2, sse::mul2));
        IF_ARCH_X86(call("div2 sse", 16, native::div2, sse::div2));
    }
UTEST_END


