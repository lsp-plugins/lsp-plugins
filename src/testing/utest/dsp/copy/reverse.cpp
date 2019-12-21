/*
 * reverse.cpp
 *
 *  Created on: 30 авг. 2018 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <test/FloatBuffer.h>
#include <math.h>

namespace native
{
    void reverse1(float *dst, size_t count);
    void reverse2(float *dst, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void reverse1(float *dst, size_t count);
        void reverse2(float *dst, const float *src, size_t count);
    }

    namespace avx
    {
        void reverse1(float *dst, size_t count);
        void reverse2(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void reverse1(float *dst, size_t count);
        void reverse2(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void reverse1(float *dst, size_t count);
        void reverse2(float *dst, const float *src, size_t count);
    }
)

typedef void (* reverse1_t)(float *dst, size_t count);
typedef void (* reverse2_t)(float *dst, const float *src, size_t count);

UTEST_BEGIN("dsp.copy", reverse)
    void call(const char *label, size_t align, reverse1_t func1, reverse1_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0x80, 0x100, 0x1ff, 999, 0xfff)
        {
            for (size_t mask=0; mask <= 0x01; ++mask)
            {
                FloatBuffer src(count, align, mask & 0x01);
                FloatBuffer dst1(src);
                FloatBuffer dst2(src);

                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));
                func1(dst1, count);
                func2(dst2, count);

                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                if (!dst1.equals_absolute(dst2))
                {
                    src.dump("src");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    void call(const char *label, size_t align, reverse2_t func1, reverse2_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, /*0, */ 1, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0x80, 0x100, 0x1ff, 999, 0xfff)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                FloatBuffer src(count, align, mask & 0x01);
                FloatBuffer dst1(count, align, mask & 0x02);
                FloatBuffer dst2(dst1);

                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));
                func1(dst1, src, count);
                func2(dst2, src, count);

                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                if (!dst1.equals_absolute(dst2))
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
        #define CALL(native, func, align)   \
            call(#func, align, native, func)

        IF_ARCH_X86(CALL(native::reverse1, sse::reverse1, 16));
        IF_ARCH_X86(CALL(native::reverse2, sse::reverse2, 16));
        IF_ARCH_X86(CALL(native::reverse1, avx::reverse1, 32));
        IF_ARCH_X86(CALL(native::reverse2, avx::reverse2, 32));

        IF_ARCH_ARM(CALL(native::reverse1, neon_d32::reverse1, 16));
        IF_ARCH_ARM(CALL(native::reverse2, neon_d32::reverse2, 16));

        IF_ARCH_AARCH64(CALL(native::reverse1, asimd::reverse1, 16));
        IF_ARCH_AARCH64(CALL(native::reverse2, asimd::reverse2, 16));
    }

UTEST_END;


