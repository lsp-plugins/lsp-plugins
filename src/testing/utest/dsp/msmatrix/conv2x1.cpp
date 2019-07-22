/*
 * conv2x1.cpp
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
    void    lr_to_mid(float *m, const float *l, const float *r, size_t count);
    void    lr_to_side(float *s, const float *l, const float *r, size_t count);
    void    ms_to_left(float *l, const float *m, const float *s, size_t count);
    void    ms_to_right(float *r, const float *m, const float *s, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void    lr_to_mid(float *m, const float *l, const float *r, size_t count);
        void    lr_to_side(float *s, const float *l, const float *r, size_t count);
        void    ms_to_left(float *l, const float *m, const float *s, size_t count);
        void    ms_to_right(float *r, const float *m, const float *s, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void    lr_to_mid(float *m, const float *l, const float *r, size_t count);
        void    lr_to_side(float *s, const float *l, const float *r, size_t count);
        void    ms_to_left(float *l, const float *m, const float *s, size_t count);
        void    ms_to_right(float *r, const float *m, const float *s, size_t count);
    }
)

typedef void (* conv2x1_t)(float *d, const float *s1, const float *s2, size_t count);

//-----------------------------------------------------------------------------
// Unit test for complex multiplication
UTEST_BEGIN("dsp.msmatrix", conv2x1)

    void call(const char *label, size_t align, conv2x1_t func1, conv2x1_t func2)
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

                FloatBuffer srcA(count, align, mask & 0x01);
                FloatBuffer srcB(count, align, mask & 0x02);
                FloatBuffer dst1(count, align, mask & 0x04);
                FloatBuffer dst2(dst1);

                // Call functions
                func1(dst1, srcA, srcB, count);
                func1(dst2, srcA, srcB, count);

                UTEST_ASSERT_MSG(srcA.valid(), "Source buffer A corrupted");
                UTEST_ASSERT_MSG(srcB.valid(), "Source buffer A corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_relative(dst2, 1e-4))
                {
                    srcA.dump("srcA");
                    srcB.dump("srcB");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    if (dst1.last_diff() >= 0)
                        printf("dst1 vs dst2 index=%d, %.6f vs %.6f\n", dst1.last_diff(), dst1.get_diff(), dst2.get_diff());
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    UTEST_MAIN
    {
        IF_ARCH_X86(call("sse:lr_to_mid", 16, native::lr_to_mid, sse::lr_to_mid));
        IF_ARCH_X86(call("sse:lr_to_side", 16, native::lr_to_side, sse::lr_to_side));
        IF_ARCH_X86(call("sse:ms_to_left", 16, native::ms_to_left, sse::ms_to_left));
        IF_ARCH_X86(call("sse:ms_to_right", 16, native::ms_to_right, sse::ms_to_right));

        IF_ARCH_ARM(call("neon_d32:lr_to_mid", 16, native::lr_to_mid, neon_d32::lr_to_mid));
        IF_ARCH_ARM(call("neon_d32:lr_to_side", 16, native::lr_to_side, neon_d32::lr_to_side));
        IF_ARCH_ARM(call("neon_d32:ms_to_left", 16, native::ms_to_left, neon_d32::ms_to_left));
        IF_ARCH_ARM(call("neon_d32:ms_to_right", 16, native::ms_to_right, neon_d32::ms_to_right));
    }
UTEST_END


