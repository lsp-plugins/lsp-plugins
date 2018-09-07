/*
 * mul2.cpp
 *
 *  Created on: 22 авг. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>
#include <core/sugar.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace native
{
    void pcomplex_mul2(float *dst, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void pcomplex_mul2(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void pcomplex_mul2(float *dst, const float *src, size_t count);
    }
)

typedef void (* pcomplex_mul2_t) (float *dst, const float *src, size_t count);

//-----------------------------------------------------------------------------
// Performance test for complex multiplication
PTEST_BEGIN("dsp.pcomplex", mul2, 5, 1000)

    void call(const char *label, float *dst, const float *src, size_t count, pcomplex_mul2_t mul)
    {
        if (!PTEST_SUPPORTED(mul))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            mul(dst, src, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *out      = alloc_aligned<float>(data, buf_size*8, 64);
        float *in       = &out[buf_size*2];
        float *backup   = &in[buf_size*2];

        for (size_t i=0; i < buf_size*4; ++i)
            out[i]          = float(rand()) / RAND_MAX;
        dsp::copy(backup, out, buf_size * 4);

        #define CALL(...) dsp::copy(out, backup, buf_size * 4); call(__VA_ARGS__)

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL("native:pcomplex_mul2", out, in, count, native::pcomplex_mul2);
            IF_ARCH_X86(CALL("sse:pcomplex_mul2", out, in, count, sse::pcomplex_mul2));
            IF_ARCH_ARM(CALL("neon_d32:pcomplex_mul2", out, in, count, neon_d32::pcomplex_mul2));

            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END


