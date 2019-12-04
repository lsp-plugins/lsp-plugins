/*
 * minmax.cpp
 *
 *  Created on: 29 авг. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/FloatBuffer.h>
#include <test/helpers.h>

namespace native
{
    float   min(const float *src, size_t count);
    float   max(const float *src, size_t count);
    void    minmax(const float *src, size_t count, float *min, float *max);

    float   abs_min(const float *src, size_t count);
    float   abs_max(const float *src, size_t count);
    void    abs_minmax(const float *src, size_t count, float *min, float *max);
}

IF_ARCH_X86(
    namespace sse
    {
        float   min(const float *src, size_t count);
        float   max(const float *src, size_t count);
        void    minmax(const float *src, size_t count, float *min, float *max);

        float   abs_min(const float *src, size_t count);
        float   abs_max(const float *src, size_t count);
        void    abs_minmax(const float *src, size_t count, float *min, float *max);
    }

    namespace avx
    {
        float   min(const float *src, size_t count);
        float   max(const float *src, size_t count);
        void    minmax(const float *src, size_t count, float *min, float *max);

        float   abs_min(const float *src, size_t count);
        float   abs_max(const float *src, size_t count);
        void    abs_minmax(const float *src, size_t count, float *min, float *max);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        float   min(const float *src, size_t count);
        float   max(const float *src, size_t count);
        void    minmax(const float *src, size_t count, float *min, float *max);

        float   abs_min(const float *src, size_t count);
        float   abs_max(const float *src, size_t count);
        void    abs_minmax(const float *src, size_t count, float *min, float *max);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        float   min(const float *src, size_t count);
        float   max(const float *src, size_t count);
        void    minmax(const float *src, size_t count, float *min, float *max);

        float   abs_min(const float *src, size_t count);
        float   abs_max(const float *src, size_t count);
        void    abs_minmax(const float *src, size_t count, float *min, float *max);
    }
)

typedef float (* ext_t)(const float *src, size_t count);
typedef void  (* minmax_t)(const float *src, size_t count, float *min, float *max);

UTEST_BEGIN("dsp.search", minmax)

    void call(const char *label, size_t align, ext_t func1, ext_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 768, 999, 1024)
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
                if (!float_equals_relative(a, b, 1e-5))
                {
                    src.dump("src1");
                    UTEST_FAIL_MSG("Result of function 1 (%f) differs result of function 2 (%f)", a, b)
                }
            }
        }
    }

    void call(const char *label, size_t align, minmax_t func1, minmax_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 768, 999, 1024)
        {
            for (size_t mask=0; mask <= 0x01; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer src(count, align, mask & 0x01);
                src.randomize_sign();

                // Call functions
                float min1, min2, max1, max2;
                func1(src, count, &min1, &max1);
                func2(src, count, &min2, &max2);

                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");

                // Compare buffers
                if ((!float_equals_relative(min1, min2, 1e-5)) && (!float_equals_relative(max1, max2, 1e-5)))
                {
                    src.dump("src1");
                    UTEST_FAIL_MSG("Result of function 1 (%f, %f) differs result of function 2 (%f, %f)", min1, max1, min2, max2)
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(native, func, align) \
            call(#func, align, native, func)

        IF_ARCH_X86(CALL(native::min, sse::min, 16));
        IF_ARCH_X86(CALL(native::max, sse::max, 16));
        IF_ARCH_X86(CALL(native::minmax, sse::minmax, 16));
        IF_ARCH_X86(CALL(native::abs_min, sse::abs_min, 16));
        IF_ARCH_X86(CALL(native::abs_max, sse::abs_max, 16));
        IF_ARCH_X86(CALL(native::abs_minmax, sse::abs_minmax, 16));

        IF_ARCH_X86(CALL(native::min, avx::min, 32));
        IF_ARCH_X86(CALL(native::max, avx::max, 32));
        IF_ARCH_X86(CALL(native::minmax, avx::minmax, 32));
        IF_ARCH_X86(CALL(native::abs_min, avx::abs_min, 32));
        IF_ARCH_X86(CALL(native::abs_max, avx::abs_max, 32));
        IF_ARCH_X86(CALL(native::abs_minmax, avx::abs_minmax, 32));

        IF_ARCH_ARM(CALL(native::min, neon_d32::min, 16));
        IF_ARCH_ARM(CALL(native::max, neon_d32::max, 16));
        IF_ARCH_ARM(CALL(native::minmax, neon_d32::minmax, 16));
        IF_ARCH_ARM(CALL(native::abs_min, neon_d32::abs_min, 16));
        IF_ARCH_ARM(CALL(native::abs_max, neon_d32::abs_max, 16));
        IF_ARCH_ARM(CALL(native::abs_minmax, neon_d32::abs_minmax, 16));

        IF_ARCH_AARCH64(CALL(native::min, asimd::min, 16));
        IF_ARCH_AARCH64(CALL(native::max, asimd::max, 16));
        IF_ARCH_AARCH64(CALL(native::minmax, asimd::minmax, 16));
        IF_ARCH_AARCH64(CALL(native::abs_min, asimd::abs_min, 16));
        IF_ARCH_AARCH64(CALL(native::abs_max, asimd::abs_max, 16));
        IF_ARCH_AARCH64(CALL(native::abs_minmax, asimd::abs_minmax, 16));
    }
UTEST_END


