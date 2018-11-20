/*
 * rgba_to_hsla.cpp
 *
 *  Created on: 17 нояб. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>
#include <core/sugar.h>

#define MIN_RANK 6
#define MAX_RANK 14

namespace native
{
    void rgba_to_hsla(float *dst, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse2
    {
        void rgba_to_hsla(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void rgba_to_hsla(float *dst, const float *src, size_t count);
    }
)

typedef void (* rgba_to_hsla_t)(float *dst, const float *src, size_t count);

//-----------------------------------------------------------------------------
// Performance test for logarithmic axis calculation
PTEST_BEGIN("dsp.graphics", rgba_to_hsla, 5, 5000)

    void call(const char *label, float *dst, float *src, size_t count, rgba_to_hsla_t func)
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
        float *src          = alloc_aligned<float>(data, (buf_size * 2) * 4, 64);
        float *dst          = &src[buf_size * 4];

        for (size_t i=0; i<buf_size*4; ++i)
            src[i]             = float(rand()) / (RAND_MAX - 1);

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            call("native::rgba_to_hsla", dst, src, count, native::rgba_to_hsla);
            IF_ARCH_X86(call("sse2::rgba_to_hsla", dst, src, count, sse2::rgba_to_hsla));
            IF_ARCH_ARM(call("neon_d32::rgba_to_hsla", dst, src, count, neon_d32::rgba_to_hsla));

            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END






