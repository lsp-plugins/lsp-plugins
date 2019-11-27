/*
 * abs.cpp
 *
 *  Created on: 30 авг. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/FloatBuffer.h>

namespace native
{
    void abs1(float *src, size_t count);
    void abs2(float *dst, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void abs1(float *src, size_t count);
        void abs2(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_X86_64(
    namespace avx
    {
        void x64_abs1(float *src, size_t count);
        void x64_abs2(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void abs1(float *src, size_t count);
        void abs2(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void abs1(float *src, size_t count);
        void abs2(float *dst, const float *src, size_t count);
    }
)

typedef void (* abs1_t)(float *src, size_t count);
typedef void (* abs2_t)(float *dst, const float *src, size_t count);

//-----------------------------------------------------------------------------
// Unit test for complex multiplication
UTEST_BEGIN("dsp.pmath", abs)

    void call(const char *label, size_t align, abs1_t func1, abs1_t func2)
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

                FloatBuffer dst1(count, align, mask & 0x01);
                dst1.randomize_sign();
                FloatBuffer dst2(dst1);

                // Call functions
                func1(dst1, count);
                func2(dst2, count);

                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_absolute(dst2, 1e-5))
                {
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    void call(const char *label, size_t align, abs2_t func1, abs2_t func2)
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
                src.randomize_sign();

                // Call functions
                func1(dst1, src, count);
                func2(dst2, src, count);

                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_absolute(dst2, 1e-5))
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
        #define CALL(native, func, align) \
            call(#func, align, native, func)

        IF_ARCH_X86(CALL(native::abs1, sse::abs1, 16));
        IF_ARCH_X86(CALL(native::abs2, sse::abs2, 16));

        IF_ARCH_X86_64(CALL(native::abs1, avx::x64_abs1, 16));
        IF_ARCH_X86_64(CALL(native::abs2, avx::x64_abs2, 16));

        IF_ARCH_ARM(CALL(native::abs1, neon_d32::abs1, 16));
        IF_ARCH_ARM(CALL(native::abs2, neon_d32::abs2, 16));

        IF_ARCH_AARCH64(CALL(native::abs1, asimd::abs1, 16));
        IF_ARCH_AARCH64(CALL(native::abs2, asimd::abs2, 16));
    }
UTEST_END


