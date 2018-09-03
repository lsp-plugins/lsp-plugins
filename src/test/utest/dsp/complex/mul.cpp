/*
 * mul.cpp
 *
 *  Created on: 23 авг. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/FloatBuffer.h>

namespace native
{
    void complex_mul(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count);
    void packed_complex_mul(float *dst, const float *src1, const float *src2, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void complex_mul(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count);
        void packed_complex_mul(float *dst, const float *src1, const float *src2, size_t count);
    }

    namespace sse3
    {
        void packed_complex_mul(float *dst, const float *src1, const float *src2, size_t count);
    }

    IF_ARCH_X86_64(
        namespace sse3
        {
            void x64_packed_complex_mul(float *dst, const float *src1, const float *src2, size_t count);
        }

        namespace avx
        {
            void x64_complex_mul(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count);
            void x64_complex_mul_fma3(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count);
            void x64_packed_complex_mul(float *dst, const float *src1, const float *src2, size_t count);
            void x64_packed_complex_mul_fma3(float *dst, const float *src1, const float *src2, size_t count);
        }
    )
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void complex_mul3(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count);
        void complex_mul3_x12(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count);
    }
)

typedef void (* complex_mul_t) (float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count);
typedef void (* packed_complex_mul_t) (float *dst, const float *src1, const float *src2, size_t count);


UTEST_BEGIN("dsp.complex", mul)

    void call(const char *text,  size_t align, packed_complex_mul_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 999)
        {
            for (size_t mask=0; mask <= 0x07; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", text, int(count), int(mask));

                FloatBuffer src1(count*2, align, mask & 0x01);
                FloatBuffer src2(count*2, align, mask & 0x02);
                FloatBuffer dst1(count*2, align, mask & 0x04);
                FloatBuffer dst2(count*2, align, mask & 0x04);

                // Call functions
                native::packed_complex_mul(dst1, src1, src2, count);
                func(dst2, src1, src2, count);

                UTEST_ASSERT_MSG(src1.valid(), "Source buffer 1 corrupted");
                UTEST_ASSERT_MSG(src2.valid(), "Source buffer 2 corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_absolute(dst2, 1e-5))
                {
                    src1.dump("src1");
                    src2.dump("src2");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", text);
                }
            }
        }
    }

    void call(const char *text,  size_t align, complex_mul_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 999)
        {
            for (size_t mask=0; mask <= 0x3f; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", text, int(count), int(mask));

                FloatBuffer src1_re(count, align, mask & 0x01);
                FloatBuffer src1_im(count, align, mask & 0x02);
                FloatBuffer src2_re(count, align, mask & 0x04);
                FloatBuffer src2_im(count, align, mask & 0x08);
                FloatBuffer dst1_re(count, align, mask & 0x10);
                FloatBuffer dst2_re(count, align, mask & 0x10);
                FloatBuffer dst1_im(count, align, mask & 0x20);
                FloatBuffer dst2_im(count, align, mask & 0x20);

                // Call functions
                native::complex_mul(dst1_re, dst1_im, src1_re, src1_im, src2_re, src2_im, count);
                func(dst2_re, dst2_im, src1_re, src1_im, src2_re, src2_im, count);

                UTEST_ASSERT_MSG(src1_re.valid(), "src1_re corrupted");
                UTEST_ASSERT_MSG(src1_im.valid(), "src1_im corrupted");
                UTEST_ASSERT_MSG(src2_re.valid(), "src2_re corrupted");
                UTEST_ASSERT_MSG(src2_im.valid(), "src2_im corrupted");
                UTEST_ASSERT_MSG(dst1_re.valid(), "dst1_re corrupted");
                UTEST_ASSERT_MSG(dst1_im.valid(), "dst1_im corrupted");
                UTEST_ASSERT_MSG(dst2_re.valid(), "dst2_re corrupted");
                UTEST_ASSERT_MSG(dst2_im.valid(), "dst2_im corrupted");

                // Compare buffers
                if (!(dst1_re.equals_absolute(dst2_re, 1e-5) && (dst1_im.equals_absolute(dst2_im, 1e-5))))
                {
                    src1_re.dump("src1_re");
                    src1_im.dump("src1_im");
                    src2_re.dump("src2_re");
                    src2_im.dump("src2_im");
                    dst1_re.dump("dst1_re");
                    dst1_im.dump("dst1_im");
                    dst2_re.dump("dst2_re");
                    dst2_im.dump("dst2_im");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", text);
                }
            }
        }
    }

    UTEST_MAIN
    {
        IF_ARCH_X86(call("unpacked_sse", 16, sse::complex_mul));
        IF_ARCH_X86_64(call("x64_unpacked_avx", 16, avx::x64_complex_mul));
        IF_ARCH_X86_64(call("x64_unpacked_fma3", 16, avx::x64_complex_mul_fma3));
        IF_ARCH_ARM(call("unpacked_neon_d32", 16, neon_d32::complex_mul3));
        IF_ARCH_ARM(call("unpacked_neon_d32_x12", 16, neon_d32::complex_mul3_x12));

        IF_ARCH_X86(call("packed_sse", 16, sse::packed_complex_mul));
        IF_ARCH_X86(call("packed_sse3", 16, sse3::packed_complex_mul));
        IF_ARCH_X86_64(call("x64_packed_sse3", 16, sse3::x64_packed_complex_mul));
        IF_ARCH_X86_64(call("x64_packed_avx", 32, avx::x64_packed_complex_mul));
        IF_ARCH_X86_64(call("x64_packed_fma3", 32, avx::x64_packed_complex_mul_fma3));
    }

UTEST_END;
