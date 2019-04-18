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
    namespace x86
    {
        void copy(float *dst, const float *src, size_t count);
    }

    namespace sse
    {
        void move(float *dst, const float *src, size_t count);
        void copy(float *dst, const float *src, size_t count);
        void copy_movntps(float *dst, const float *src, size_t count);
    }

    namespace sse3
    {
        void copy(float *dst, const float *src, size_t count);
    }

    namespace avx
    {
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

IF_ARCH_AARCH64(
    namespace asimd
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

PTEST_BEGIN("dsp.copy", copy, 5, 1000)

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

        TEST_EXPORT(::copy);
        TEST_EXPORT(::move);

        for (size_t i=MIN_RANK; i <= MAX_RANK; i += 2)
        {
            size_t count = 1 << i;

            call("std::copy", out, in, count, ::copy);
            call("std::move", out, in, count, ::move);
            call("native::copy", out, in, count, native::copy);
            call("native::move", out, in, count, native::move);

            IF_ARCH_X86(call("x86::movs_copy", out, in, count, x86::copy));
            IF_ARCH_X86(call("sse::copy", out, in, count, sse::copy));
            IF_ARCH_X86(call("sse::copy_movntps", out, in, count, sse::copy_movntps));
            IF_ARCH_X86(call("sse3::copy", out, in, count, sse3::copy));
            IF_ARCH_X86(call("avx::copy", out, in, count, avx::copy));
            IF_ARCH_X86(call("sse::move", out, in, count, sse::move));

            IF_ARCH_ARM(call("neon_d32::copy", out, in, count, neon_d32::copy));
            IF_ARCH_ARM(call("neon_d32::move", out, in, count, neon_d32::move));

            IF_ARCH_AARCH64(call("asimd::copy", out, in, count, asimd::copy));
            IF_ARCH_AARCH64(call("asimd::move", out, in, count, asimd::move));

            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END


