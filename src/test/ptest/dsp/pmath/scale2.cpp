/*
 * scale2.cpp
 *
 *  Created on: 23 авг. 2018 г.
 *      Author: sadko
 */


#include <dsp/dsp.h>
#include <test/ptest.h>
#include <core/sugar.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace native
{
    void    scale2(float *dst, float k, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void    scale2(float *dst, float k, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void    scale2(float *dst, float k, size_t count);
    }
)

typedef void (* scale2_t)(float *dst, float k, size_t count);

//-----------------------------------------------------------------------------
// Performance test for complex multiplication
PTEST_BEGIN("dsp.pmath", scale2, 5, 1000)

    void call(const char *label, float *dst, size_t count, scale2_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(dst, 0.5f, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *dst      = alloc_aligned<float>(data, buf_size * 2, 64);
        float *backup   = &dst[buf_size];

        for (size_t i=0; i < buf_size; ++i)
            dst[i]          = float(rand()) / RAND_MAX;
        dsp::copy(backup, dst, buf_size);

        #define CALL(...) \
            dsp::copy(dst, backup, buf_size); \
            call(__VA_ARGS__);

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL("native:scale2", dst, count, native::scale2);
            IF_ARCH_X86(CALL("sse:scale2", dst, count, sse::scale2));
            IF_ARCH_ARM(CALL("neon_d32:scale2", dst, count, neon_d32::scale2));
            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END


