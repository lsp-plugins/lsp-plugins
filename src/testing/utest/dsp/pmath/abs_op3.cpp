/*
 * abs_op3.cpp
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
    void    abs_add3(float *dst, const float *src1, const float *src2, size_t count);
    void    abs_sub3(float *dst, const float *src1, const float *src2, size_t count);
    void    abs_rsub3(float *dst, const float *src1, const float *src2, size_t count);
    void    abs_mul3(float *dst, const float *src1, const float *src2, size_t count);
    void    abs_div3(float *dst, const float *src1, const float *src2, size_t count);
    void    abs_rdiv3(float *dst, const float *src1, const float *src2, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void    abs_add3(float *dst, const float *src1, const float *src2, size_t count);
        void    abs_sub3(float *dst, const float *src1, const float *src2, size_t count);
        void    abs_rsub3(float *dst, const float *src1, const float *src2, size_t count);
        void    abs_mul3(float *dst, const float *src1, const float *src2, size_t count);
        void    abs_div3(float *dst, const float *src1, const float *src2, size_t count);
        void    abs_rdiv3(float *dst, const float *src1, const float *src2, size_t count);
    }
)

IF_ARCH_X86_64(
    namespace avx
    {
        void    x64_abs_add3(float *dst, const float *src1, const float *src2, size_t count);
        void    x64_abs_sub3(float *dst, const float *src1, const float *src2, size_t count);
        void    x64_abs_rsub3(float *dst, const float *src1, const float *src2, size_t count);
        void    x64_abs_mul3(float *dst, const float *src1, const float *src2, size_t count);
        void    x64_abs_div3(float *dst, const float *src1, const float *src2, size_t count);
        void    x64_abs_rdiv3(float *dst, const float *src1, const float *src2, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void    abs_add3(float *dst, const float *src1, const float *src2, size_t count);
        void    abs_sub3(float *dst, const float *src1, const float *src2, size_t count);
        void    abs_rsub3(float *dst, const float *src1, const float *src2, size_t count);
        void    abs_mul3(float *dst, const float *src1, const float *src2, size_t count);
        void    abs_div3(float *dst, const float *src1, const float *src2, size_t count);
        void    abs_rdiv3(float *dst, const float *src1, const float *src2, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void    abs_add3(float *dst, const float *src1, const float *src2, size_t count);
        void    abs_sub3(float *dst, const float *src1, const float *src2, size_t count);
        void    abs_rsub3(float *dst, const float *src1, const float *src2, size_t count);
        void    abs_mul3(float *dst, const float *src1, const float *src2, size_t count);
        void    abs_div3(float *dst, const float *src1, const float *src2, size_t count);
        void    abs_rdiv3(float *dst, const float *src1, const float *src2, size_t count);
    }
)

typedef void (* abs_op3_t)(float *dst, const float *src1, const float *src2, size_t count);

//-----------------------------------------------------------------------------
// Unit test for simple operations
UTEST_BEGIN("dsp.pmath", abs_op3)

    void call(const char *label, size_t align, abs_op3_t func1, abs_op3_t func2)
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
                FloatBuffer dst2(count, align, mask & 0x04);

                // Call functions
                src1.randomize_sign();
                src2.randomize_sign();
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
        #define CALL(native, func, align) \
            call(#func, align, native, func);

        IF_ARCH_X86(CALL(native::abs_add3, sse::abs_add3, 16));
        IF_ARCH_X86(CALL(native::abs_sub3, sse::abs_sub3, 16));
        IF_ARCH_X86(CALL(native::abs_rsub3, sse::abs_rsub3, 16));
        IF_ARCH_X86(CALL(native::abs_mul3, sse::abs_mul3, 16));
        IF_ARCH_X86(CALL(native::abs_div3, sse::abs_div3, 16));
        IF_ARCH_X86(CALL(native::abs_rdiv3, sse::abs_rdiv3, 16));

        IF_ARCH_X86_64(CALL(native::abs_add3, avx::x64_abs_add3, 32));
        IF_ARCH_X86_64(CALL(native::abs_sub3, avx::x64_abs_sub3, 32));
        IF_ARCH_X86_64(CALL(native::abs_rsub3, avx::x64_abs_rsub3, 32));
        IF_ARCH_X86_64(CALL(native::abs_mul3, avx::x64_abs_mul3, 32));
        IF_ARCH_X86_64(CALL(native::abs_div3, avx::x64_abs_div3, 32));
        IF_ARCH_X86_64(CALL(native::abs_rdiv3, avx::x64_abs_rdiv3, 32));

        IF_ARCH_ARM(CALL(native::abs_add3, neon_d32::abs_add3, 16));
        IF_ARCH_ARM(CALL(native::abs_sub3, neon_d32::abs_sub3, 16));
        IF_ARCH_ARM(CALL(native::abs_rsub3, neon_d32::abs_rsub3, 16));
        IF_ARCH_ARM(CALL(native::abs_mul3, neon_d32::abs_mul3, 16));
        IF_ARCH_ARM(CALL(native::abs_div3, neon_d32::abs_div3, 16));
        IF_ARCH_ARM(CALL(native::abs_rdiv3, neon_d32::abs_rdiv3, 16));

        IF_ARCH_AARCH64(CALL(native::abs_add3, asimd::abs_add3, 16));
        IF_ARCH_AARCH64(CALL(native::abs_sub3, asimd::abs_sub3, 16));
        IF_ARCH_AARCH64(CALL(native::abs_rsub3, asimd::abs_rsub3, 16));
        IF_ARCH_AARCH64(CALL(native::abs_mul3, asimd::abs_mul3, 16));
        IF_ARCH_AARCH64(CALL(native::abs_div3, asimd::abs_div3, 16));
        IF_ARCH_AARCH64(CALL(native::abs_rdiv3, asimd::abs_rdiv3, 16));
    }
UTEST_END



