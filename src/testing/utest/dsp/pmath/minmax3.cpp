/*
 * minmax3.cpp
 *
 *  Created on: 5 июл. 2020 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/FloatBuffer.h>

namespace native
{
    void pmin3(float *dst, const float *a, const float *b, size_t count);
    void pmax3(float *dst, const float *a, const float *b, size_t count);
    void psmin3(float *dst, const float *a, const float *b, size_t count);
    void psmax3(float *dst, const float *a, const float *b, size_t count);
    void pamin3(float *dst, const float *a, const float *b, size_t count);
    void pamax3(float *dst, const float *a, const float *b, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void pmin3(float *dst, const float *a, const float *b, size_t count);
        void pmax3(float *dst, const float *a, const float *b, size_t count);
        void psmin3(float *dst, const float *a, const float *b, size_t count);
        void psmax3(float *dst, const float *a, const float *b, size_t count);
        void pamin3(float *dst, const float *a, const float *b, size_t count);
        void pamax3(float *dst, const float *a, const float *b, size_t count);
    }
)

typedef void (* min3_t)(float *dst, const float *a, const float *b, size_t count);

//-----------------------------------------------------------------------------
// Unit test
UTEST_BEGIN("dsp.pmath", minmax3)

    void call(const char *label, size_t align, min3_t func1, min3_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 999, 0xfff)
        {
            for (size_t mask=0; mask <= 0x07; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer a(count, align, mask & 0x01);
                FloatBuffer b(count, align, mask & 0x02);
                a.randomize_sign();
                b.randomize_sign();

                FloatBuffer dst1(count, align, mask & 0x04);
                FloatBuffer dst2(dst1);

                // Call functions
                func1(dst1, a, b, count);
                func2(dst2, a, b, count);

                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_adaptive(dst2, 1e-4))
                {
                    a.dump("srca");
                    b.dump("srcb");
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

        IF_ARCH_X86(CALL(native::pmin3, sse::pmin3, 16));
        IF_ARCH_X86(CALL(native::pmax3, sse::pmax3, 16));
        IF_ARCH_X86(CALL(native::psmin3, sse::psmin3, 16));
        IF_ARCH_X86(CALL(native::psmax3, sse::psmax3, 16));
        IF_ARCH_X86(CALL(native::pamin3, sse::pamin3, 16));
        IF_ARCH_X86(CALL(native::pamax3, sse::pamax3, 16));

    }
UTEST_END
