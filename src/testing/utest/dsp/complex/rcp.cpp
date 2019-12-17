/*
 * rcp.cpp
 *
 *  Created on: 23 авг. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/FloatBuffer.h>

namespace native
{
    void complex_rcp1(float *dst_re, float *dst_im, size_t count);
    void complex_rcp2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void complex_rcp1(float *dst_re, float *dst_im, size_t count);
        void complex_rcp2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count);
    }

    namespace avx
    {
        void complex_rcp1(float *dst_re, float *dst_im, size_t count);
        void complex_rcp2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count);

        void complex_rcp1_fma3(float *dst_re, float *dst_im, size_t count);
        void complex_rcp2_fma3(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void complex_rcp1(float *dst_re, float *dst_im, size_t count);
        void complex_rcp2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void complex_rcp1(float *dst_re, float *dst_im, size_t count);
        void complex_rcp2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count);
    }
)

typedef void (* complex_rcp1_t) (float *dst_re, float *dst_im, size_t count);
typedef void (* complex_rcp2_t) (float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count);

UTEST_BEGIN("dsp.complex", rcp)

    void call(const char *text, size_t align, complex_rcp1_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
                32, 33, 37, 48, 49, 64, 65, 0x3f, 100, 999, 0x1fff)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", text, int(count), int(mask));

                FloatBuffer dst1_re(count, align, mask & 0x01);
                FloatBuffer dst1_im(count, align, mask & 0x02);
                FloatBuffer dst2_re(dst1_re);
                FloatBuffer dst2_im(dst1_im);

                // Call functions
                native::complex_rcp1(dst1_re, dst1_im, count);
                func(dst2_re, dst2_im, count);

                UTEST_ASSERT_MSG(dst1_re.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst1_im.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2_re.valid(), "Destination buffer 2 corrupted");
                UTEST_ASSERT_MSG(dst2_im.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if ((!dst1_re.equals_relative(dst2_re, 1e-4)) || (!dst1_im.equals_relative(dst2_im, 1e-4)))
                {
                    dst1_re.dump("dst1_re");
                    dst1_im.dump("dst1_im");
                    dst2_re.dump("dst2_re");
                    dst2_im.dump("dst2_im");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", text);
                }
            }
        }
    }

    void call(const char *text, size_t align, complex_rcp2_t func)
    {
        if (!UTEST_SUPPORTED(func))
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
                native::complex_rcp2(dst1_re, dst1_im, src_re, src_im, count);
                func(dst2_re, dst2_im, src_re, src_im, count);

                UTEST_ASSERT_MSG(src_re.valid(), "Source buffer corrupted");
                UTEST_ASSERT_MSG(src_im.valid(), "Source buffer corrupted");
                UTEST_ASSERT_MSG(dst1_re.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst1_im.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2_re.valid(), "Destination buffer 2 corrupted");
                UTEST_ASSERT_MSG(dst2_im.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if ((!dst1_re.equals_relative(dst2_re, 1e-4)) || (!dst1_im.equals_relative(dst2_im, 1e-4)))
                {
                    src_re.dump("src_re");
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

    UTEST_MAIN
    {
        #define CALL(func, align) \
            call(#func, align, func)

        IF_ARCH_X86(CALL(sse::complex_rcp1, 16));
        IF_ARCH_X86(CALL(sse::complex_rcp2, 16));
        IF_ARCH_X86(CALL(avx::complex_rcp1, 32));
        IF_ARCH_X86(CALL(avx::complex_rcp2, 32));
        IF_ARCH_X86(CALL(avx::complex_rcp1_fma3, 32));
        IF_ARCH_X86(CALL(avx::complex_rcp2_fma3, 32));

        IF_ARCH_ARM(CALL(neon_d32::complex_rcp1, 16));
        IF_ARCH_ARM(CALL(neon_d32::complex_rcp2, 16));

        IF_ARCH_AARCH64(CALL(asimd::complex_rcp1, 16));
        IF_ARCH_AARCH64(CALL(asimd::complex_rcp2, 16));
    }

UTEST_END;
