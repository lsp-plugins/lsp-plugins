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
    void hsla_to_rgba(float *dst, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse2
    {
        void hsla_to_rgba(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void hsla_to_rgba(float *dst, const float *src, size_t count);
    }
)

typedef void (* hsla_to_rgba_t)(float *dst, const float *src, size_t count);

UTEST_BEGIN("dsp.graphics", hsla_to_rgba)
    void call(const char *label, size_t align, hsla_to_rgba_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

//        size_t count = 1;

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

//                src.vfill(0, 4,
//                        0.86844, 0.07115, 0.49626, 0.57571
//                    );

//                src.vfill(0, 16,
//                        0.0f, 0.0f, 0.5f, 0.12345,
//                        0.0f, 1.0f, 0.5f, 0.12345,
//                        0.0f, 1.0f, 1.0f, 0.12345,
//                        0.5f, 0.5f, 0.5f, 0.12345
//                    );

                /*
                    Internal state table:

                    T1  = 0.5   0     1     0.25
                    T2  = 0.5   1     1     0.75
                    TR  = 0.33  0.33  0.33  0.83
                    TG  = 0     0     0     0.5
                    TB  = 0.66  0.66  0.66  0.16
                    K   = 0     6     0     3
                    R   = 0.5   1     1     0.25
                    G   = 0.5   0     1     0.75
                    B   = 0.5   0     1     0.75

                    ktr = 0.50  2.00  1.00  2.75
                    ktg = 0.50  0.00  1.00  1.75
                    ktb = 0.50  4.00  1.00  0.75
                    rtr = 0.50  2.00  1.00  -0.25
                    rtg = 0.50  4.00  1.00  0.75
                    rtb = 0.50  0.00  1.00  1.75

                 */

//                src.dump("src");

                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Call functions
                native::hsla_to_rgba(dst1, src, count);
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
        IF_ARCH_X86(call("sse2::hsla_to_rgba", 16, sse2::hsla_to_rgba));
        IF_ARCH_ARM(call("neon_d32::hsla_to_rgba", 16, neon_d32::hsla_to_rgba));
    }

UTEST_END;




