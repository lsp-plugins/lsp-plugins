/*
 * r2c.cpp
 *
 *  Created on: 29 авг. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/FloatBuffer.h>

namespace native
{
    void packed_real_to_complex(float *dst, const float *src, size_t count);
    void packed_complex_to_real(float *dst, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void packed_real_to_complex(float *dst, const float *src, size_t count);
        void packed_complex_to_real(float *dst, const float *src, size_t count);
    }
)

typedef void (* complex_cvt_t) (float *dst, const float *src, size_t count);

UTEST_BEGIN("dsp.complex", r2c)

    void call(const char *label, size_t align, size_t sk, size_t dk, complex_cvt_t func1, complex_cvt_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 768, 999, 1024)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer src(count*sk, align, mask & 0x01);
                FloatBuffer dst1(count*dk, align, mask & 0x02);
                FloatBuffer dst2(dst1);

                // Call functions
                func1(dst1, src, count);
                func2(dst2, src, count);

                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_absolute(dst2, 1e-5))
                {
                    src.dump("dst");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    UTEST_MAIN
    {
        IF_ARCH_X86(call("packed_r2c_sse", 16, 1, 2, native::packed_real_to_complex, sse::packed_real_to_complex));
        IF_ARCH_X86(call("packed_c2r_sse", 16, 2, 1, native::packed_complex_to_real, sse::packed_complex_to_real));
    }

UTEST_END


