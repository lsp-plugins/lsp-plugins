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
    void pcomplex_rcp1(float *dst, size_t count);
    void pcomplex_rcp2(float *dst, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void pcomplex_rcp1(float *dst, size_t count);
        void pcomplex_rcp2(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void pcomplex_rcp1(float *dst, size_t count);
        void pcomplex_rcp2(float *dst, const float *src, size_t count);
    }
)

typedef void (* pcomplex_rcp1_t) (float *dst, size_t count);
typedef void (* pcomplex_rcp2_t) (float *dst, const float *src, size_t count);


UTEST_BEGIN("dsp.pcomplex", rcp)
    void call(const char *text, size_t align, pcomplex_rcp1_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
                32, 33, 37, 48, 49, 64, 65, 0x3f, 100, 999, 0x1fff)
        {
            for (size_t mask=0; mask <= 0x01; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", text, int(count), int(mask));

                FloatBuffer dst1(count*2, align, mask & 0x01);
                FloatBuffer dst2(dst1);

                // Call functions
                native::pcomplex_rcp1(dst1, count);
                func(dst2, count);

                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_relative(dst2, 1e-4))
                {
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", text);
                }
            }
        }
    }

    void call(const char *text, size_t align, pcomplex_rcp2_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
                32, 33, 37, 48, 49, 64, 65, 0x3f, 100, 999, 0x1fff)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", text, int(count), int(mask));

                FloatBuffer src(count*2, align, mask & 0x01);
                FloatBuffer dst1(count*2, align, mask & 0x02);
                FloatBuffer dst2(dst1);

                // Call functions
                native::pcomplex_rcp2(dst1, src, count);
                func(dst2, src, count);

                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_relative(dst2, 1e-4))
                {
                    src.dump("dst");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", text);
                }
            }
        }
    }

    UTEST_MAIN
    {
        IF_ARCH_X86(call("sse:pcomplex_rcp1", 16, sse::pcomplex_rcp1));
        IF_ARCH_X86(call("sse:pcomplex_rcp2", 16, sse::pcomplex_rcp2));

        IF_ARCH_ARM(call("neon_d32:pcomplex_rcp1", 16, neon_d32::pcomplex_rcp1));
        IF_ARCH_ARM(call("neon_d32:pcomplex_rcp2", 16, neon_d32::pcomplex_rcp2));
    }

UTEST_END;
