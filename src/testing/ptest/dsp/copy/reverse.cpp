/*
 * reverse.cpp
 *
 *  Created on: 26 нояб. 2019 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>
#include <test/helpers.h>
#include <math.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace native
{
    void reverse1(float *dst, size_t count);
    void reverse2(float *dst, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void reverse1(float *dst, size_t count);
        void reverse2(float *dst, const float *src, size_t count);
    }

    namespace avx
    {
        void reverse1(float *dst, size_t count);
        void reverse2(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void reverse1(float *dst, size_t count);
        void reverse2(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void reverse1(float *dst, size_t count);
        void reverse2(float *dst, const float *src, size_t count);
    }
)

typedef void (* reverse1_t)(float *dst, size_t count);
typedef void (* reverse2_t)(float *dst, const float *src, size_t count);


//-----------------------------------------------------------------------------
// Performance test for destination buffer filling
PTEST_BEGIN("dsp.copy", reverse, 5, 5000)

    void call(const char *label, float *dst, size_t count, reverse1_t func)
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

    void call(const char *label, float *dst, const float *src, size_t count, reverse2_t func)
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

        float *dst      = alloc_aligned<float>(data, buf_size*3, 64);
        float *src      = &dst[buf_size];
        float *backup   = &src[buf_size];

        randomize_positive(dst, buf_size*3);

        #define CALL1(func) \
            dsp::copy(dst, backup, buf_size); \
            call(#func, dst, count, func)
        #define CALL2(func) \
            call(#func, dst, src, count, func)

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL1(native::reverse1);
            IF_ARCH_X86(CALL1(sse::reverse1));
            IF_ARCH_X86(CALL1(avx::reverse1));
            IF_ARCH_ARM(CALL1(neon_d32::reverse1));
            IF_ARCH_AARCH64(CALL1(asimd::reverse1));
            PTEST_SEPARATOR;

            CALL2(native::reverse2);
            IF_ARCH_X86(CALL2(sse::reverse2));
            IF_ARCH_X86(CALL2(avx::reverse2));
            IF_ARCH_ARM(CALL2(neon_d32::reverse2));
            IF_ARCH_AARCH64(CALL2(asimd::reverse2));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }

PTEST_END


