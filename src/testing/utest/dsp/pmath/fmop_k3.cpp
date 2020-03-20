/*
 * fmop_k3.cpp
 *
 *  Created on: 20 нояб. 2019 г.
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
    void    fmadd_k3(float *dst, const float *src, float k, size_t count);
    void    fmsub_k3(float *dst, const float *src, float k, size_t count);
    void    fmrsub_k3(float *dst, const float *src, float k, size_t count);
    void    fmmul_k3(float *dst, const float *src, float k, size_t count);
    void    fmdiv_k3(float *dst, const float *src, float k, size_t count);
    void    fmrdiv_k3(float *dst, const float *src, float k, size_t count);
    void    fmmod_k3(float *dst, const float *src, float k, size_t count);
    void    fmrmod_k3(float *dst, const float *src, float k, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void    fmadd_k3(float *dst, const float *src, float k, size_t count);
        void    fmsub_k3(float *dst, const float *src, float k, size_t count);
        void    fmrsub_k3(float *dst, const float *src, float k, size_t count);
        void    fmmul_k3(float *dst, const float *src, float k, size_t count);
        void    fmdiv_k3(float *dst, const float *src, float k, size_t count);
        void    fmrdiv_k3(float *dst, const float *src, float k, size_t count);
    }

    namespace sse2
    {
        void    fmmod_k3(float *dst, const float *src, float k, size_t count);
        void    fmrmod_k3(float *dst, const float *src, float k, size_t count);
    }

    namespace avx
    {
        void    fmadd_k3(float *dst, const float *src, float k, size_t count);
        void    fmsub_k3(float *dst, const float *src, float k, size_t count);
        void    fmrsub_k3(float *dst, const float *src, float k, size_t count);
        void    fmmul_k3(float *dst, const float *src, float k, size_t count);
        void    fmdiv_k3(float *dst, const float *src, float k, size_t count);
        void    fmrdiv_k3(float *dst, const float *src, float k, size_t count);
        void    fmmod_k3(float *dst, const float *src, float k, size_t count);
        void    fmrmod_k3(float *dst, const float *src, float k, size_t count);

        void    fmadd_k3_fma3(float *dst, const float *src, float k, size_t count);
        void    fmsub_k3_fma3(float *dst, const float *src, float k, size_t count);
        void    fmrsub_k3_fma3(float *dst, const float *src, float k, size_t count);
        void    fmmod_k3_fma3(float *dst, const float *src, float k, size_t count);
        void    fmrmod_k3_fma3(float *dst, const float *src, float k, size_t count);
    }

    namespace avx2
    {
        void    fmadd_k3(float *dst, const float *src, float k, size_t count);
        void    fmsub_k3(float *dst, const float *src, float k, size_t count);
        void    fmrsub_k3(float *dst, const float *src, float k, size_t count);
        void    fmmul_k3(float *dst, const float *src, float k, size_t count);
        void    fmdiv_k3(float *dst, const float *src, float k, size_t count);
        void    fmrdiv_k3(float *dst, const float *src, float k, size_t count);
        void    fmmod_k3(float *dst, const float *src, float k, size_t count);
        void    fmrmod_k3(float *dst, const float *src, float k, size_t count);

        void    fmadd_k3_fma3(float *dst, const float *src, float k, size_t count);
        void    fmsub_k3_fma3(float *dst, const float *src, float k, size_t count);
        void    fmrsub_k3_fma3(float *dst, const float *src, float k, size_t count);
        void    fmmod_k3_fma3(float *dst, const float *src, float k, size_t count);
        void    fmrmod_k3_fma3(float *dst, const float *src, float k, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void    fmadd_k3(float *dst, const float *src, float k, size_t count);
        void    fmsub_k3(float *dst, const float *src, float k, size_t count);
        void    fmrsub_k3(float *dst, const float *src, float k, size_t count);
        void    fmmul_k3(float *dst, const float *src, float k, size_t count);
        void    fmdiv_k3(float *dst, const float *src, float k, size_t count);
        void    fmrdiv_k3(float *dst, const float *src, float k, size_t count);
        void    fmmod_k3(float *dst, const float *src, float k, size_t count);
        void    fmrmod_k3(float *dst, const float *src, float k, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void    fmadd_k3(float *dst, const float *src, float k, size_t count);
        void    fmsub_k3(float *dst, const float *src, float k, size_t count);
        void    fmrsub_k3(float *dst, const float *src, float k, size_t count);
        void    fmmul_k3(float *dst, const float *src, float k, size_t count);
        void    fmdiv_k3(float *dst, const float *src, float k, size_t count);
        void    fmrdiv_k3(float *dst, const float *src, float k, size_t count);
        void    fmmod_k3(float *dst, const float *src, float k, size_t count);
        void    fmrmod_k3(float *dst, const float *src, float k, size_t count);
    }
)

typedef void (* fmop_k3_t)(float *dst, const float *src, float k, size_t count);

UTEST_BEGIN("dsp.pmath", fmop_k3)

    void call(const char *label, size_t align, fmop_k3_t func1, fmop_k3_t func2)
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
                FloatBuffer dst1(count, align, mask & 0x02);
                dst1.randomize_sign();
                FloatBuffer dst2(dst1);

                // Call functions
                src.randomize_sign();
                func1(dst1, src, 0.4f, count);
                func2(dst2, src, 0.4f, count);

                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_adaptive(dst2, 1e-4))
                {
                    src.dump("src ");
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
        #define CALL(native, func, align) \
            call(#func, align, native, func)

        IF_ARCH_X86(CALL(native::fmadd_k3, sse::fmadd_k3, 16));
        IF_ARCH_X86(CALL(native::fmsub_k3, sse::fmsub_k3, 16));
        IF_ARCH_X86(CALL(native::fmrsub_k3, sse::fmrsub_k3, 16));
        IF_ARCH_X86(CALL(native::fmmul_k3, sse::fmmul_k3, 16));
        IF_ARCH_X86(CALL(native::fmdiv_k3, sse::fmdiv_k3, 16));
        IF_ARCH_X86(CALL(native::fmrdiv_k3, sse::fmrdiv_k3, 16));
        IF_ARCH_X86(CALL(native::fmmod_k3, sse2::fmmod_k3, 16));
        IF_ARCH_X86(CALL(native::fmrmod_k3, sse2::fmrmod_k3, 16));

        IF_ARCH_X86(CALL(native::fmadd_k3, avx::fmadd_k3, 32));
        IF_ARCH_X86(CALL(native::fmsub_k3, avx::fmsub_k3, 32));
        IF_ARCH_X86(CALL(native::fmrsub_k3, avx::fmrsub_k3, 32));
        IF_ARCH_X86(CALL(native::fmmul_k3, avx::fmmul_k3, 32));
        IF_ARCH_X86(CALL(native::fmdiv_k3, avx::fmdiv_k3, 32));
        IF_ARCH_X86(CALL(native::fmrdiv_k3, avx::fmrdiv_k3, 32));
        IF_ARCH_X86(CALL(native::fmmod_k3, avx::fmmod_k3, 32));
        IF_ARCH_X86(CALL(native::fmrmod_k3, avx::fmrmod_k3, 32));
        IF_ARCH_X86(CALL(native::fmadd_k3, avx::fmadd_k3_fma3, 32));
        IF_ARCH_X86(CALL(native::fmsub_k3, avx::fmsub_k3_fma3, 32));
        IF_ARCH_X86(CALL(native::fmrsub_k3, avx::fmrsub_k3_fma3, 32));
        IF_ARCH_X86(CALL(native::fmmod_k3, avx::fmmod_k3_fma3, 32));
        IF_ARCH_X86(CALL(native::fmrmod_k3, avx::fmrmod_k3_fma3, 32));

        IF_ARCH_X86(CALL(native::fmadd_k3, avx2::fmadd_k3, 32));
        IF_ARCH_X86(CALL(native::fmsub_k3, avx2::fmsub_k3, 32));
        IF_ARCH_X86(CALL(native::fmrsub_k3, avx2::fmrsub_k3, 32));
        IF_ARCH_X86(CALL(native::fmmul_k3, avx2::fmmul_k3, 32));
        IF_ARCH_X86(CALL(native::fmdiv_k3, avx2::fmdiv_k3, 32));
        IF_ARCH_X86(CALL(native::fmrdiv_k3, avx2::fmrdiv_k3, 32));
        IF_ARCH_X86(CALL(native::fmmod_k3, avx2::fmmod_k3, 32));
        IF_ARCH_X86(CALL(native::fmrmod_k3, avx2::fmrmod_k3, 32));
        IF_ARCH_X86(CALL(native::fmadd_k3, avx2::fmadd_k3_fma3, 32));
        IF_ARCH_X86(CALL(native::fmsub_k3, avx2::fmsub_k3_fma3, 32));
        IF_ARCH_X86(CALL(native::fmrsub_k3, avx2::fmrsub_k3_fma3, 32));
        IF_ARCH_X86(CALL(native::fmmod_k3, avx2::fmmod_k3_fma3, 32));
        IF_ARCH_X86(CALL(native::fmrmod_k3, avx2::fmrmod_k3_fma3, 32));

        IF_ARCH_ARM(CALL(native::fmadd_k3, neon_d32::fmadd_k3, 16));
        IF_ARCH_ARM(CALL(native::fmsub_k3, neon_d32::fmsub_k3, 16));
        IF_ARCH_ARM(CALL(native::fmrsub_k3, neon_d32::fmrsub_k3, 16));
        IF_ARCH_ARM(CALL(native::fmmul_k3, neon_d32::fmmul_k3, 16));
        IF_ARCH_ARM(CALL(native::fmdiv_k3, neon_d32::fmdiv_k3, 16));
        IF_ARCH_ARM(CALL(native::fmrdiv_k3, neon_d32::fmrdiv_k3, 16));
        IF_ARCH_ARM(CALL(native::fmmod_k3, neon_d32::fmmod_k3, 16));
        IF_ARCH_ARM(CALL(native::fmrmod_k3, neon_d32::fmrmod_k3, 16));

        IF_ARCH_AARCH64(CALL(native::fmadd_k3, asimd::fmadd_k3, 16));
        IF_ARCH_AARCH64(CALL(native::fmsub_k3, asimd::fmsub_k3, 16));
        IF_ARCH_AARCH64(CALL(native::fmrsub_k3, asimd::fmrsub_k3, 16));
        IF_ARCH_AARCH64(CALL(native::fmmul_k3, asimd::fmmul_k3, 16));
        IF_ARCH_AARCH64(CALL(native::fmdiv_k3, asimd::fmdiv_k3, 16));
        IF_ARCH_AARCH64(CALL(native::fmrdiv_k3, asimd::fmrdiv_k3, 16));
        IF_ARCH_AARCH64(CALL(native::fmmod_k3, asimd::fmmod_k3, 16));
        IF_ARCH_AARCH64(CALL(native::fmrmod_k3, asimd::fmrmod_k3, 16));
    }
UTEST_END


