/*
 * fmop_k2.cpp
 *
 *  Created on: 20 нояб. 2019 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>
#include <test/helpers.h>
#include <core/sugar.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace native
{
    void    fmadd_k3(float *dst, const float *src, float k, size_t count);
    void    fmsub_k3(float *dst, const float *src, float k, size_t count);
    void    fmrsub_k3(float *dst, const float *src, float k, size_t count);
    void    fmmul_k3(float *dst, const float *src, float k, size_t count);
    void    fmdiv_k3(float *dst, const float *src, float k, size_t count);
    void    fmrdiv_k3(float *dst, const float *src, float k, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void    fmadd_k3(float *dst, const float *src, float k, size_t count);
        void    fmsub_k3(float *dst, const float *src, float k, size_t count);
        void    fmrsub_k3(float *dst, const float *src, float k, size_t count);
        void    fmmul_k3(float *dst, const float *src, float k, size_t count);
        void    fmdiv_k3(float *dst, const float *src, float k, size_t count);
        void    fmrdiv_k3(float *dst, const float *src, float k, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void    fmadd_k3(float *dst, const float *src, float k, size_t count);
        void    fmsub_k3(float *dst, const float *src, float k, size_t count);
        void    fmrsub_k3(float *dst, const float *src, float k, size_t count);
        void    fmmul_k3(float *dst, const float *src, float k, size_t count);
        void    fmdiv_k3(float *dst, const float *src, float k, size_t count);
        void    fmrdiv_k3(float *dst, const float *src, float k, size_t count);
    }
)

typedef void (* fmop_k3_t)(float *dst, const float *src, float k, size_t count);

//-----------------------------------------------------------------------------
// Performance test for complex multiplication
PTEST_BEGIN("dsp.pmath", fmop_k3, 5, 1000)

    void call(const char *label, float *dst, const float *src, size_t count, fmop_k3_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(dst, src, 0.5f, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *dst      = alloc_aligned<float>(data, buf_size * 4, 64);
        float *src      = &dst[buf_size];
        float *backup   = &src[buf_size];

        randomize_sign(dst, buf_size*2);
        dsp::copy(backup, dst, buf_size*2);

        #define CALL(method) \
            dsp::copy(dst, backup, buf_size*2); \
            call(#method, dst, src, count, method);

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(native::fmadd_k3);
            IF_ARCH_X86(CALL(sse::fmadd_k3));
            IF_ARCH_ARM(CALL(neon_d32::fmadd_k3));
            PTEST_SEPARATOR;

            CALL(native::fmsub_k3);
            IF_ARCH_X86(CALL(sse::fmsub_k3));
            IF_ARCH_ARM(CALL(neon_d32::fmsub_k3));
            PTEST_SEPARATOR;

            CALL(native::fmrsub_k3);
            IF_ARCH_X86(CALL(sse::fmrsub_k3));
            IF_ARCH_ARM(CALL(neon_d32::fmrsub_k3));
            PTEST_SEPARATOR;

            CALL(native::fmmul_k3);
            IF_ARCH_X86(CALL(sse::fmmul_k3));
            IF_ARCH_ARM(CALL(neon_d32::fmmul_k3));
            PTEST_SEPARATOR;

            CALL(native::fmdiv_k3);
            IF_ARCH_X86(CALL(sse::fmdiv_k3));
            IF_ARCH_ARM(CALL(neon_d32::fmdiv_k3));
            PTEST_SEPARATOR;

            CALL(native::fmrdiv_k3);
            IF_ARCH_X86(CALL(sse::fmrdiv_k3));
            IF_ARCH_ARM(CALL(neon_d32::fmrdiv_k3));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }
PTEST_END





