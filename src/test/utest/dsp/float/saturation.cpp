/*
 * saturation.cpp
 *
 *  Created on: 28 авг. 2018 г.
 *      Author: sadko
 */


#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/FloatBuffer.h>

namespace native
{
    void copy_saturated(float *dst, const float *src, size_t count);
    void saturate(float *dst, size_t count);
}

IF_ARCH_X86(
    namespace x86
    {
        void copy_saturated(float *dst, const float *src, size_t count);
        void saturate(float *dst, size_t count);

        void copy_saturated_cmov(float *dst, const float *src, size_t count);
        void saturate_cmov(float *dst, size_t count);
    }

    namespace sse2
    {
        void copy_saturated(float *dst, const float *src, size_t count);
        void saturate(float *dst, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void copy_saturated(float *dst, const float *src, size_t count);
        void saturate(float *dst, size_t count);
    }
)

typedef void (* copy_saturated_t)(float *dst, const float *src, size_t count);
typedef void (* saturate_t)(float *dst, size_t count);

UTEST_BEGIN("dsp.float", saturation)

    void init_buf(FloatBuffer &buf)
    {
        for (size_t i=0; i<buf.size(); ++i)
        {
            switch (i%6)
            {
                case 0:
                    buf[i]          = +INFINITY;
                    break;
                case 1:
                    buf[i]          = -INFINITY;
                    break;
                case 2:
                    buf[i]          = NAN;
                    break;
                case 3:
                    buf[i]          = -NAN;
                    break;
                case 4:
                    buf[i]          = float(rand()) / RAND_MAX;
                    break;
                default:
                    buf[i]          = - float(rand()) / RAND_MAX;
                    break;
            }
        }
    }

    void call(const char *label, size_t align, copy_saturated_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                        32, 64, 65, 100, 768, 999, 1024, 0x1fff)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer src(count, align, mask & 0x01);
                init_buf(src);
                FloatBuffer dst1(count, align, mask & 0x02);
                FloatBuffer dst2(dst1);

                // Call functions
                native::copy_saturated(dst1, src, count);
                func(dst2, src, count);

                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_relative(dst2, 1e-5))
                {
                    src.dump("src ");
                    src.dump_hex("srch");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    void call(const char *label, size_t align, saturate_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 768, 999, 1024, 0x1fff)
        {
            for (size_t mask=0; mask <= 0x01; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer dst1(count, align, mask & 0x02);
                init_buf(dst1);
                FloatBuffer dst2(dst1);

                // Call functions
                native::saturate(dst1, count);
                func(dst2, count);

                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_relative(dst2, 1e-5))
                {
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    UTEST_MAIN
    {
        IF_ARCH_X86(call("x86::copy_sat", 16, x86::copy_saturated));
        IF_ARCH_X86(call("x86::copy_sat_cmov", 16, x86::copy_saturated_cmov));
        IF_ARCH_X86(call("sse2::copy_sat", 16, sse2::copy_saturated));
        IF_ARCH_ARM(call("neon_d32::copy_sat", 16, neon_d32::copy_saturated));

        IF_ARCH_X86(call("x86::sat", 16, x86::saturate));
        IF_ARCH_X86(call("x86::sat_cmov", 16, x86::saturate_cmov));
        IF_ARCH_X86(call("sse2::sat", 16, sse2::saturate));
        IF_ARCH_ARM(call("neon_d32::sat", 16, neon_d32::saturate));
    }

UTEST_END;






