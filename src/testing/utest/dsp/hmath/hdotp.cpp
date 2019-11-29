/*
 * hdotp.cpp
 *
 *  Created on: 29 нояб. 2019 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/FloatBuffer.h>
#include <test/helpers.h>

#ifndef TOLERANCE
    #define TOLERANCE 1e-4
#endif

namespace native
{
    float h_dotp(const float *a, const float *b, size_t count);
    float h_sqr_dotp(const float *a, const float *b, size_t count);
    float h_abs_dotp(const float *a, const float *b, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        float h_dotp(const float *a, const float *b, size_t count);
        float h_sqr_dotp(const float *a, const float *b, size_t count);
        float h_abs_dotp(const float *a, const float *b, size_t count);
    }

    namespace avx
    {
        float h_dotp(const float *a, const float *b, size_t count);
        float h_sqr_dotp(const float *a, const float *b, size_t count);
        float h_abs_dotp(const float *a, const float *b, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        float h_dotp(const float *a, const float *b, size_t count);
        float h_sqr_dotp(const float *a, const float *b, size_t count);
        float h_abs_dotp(const float *a, const float *b, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        float h_dotp(const float *a, const float *b, size_t count);
        float h_sqr_dotp(const float *a, const float *b, size_t count);
        float h_abs_dotp(const float *a, const float *b, size_t count);
    }
)

typedef float (* h_dotp_t)(const float *a, const float *b, size_t count);

UTEST_BEGIN("dsp.hmath", hdotp)

    void call(const char *label, size_t align, h_dotp_t func1, h_dotp_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 768, 999, 0x1fff)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer a(count, align, mask & 0x01);
                FloatBuffer b(count, align, mask & 0x02);

                a.randomize_sign();
                b.randomize_sign();

                // Call functions
                float xa = func1(a, b, count);
                float xb = func2(a, b, count);

                UTEST_ASSERT_MSG(a.valid(), "Source buffer A corrupted");
                UTEST_ASSERT_MSG(b.valid(), "Source buffer B corrupted");

                // Compare buffers
                if (!float_equals_adaptive(xa, xb, TOLERANCE))
                {
                    a.dump("A");
                    b.dump("B");
                    UTEST_FAIL_MSG("Result of function 1 (%f) differs result of function 2 (%f)", xa, xb)
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(native, func, align) \
            call(#func, align, native, func);

        IF_ARCH_X86(CALL(native::h_dotp, sse::h_dotp, 16));
        IF_ARCH_X86(CALL(native::h_sqr_dotp, sse::h_sqr_dotp, 16));
        IF_ARCH_X86(CALL(native::h_abs_dotp, sse::h_abs_dotp, 16));

        IF_ARCH_X86(CALL(native::h_dotp, avx::h_dotp, 32));
        IF_ARCH_X86(CALL(native::h_sqr_dotp, avx::h_sqr_dotp, 32));
        IF_ARCH_X86(CALL(native::h_abs_dotp, avx::h_abs_dotp, 32));

        IF_ARCH_ARM(CALL(native::h_dotp, neon_d32::h_dotp, 16));
        IF_ARCH_ARM(CALL(native::h_sqr_dotp, neon_d32::h_sqr_dotp, 16));
        IF_ARCH_ARM(CALL(native::h_abs_dotp, neon_d32::h_abs_dotp, 16));

        IF_ARCH_AARCH64(CALL(native::h_dotp, asimd::h_dotp, 16));
        IF_ARCH_AARCH64(CALL(native::h_sqr_dotp, asimd::h_sqr_dotp, 16));
        IF_ARCH_AARCH64(CALL(native::h_abs_dotp, asimd::h_abs_dotp, 16));
    }
UTEST_END
