/*
 * exp.cpp
 *
 *  Created on: 7 дек. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>
#include <core/sugar.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace native
{
    void exp1(float *dst, size_t count);
    void exp2(float *dst, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse2
    {
        void exp1(float *dst, size_t count);
        void exp2(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_X86_64(
    namespace avx2
    {
        void x64_exp1(float *dst, size_t count);
        void x64_exp2(float *dst, const float *src, size_t count);

        void x64_exp1_fma3(float *dst, size_t count);
        void x64_exp2_fma3(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void exp1(float *dst, size_t count);
        void exp2(float *dst, const float *src, size_t count);
    }
)

typedef void (* exp1_t)(float *dst, size_t count);
typedef void (* exp2_t)(float *dst, const float *src, size_t count);

//-----------------------------------------------------------------------------
// Performance test
PTEST_BEGIN("dsp.pmath", exp, 5, 1000)

    void call(const char *label, float *dst, const float *src, size_t count, exp1_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            dsp::copy(dst, src, count);
            func(dst, count);
        );
    }

    void call(const char *label, float *dst, const float *src, size_t count, exp2_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(dst, src, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *dst      = alloc_aligned<float>(data, buf_size * 3, 64);
        float *src      = &dst[buf_size];
        float *backup   = &src[buf_size];

        for (size_t i=0; i < buf_size*3; ++i)
            dst[i]          = float(rand()) / RAND_MAX;
        dsp::copy(backup, dst, buf_size);

        #define CALL(...) \
            dsp::copy(dst, backup, buf_size); \
            call(__VA_ARGS__);

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL("native::exp1", dst, src, count, native::exp1);
            IF_ARCH_X86(CALL("sse2::exp1", dst, src, count, sse2::exp1));
            IF_ARCH_X86_64(CALL("avx2::x64_exp1", dst, src, count, avx2::x64_exp1));
            IF_ARCH_X86_64(CALL("avx2::x64_exp1_fma3", dst, src, count, avx2::x64_exp1_fma3));
            IF_ARCH_ARM(CALL("neon_d32::exp1", dst, src, count, neon_d32::exp1));
            PTEST_SEPARATOR;

            CALL("native::exp2", dst, src, count, native::exp2);
            IF_ARCH_X86(CALL("sse2::exp2", dst, src, count, sse2::exp2));
            IF_ARCH_X86_64(CALL("avx2::x64_exp2", dst, src, count, avx2::x64_exp2));
            IF_ARCH_X86_64(CALL("avx2::x64_exp2_fma3", dst, src, count, avx2::x64_exp2_fma3));
            IF_ARCH_ARM(CALL("neon_d32::exp2", dst, src, count, neon_d32::exp2));
            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END





