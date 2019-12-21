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
}

IF_ARCH_X86(
    namespace sse
    {
        void complex_rcp1(float *dst_re, float *dst_im, size_t count);
        void complex_rcp2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count);
    }

    namespace avx
    {
        void complex_rcp1(float *dst_re, float *dst_im, size_t count);
        void complex_rcp2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count);

        void complex_rcp1_fma3(float *dst_re, float *dst_im, size_t count);
        void complex_rcp2_fma3(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void complex_rcp1(float *dst_re, float *dst_im, size_t count);
        void complex_rcp2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void complex_rcp1(float *dst_re, float *dst_im, size_t count);
        void complex_rcp2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count);
    }
)

typedef void (* complex_rcp1_t) (float *dst_re, float *dst_im, size_t count);
typedef void (* complex_rcp2_t) (float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count);

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

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *out      = alloc_aligned<float>(data, buf_size * 8, 64);
        float *in       = &out[buf_size*2];
        float *backup   = &in[buf_size*2];

        for (size_t i=0; i < buf_size*4; ++i)
            out[i]          = float(rand()) / RAND_MAX;
        dsp::copy(backup, out, buf_size * 4);

        #define CALL1(func) \
            dsp::copy(out, backup, buf_size * 4); \
            call(#func, out, count, func)

        #define CALL2(func) \
            dsp::copy(out, backup, buf_size * 4); \
            call(#func, out, in, count, func)

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL1(native::complex_rcp1);
            IF_ARCH_X86(CALL1(sse::complex_rcp1));
            IF_ARCH_X86(CALL1(avx::complex_rcp1));
            IF_ARCH_X86(CALL1(avx::complex_rcp1_fma3));
            IF_ARCH_ARM(CALL1(neon_d32::complex_rcp1));
            IF_ARCH_AARCH64(CALL1(asimd::complex_rcp1));
            PTEST_SEPARATOR;

            CALL2(native::complex_rcp2);
            IF_ARCH_X86(CALL2(sse::complex_rcp2));
            IF_ARCH_X86(CALL2(avx::complex_rcp2));
            IF_ARCH_X86(CALL2(avx::complex_rcp2_fma3));
            IF_ARCH_ARM(CALL2(neon_d32::complex_rcp2));
            IF_ARCH_AARCH64(CALL2(asimd::complex_rcp2));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }
PTEST_END


