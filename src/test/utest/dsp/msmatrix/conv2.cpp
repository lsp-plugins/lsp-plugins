/*
 * conv2.cpp
 *
 *  Created on: 03 окт. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/FloatBuffer.h>
#include <core/sugar.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace native
{
    void    lr_to_ms(float *m, float *s, const float *l, const float *r, size_t count);
    void    ms_to_lr(float *l, float *r, const float *m, const float *s, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void    lr_to_ms(float *m, float *s, const float *l, const float *r, size_t count);
        void    ms_to_lr(float *l, float *r, const float *m, const float *s, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void    lr_to_ms(float *m, float *s, const float *l, const float *r, size_t count);
        void    ms_to_lr(float *l, float *r, const float *m, const float *s, size_t count);
    }
)

typedef void (* conv2_t)(float *d1, float *d2, const float *s1, const float *s2, size_t count);

//-----------------------------------------------------------------------------
// Unit test for complex multiplication
UTEST_BEGIN("dsp.msmatrix", conv2)

    void call(const char *label, size_t align, conv2_t func1, conv2_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 999, 0xfff)
        {
            for (size_t mask=0; mask <= 0x0f; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer srcA(count, align, mask & 0x01);
                FloatBuffer srcB(count, align, mask & 0x02);
                FloatBuffer dst1A(count, align, mask & 0x04);
                FloatBuffer dst1B(count, align, mask & 0x08);
                FloatBuffer dst2A(dst1A);
                FloatBuffer dst2B(dst1B);

                // Call functions
                func1(dst1A, dst1B, srcA, srcB, count);
                func1(dst2A, dst2B, srcA, srcB, count);

                UTEST_ASSERT_MSG(srcA.valid(), "Source buffer A corrupted");
                UTEST_ASSERT_MSG(srcB.valid(), "Source buffer A corrupted");
                UTEST_ASSERT_MSG(dst1A.valid(), "Destination buffer 1A corrupted");
                UTEST_ASSERT_MSG(dst1B.valid(), "Destination buffer 1B corrupted");
                UTEST_ASSERT_MSG(dst2A.valid(), "Destination buffer 2A corrupted");
                UTEST_ASSERT_MSG(dst2B.valid(), "Destination buffer 2B corrupted");

                // Compare buffers
                if ((!dst1A.equals_relative(dst2A, 1e-4)) || (!dst1B.equals_relative(dst2B, 1e-4)))
                {
                    srcA.dump("srcA ");
                    srcB.dump("srcA ");
                    dst1A.dump("dst1A");
                    dst1B.dump("dst1B");
                    dst2A.dump("dst2A");
                    dst2B.dump("dst2B");
                    if (dst1A.last_diff() >= 0)
                        printf("dst1A vs dst2A index=%d, %.6f vs %.6f\n", dst1A.last_diff(), dst1A.get_diff(), dst2A.get_diff());
                    if (dst1B.last_diff() >= 0)
                        printf("dst1B vs dst2B index=%d, %.6f vs %.6f\n", dst1B.last_diff(), dst1B.get_diff(), dst2B.get_diff());
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    UTEST_MAIN
    {
        IF_ARCH_X86(call("sse:lr_to_ms", 16, native::lr_to_ms, sse::lr_to_ms));
        IF_ARCH_X86(call("sse:ms_to_lr", 16, native::ms_to_lr, sse::ms_to_lr));

        IF_ARCH_ARM(call("neon_d32:lr_to_ms", 16, native::lr_to_ms, neon_d32::lr_to_ms));
        IF_ARCH_ARM(call("neon_d32:ms_to_lr", 16, native::ms_to_lr, neon_d32::ms_to_lr));
    }
UTEST_END


