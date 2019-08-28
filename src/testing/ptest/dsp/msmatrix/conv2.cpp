/*
 * conv2.cpp
 *
 *  Created on: 3 окт. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>
#include <string.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace native
{
    void    lr_to_ms(float *m, float *s, const float *l, const float *r, size_t count);
    void    ms_to_lr(float *l, float *r, const float *m, const float *s, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void    lr_to_ms(float *m, float *s, const float *l, const float *r, size_t count);
        void    ms_to_lr(float *l, float *r, const float *m, const float *s, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void    lr_to_ms(float *m, float *s, const float *l, const float *r, size_t count);
        void    ms_to_lr(float *l, float *r, const float *m, const float *s, size_t count);
    }
)

typedef void (* conv2_t)(float *d1, float *d2, const float *s1, const float *s2, size_t count);

PTEST_BEGIN("dsp.msmatrix", conv2, 5, 1000)

    void call(const char *label, float *d1, float *d2, const float *s1, const float *s2, size_t count, conv2_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(d1, d2, s1, s2, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;

        float *d1       = alloc_aligned<float>(data, buf_size * 4, 64);
        float *d2       = &d1[buf_size];
        float *s1       = &d2[buf_size];
        float *s2       = &s1[buf_size];

        for (size_t i=0; i < buf_size * 4; ++i)
            d1[i]           = float(rand()) / RAND_MAX;

        for (size_t i=MIN_RANK; i <= MAX_RANK; i++)
        {
            size_t count = 1 << i;

            call("native:lr_to_ms", d1, d2, s1, s2, count, native::lr_to_ms);
            IF_ARCH_X86(call("sse:lr_to_ms", d1, d2, s1, s2, count, sse::lr_to_ms));
            IF_ARCH_ARM(call("neon_d32:lr_to_ms", d1, d2, s1, s2, count, neon_d32::lr_to_ms));
            PTEST_SEPARATOR;

            call("native:ms_to_lr", d1, d2, s1, s2, count, native::ms_to_lr);
            IF_ARCH_X86(call("sse:ms_to_lr", d1, d2, s1, s2, count, sse::ms_to_lr));
            IF_ARCH_ARM(call("neon_d32:ms_to_lr", d1, d2, s1, s2, count, neon_d32::ms_to_lr));
            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END


