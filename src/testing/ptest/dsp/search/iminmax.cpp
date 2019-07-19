/*
 * iminmax.cpp
 *
 *  Created on: 10 окт. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>

#define MIN_RANK 8
#define MAX_RANK 16

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

//-----------------------------------------------------------------------------
// Performance test for minimum and maximum searching
PTEST_BEGIN("dsp.search", iminmax, 5, 1000)

    void call(const char *label, const float *in, size_t count, cond_index_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(in, count);
        );
    }

    void call(const char *label, const float *in, size_t count, cond_minmax_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);
        size_t min, max;

        PTEST_LOOP(buf,
            func(in, count, &min, &max);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;

        float *in       = alloc_aligned<float>(data, buf_size, 64);
        for (size_t i=0; i < (1 << MAX_RANK); ++i)
            in[i]          = float(rand()) / RAND_MAX;

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            //--------------
            call("native::min_index", in, count, native::min_index);
            IF_ARCH_X86(call("sse2::min_index", in, count, sse2::min_index));
            IF_ARCH_ARM(call("neon_d32::min_index", in, count, neon_d32::min_index));

            call("native::abs_min_index", in, count, native::abs_min_index);
            IF_ARCH_X86(call("sse2::abs_min_index", in, count, sse2::abs_min_index));
            IF_ARCH_ARM(call("neon_d32::abs_min_index", in, count, neon_d32::abs_min_index));
            PTEST_SEPARATOR;

            //--------------
            call("native::max_index", in, count, native::max_index);
            IF_ARCH_X86(call("sse2::max_index", in, count, sse2::max_index));
            IF_ARCH_ARM(call("neon_d32::max_index", in, count, neon_d32::max_index));

            call("native::abs_max_index", in, count, native::abs_max_index);
            IF_ARCH_X86(call("sse2::abs_max_index", in, count, sse2::abs_max_index));
            IF_ARCH_ARM(call("neon_d32::abs_max_index", in, count, neon_d32::abs_max_index));
            PTEST_SEPARATOR;

            //--------------
            call("native::minmax_index", in, count, native::minmax_index);
            IF_ARCH_X86(call("sse2::minmax_index", in, count, sse2::minmax_index));
            IF_ARCH_ARM(call("neon_d32::minmax_index", in, count, neon_d32::minmax_index));

            call("native::abs_minmax_index", in, count, native::abs_minmax_index);
            IF_ARCH_X86(call("sse2::abs_minmax_index", in, count, sse2::abs_minmax_index));
            IF_ARCH_ARM(call("neon_d32::abs_minmax_index", in, count, neon_d32::abs_minmax_index));
            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }

PTEST_END


