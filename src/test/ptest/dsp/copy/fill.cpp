/*
 * fill.cpp
 *
 *  Created on: 9 окт. 2018 г.
 *      Author: sadko
 */

#include <test/ptest.h>
#include <math.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace native
{
    void fill(float *dst, float value, size_t count);
    void fill_one(float *dst, size_t count);
    void fill_zero(float *dst, size_t count);
    void fill_minus_one(float *dst, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void fill(float *dst, float value, size_t count);
        void fill_one(float *dst, size_t count);
        void fill_zero(float *dst, size_t count);
        void fill_minus_one(float *dst, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void fill(float *dst, float value, size_t count);
        void fill_one(float *dst, size_t count);
        void fill_zero(float *dst, size_t count);
        void fill_minus_one(float *dst, size_t count);
    }
)

typedef void (* fill_t)(float *dst, size_t count);
typedef void (* fill_value_t)(float *dst, float value, size_t count);

//-----------------------------------------------------------------------------
// Performance test for destination buffer filling
PTEST_BEGIN("dsp.copy", fill, 5, 5000)

    void call(const char *label, float *out, size_t count, fill_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(out, count);
        );
    }

    void call(const char *label, float *out, size_t count, fill_value_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(out, M_PI, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;

        float *out       = alloc_aligned<float>(data, buf_size, 64);

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            call("native::fill", out, count, native::fill);
            IF_ARCH_X86(call("sse::fill", out, count, sse::fill));
            IF_ARCH_ARM(call("neon_d32::fill", out, count, neon_d32::fill));
            PTEST_SEPARATOR;

            call("native::fill_zero", out, count, native::fill_zero);
            IF_ARCH_X86(call("sse::fill_zero", out, count, sse::fill_zero));
            IF_ARCH_ARM(call("neon_d32::fill_zero", out, count, neon_d32::fill_zero));
            PTEST_SEPARATOR;

            call("native::fill_one", out, count, native::fill_one);
            IF_ARCH_X86(call("sse::fill_one", out, count, sse::fill_one));
            IF_ARCH_ARM(call("neon_d32::fill_one", out, count, neon_d32::fill_one));
            PTEST_SEPARATOR;

            call("native::fill_minus_one", out, count, native::fill_minus_one);
            IF_ARCH_X86(call("sse::fill_minus_one", out, count, sse::fill_minus_one));
            IF_ARCH_ARM(call("neon_d32::fill_minus_one", out, count, neon_d32::fill_minus_one));
            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }

PTEST_END
