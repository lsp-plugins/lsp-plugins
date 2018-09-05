/*
 * div.cpp
 *
 *  Created on: 5 сент. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/FloatBuffer.h>

namespace native
{
    void complex_div2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count);
    void complex_rdiv2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count);
    void complex_div3(float *dst_re, float *dst_im, const float *t_re, const float *t_im, const float *b_re, const float *b_im, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void complex_div2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count);
        void complex_rdiv2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count);
        void complex_div3(float *dst_re, float *dst_im, const float *t_re, const float *t_im, const float *b_re, const float *b_im, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void complex_div2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count);
        void complex_rdiv2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count);
        void complex_div3(float *dst_re, float *dst_im, const float *t_re, const float *t_im, const float *b_re, const float *b_im, size_t count);
    }
)

typedef void (* complex_div2_t) (float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count);
typedef void (* complex_div3_t) (float *dst_re, float *dst_im, const float *t_re, const float *t_im, const float *b_re, const float *b_im, size_t count);

UTEST_BEGIN("dsp.complex", div)

    void call(const char *text,  size_t align, complex_div2_t func1, complex_div2_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
                32, 33, 37, 48, 49, 64, 65, 0x3f, 100, 999, 0x1fff)
        {
            for (size_t mask=0; mask <= 0x0f; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", text, int(count), int(mask));

                FloatBuffer src_re(count, align, mask & 0x01);
                FloatBuffer src_im(count, align, mask & 0x02);
                FloatBuffer dst1_re(count, align, mask & 0x04);
                FloatBuffer dst1_im(count, align, mask & 0x08);
                FloatBuffer dst2_re(dst1_re);
                FloatBuffer dst2_im(dst1_im);

                // Call functions
                func1(dst1_re, dst1_im, src_re, src_im, count);
                func2(dst2_re, dst2_im, src_re, src_im, count);

                UTEST_ASSERT_MSG(src_re.valid(), "Source buffer RE corrupted");
                UTEST_ASSERT_MSG(src_im.valid(), "Source buffer IM corrupted");
                UTEST_ASSERT_MSG(dst1_re.valid(), "Destination buffer 1 RE corrupted");
                UTEST_ASSERT_MSG(dst1_im.valid(), "Destination buffer 1 IM corrupted");
                UTEST_ASSERT_MSG(dst2_re.valid(), "Destination buffer 2 RE corrupted");
                UTEST_ASSERT_MSG(dst2_im.valid(), "Destination buffer 2 IM corrupted");

                // Compare buffers
                if ((!dst2_re.equals_absolute(dst1_re, 1e-4)) || (!dst2_im.equals_absolute(dst1_im, 1e-4)))
                {
                    src_re.dump("src_re ");
                    src_im.dump("src_im");
                    dst1_re.dump("dst1_re");
                    dst1_im.dump("dst1_im");
                    dst2_re.dump("dst2_re");
                    dst2_im.dump("dst2_im");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", text);
                }
            }
        }
    }

    void call(const char *text,  size_t align, complex_div3_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
                32, 33, 37, 48, 49, 64, 65, 0x3f, 100, 999, 0x1fff)
        {
            for (size_t mask=0; mask <= 0x3f; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", text, int(count), int(mask));

                FloatBuffer src1_re(count, align, mask & 0x01);
                FloatBuffer src1_im(count, align, mask & 0x02);
                FloatBuffer src2_re(count, align, mask & 0x04);
                FloatBuffer src2_im(count, align, mask & 0x08);
                FloatBuffer dst1_re(count, align, mask & 0x10);
                FloatBuffer dst1_im(count, align, mask & 0x20);
                FloatBuffer dst2_re(dst1_re);
                FloatBuffer dst2_im(dst1_im);

                // Call functions
                native::complex_div3(dst1_re, dst1_im, src1_re, src1_im, src2_re, src2_im, count);
                func(dst2_re, dst2_im, src1_re, src1_im, src2_re, src2_im, count);

                UTEST_ASSERT_MSG(src1_re.valid(), "Source buffer 1 RE corrupted");
                UTEST_ASSERT_MSG(src1_im.valid(), "Source buffer 1 IM corrupted");
                UTEST_ASSERT_MSG(src2_re.valid(), "Source buffer 2 RE corrupted");
                UTEST_ASSERT_MSG(src2_im.valid(), "Source buffer 2 IM corrupted");
                UTEST_ASSERT_MSG(dst1_re.valid(), "Destination buffer 1 RE corrupted");
                UTEST_ASSERT_MSG(dst1_im.valid(), "Destination buffer 1 IM corrupted");
                UTEST_ASSERT_MSG(dst2_re.valid(), "Destination buffer 2 RE corrupted");
                UTEST_ASSERT_MSG(dst2_im.valid(), "Destination buffer 2 IM corrupted");

                // Compare buffers
                if ((!dst2_re.equals_absolute(dst1_re, 1e-4)) || (!dst2_im.equals_absolute(dst1_im, 1e-4)))
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
        IF_ARCH_X86(call("sse:complex_div2", 16, native::complex_div2, sse::complex_div2));
        IF_ARCH_X86(call("sse:complex_rdiv2", 16, native::complex_rdiv2, sse::complex_rdiv2));
        IF_ARCH_X86(call("sse:complex_div3", 16, sse::complex_div3));
        IF_ARCH_ARM(call("neon_d32:complex_div2", 16, native::complex_div2, neon_d32::complex_div2));
        IF_ARCH_ARM(call("neon_d32:complex_rdiv2", 16, native::complex_rdiv2, neon_d32::complex_rdiv2));
        IF_ARCH_ARM(call("neon_d32:complex_div3", 16, neon_d32::complex_div3));
    }

UTEST_END;



