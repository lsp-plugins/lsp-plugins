/*
 * mul.cpp
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
    void complex_mod(float *dst_mod, const float *src_re, const float *src_im, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void complex_mod(float *dst_mod, const float *src_re, const float *src_im, size_t count);
    }

    namespace avx
    {
        void complex_mod(float *dst_mod, const float *src_re, const float *src_im, size_t count);
        void complex_mod_fma3(float *dst_mod, const float *src_re, const float *src_im, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void complex_mod(float *dst_mod, const float *src_re, const float *src_im, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void complex_mod(float *dst_mod, const float *src_re, const float *src_im, size_t count);
    }
)

typedef void (* complex_mod_t)(float *dst_mod, const float *src_re, const float *src_im, size_t count);

//-----------------------------------------------------------------------------
// Performance test for complex multiplication
PTEST_BEGIN("dsp.complex", mod, 5, 1000)

    void call(const char *label, float *dst, const float *src, size_t count, complex_mod_t mod)
    {
        if (!PTEST_SUPPORTED(mod))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            mod(dst, src, &src[count], count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *out      = alloc_aligned<float>(data, buf_size * 6, 64);
        float *in       = &out[buf_size];
        float *backup   = &in[buf_size*2];

        for (size_t i=0; i < buf_size*3; ++i)
            out[i]          = float(rand()) / RAND_MAX;
        dsp::copy(backup, out, buf_size * 3);

        #define CALL(func) \
            dsp::copy(out, backup, buf_size * 3); \
            call(#func, out, in, count, func)

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(native::complex_mod);
            IF_ARCH_X86(CALL(sse::complex_mod));
            IF_ARCH_X86(CALL(avx::complex_mod));
            IF_ARCH_X86(CALL(avx::complex_mod_fma3));
            IF_ARCH_ARM(CALL(neon_d32::complex_mod));
            IF_ARCH_AARCH64(CALL(asimd::complex_mod));

            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END


