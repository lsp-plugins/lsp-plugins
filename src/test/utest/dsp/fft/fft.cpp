/*
 * fft.cpp
 *
 *  Created on: 30 авг. 2018 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <test/FloatBuffer.h>
#include <dsp/dsp.h>

namespace native
{
    void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
    void reverse_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
    void packed_direct_fft(float *dst, const float *src, size_t rank);
    void packed_reverse_fft(float *dst, const float *src, size_t rank);

    void conv_direct_fft(float *dst, const float *src, size_t rank);
}

IF_ARCH_X86(
    namespace sse
    {
        void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        void reverse_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        void packed_direct_fft(float *dst, const float *src, size_t rank);
        void packed_reverse_fft(float *dst, const float *src, size_t rank);

        void conv_direct_fft(float *dst, const float *src, size_t rank);
    }
)

typedef void (* fft_t)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
typedef void (* packed_fft_t)(float *dst, const float *src, size_t rank);

UTEST_BEGIN("dsp.fft", fft)

    void call(const char *label, size_t align, fft_t func1, fft_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        for (size_t rank=6; rank<=16; ++rank)
        {
            size_t count = 1 << rank;
            for (size_t mask=0; mask <= 0x0f; ++mask)
            {
                FloatBuffer src_re(count, align, mask & 0x01);
                FloatBuffer src_im(count, align, mask & 0x02);
                FloatBuffer dst1_re(count, align, mask & 0x04);
                FloatBuffer dst1_im(count, align, mask & 0x08);
                FloatBuffer dst2_re(dst1_re);
                FloatBuffer dst2_im(dst1_im);

                func1(dst1_re, dst1_im, src_re, src_im, rank);
                func2(dst2_re, dst2_im, src_re, src_im, rank);

                UTEST_ASSERT_MSG(src_re.valid(), "Source buffer RE corrupted");
                UTEST_ASSERT_MSG(src_im.valid(), "Source buffer IM corrupted");
                UTEST_ASSERT_MSG(dst1_re.valid(), "Destination buffer 1 RE corrupted");
                UTEST_ASSERT_MSG(dst1_im.valid(), "Destination buffer 1 IM corrupted");
                UTEST_ASSERT_MSG(dst2_re.valid(), "Destination buffer 2 RE corrupted");
                UTEST_ASSERT_MSG(dst2_im.valid(), "Destination buffer 2 IM corrupted");

                // Compare buffers
                if ((!dst1_re.equals_relative(dst2_re, 1e-4)) || (!dst1_im.equals_relative(dst2_im, 1e-4)))
                {
                    src_re.dump("src_re ");
                    src_re.dump("src_im ");
                    dst1_re.dump("dst1_re");
                    dst1_im.dump("dst1_im");
                    dst2_re.dump("dst2_re");
                    dst2_im.dump("dst2_im");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    void call(const char *label, size_t align, packed_fft_t func1, packed_fft_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        for (size_t rank=6; rank<=16; ++rank)
        {
            size_t count = 1 << (rank + 1);
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                FloatBuffer src(count, align, mask & 0x01);
                FloatBuffer dst1(count, align, mask & 0x02);
                FloatBuffer dst2(dst1);

                func1(dst1, src, rank);
                func2(dst2, src, rank);

                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if ((!dst1.equals_relative(dst2, 1e-4)))
                {
                    src.dump("src ");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    UTEST_MAIN
    {
        // Do tests
        IF_ARCH_X86(call("direct_fft", 16, native::direct_fft, sse::direct_fft));
        IF_ARCH_X86(call("reverse_fft", 16, native::reverse_fft, sse::reverse_fft));
        IF_ARCH_X86(call("packed_direct_fft", 16, native::packed_direct_fft, sse::packed_direct_fft));
        IF_ARCH_X86(call("packed_reverse_fft", 16, native::packed_reverse_fft, sse::packed_reverse_fft));
        IF_ARCH_X86(call("conv_direct_fft", 16, native::conv_direct_fft, sse::conv_direct_fft));
    }
UTEST_END;
