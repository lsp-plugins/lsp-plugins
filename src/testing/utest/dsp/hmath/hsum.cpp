/*
 * hsum.cpp
 *
 *  Created on: 29 авг. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/FloatBuffer.h>
#include <test/helpers.h>

#ifdef ARCH_ARM
    #define TOLERANCE 1e-3
#endif

#ifndef TOLERANCE
    #define TOLERANCE 1e-4
#endif

namespace native
{
    float h_sum(const float *src, size_t count);
    float h_sqr_sum(const float *src, size_t count);
    float h_abs_sum(const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        float h_sum(const float *src, size_t count);
        float h_sqr_sum(const float *src, size_t count);
        float h_abs_sum(const float *src, size_t count);
    }

    namespace avx
    {
        float h_sum(const float *src, size_t count);
        float h_sqr_sum(const float *src, size_t count);
        float h_sqr_sum_fma3(const float *src, size_t count);
        float h_abs_sum(const float *src, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        float h_sum(const float *src, size_t count);
        float h_sqr_sum(const float *src, size_t count);
        float h_abs_sum(const float *src, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        float h_sum(const float *src, size_t count);
        float h_sqr_sum(const float *src, size_t count);
        float h_abs_sum(const float *src, size_t count);
    }
)

typedef float (* h_sum_t)(const float *src, size_t count);

UTEST_BEGIN("dsp.hmath", hsum)

    void call(const char *label, size_t align, h_sum_t func1, h_sum_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 768, 999, 0x1fff)
        {
            for (size_t mask=0; mask <= 0x01; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer src(count, align, mask & 0x01);
                src.randomize_sign();

                // Call functions
                float a = func1(src, count);
                float b = func2(src, count);

                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");

                // Compare buffers
                if (!float_equals_adaptive(a, b, TOLERANCE))
                {
                    src.dump("src1");
                    UTEST_FAIL_MSG("Result of function 1 (%f) differs result of function 2 (%f)", a, b)
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(native, func, align) \
            call(#func, align, native, func);

        IF_ARCH_X86(CALL(native::h_sum, sse::h_sum, 16));
        IF_ARCH_X86(CALL(native::h_sqr_sum, sse::h_sqr_sum, 16));
        IF_ARCH_X86(CALL(native::h_abs_sum, sse::h_abs_sum, 16));

        IF_ARCH_X86(CALL(native::h_sum, avx::h_sum, 32));
        IF_ARCH_X86(CALL(native::h_sqr_sum, avx::h_sqr_sum, 32));
        IF_ARCH_X86(CALL(native::h_sqr_sum, avx::h_sqr_sum_fma3, 32));
        IF_ARCH_X86(CALL(native::h_abs_sum, avx::h_abs_sum, 32));

        IF_ARCH_ARM(CALL(native::h_sum, neon_d32::h_sum, 16));
        IF_ARCH_ARM(CALL(native::h_sqr_sum, neon_d32::h_sqr_sum, 16));
        IF_ARCH_ARM(CALL(native::h_abs_sum, neon_d32::h_abs_sum, 16));

        IF_ARCH_AARCH64(CALL(native::h_sum, asimd::h_sum, 16));
        IF_ARCH_AARCH64(CALL(native::h_sqr_sum, asimd::h_sqr_sum, 16));
        IF_ARCH_AARCH64(CALL(native::h_abs_sum, asimd::h_abs_sum, 16));
    }
UTEST_END
