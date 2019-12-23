/*
 * norm.cpp
 *
 *  Created on: 18 дек. 2019 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <test/FloatBuffer.h>
#include <dsp/dsp.h>

#define MIN_RANK    4
#define MAX_RANK    16

namespace native
{
    void normalize_fft3(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
    void normalize_fft2(float *dst_re, float *dst_im, size_t rank);
}

IF_ARCH_X86(
    namespace sse
    {
        void normalize_fft3(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        void normalize_fft2(float *dst_re, float *dst_im, size_t rank);
    }

    namespace avx
    {
        void normalize_fft3(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        void normalize_fft2(float *dst_re, float *dst_im, size_t rank);
    }

    namespace avx2
    {
        void normalize_fft3(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        void normalize_fft2(float *dst_re, float *dst_im, size_t rank);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void normalize_fft3(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        void normalize_fft2(float *dst_re, float *dst_im, size_t rank);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void normalize_fft3(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        void normalize_fft2(float *dst_re, float *dst_im, size_t rank);
    }
)

typedef void (* normalize_fft3_t)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
typedef void (* normalize_fft2_t)(float *dst_re, float *dst_im, size_t rank);

UTEST_BEGIN("dsp.fft", norm)

    void call(const char *label, size_t align, normalize_fft3_t func1, normalize_fft3_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        for (size_t rank=MIN_RANK; rank <= MAX_RANK; ++rank)
        {
            size_t count = 1 << rank;

            for (size_t mask=0; mask <= 0x0f; ++mask)
            {
                FloatBuffer src_re(count, align, mask & 0x01);
                FloatBuffer src_im(count, align, mask & 0x02);
                FloatBuffer dst1_re(count, align, mask & 0x04);
                FloatBuffer dst1_im(count, align, mask & 0x08);
                FloatBuffer dst2_re(count, align, mask & 0x04);
                FloatBuffer dst2_im(count, align, mask & 0x08);

                printf("Testing '%s' for rank=%d, mask=0x%x...\n", label, int(rank), int(mask));
                src_re.randomize_sign();
                src_im.randomize_sign();
                func1(dst1_re, dst1_im, src_re, src_im, rank);
                func2(dst2_re, dst2_im, src_re, src_im, rank);

                UTEST_ASSERT_MSG(src_re.valid(), "Source buffer RE corrupted");
                UTEST_ASSERT_MSG(src_im.valid(), "Source buffer IM corrupted");
                UTEST_ASSERT_MSG(dst1_re.valid(), "Destination buffer 1 RE corrupted");
                UTEST_ASSERT_MSG(dst1_im.valid(), "Destination buffer 1 IM corrupted");
                UTEST_ASSERT_MSG(dst2_re.valid(), "Destination buffer 2 RE corrupted");
                UTEST_ASSERT_MSG(dst2_im.valid(), "Destination buffer 2 IM corrupted");

                // Compare buffers
                if ((!dst1_re.equals_adaptive(dst2_re)) || (!dst1_im.equals_adaptive(dst2_im)))
                {
                    src_re.dump("src_re ");
                    src_im.dump("src_im ");
                    dst1_re.dump("dst1_re");
                    dst2_re.dump("dst2_re");
                    dst1_im.dump("dst1_im");
                    dst2_im.dump("dst2_im");

                    ssize_t diff = dst1_re.last_diff();
                    if (diff >= 0)
                    {
                        UTEST_FAIL_MSG("Real output of functions for test '%s' differs at sample %d (%.5f vs %.5f)",
                                label, int(diff), dst1_re.get(diff), dst2_re.get(diff));
                    }
                    else
                    {
                        diff = dst1_im.last_diff();
                        UTEST_FAIL_MSG("Imaginary output of functions for test '%s' differs at sample %d (%.5f vs %.5f)",
                                label, int(diff), dst1_im.get(diff), dst2_im.get(diff));
                    }
                }
            }
        }
    }

    void call(const char *label, size_t align, normalize_fft2_t func1, normalize_fft2_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        for (size_t rank=MIN_RANK; rank <= MAX_RANK; ++rank)
        {
            size_t count = 1 << rank;

            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                FloatBuffer dst1_re(count, align, mask & 0x01);
                FloatBuffer dst1_im(count, align, mask & 0x02);
                dst1_re.randomize_sign();
                dst1_im.randomize_sign();
                FloatBuffer dst2_re(dst1_re);
                FloatBuffer dst2_im(dst1_im);

                printf("Testing '%s' for rank=%d, mask=0x%x...\n", label, int(rank), int(mask));
                func1(dst1_re, dst1_im, rank);
                func2(dst2_re, dst2_im, rank);

                UTEST_ASSERT_MSG(dst1_re.valid(), "Destination buffer 1 RE corrupted");
                UTEST_ASSERT_MSG(dst1_im.valid(), "Destination buffer 1 IM corrupted");
                UTEST_ASSERT_MSG(dst2_re.valid(), "Destination buffer 2 RE corrupted");
                UTEST_ASSERT_MSG(dst2_im.valid(), "Destination buffer 2 IM corrupted");

                // Compare buffers
                if ((!dst1_re.equals_adaptive(dst2_re)) || (!dst1_im.equals_adaptive(dst2_im)))
                {
                    dst1_re.dump("dst1_re");
                    dst2_re.dump("dst2_re");
                    dst1_im.dump("dst1_im");
                    dst2_im.dump("dst2_im");

                    ssize_t diff = dst1_re.last_diff();
                    if (diff >= 0)
                    {
                        UTEST_FAIL_MSG("Real output of functions for test '%s' differs at sample %d (%.5f vs %.5f)",
                                label, int(diff), dst1_re.get(diff), dst2_re.get(diff));
                    }
                    else
                    {
                        diff = dst1_im.last_diff();
                        UTEST_FAIL_MSG("Imaginary output of functions for test '%s' differs at sample %d (%.5f vs %.5f)",
                                label, int(diff), dst1_im.get(diff), dst2_im.get(diff));
                    }
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(native, func, align) \
            call(#func, align, native, func)

        // Do tests
        IF_ARCH_X86(CALL(native::normalize_fft2, sse::normalize_fft2, 16));
        IF_ARCH_X86(CALL(native::normalize_fft3, sse::normalize_fft3, 16));

        IF_ARCH_X86(CALL(native::normalize_fft2, avx::normalize_fft2, 32));
        IF_ARCH_X86(CALL(native::normalize_fft3, avx::normalize_fft3, 32));

        IF_ARCH_X86(CALL(native::normalize_fft2, avx2::normalize_fft2, 32));
        IF_ARCH_X86(CALL(native::normalize_fft3, avx2::normalize_fft3, 32));

        IF_ARCH_ARM(CALL(native::normalize_fft2, neon_d32::normalize_fft2, 16));
        IF_ARCH_ARM(CALL(native::normalize_fft3, neon_d32::normalize_fft3, 16));

        IF_ARCH_AARCH64(CALL(native::normalize_fft2, asimd::normalize_fft2, 16));
        IF_ARCH_AARCH64(CALL(native::normalize_fft3, asimd::normalize_fft3, 16));
    }
UTEST_END




