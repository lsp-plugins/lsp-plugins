/*
 * fmop3.cpp
 *
 *  Created on: 22 нояб. 2019 г.
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
    void    fmadd3(float *dst, const float *a, const float *b, size_t count);
    void    fmsub3(float *dst, const float *a, const float *b, size_t count);
    void    fmrsub3(float *dst, const float *a, const float *b, size_t count);
    void    fmmul3(float *dst, const float *a, const float *b, size_t count);
    void    fmdiv3(float *dst, const float *a, const float *b, size_t count);
    void    fmrdiv3(float *dst, const float *a, const float *b, size_t count);
    void    fmmod3(float *dst, const float *a, const float *b, size_t count);
    void    fmrmod3(float *dst, const float *a, const float *b, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void    fmadd3(float *dst, const float *a, const float *b, size_t count);
        void    fmsub3(float *dst, const float *a, const float *b, size_t count);
        void    fmrsub3(float *dst, const float *a, const float *b, size_t count);
        void    fmmul3(float *dst, const float *a, const float *b, size_t count);
        void    fmdiv3(float *dst, const float *a, const float *b, size_t count);
        void    fmrdiv3(float *dst, const float *a, const float *b, size_t count);
    }

    namespace sse2
    {
        void    fmmod3(float *dst, const float *a, const float *b, size_t count);
        void    fmrmod3(float *dst, const float *a, const float *b, size_t count);
    }

    namespace avx
    {
        void    fmadd3(float *dst, const float *a, const float *b, size_t count);
        void    fmsub3(float *dst, const float *a, const float *b, size_t count);
        void    fmrsub3(float *dst, const float *a, const float *b, size_t count);
        void    fmmul3(float *dst, const float *a, const float *b, size_t count);
        void    fmdiv3(float *dst, const float *a, const float *b, size_t count);
        void    fmrdiv3(float *dst, const float *a, const float *b, size_t count);
        void    fmmod3(float *dst, const float *a, const float *b, size_t count);
        void    fmrmod3(float *dst, const float *a, const float *b, size_t count);

        void    fmadd3_fma3(float *dst, const float *a, const float *b, size_t count);
        void    fmsub3_fma3(float *dst, const float *a, const float *b, size_t count);
        void    fmrsub3_fma3(float *dst, const float *a, const float *b, size_t count);
        void    fmmod3_fma3(float *dst, const float *a, const float *b, size_t count);
        void    fmrmod3_fma3(float *dst, const float *a, const float *b, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void    fmadd3(float *dst, const float *a, const float *b, size_t count);
        void    fmsub3(float *dst, const float *a, const float *b, size_t count);
        void    fmrsub3(float *dst, const float *a, const float *b, size_t count);
        void    fmmul3(float *dst, const float *a, const float *b, size_t count);
        void    fmdiv3(float *dst, const float *a, const float *b, size_t count);
        void    fmrdiv3(float *dst, const float *a, const float *b, size_t count);
        void    fmmod3(float *dst, const float *a, const float *b, size_t count);
        void    fmrmod3(float *dst, const float *a, const float *b, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void    fmadd3(float *dst, const float *a, const float *b, size_t count);
        void    fmsub3(float *dst, const float *a, const float *b, size_t count);
        void    fmrsub3(float *dst, const float *a, const float *b, size_t count);
        void    fmmul3(float *dst, const float *a, const float *b, size_t count);
        void    fmdiv3(float *dst, const float *a, const float *b, size_t count);
        void    fmrdiv3(float *dst, const float *a, const float *b, size_t count);
        void    fmmod3(float *dst, const float *a, const float *b, size_t count);
        void    fmrmod3(float *dst, const float *a, const float *b, size_t count);
    }
)


typedef void (* fmop3_t)(float *dst, const float *a, const float *b, size_t count);

UTEST_BEGIN("dsp.pmath", fmop3)

    void call(const char *label, size_t align, fmop3_t func1, fmop3_t func2)
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
                FloatBuffer dst1(count, align, mask & 0x04);
                dst1.randomize_sign();
                FloatBuffer dst2(dst1);

                // Call functions
                a.randomize_sign();
                b.randomize_sign();
                func1(dst1, a, b, count);
                func2(dst2, a, b, count);

                UTEST_ASSERT_MSG(a.valid(), "Buffer A corrupted");
                UTEST_ASSERT_MSG(b.valid(), "Buffer B corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_adaptive(dst2, 1e-4))
                {
                    a.dump("a   ");
                    b.dump("b   ");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    printf("index=%d, %.6f vs %.6f\n", dst1.last_diff(), dst1.get_diff(), dst2.get_diff());
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(native, func, align) call(#func, align, native, func)

        IF_ARCH_X86(CALL(native::fmadd3, sse::fmadd3, 16));
        IF_ARCH_X86(CALL(native::fmsub3, sse::fmsub3, 16));
        IF_ARCH_X86(CALL(native::fmrsub3, sse::fmrsub3, 16));
        IF_ARCH_X86(CALL(native::fmmul3, sse::fmmul3, 16));
        IF_ARCH_X86(CALL(native::fmdiv3, sse::fmdiv3, 16));
        IF_ARCH_X86(CALL(native::fmrdiv3, sse::fmrdiv3, 16));
        IF_ARCH_X86(CALL(native::fmmod3, sse2::fmmod3, 16));
        IF_ARCH_X86(CALL(native::fmrmod3, sse2::fmrmod3, 16));

        IF_ARCH_X86(CALL(native::fmadd3, avx::fmadd3, 32));
        IF_ARCH_X86(CALL(native::fmsub3, avx::fmsub3, 32));
        IF_ARCH_X86(CALL(native::fmrsub3, avx::fmrsub3, 32));
        IF_ARCH_X86(CALL(native::fmmul3, avx::fmmul3, 32));
        IF_ARCH_X86(CALL(native::fmdiv3, avx::fmdiv3, 32));
        IF_ARCH_X86(CALL(native::fmrdiv3, avx::fmrdiv3, 32));
        IF_ARCH_X86(CALL(native::fmmod3, avx::fmmod3, 32));
        IF_ARCH_X86(CALL(native::fmrmod3, avx::fmrmod3, 32));
        IF_ARCH_X86(CALL(native::fmadd3, avx::fmadd3_fma3, 32));
        IF_ARCH_X86(CALL(native::fmsub3, avx::fmsub3_fma3, 32));
        IF_ARCH_X86(CALL(native::fmrsub3, avx::fmrsub3_fma3, 32));
        IF_ARCH_X86(CALL(native::fmmod3, avx::fmmod3_fma3, 32));
        IF_ARCH_X86(CALL(native::fmrmod3, avx::fmrmod3_fma3, 32));

        IF_ARCH_ARM(CALL(native::fmadd3, neon_d32::fmadd3, 16));
        IF_ARCH_ARM(CALL(native::fmsub3, neon_d32::fmsub3, 16));
        IF_ARCH_ARM(CALL(native::fmrsub3, neon_d32::fmrsub3, 16));
        IF_ARCH_ARM(CALL(native::fmmul3, neon_d32::fmmul3, 16));
        IF_ARCH_ARM(CALL(native::fmdiv3, neon_d32::fmdiv3, 16));
        IF_ARCH_ARM(CALL(native::fmrdiv3, neon_d32::fmrdiv3, 16));
        IF_ARCH_ARM(CALL(native::fmmod3, neon_d32::fmmod3, 16));
        IF_ARCH_ARM(CALL(native::fmrmod3, neon_d32::fmrmod3, 16));

        IF_ARCH_AARCH64(CALL(native::fmadd3, asimd::fmadd3, 16));
        IF_ARCH_AARCH64(CALL(native::fmsub3, asimd::fmsub3, 16));
        IF_ARCH_AARCH64(CALL(native::fmrsub3, asimd::fmrsub3, 16));
        IF_ARCH_AARCH64(CALL(native::fmmul3, asimd::fmmul3, 16));
        IF_ARCH_AARCH64(CALL(native::fmdiv3, asimd::fmdiv3, 16));
        IF_ARCH_AARCH64(CALL(native::fmrdiv3, asimd::fmrdiv3, 16));
        IF_ARCH_AARCH64(CALL(native::fmmod3, asimd::fmmod3, 16));
        IF_ARCH_AARCH64(CALL(native::fmrmod3, asimd::fmrmod3, 16));
    }
UTEST_END



