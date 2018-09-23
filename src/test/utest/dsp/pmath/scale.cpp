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
    void    scale2(float *dst, float k, size_t count);
    void    scale3(float *dst, const float *src, float k, size_t count);

    void    scale_add3(float *dst, const float *src, float k, size_t count);
    void    scale_sub3(float *dst, const float *src, float k, size_t count);
    void    scale_mul3(float *dst, const float *src, float k, size_t count);
    void    scale_div3(float *dst, const float *src, float k, size_t count);

    void    scale_add4(float *dst, const float *src1, const float *src2, float k, size_t count);
    void    scale_sub4(float *dst, const float *src1, const float *src2, float k, size_t count);
    void    scale_mul4(float *dst, const float *src1, const float *src2, float k, size_t count);
    void    scale_div4(float *dst, const float *src1, const float *src2, float k, size_t count);
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

        void    scale_add4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    scale_sub4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    scale_mul4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    scale_div4(float *dst, const float *src1, const float *src2, float k, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void    scale2(float *dst, float k, size_t count);
        void    scale3(float *dst, const float *src, float k, size_t count);


        void    scale_add3(float *dst, const float *src, float k, size_t count);
        void    scale_sub3(float *dst, const float *src, float k, size_t count);
        void    scale_mul3(float *dst, const float *src, float k, size_t count);
        void    scale_div3(float *dst, const float *src, float k, size_t count);

        void    scale_add4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    scale_sub4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    scale_mul4(float *dst, const float *src1, const float *src2, float k, size_t count);
        void    scale_div4(float *dst, const float *src1, const float *src2, float k, size_t count);
    }
)

typedef void (* scale2_t)(float *dst, float k, size_t count);
typedef void (* scale3_t)(float *dst, const float *src, float k, size_t count);
typedef void (* scale4_t)(float *dst, const float *src1, const float *src2, float k, size_t count);

//-----------------------------------------------------------------------------
// Unit test for complex multiplication
UTEST_BEGIN("dsp.pmath", scale)

    void call(const char *label, size_t align, scale2_t func1, scale2_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 999, 0xfff)
        {
            for (size_t mask=0; mask <= 0x01; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer src(count, align, mask & 0x01);
                FloatBuffer dst1(src);
                FloatBuffer dst2(src);

                // Call functions
                func1(dst1, 0.5f, count);
                func2(dst2, 0.5f, count);

                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_relative(dst2, 1e-4))
                {
                    src.dump("src ");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    printf("index=%d, %.6f vs %.6f\n", dst1.last_diff(), dst1.get_diff(), dst2.get_diff());
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    void call(const char *label, size_t align, scale3_t func1, scale3_t func2)
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
                func1(dst1, src, 0.5f, count);
                func2(dst2, src, 0.5f, count);

                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_relative(dst2, 1e-4))
                {
                    src.dump("src ");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    printf("index=%d, %.6f vs %.6f\n", dst1.last_diff(), dst1.get_diff(), dst2.get_diff());
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    void call(const char *label, size_t align, scale4_t func1, scale4_t func2)
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
                func1(dst1, src1, src2, 0.5f, count);
                func2(dst2, src1, src2, 0.5f, count);

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
        IF_ARCH_X86(call("sse:scale2", 16, native::scale2, sse::scale2));
        IF_ARCH_X86(call("sse:scale3", 16, native::scale3, sse::scale3));
        IF_ARCH_ARM(call("neon_d32:scale2", 16, native::scale2, neon_d32::scale2));
        IF_ARCH_ARM(call("neon_d32:scale3", 16, native::scale3, neon_d32::scale3));

        IF_ARCH_X86(call("sse:scale_add3", 16, native::scale_add3, sse::scale_add3));
        IF_ARCH_X86(call("sse:scale_sub3", 16, native::scale_sub3, sse::scale_sub3));
        IF_ARCH_X86(call("sse:scale_mul3", 16, native::scale_mul3, sse::scale_mul3));
        IF_ARCH_X86(call("sse:scale_div3", 16, native::scale_div3, sse::scale_div3));

        IF_ARCH_ARM(call("neon_d32:scale_add3", 16, native::scale_add3, neon_d32::scale_add3));
        IF_ARCH_ARM(call("neon_d32:scale_sub3", 16, native::scale_sub3, neon_d32::scale_sub3));
        IF_ARCH_ARM(call("neon_d32:scale_mul3", 16, native::scale_mul3, neon_d32::scale_mul3));
        IF_ARCH_ARM(call("neon_d32:scale_div3", 16, native::scale_div3, neon_d32::scale_div3));

        IF_ARCH_X86(call("sse:scale_add4", 16, native::scale_add4, sse::scale_add4));
        IF_ARCH_X86(call("sse:scale_sub4", 16, native::scale_sub4, sse::scale_sub4));
        IF_ARCH_X86(call("sse:scale_mul4", 16, native::scale_mul4, sse::scale_mul4));
        IF_ARCH_X86(call("sse:scale_div4", 16, native::scale_div4, sse::scale_div4));

        IF_ARCH_ARM(call("neon_d32:scale_add4", 16, native::scale_add4, neon_d32::scale_add4));
        IF_ARCH_ARM(call("neon_d32:scale_sub4", 16, native::scale_sub4, neon_d32::scale_sub4));
        IF_ARCH_ARM(call("neon_d32:scale_mul4", 16, native::scale_mul4, neon_d32::scale_mul4));
        IF_ARCH_ARM(call("neon_d32:scale_div4", 16, native::scale_div4, neon_d32::scale_div4));
    }
UTEST_END


