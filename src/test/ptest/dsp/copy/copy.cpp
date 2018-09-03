/*
 * copy.cpp
 *
 *  Created on: 3 сент. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>
#include <string.h>

#define MIN_RANK 8
#define MAX_RANK 20

namespace native
{
    void move(float *dst, const float *src, size_t count);
    void copy(float *dst, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void move(float *dst, const float *src, size_t count);
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

// Standard implementation provided by C library
static void copy(float *dst, const float *src, size_t count)
{
    memcpy(dst, src, count * sizeof(float));
}

static void move(float *dst, const float *src, size_t count)
{
    memmove(dst, src, count * sizeof(float));
}

PTEST_BEGIN("dsp.copy", copy, 5, 10000)

    void call(const char *label, float *dst, const float *src, size_t count, copy_t copy)
    {
        if (!PTEST_SUPPORTED(copy))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            copy(dst, src, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;

        float *out      = alloc_aligned<float>(data, buf_size * 2, 64);
        float *in       = &out[buf_size];

        for (size_t i=0; i < (1 << MAX_RANK); ++i)
            in[i]          = float(rand()) / RAND_MAX;

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            call("std_copy", out, in, count, ::copy);
            call("std_move", out, in, count, ::move);
            call("native_copy", out, in, count, native::copy);
            call("native_move", out, in, count, native::move);
            IF_ARCH_X86(call("sse_copy", out, in, count, sse::copy));
            IF_ARCH_X86(call("sse_move", out, in, count, sse::move));
            IF_ARCH_ARM(call("neon_d16_copy", out, in, count, neon_d16::copy));
            IF_ARCH_ARM(call("neon_d16_move", out, in, count, neon_d16::move));

            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END


