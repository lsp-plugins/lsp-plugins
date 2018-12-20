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

IF_ARCH_ARM(
    namespace neon_d32
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

    void call(const char *label, size_t align, abs1_t func)
    {
        if (!UTEST_SUPPORTED(func))
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
                native::abs1(dst1, count);
                func(dst2, count);

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

    void call(const char *label, size_t align, abs2_t func)
    {
        if (!UTEST_SUPPORTED(func))
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
                native::abs2(dst1, src, count);
                func(dst2, src, count);

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
        IF_ARCH_X86(call("sse::abs1", 16, sse::abs1));
        IF_ARCH_X86(call("sse::abs2", 16, sse::abs2));
        IF_ARCH_ARM(call("neon_d32::abs1", 16, neon_d32::abs1));
        IF_ARCH_ARM(call("neon_d32::abs2", 16, neon_d32::abs2));
    }
UTEST_END


