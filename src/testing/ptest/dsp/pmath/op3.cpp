/*
 * simple2.cpp
 *
 *  Created on: 23 авг. 2018 г.
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
    void    add3(float *dst, const float *src1, const float *src2, size_t count);
    void    sub3(float *dst, const float *src1, const float *src2, size_t count);
    void    mul3(float *dst, const float *src1, const float *src2, size_t count);
    void    div3(float *dst, const float *src1, const float *src2, size_t count);
    void    mod3(float *dst, const float *src1, const float *src2, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void    add3(float *dst, const float *src1, const float *src2, size_t count);
        void    sub3(float *dst, const float *src1, const float *src2, size_t count);
        void    mul3(float *dst, const float *src1, const float *src2, size_t count);
        void    div3(float *dst, const float *src1, const float *src2, size_t count);
    }

    namespace sse2
    {
        void    mod3(float *dst, const float *src1, const float *src2, size_t count);
    }

    namespace avx
    {
        void    add3(float *dst, const float *src1, const float *src2, size_t count);
        void    sub3(float *dst, const float *src1, const float *src2, size_t count);
        void    mul3(float *dst, const float *src1, const float *src2, size_t count);
        void    div3(float *dst, const float *src1, const float *src2, size_t count);
        void    mod3(float *dst, const float *src1, const float *src2, size_t count);
        void    mod3_fma3(float *dst, const float *src1, const float *src2, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void    add3(float *dst, const float *src1, const float *src2, size_t count);
        void    sub3(float *dst, const float *src1, const float *src2, size_t count);
        void    mul3(float *dst, const float *src1, const float *src2, size_t count);
        void    div3(float *dst, const float *src1, const float *src2, size_t count);
        void    mod3(float *dst, const float *src1, const float *src2, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void    add3(float *dst, const float *src1, const float *src2, size_t count);
        void    sub3(float *dst, const float *src1, const float *src2, size_t count);
        void    mul3(float *dst, const float *src1, const float *src2, size_t count);
        void    div3(float *dst, const float *src1, const float *src2, size_t count);
        void    mod3(float *dst, const float *src1, const float *src2, size_t count);
    }
)

typedef void (* func3)(float *dst, const float *src1, const float *src2, size_t count);

//-----------------------------------------------------------------------------
PTEST_BEGIN("dsp.pmath", op3, 5, 1000)

    void call(const char *label, float *dst, const float *src1, const float *src2, size_t count, func3 func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(dst, src1, src2, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *dst      = alloc_aligned<float>(data, buf_size * 3, 64);
        float *src1     = &dst[buf_size];
        float *src2     = &src1[buf_size];

        randomize_sign(src1, buf_size);
        randomize_sign(src2, buf_size);

        #define CALL(func) \
            call(#func, dst, src1, src2, count, func)

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(native::add3);
            IF_ARCH_X86(CALL(sse::add3));
            IF_ARCH_X86(CALL(avx::add3));
            IF_ARCH_ARM(CALL(neon_d32::add3));
            IF_ARCH_AARCH64(CALL(asimd::add3));
            PTEST_SEPARATOR;

            CALL(native::sub3);
            IF_ARCH_X86(CALL(sse::sub3));
            IF_ARCH_X86(CALL(avx::sub3));
            IF_ARCH_ARM(CALL(neon_d32::sub3));
            IF_ARCH_AARCH64(CALL(asimd::sub3));
            PTEST_SEPARATOR;

            CALL(native::mul3);
            IF_ARCH_X86(CALL(sse::mul3));
            IF_ARCH_X86(CALL(avx::mul3));
            IF_ARCH_ARM(CALL(neon_d32::mul3));
            IF_ARCH_AARCH64(CALL(asimd::mul3));
            PTEST_SEPARATOR;

            CALL(native::div3);
            IF_ARCH_X86(CALL(sse::div3));
            IF_ARCH_X86(CALL(avx::div3));
            IF_ARCH_ARM(CALL(neon_d32::div3));
            IF_ARCH_AARCH64(CALL(asimd::div3));
            PTEST_SEPARATOR;

            CALL(native::mod3);
            IF_ARCH_X86(CALL(sse2::mod3));
            IF_ARCH_X86(CALL(avx::mod3));
            IF_ARCH_X86(CALL(avx::mod3_fma3));
            IF_ARCH_ARM(CALL(neon_d32::mod3));
            IF_ARCH_AARCH64(CALL(asimd::mod3));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }
PTEST_END


