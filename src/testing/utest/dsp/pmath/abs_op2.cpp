/*
 * abs_op2.cpp
 *
 *  Created on: 19 нояб. 2019 г.
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
    void    abs_add2(float *dst, const float *src, size_t count);
    void    abs_sub2(float *dst, const float *src, size_t count);
    void    abs_rsub2(float *dst, const float *src, size_t count);
    void    abs_mul2(float *dst, const float *src, size_t count);
    void    abs_div2(float *dst, const float *src, size_t count);
    void    abs_rdiv2(float *dst, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void    abs_add2(float *dst, const float *src, size_t count);
        void    abs_sub2(float *dst, const float *src, size_t count);
        void    abs_rsub2(float *dst, const float *src, size_t count);
        void    abs_mul2(float *dst, const float *src, size_t count);
        void    abs_div2(float *dst, const float *src, size_t count);
        void    abs_rdiv2(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_X86_64(
    namespace avx
    {
        void    x64_abs_add2(float *dst, const float *src, size_t count);
        void    x64_abs_sub2(float *dst, const float *src, size_t count);
        void    x64_abs_rsub2(float *dst, const float *src, size_t count);
        void    x64_abs_mul2(float *dst, const float *src, size_t count);
        void    x64_abs_div2(float *dst, const float *src, size_t count);
        void    x64_abs_rdiv2(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void    abs_add2(float *dst, const float *src, size_t count);
        void    abs_sub2(float *dst, const float *src, size_t count);
        void    abs_rsub2(float *dst, const float *src, size_t count);
        void    abs_mul2(float *dst, const float *src, size_t count);
        void    abs_div2(float *dst, const float *src, size_t count);
        void    abs_rdiv2(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void    abs_add2(float *dst, const float *src, size_t count);
        void    abs_sub2(float *dst, const float *src, size_t count);
        void    abs_rsub2(float *dst, const float *src, size_t count);
        void    abs_mul2(float *dst, const float *src, size_t count);
        void    abs_div2(float *dst, const float *src, size_t count);
        void    abs_rdiv2(float *dst, const float *src, size_t count);
    }
)

typedef void (* abs_op2_t)(float *dst, const float *src, size_t count);

//-----------------------------------------------------------------------------
// Unit test for simple operations
UTEST_BEGIN("dsp.pmath", abs_op2)

    void call(const char *label, size_t align, abs_op2_t func1, abs_op2_t func2)
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
                FloatBuffer dst(count, align, mask & 0x02);

                src.randomize_sign();
                dst.randomize_sign();
                FloatBuffer dst1(dst);
                FloatBuffer dst2(dst);

                // Call functions
                func1(dst1, src, count);
                func2(dst2, src, count);

                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
                UTEST_ASSERT_MSG(dst.valid(), "Destination buffer corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_relative(dst2, 1e-4))
                {
                    src.dump("src ");
                    dst.dump("dst ");
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
        #define CALL(native, func, align) \
            call(#func, align, native, func);

        IF_ARCH_X86(CALL(native::abs_add2, sse::abs_add2, 16));
        IF_ARCH_X86(CALL(native::abs_sub2, sse::abs_sub2, 16));
        IF_ARCH_X86(CALL(native::abs_rsub2, sse::abs_rsub2, 16));
        IF_ARCH_X86(CALL(native::abs_mul2, sse::abs_mul2, 16));
        IF_ARCH_X86(CALL(native::abs_div2, sse::abs_div2, 16));
        IF_ARCH_X86(CALL(native::abs_rdiv2, sse::abs_rdiv2, 16));

        IF_ARCH_X86_64(CALL(native::abs_add2, avx::x64_abs_add2, 32));
        IF_ARCH_X86_64(CALL(native::abs_sub2, avx::x64_abs_sub2, 32));
        IF_ARCH_X86_64(CALL(native::abs_rsub2, avx::x64_abs_rsub2, 32));
        IF_ARCH_X86_64(CALL(native::abs_mul2, avx::x64_abs_mul2, 32));
        IF_ARCH_X86_64(CALL(native::abs_div2, avx::x64_abs_div2, 32));
        IF_ARCH_X86_64(CALL(native::abs_rdiv2, avx::x64_abs_rdiv2, 32));

        IF_ARCH_ARM(CALL(native::abs_add2, neon_d32::abs_add2, 16));
        IF_ARCH_ARM(CALL(native::abs_sub2, neon_d32::abs_sub2, 16));
        IF_ARCH_ARM(CALL(native::abs_rsub2, neon_d32::abs_rsub2, 16));
        IF_ARCH_ARM(CALL(native::abs_mul2, neon_d32::abs_mul2, 16));
        IF_ARCH_ARM(CALL(native::abs_div2, neon_d32::abs_div2, 16));
        IF_ARCH_ARM(CALL(native::abs_rdiv2, neon_d32::abs_rdiv2, 16));

        IF_ARCH_AARCH64(CALL(native::abs_add2, asimd::abs_add2, 16));
        IF_ARCH_AARCH64(CALL(native::abs_sub2, asimd::abs_sub2, 16));
        IF_ARCH_AARCH64(CALL(native::abs_rsub2, asimd::abs_rsub2, 16));
        IF_ARCH_AARCH64(CALL(native::abs_mul2, asimd::abs_mul2, 16));
        IF_ARCH_AARCH64(CALL(native::abs_div2, asimd::abs_div2, 16));
        IF_ARCH_AARCH64(CALL(native::abs_rdiv2, asimd::abs_rdiv2, 16));
    }
UTEST_END


