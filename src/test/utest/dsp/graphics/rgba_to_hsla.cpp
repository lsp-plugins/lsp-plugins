/*
 * hsla_to_rgba.cpp
 *
 *  Created on: 16 нояб. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/FloatBuffer.h>

namespace native
{
    void rgba_to_hsla(float *dst, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse2
    {
        void rgba_to_hsla(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void rgba_to_hsla(float *dst, const float *src, size_t count);
    }
)

typedef void (* rgba_to_hsla_t)(float *dst, const float *src, size_t count);

UTEST_BEGIN("dsp.graphics", rgba_to_hsla)
    void call(const char *label, size_t align, rgba_to_hsla_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 768, 999, 1024, 0x1fff)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer src(count*4, align, mask & 0x01);
                FloatBuffer dst1(count*4, align, mask & 0x02);
                FloatBuffer dst2(dst1);

                src.randomize_0to1();

//                src.vfill(0, 16,
//                        0.50000, 0.50000, 0.50000, 0.12345,
//                        1.00000, 0.00000, 0.00000, 0.12345,
//                        1.00000, 1.00000, 1.00000, 0.12345,
//                        0.25000, 0.75000, 0.75000, 0.12345
//                    );

//                src.dump("src");

                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Call functions
                native::rgba_to_hsla(dst1, src, count);
                func(dst2, src, count);

//                dst1.dump("dst1");
//                dst2.dump("dst2");

                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_absolute(dst2, 1e-3f))
                {
                    src.dump("src ");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Result of functions differs at sample %d: %.5f vs %.5f",
                        int(dst1.last_diff()), dst1.get_diff(), dst2.get_diff());
                }
            }
        }
    }

    UTEST_MAIN
    {
        IF_ARCH_X86(call("sse2::rgba_to_hsla", 16, sse2::rgba_to_hsla));
        IF_ARCH_ARM(call("neon_d32::rgba_to_hsla", 16, neon_d32::rgba_to_hsla));
    }

UTEST_END;




