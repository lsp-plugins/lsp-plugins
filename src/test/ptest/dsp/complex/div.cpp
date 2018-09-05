/*
 * div.cpp
 *
 *  Created on: 5 сент. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>
#include <core/sugar.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace native
{
    void complex_div2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count);
    void complex_rdiv2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void complex_div2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count);
        void complex_rdiv2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void complex_div2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count);
        void complex_rdiv2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count);
    }
)

typedef void (* complex_div2_t) (float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count);

//-----------------------------------------------------------------------------
// Performance test for complex division
PTEST_BEGIN("dsp.complex", div, 5, 1000)

    void call(const char *label, float *dst, const float *src, size_t count, complex_div2_t mul)
    {
        if (!PTEST_SUPPORTED(mul))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            mul(dst, &dst[count], src, &src[count], count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *out      = alloc_aligned<float>(data, buf_size * 6, 64);
        float *in1      = &out[buf_size*2];
        float *in2      = &in1[buf_size*2];

        for (size_t i=0; i < (1 << (MAX_RANK + 1)); ++i)
        {
            out[i]          = float(rand()) / RAND_MAX;
            in1[i]          = float(rand()) / RAND_MAX;
            in2[i]          = float(rand()) / RAND_MAX;
        }

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            call("native:complex_div2", out, in1, count, native::complex_div2);
            call("native:complex_rdiv2", out, in1, count, native::complex_rdiv2);
            IF_ARCH_X86(call("sse:complex_div2", out, in1, count, sse::complex_div2));
            IF_ARCH_X86(call("sse:complex_rdiv2", out, in1, count, sse::complex_rdiv2));
            IF_ARCH_ARM(call("neon_d32:complex_div2", out, in1, count, neon_d32::complex_div2));
            IF_ARCH_ARM(call("neon_d32:complex_rdiv2", out, in1, count, neon_d32::complex_rdiv2));

            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END





