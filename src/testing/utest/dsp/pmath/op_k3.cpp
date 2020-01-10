/*
 * op_k2.cpp
 *
 *  Created on: 18 нояб. 2019 г.
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
    void    add_k3(float *dst, const float *src, float k, size_t count);
    void    sub_k3(float *dst, const float *src, float k, size_t count);
    void    rsub_k3(float *dst, const float *src, float k, size_t count);
    void    mul_k3(float *dst, const float *src, float k, size_t count);
    void    div_k3(float *dst, const float *src, float k, size_t count);
    void    rdiv_k3(float *dst, const float *src, float k, size_t count);
    void    mod_k3(float *dst, const float *src, float k, size_t count);
    void    rmod_k3(float *dst, const float *src, float k, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void    add_k3(float *dst, const float *src, float k, size_t count);
        void    sub_k3(float *dst, const float *src, float k, size_t count);
        void    rsub_k3(float *dst, const float *src, float k, size_t count);
        void    mul_k3(float *dst, const float *src, float k, size_t count);
        void    div_k3(float *dst, const float *src, float k, size_t count);
        void    rdiv_k3(float *dst, const float *src, float k, size_t count);
    }

    namespace sse2
    {
        void    mod_k3(float *dst, const float *src, float k, size_t count);
        void    rmod_k3(float *dst, const float *src, float k, size_t count);
    }

    namespace avx
    {
        void    add_k3(float *dst, const float *src, float k, size_t count);
        void    sub_k3(float *dst, const float *src, float k, size_t count);
        void    rsub_k3(float *dst, const float *src, float k, size_t count);
        void    mul_k3(float *dst, const float *src, float k, size_t count);
        void    div_k3(float *dst, const float *src, float k, size_t count);
        void    rdiv_k3(float *dst, const float *src, float k, size_t count);
        void    mod_k3(float *dst, const float *src, float k, size_t count);
        void    rmod_k3(float *dst, const float *src, float k, size_t count);
        void    mod_k3_fma3(float *dst, const float *src, float k, size_t count);
        void    rmod_k3_fma3(float *dst, const float *src, float k, size_t count);
    }

    namespace avx2
    {
        void    add_k3(float *dst, const float *src, float k, size_t count);
        void    sub_k3(float *dst, const float *src, float k, size_t count);
        void    mul_k3(float *dst, const float *src, float k, size_t count);
        void    div_k3(float *dst, const float *src, float k, size_t count);
        void    rsub_k3(float *dst, const float *src, float k, size_t count);
        void    rdiv_k3(float *dst, const float *src, float k, size_t count);
        void    mod_k3(float *dst, const float *src, float k, size_t count);
        void    rmod_k3(float *dst, const float *src, float k, size_t count);
        void    mod_k3_fma3(float *dst, const float *src, float k, size_t count);
        void    rmod_k3_fma3(float *dst, const float *src, float k, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void    add_k3(float *dst, const float *src, float k, size_t count);
        void    sub_k3(float *dst, const float *src, float k, size_t count);
        void    rsub_k3(float *dst, const float *src, float k, size_t count);
        void    mul_k3(float *dst, const float *src, float k, size_t count);
        void    div_k3(float *dst, const float *src, float k, size_t count);
        void    rdiv_k3(float *dst, const float *src, float k, size_t count);
        void    mod_k3(float *dst, const float *src, float k, size_t count);
        void    rmod_k3(float *dst, const float *src, float k, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void    add_k3(float *dst, const float *src, float k, size_t count);
        void    sub_k3(float *dst, const float *src, float k, size_t count);
        void    rsub_k3(float *dst, const float *src, float k, size_t count);
        void    mul_k3(float *dst, const float *src, float k, size_t count);
        void    div_k3(float *dst, const float *src, float k, size_t count);
        void    rdiv_k3(float *dst, const float *src, float k, size_t count);
        void    mod_k3(float *dst, const float *src, float k, size_t count);
        void    rmod_k3(float *dst, const float *src, float k, size_t count);
    }
)

typedef void (* op_k3_t)(float *dst, const float *src, float k, size_t count);

UTEST_BEGIN("dsp.pmath", op_k3)

    void call(const char *label, size_t align, op_k3_t func1, op_k3_t func2)
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
                FloatBuffer dst2(count, align, mask & 0x02);

                // Call functions
                func1(dst1, src, 0.5f, count);
                func2(dst2, src, 0.5f, count);

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

        IF_ARCH_X86(CALL(native::add_k3, sse::add_k3, 16));
        IF_ARCH_X86(CALL(native::sub_k3, sse::sub_k3, 16));
        IF_ARCH_X86(CALL(native::rsub_k3, sse::rsub_k3, 16));
        IF_ARCH_X86(CALL(native::mul_k3, sse::mul_k3, 16));
        IF_ARCH_X86(CALL(native::div_k3, sse::div_k3, 16));
        IF_ARCH_X86(CALL(native::rdiv_k3, sse::rdiv_k3, 16));
        IF_ARCH_X86(CALL(native::mod_k3, sse2::mod_k3, 16));
        IF_ARCH_X86(CALL(native::rmod_k3, sse2::rmod_k3, 16));

        IF_ARCH_X86(CALL(native::add_k3, avx::add_k3, 32));
        IF_ARCH_X86(CALL(native::sub_k3, avx::sub_k3, 32));
        IF_ARCH_X86(CALL(native::rsub_k3, avx::rsub_k3, 32));
        IF_ARCH_X86(CALL(native::mul_k3, avx::mul_k3, 32));
        IF_ARCH_X86(CALL(native::div_k3, avx::div_k3, 32));
        IF_ARCH_X86(CALL(native::rdiv_k3, avx::rdiv_k3, 32));
        IF_ARCH_X86(CALL(native::mod_k3, avx::mod_k3, 32));
        IF_ARCH_X86(CALL(native::rmod_k3, avx::rmod_k3, 32));
        IF_ARCH_X86(CALL(native::mod_k3, avx::mod_k3_fma3, 32));
        IF_ARCH_X86(CALL(native::rmod_k3, avx::rmod_k3_fma3, 32));

        IF_ARCH_X86(CALL(native::add_k3, avx2::add_k3, 32));
        IF_ARCH_X86(CALL(native::sub_k3, avx2::sub_k3, 32));
        IF_ARCH_X86(CALL(native::rsub_k3, avx2::rsub_k3, 32));
        IF_ARCH_X86(CALL(native::mul_k3, avx2::mul_k3, 32));
        IF_ARCH_X86(CALL(native::div_k3, avx2::div_k3, 32));
        IF_ARCH_X86(CALL(native::rdiv_k3, avx2::rdiv_k3, 32));
        IF_ARCH_X86(CALL(native::mod_k3, avx2::mod_k3, 32));
        IF_ARCH_X86(CALL(native::rmod_k3, avx2::rmod_k3, 32));
        IF_ARCH_X86(CALL(native::mod_k3, avx2::mod_k3_fma3, 32));
        IF_ARCH_X86(CALL(native::rmod_k3, avx2::rmod_k3_fma3, 32));

        IF_ARCH_ARM(CALL(native::add_k3, neon_d32::add_k3, 16));
        IF_ARCH_ARM(CALL(native::sub_k3, neon_d32::sub_k3, 16));
        IF_ARCH_ARM(CALL(native::rsub_k3, neon_d32::rsub_k3, 16));
        IF_ARCH_ARM(CALL(native::mul_k3, neon_d32::mul_k3, 16));
        IF_ARCH_ARM(CALL(native::div_k3, neon_d32::div_k3, 16));
        IF_ARCH_ARM(CALL(native::rdiv_k3, neon_d32::rdiv_k3, 16));
        IF_ARCH_ARM(CALL(native::mod_k3, neon_d32::mod_k3, 16));
        IF_ARCH_ARM(CALL(native::rmod_k3, neon_d32::rmod_k3, 16));

        IF_ARCH_AARCH64(CALL(native::add_k3, asimd::add_k3, 16));
        IF_ARCH_AARCH64(CALL(native::sub_k3, asimd::sub_k3, 16));
        IF_ARCH_AARCH64(CALL(native::rsub_k3, asimd::rsub_k3, 16));
        IF_ARCH_AARCH64(CALL(native::mul_k3, asimd::mul_k3, 16));
        IF_ARCH_AARCH64(CALL(native::div_k3, asimd::div_k3, 16));
        IF_ARCH_AARCH64(CALL(native::rdiv_k3, asimd::rdiv_k3, 16));
        IF_ARCH_AARCH64(CALL(native::mod_k3, asimd::mod_k3, 16));
        IF_ARCH_AARCH64(CALL(native::rmod_k3, asimd::rmod_k3, 16));
    }
UTEST_END





