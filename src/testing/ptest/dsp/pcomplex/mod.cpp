/*
 * mod.cpp
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
    void pcomplex_mod(float *dst_mod, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void pcomplex_mod(float *dst_mod, const float *src, size_t count);
    }

    namespace sse3
    {
        void pcomplex_mod(float *dst_mod, const float *src, size_t count);
        void x64_pcomplex_mod(float *dst_mod, const float *src, size_t count);
    }

    namespace avx
    {
        void pcomplex_mod(float *dst_mod, const float *src, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void pcomplex_mod(float *dst_mod, const float *src, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void pcomplex_mod(float *dst_mod, const float *src, size_t count);
    }
)

typedef void (* pcomplex_mod_t)(float *dst_mod, const float *src, size_t count);

//-----------------------------------------------------------------------------
// Performance test for complex multiplication
PTEST_BEGIN("dsp.pcomplex", mod, 5, 1000)

    void call(const char *label, float *dst, const float *src1, size_t count, pcomplex_mod_t mod)
    {
        if (!PTEST_SUPPORTED(mod))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            mod(dst, src1, count);
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

            CALL(native::pcomplex_mod);
            IF_ARCH_X86(CALL(sse::pcomplex_mod));
            IF_ARCH_X86(CALL(sse3::pcomplex_mod));
            IF_ARCH_X86_64(CALL(sse3::x64_pcomplex_mod));
            IF_ARCH_X86(CALL(avx::pcomplex_mod));
            IF_ARCH_ARM(CALL(neon_d32::pcomplex_mod));
            IF_ARCH_AARCH64(CALL(asimd::pcomplex_mod));

            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END


