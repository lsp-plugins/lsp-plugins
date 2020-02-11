/*
 * saturation.cpp
 *
 *  Created on: 28 авг. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>
#include <core/sugar.h>

#define MIN_RANK 7
#define MAX_RANK 16

namespace native
{
    void copy_saturated(float *dst, const float *src, size_t count);
    void saturate(float *dst, size_t count);
}

IF_ARCH_X86(
    namespace sse2
    {
        void copy_saturated(float *dst, const float *src, size_t count);
        void saturate(float *dst, size_t count);
    }

    namespace avx2
    {
        void copy_saturated(float *dst, const float *src, size_t count);
        void saturate(float *dst, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void copy_saturated(float *dst, const float *src, size_t count);
        void saturate(float *dst, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void copy_saturated(float *dst, const float *src, size_t count);
        void saturate(float *dst, size_t count);
    }
)

typedef void (* copy_saturated_t)(float *dst, const float *src, size_t count);
typedef void (* saturate_t)(float *dst, size_t count);

//-----------------------------------------------------------------------------
// Performance test for logarithmic axis calculation
PTEST_BEGIN("dsp.float", saturation, 5, 10000)

    void call(const char *label, float *dst, const float *src, size_t count, copy_saturated_t func)
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

    void call(const char *label, float *dst, const float *src, size_t count, saturate_t func)
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
                case 4:  src[i] = float(rand()) / RAND_MAX; break;
                default: src[i] = - float(rand()) / RAND_MAX; break;
            }
        }

        #define CALL(func) \
            call(#func, dst, src, count, func)

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(native::saturate);
            IF_ARCH_X86(CALL(sse2::saturate));
            IF_ARCH_X86(CALL(avx2::saturate));
            IF_ARCH_ARM(CALL(neon_d32::saturate));
            IF_ARCH_AARCH64(CALL(asimd::saturate));
            PTEST_SEPARATOR;

            CALL(native::copy_saturated);
            IF_ARCH_X86(CALL(sse2::copy_saturated));
            IF_ARCH_X86(CALL(avx2::copy_saturated));
            IF_ARCH_ARM(CALL(neon_d32::copy_saturated));
            IF_ARCH_AARCH64(CALL(asimd::copy_saturated));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }
PTEST_END




