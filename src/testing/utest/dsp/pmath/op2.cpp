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
// Unit test for simple operations
UTEST_BEGIN("dsp.pmath", op2)

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
                src.randomize_sign();
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
        IF_ARCH_X86(call("sse::add2", 16, native::add2, sse::add2));
        IF_ARCH_X86(call("sse::sub2", 16, native::sub2, sse::sub2));
//        IF_ARCH_X86(call("sse::rsub2", 16, native::rsub2, sse::rsub2));
        IF_ARCH_X86(call("sse::mul2", 16, native::mul2, sse::mul2));
        IF_ARCH_X86(call("sse::div2", 16, native::div2, sse::div2));
//        IF_ARCH_X86(call("sse::rdiv2", 16, native::rdiv2, sse::rdiv2));

        IF_ARCH_ARM(call("neon_d32::add2", 16, native::add2, neon_d32::add2));
        IF_ARCH_ARM(call("neon_d32::sub2", 16, native::sub2, neon_d32::sub2));
//        IF_ARCH_ARM(call("neon_d32::rsub2", 16, native::rsub2, neon_d32::rsub2));
        IF_ARCH_ARM(call("neon_d32::mul2", 16, native::mul2, neon_d32::mul2));
        IF_ARCH_ARM(call("neon_d32::div2", 16, native::div2, neon_d32::div2));
//        IF_ARCH_ARM(call("neon_d32::rdiv2", 16, native::rdiv2, neon_d32::rdiv2));

        IF_ARCH_AARCH64(call("asimd::add2", 16, native::add2, asimd::add2));
        IF_ARCH_AARCH64(call("asimd::sub2", 16, native::sub2, asimd::sub2));
        IF_ARCH_AARCH64(call("asimd::rsub2", 16, native::rsub2, asimd::rsub2));
        IF_ARCH_AARCH64(call("asimd::mul2", 16, native::mul2, asimd::mul2));
        IF_ARCH_AARCH64(call("asimd::div2", 16, native::div2, asimd::div2));
        IF_ARCH_AARCH64(call("asimd::rdiv2", 16, native::rdiv2, asimd::rdiv2));
    }
UTEST_END


