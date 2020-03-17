/*
 * limit_saturate.cpp
 *
 *  Created on: 18 нояб. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>
#include <core/sugar.h>

#define MIN_RANK 7
#define MAX_RANK 16

namespace native
{
    void limit_saturate1(float *dst, size_t count);
    void limit_saturate2(float *dst, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse2
    {
        void limit_saturate1(float *dst, size_t count);
        void limit_saturate2(float *dst, const float *src, size_t count);
    }

    namespace avx2
    {
        void limit_saturate1(float *dst, size_t count);
        void limit_saturate2(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void limit_saturate1(float *dst, size_t count);
        void limit_saturate2(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void limit_saturate1(float *dst, size_t count);
        void limit_saturate2(float *dst, const float *src, size_t count);
    }
)

typedef void (* limit_saturate1_t)(float *dst, size_t count);
typedef void (* limit_saturate2_t)(float *dst, const float *src, size_t count);

//-----------------------------------------------------------------------------
// Performance test for logarithmic axis calculation
PTEST_BEGIN("dsp.float", limit_saturate, 5, 10000)

    void call(const char *label, float *dst, const float *src, size_t count, limit_saturate2_t func)
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

    void call(const char *label, float *dst, const float *src, size_t count, limit_saturate1_t func)
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

        for (size_t i=0; i<buf_size; ++i)
        {
            switch (i % 6)
            {
                case 0:  src[i] = +INFINITY; break;
                case 1:  src[i] = -INFINITY; break;
                case 2:  src[i] = NAN; break;
                case 3:  src[i] = -NAN; break;
                case 4:  src[i] = (2.0f * rand()) / RAND_MAX; break;
                default: src[i] = (-2.0f * rand()) / RAND_MAX; break;
            }
        }

        #define CALL(func) \
            call(#func, dst, src, count, func)

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(native::limit_saturate1);
            IF_ARCH_X86(CALL(sse2::limit_saturate1));
            IF_ARCH_X86(CALL(avx2::limit_saturate1));
            IF_ARCH_ARM(CALL(neon_d32::limit_saturate1));
            IF_ARCH_AARCH64(CALL(asimd::limit_saturate1));
            PTEST_SEPARATOR;

            CALL(native::limit_saturate2);
            IF_ARCH_X86(CALL(sse2::limit_saturate2));
            IF_ARCH_X86(CALL(avx2::limit_saturate2));
            IF_ARCH_ARM(CALL(neon_d32::limit_saturate2));
            IF_ARCH_AARCH64(CALL(asimd::limit_saturate2));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }
PTEST_END





