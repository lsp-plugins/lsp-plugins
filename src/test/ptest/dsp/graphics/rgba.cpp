/*
 * rgba.cpp
 *
 *  Created on: 17 сент. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>
#include <core/sugar.h>

#define MIN_RANK 8
#define MAX_RANK 20

namespace native
{
    void rgba32_to_bgra32(void *dst, const void *src, size_t count);
}

IF_ARCH_X86(
    namespace x86
    {
        void rgba32_to_bgra32(void *dst, const void *src, size_t count);
    }

    namespace sse
    {
        void rgba32_to_bgra32(void *dst, const void *src, size_t count);
    }
)

IF_ARCH_X86_64(
    namespace sse3
    {
        void x64_rgba32_to_bgra32(void *dst, const void *src, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void rgba32_to_bgra32(void *dst, const void *src, size_t count);
    }
)

typedef void (* rgba32_to_bgra32_t)(void *dst, const void *src, size_t count);

//-----------------------------------------------------------------------------
// Performance test for logarithmic axis calculation
PTEST_BEGIN("dsp.graphics", rgba, 5, 1000)

    void call(const char *label, uint8_t *dst, uint8_t *src, size_t count, rgba32_to_bgra32_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s pixels...\n", buf);

        PTEST_LOOP(buf,
            func(dst, src, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size     = 1 << MAX_RANK;
        uint8_t *data       = NULL;
        uint8_t *src        = alloc_aligned<uint8_t>(data, (buf_size * 2) * 4, 64);
        uint8_t *dst        = &src[buf_size * 4];

        for (size_t i=0; i<buf_size*2; ++i)
             src[i]             = uint8_t(rand());

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            call("native:rgba32_to_bgra32", dst, src, count, native::rgba32_to_bgra32);
            IF_ARCH_X86(call("x86:rgba32_to_bgra32", dst, src, count, x86::rgba32_to_bgra32));
            IF_ARCH_X86(call("sse:rgba32_to_bgra32", dst, src, count, sse::rgba32_to_bgra32));
            IF_ARCH_X86_64(call("sse3:x64_rgba32_to_bgra32", dst, src, count, sse3::x64_rgba32_to_bgra32));
            IF_ARCH_ARM(call("neon_d32:rgba32_to_bgra32", dst, src, count, neon_d32::rgba32_to_bgra32));

            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END


