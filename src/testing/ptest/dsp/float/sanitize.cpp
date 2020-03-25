/*
 * sanitize.cpp
 *
 *  Created on: 25 мар. 2020 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>
#include <test/helpers.h>
#include <core/sugar.h>

#define MIN_RANK 7
#define MAX_RANK 16

namespace native
{
    void sanitize1(float *dst, size_t count);
    void sanitize2(float *dst, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse2
    {
        void sanitize1(float *dst, size_t count);
        void sanitize2(float *dst, const float *src, size_t count);
    }

    namespace avx
    {
        void sanitize1(float *dst, size_t count);
        void sanitize2(float *dst, const float *src, size_t count);
    }

    namespace avx2
    {
        void sanitize1(float *dst, size_t count);
        void sanitize2(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void sanitize1(float *dst, size_t count);
        void sanitize2(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void sanitize1(float *dst, size_t count);
        void sanitize2(float *dst, const float *src, size_t count);
    }
)

typedef void (* sanitize1_t)(float *dst, size_t count);
typedef void (* sanitize2_t)(float *dst, const float *src, size_t count);


//-----------------------------------------------------------------------------
// Performance test for logarithmic axis calculation
PTEST_BEGIN("dsp.float", sanitize, 5, 10000)

    void call(const char *label, float *dst, const float *src, size_t count, sanitize2_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s samples...\n", buf);

        PTEST_LOOP(buf,
            func(dst, src, count);
        );
    }

    void call(const char *label, float *dst, const float *src, size_t count, sanitize1_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s samples...\n", buf);

        PTEST_LOOP(buf,
            dsp::copy(dst, src, count);
            func(dst, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size     = 1 << MAX_RANK;
        uint8_t *data       = NULL;

        float *dst          = alloc_aligned<float>(data, buf_size * 2, 64);
        float *src          = &dst[buf_size];
        randomize(src, buf_size, -1.0f, 1.0f);

        #define CALL(func) \
            call(#func, dst, src, count, func)

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(native::sanitize1);
            IF_ARCH_X86(CALL(sse2::sanitize1));
            IF_ARCH_X86(CALL(avx::sanitize1));
            IF_ARCH_X86(CALL(avx2::sanitize1));
            IF_ARCH_ARM(CALL(neon_d32::sanitize1));
            IF_ARCH_AARCH64(CALL(asimd::sanitize1));
            PTEST_SEPARATOR;

            CALL(native::sanitize2);
            IF_ARCH_X86(CALL(sse2::sanitize2));
            IF_ARCH_X86(CALL(avx::sanitize2));
            IF_ARCH_X86(CALL(avx2::sanitize2));
            IF_ARCH_ARM(CALL(neon_d32::sanitize2));
            IF_ARCH_AARCH64(CALL(asimd::sanitize2));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }
PTEST_END

