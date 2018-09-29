/*
 * copy.cpp
 *
 *  Created on: 30 авг. 2018 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <test/FloatBuffer.h>
#include <math.h>

namespace native
{
    void move(float *dst, const float *src, size_t count);
    void copy(float *dst, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace x86
    {
        void copy(float *dst, const float *src, size_t count);
    }

    namespace sse
    {
        void move(float *dst, const float *src, size_t count);
        void copy(float *dst, const float *src, size_t count);
        void copy_movntps(float *dst, const float *src, size_t count);
    }

    namespace sse3
    {
        void copy(float *dst, const float *src, size_t count);
    }

    namespace avx
    {
        void copy(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void move(float *dst, const float *src, size_t count);
        void copy(float *dst, const float *src, size_t count);
    }
)

typedef void (* copy_t)(float *dst, const float *src, size_t count);

UTEST_BEGIN("dsp.copy", copy)
    void call(const char *label, size_t align, copy_t func1, copy_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0x80, 0x100, 0x1ff, 999, 0xfff, 0x1fff)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                FloatBuffer src(count, align, mask & 0x01);
                FloatBuffer dst1(count, align, mask & 0x02);
                FloatBuffer dst2(dst1);

                printf("Testing %s, of %d samples, mask=%x\n", label, int(count), int(mask));
                func1(dst1, src, count);
                func2(dst2, src, count);

                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                if (!dst1.equals_absolute(dst2))
                {
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }

                printf("Testing %s, of %d samples from %d offset, mask=%x\n", label, int(count >> 1), int(count >> 2), int(mask));
                float *dptr1 = dst1, *dptr2 = dst2;
                func1(dptr1, &dptr1[count >> 2], count >> 1);
                func2(dptr2, &dptr2[count >> 2], count >> 1);

                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                if (!dst1.equals_absolute(dst2))
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
        IF_ARCH_X86(call("copy_movs", 16, native::copy, x86::copy));
        IF_ARCH_X86(call("copy_sse", 16, native::copy, sse::copy));
        IF_ARCH_X86(call("copy_movntps", 16, native::copy, sse::copy_movntps));
        IF_ARCH_X86(call("move_sse", 16, native::move, sse::move));
        IF_ARCH_X86(call("copy_sse3", 16, native::copy, sse3::copy));
        IF_ARCH_X86(call("copy_avx", 16, native::copy, avx::copy));

        IF_ARCH_ARM(call("copy_neon_d32", 16, native::move, neon_d32::copy));
        IF_ARCH_ARM(call("move_neon_d32", 16, native::move, neon_d32::move));
    }

UTEST_END;





