/*
 * mul3.cpp
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
    void pcomplex_mul3(float *dst, const float *src1, const float *src2, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void pcomplex_mul3(float *dst, const float *src1, const float *src2, size_t count);
    }

    namespace sse3
    {
        void pcomplex_mul3(float *dst, const float *src1, const float *src2, size_t count);
    }

    IF_ARCH_X86_64(
        namespace sse3
        {
            void x64_pcomplex_mul3(float *dst, const float *src1, const float *src2, size_t count);
        }

        namespace avx
        {
            void x64_pcomplex_mul3(float *dst, const float *src1, const float *src2, size_t count);
            void x64_pcomplex_mul3_fma3(float *dst, const float *src1, const float *src2, size_t count);
        }
    )
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void pcomplex_mul3(float *dst, const float *src1, const float *src2, size_t count);
    }
)

typedef void (* pcomplex_mul3_t) (float *dst, const float *src1, const float *src2, size_t count);

//-----------------------------------------------------------------------------
// Performance test for complex multiplication
PTEST_BEGIN("dsp.pcomplex", mul3, 5, 1000)

    void call(const char *label, float *dst, const float *src1, const float *src2, size_t count, pcomplex_mul3_t mul)
    {
        if (!PTEST_SUPPORTED(mul))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            mul(dst, src1, src2, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *out      = alloc_aligned<float>(data, buf_size*12, 64);
        float *in1      = &out[buf_size*2];
        float *in2      = &in1[buf_size*2];
        float *backup   = &in2[buf_size*2];

        for (size_t i=0; i < buf_size*6; ++i)
            out[i]          = float(rand()) / RAND_MAX;
        dsp::copy(backup, out, buf_size * 6);

        #define CALL(...) dsp::copy(out, backup, buf_size * 6); call(__VA_ARGS__)

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL("native:pcomplex_mul3", out, in1, in2, count, native::pcomplex_mul3);
            IF_ARCH_X86(CALL("sse:pcomplex_mul3", out, in1, in2, count, sse::pcomplex_mul3));
            IF_ARCH_X86(CALL("sse3:pcomplex_mul3", out, in1, in2, count, sse3::pcomplex_mul3));
            IF_ARCH_X86_64(CALL("x64_sse3:pcomplex_mul3", out, in1, in2, count, sse3::x64_pcomplex_mul3));
            IF_ARCH_X86_64(CALL("x64_avx:pcomplex_mul3", out, in1, in2, count, avx::x64_pcomplex_mul3));
            IF_ARCH_X86_64(CALL("x64_fma3:pcomplex_mul3", out, in1, in2, count, avx::x64_pcomplex_mul3_fma3));
            IF_ARCH_ARM(CALL("neon_d32:pcomplex_mul3", out, in1, in2, count, neon_d32::pcomplex_mul3));

            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END


