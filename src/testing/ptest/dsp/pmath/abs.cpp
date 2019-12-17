/*
 * abs.cpp
 *
 *  Created on: 22 сент. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>
#include <core/sugar.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace native
{
    void abs1(float *src, size_t count);
    void abs2(float *dst, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void abs1(float *src, size_t count);
        void abs2(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_X86_64(
    namespace avx
    {
        void x64_abs1(float *src, size_t count);
        void x64_abs2(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void abs1(float *src, size_t count);
        void abs2(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void abs1(float *src, size_t count);
        void abs2(float *dst, const float *src, size_t count);
    }
)

typedef void (* abs1_t)(float *src, size_t count);
typedef void (* abs2_t)(float *dst, const float *src, size_t count);

//-----------------------------------------------------------------------------
// Performance test
PTEST_BEGIN("dsp.pmath", abs, 5, 1000)

    void call(const char *label, float *dst, size_t count, abs1_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(dst, count);
        );
    }

    void call(const char *label, float *dst, const float *src, size_t count, abs2_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(dst, src, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *dst      = alloc_aligned<float>(data, buf_size * 3, 64);
        float *src      = &dst[buf_size];
        float *backup   = &src[buf_size];

        for (size_t i=0; i < buf_size*3; ++i)
            dst[i]          = float(rand()) / RAND_MAX;
        dsp::copy(backup, dst, buf_size);

        #define CALL(...) \
            dsp::copy(dst, backup, buf_size); \
            call(__VA_ARGS__);

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL("native::abs1", dst, count, native::abs1);
            IF_ARCH_X86(CALL("sse::abs1", dst, count, sse::abs1));
            IF_ARCH_X86_64(CALL("avx::x64_abs1", dst, count, avx::x64_abs1));
            IF_ARCH_ARM(CALL("neon_d32::abs1", dst, count, neon_d32::abs1));
            IF_ARCH_AARCH64(CALL("asimd::abs1", dst, count, asimd::abs1));
            PTEST_SEPARATOR;

            CALL("native::abs2", dst, src, count, native::abs2);
            IF_ARCH_X86(CALL("sse::abs2", dst, src, count, sse::abs2));
            IF_ARCH_X86_64(CALL("avx::x64_abs2", dst, src, count, avx::x64_abs2));
            IF_ARCH_ARM(CALL("neon_d32::abs2", dst, src, count, neon_d32::abs2));
            IF_ARCH_AARCH64(CALL("asimd::abs2", dst, src, count, asimd::abs2));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }
PTEST_END


