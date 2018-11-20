/*
 * rgba_to_bgra32.cpp
 *
 *  Created on: 17 нояб. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/FloatBuffer.h>
#include <test/ByteBuffer.h>

namespace native
{
    void rgba_to_bgra32(void *dst, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse2
    {
        void rgba_to_bgra32(void *dst, const float *src, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void rgba_to_bgra32(void *dst, const float *src, size_t count);
    }
)

typedef void (* rgba_to_bgra32_t)(void *dst, const float *src, size_t count);

static const float X_1_8 = 0.125f; // 1/8

UTEST_BEGIN("dsp.graphics", rgba_to_bgra32)
    void call(const char *label, size_t align, rgba_to_bgra32_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

//        size_t count = 4;
        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 768, 999, 1024, 0x1fff)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer src(count*4, align, mask & 0x01);
                ByteBuffer dst1(count*4, align, mask & 0x02);
                ByteBuffer dst2(count*4, align, mask & 0x02);

                float *dst = src.data();
                for (size_t i=0; i<count; ++i, dst += 4)
                {
                    dst[0]  = (i & 0x03) * X_1_8;
                    dst[1]  = ((i >> 2) & 0x03) * X_1_8;
                    dst[2]  = ((i >> 4) & 0x03) * X_1_8;
                    dst[3]  = ((i >> 6) & 0x03) * X_1_8;
                }

                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Call functions
                native::rgba_to_bgra32(dst1.data(), src, count);
                func(dst2.data(), src, count);

                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals(dst2))
                {
                    src.dump("src ");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Result of functions differs");
                }
            }
        }
    }

    UTEST_MAIN
    {
        IF_ARCH_X86(call("sse2::rgba_to_bgra32", 16, sse2::rgba_to_bgra32));
        IF_ARCH_ARM(call("neon_d32::rgba_to_bgra32", 16, neon_d32::rgba_to_bgra32));
    }

UTEST_END;







