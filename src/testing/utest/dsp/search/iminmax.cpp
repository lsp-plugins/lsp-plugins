/*
 * iminmax.cpp
 *
 *  Created on: 10 окт. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/FloatBuffer.h>
#include <test/helpers.h>

namespace native
{
    size_t  min_index(const float *src, size_t count);
    size_t  max_index(const float *src, size_t count);
    void    minmax_index(const float *src, size_t count, size_t *min, size_t *max);

    size_t  abs_min_index(const float *src, size_t count);
    size_t  abs_max_index(const float *src, size_t count);
    void    abs_minmax_index(const float *src, size_t count, size_t *min, size_t *max);
}

IF_ARCH_X86(
    namespace sse2
    {
        size_t  min_index(const float *src, size_t count);
        size_t  max_index(const float *src, size_t count);
        void    minmax_index(const float *src, size_t count, size_t *min, size_t *max);

        size_t  abs_min_index(const float *src, size_t count);
        size_t  abs_max_index(const float *src, size_t count);
        void    abs_minmax_index(const float *src, size_t count, size_t *min, size_t *max);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        size_t  min_index(const float *src, size_t count);
        size_t  max_index(const float *src, size_t count);
        void    minmax_index(const float *src, size_t count, size_t *min, size_t *max);

        size_t  abs_min_index(const float *src, size_t count);
        size_t  abs_max_index(const float *src, size_t count);
        void    abs_minmax_index(const float *src, size_t count, size_t *min, size_t *max);
    }
)

typedef size_t  (* cond_index_t)(const float *src, size_t count);
typedef void    (* cond_minmax_t)(const float *src, size_t count, size_t *min, size_t *max);

UTEST_BEGIN("dsp.search", iminmax)

    void call(const char *label, size_t align, cond_index_t func1, cond_index_t func2)
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
                size_t a = func1(src, count);
                size_t b = func2(src, count);

                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");

                // Compare buffers
                if (a != b)
                {
                    src.dump("src");
                    UTEST_FAIL_MSG("Result of function 1 (%d) differs result of function 2 (%d)", int(a), int(b))
                }
            }
        }
    }

    void call(const char *label, size_t align, cond_minmax_t func1, cond_minmax_t func2)
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
                size_t min_a, max_a, min_b, max_b;
                func1(src, count, &min_a, &max_a);
                func2(src, count, &min_b, &max_b);

                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");

                // Compare buffers
                if (min_a != min_b)
                {
                    src.dump("src");
                    UTEST_FAIL_MSG("Result of min differs (f1=%d vs f2=%d)", int(min_a), int(min_b))
                }
                else if (max_a != max_b)
                {
                    src.dump("src");
                    UTEST_FAIL_MSG("Result of max differs (f1=%d vs f2=%d)", int(max_a), int(max_b))
                }
            }
        }
    }

    UTEST_MAIN
    {
        IF_ARCH_X86(call("sse2:min_index", 16, native::min_index, sse2::min_index));
        IF_ARCH_X86(call("sse2:max_index", 16, native::max_index, sse2::max_index));
        IF_ARCH_X86(call("sse2:minmax_index", 16, native::minmax_index, sse2::minmax_index));
        IF_ARCH_ARM(call("neon_d32:min_index", 16, native::min_index, neon_d32::min_index));
        IF_ARCH_ARM(call("neon_d32:max_index", 16, native::max_index, neon_d32::max_index));
        IF_ARCH_ARM(call("neon_d32:minmax_index", 16, native::minmax_index, neon_d32::minmax_index));

        IF_ARCH_X86(call("sse2:abs_min_index", 16, native::abs_min_index, sse2::abs_min_index));
        IF_ARCH_X86(call("sse2:abs_max_index", 16, native::abs_max_index, sse2::abs_max_index));
        IF_ARCH_X86(call("sse2:abs_minmax_index", 16, native::abs_minmax_index, sse2::abs_minmax_index));
        IF_ARCH_ARM(call("neon_d32:abs_min_index", 16, native::abs_min_index, neon_d32::abs_min_index));
        IF_ARCH_ARM(call("neon_d32:abs_max_index", 16, native::abs_max_index, neon_d32::abs_max_index));
        IF_ARCH_ARM(call("neon_d32:abs_minmax_index", 16, native::abs_minmax_index, neon_d32::abs_minmax_index));
    }
UTEST_END



