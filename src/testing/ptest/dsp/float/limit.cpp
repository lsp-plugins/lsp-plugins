/*
 * limit.cpp
 *
 *  Created on: 17 дек. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>
#include <core/sugar.h>

#define MIN_RANK 7
#define MAX_RANK 16

namespace native
{
    void limit1(float *dst, float min, float max, size_t count);
    void limit2(float *dst, const float *src, float min, float max, size_t count);
}

IF_ARCH_X86(
    namespace sse2
    {
        void limit1(float *dst, float min, float max, size_t count);
        void limit2(float *dst, const float *src, float min, float max, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void limit1(float *dst, float min, float max, size_t count);
        void limit2(float *dst, const float *src, float min, float max, size_t count);
    }
)

typedef void (* limit1_t)(float *dst, float min, float max, size_t count);
typedef void (* limit2_t)(float *dst, const float *src, float min, float max, size_t count);

//-----------------------------------------------------------------------------
// Performance test for logarithmic axis calculation
PTEST_BEGIN("dsp.float", limit, 5, 10000)

    void call(const char *label, float *dst, const float *src, size_t count, limit2_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s samples...\n", buf);

        PTEST_LOOP(buf,
            func(dst, src, -1.0f, 1.0f, count);
        );
    }

    void call(const char *label, float *dst, const float *src, size_t count, limit1_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s samples...\n", buf);

        PTEST_LOOP(buf,
            dsp::copy(dst, src, count);
            func(dst, -1.0f, 1.0f, count);
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

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            call("native::limit1", dst, src, count, native::limit1);
            call("native::limit2", dst, src, count, native::limit2);

            IF_ARCH_X86(call("sse2::limit1", dst, src, count, sse2::limit1));
            IF_ARCH_X86(call("sse2::limit2", dst, src, count, sse2::limit2));

            IF_ARCH_ARM(call("neon_d32::limit1", dst, src, count, neon_d32::limit1));
            IF_ARCH_ARM(call("neon_d32::limit2", dst, src, count, neon_d32::limit2));

            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END





