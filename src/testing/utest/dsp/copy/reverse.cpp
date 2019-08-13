/*
 * reverse.cpp
 *
 *  Created on: 30 авг. 2018 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <test/FloatBuffer.h>
#include <math.h>

namespace native
{
    void reverse1(float *dst, size_t count);
    void reverse2(float *dst, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void reverse1(float *dst, size_t count);
        void reverse2(float *dst, const float *src, size_t count);
    }
)

typedef void (* reverse1_t)(float *dst, size_t count);
typedef void (* reverse2_t)(float *dst, const float *src, size_t count);

UTEST_BEGIN("dsp.copy", reverse)
    void call(const char *label, size_t align, reverse1_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        UTEST_FOREACH(count, 0, 1, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0x80, 0x100, 0x1ff, 999, 0xfff)
        {
            for (size_t mask=0; mask <= 0x01; ++mask)
            {
                FloatBuffer src(count, align, mask & 0x01);
                FloatBuffer dst1(src);
                FloatBuffer dst2(src);

                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));
                native::reverse1(dst1, count);
                func(dst2, count);

                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                if (!dst1.equals_absolute(dst2))
                {
                    src.dump("src");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    void call(const char *label, size_t align, reverse2_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        UTEST_FOREACH(count, /*0, */ 1, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0x80, 0x100, 0x1ff, 999, 0xfff)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                FloatBuffer src(count, align, mask & 0x01);
                FloatBuffer dst1(count, align, mask & 0x02);
                FloatBuffer dst2(dst1);

                native::reverse2(dst1, src, count);
                func(dst2, src, count);

                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                if (!dst1.equals_absolute(dst2))
                {
                    src.dump("src");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    UTEST_MAIN
    {
        IF_ARCH_X86(call("sse_reverse1", 16, sse::reverse1));
        IF_ARCH_X86(call("sse_reverse2", 16, sse::reverse2));
    }

UTEST_END;


