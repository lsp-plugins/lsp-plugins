/*
 * rcp.cpp
 *
 *  Created on: 05 сен. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>
#include <core/sugar.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace native
{
    void complex_rcp1(float *dst_re, float *dst_im, size_t count);
    void complex_rcp2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count);
    void pcomplex_rcp1(float *dst, size_t count);
    void pcomplex_rcp2(float *dst, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void complex_rcp1(float *dst_re, float *dst_im, size_t count);
        void complex_rcp2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count);
        void pcomplex_rcp1(float *dst, size_t count);
        void pcomplex_rcp2(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void complex_rcp1(float *dst_re, float *dst_im, size_t count);
        void complex_rcp2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count);
    }
)

typedef void (* complex_rcp1_t) (float *dst_re, float *dst_im, size_t count);
typedef void (* complex_rcp2_t) (float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count);
typedef void (* pcomplex_rcp1_t) (float *dst, size_t count);
typedef void (* pcomplex_rcp2_t) (float *dst, const float *src, size_t count);

//-----------------------------------------------------------------------------
// Performance test for complex reciprocal
PTEST_BEGIN("dsp.complex", rcp, 5, 1000)

    void call(const char *label, float *dst, size_t count, complex_rcp1_t rcp)
    {
        if (!PTEST_SUPPORTED(rcp))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            rcp(dst, &dst[count], count);
        );
    }

    void call(const char *label, float *dst, const float *src, size_t count, complex_rcp2_t rcp)
    {
        if (!PTEST_SUPPORTED(rcp))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            rcp(dst, &dst[count], src, &src[count], count);
        );
    }

    void call(const char *label, float *dst, size_t count, pcomplex_rcp1_t rcp)
    {
        if (!PTEST_SUPPORTED(rcp))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            rcp(dst, count);
        );
    }

    void call(const char *label, float *dst, const float *src, size_t count, pcomplex_rcp2_t rcp)
    {
        if (!PTEST_SUPPORTED(rcp))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            rcp(dst, src, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *out      = alloc_aligned<float>(data, buf_size * 4, 64);
        float *in       = &out[buf_size];

        for (size_t i=0; i < (1 << (MAX_RANK + 1)); ++i)
        {
            in[i]           = float(rand()) / RAND_MAX;
            out[i]          = float(rand()) / RAND_MAX;
        }

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            call("native:complex_rcp1", out, count, native::complex_rcp1);
            call("native:complex_rcp2", out, in, count, native::complex_rcp2);
            call("native:pcomplex_rcp1", out, count, native::pcomplex_rcp1);
            call("native:pcomplex_rcp2", out, in, count, native::pcomplex_rcp2);

            IF_ARCH_X86(call("sse:complex_rcp1", out, count, sse::complex_rcp1));
            IF_ARCH_X86(call("sse:complex_rcp2", out, in, count, sse::complex_rcp2));
            IF_ARCH_X86(call("sse:pcomplex_rcp1", out, count, sse::pcomplex_rcp1));
            IF_ARCH_X86(call("sse:pcomplex_rcp2", out, in, count, sse::pcomplex_rcp2));

            IF_ARCH_ARM(call("neon_d32:complex_rcp1", out, count, neon_d32::complex_rcp1));
            IF_ARCH_ARM(call("neon_d32:complex_rcp2", out, count, neon_d32::complex_rcp2));

            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END


