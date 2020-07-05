/*
 * minmax2.cpp
 *
 *  Created on: 5 июл. 2020 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/FloatBuffer.h>

namespace native
{
    void pmin2(float *dst, const float *src, size_t count);
    void pmax2(float *dst, const float *src, size_t count);
    void psmin2(float *dst, const float *src, size_t count);
    void psmax2(float *dst, const float *src, size_t count);
    void pamin2(float *dst, const float *src, size_t count);
    void pamax2(float *dst, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void pmin2(float *dst, const float *src, size_t count);
        void pmax2(float *dst, const float *src, size_t count);
        void psmin2(float *dst, const float *src, size_t count);
        void psmax2(float *dst, const float *src, size_t count);
        void pamin2(float *dst, const float *src, size_t count);
        void pamax2(float *dst, const float *src, size_t count);
    }
)

typedef void (* min2_t)(float *dst, const float *src, size_t count);

//-----------------------------------------------------------------------------
// Unit test
UTEST_BEGIN("dsp.pmath", minmax2)

    void call(const char *label, size_t align, min2_t func1, min2_t func2)
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
                src.randomize_sign();

                FloatBuffer dst1(count, align, mask & 0x02);
                dst1.randomize_sign();
                FloatBuffer dst2(dst1);

                // Call functions
                func1(dst1, src, count);
                func2(dst2, src, count);

                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_adaptive(dst2, 1e-4))
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

        IF_ARCH_X86(CALL(native::pmin2, sse::pmin2, 16));
        IF_ARCH_X86(CALL(native::pmax2, sse::pmax2, 16));
        IF_ARCH_X86(CALL(native::psmin2, sse::psmin2, 16));
        IF_ARCH_X86(CALL(native::psmax2, sse::psmax2, 16));
        IF_ARCH_X86(CALL(native::pamin2, sse::pamin2, 16));
        IF_ARCH_X86(CALL(native::pamax2, sse::pamax2, 16));

    }
UTEST_END



