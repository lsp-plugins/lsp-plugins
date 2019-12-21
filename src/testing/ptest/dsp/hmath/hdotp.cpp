/*
 * hdotp.cpp
 *
 *  Created on: 29 нояб. 2019 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/helpers.h>
#include <test/ptest.h>
#include <core/sugar.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace native
{
    float h_dotp(const float *a, const float *b, size_t count);
    float h_sqr_dotp(const float *a, const float *b, size_t count);
    float h_abs_dotp(const float *a, const float *b, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        float h_dotp(const float *a, const float *b, size_t count);
        float h_sqr_dotp(const float *a, const float *b, size_t count);
        float h_abs_dotp(const float *a, const float *b, size_t count);
    }

    namespace avx
    {
        float h_dotp(const float *a, const float *b, size_t count);
        float h_sqr_dotp(const float *a, const float *b, size_t count);
        float h_abs_dotp(const float *a, const float *b, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        float h_dotp(const float *a, const float *b, size_t count);
        float h_sqr_dotp(const float *a, const float *b, size_t count);
        float h_abs_dotp(const float *a, const float *b, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        float h_dotp(const float *a, const float *b, size_t count);
        float h_sqr_dotp(const float *a, const float *b, size_t count);
        float h_abs_dotp(const float *a, const float *b, size_t count);
    }
)

typedef float (* h_dotp_t)(const float *a, const float *b, size_t count);

PTEST_BEGIN("dsp.hmath", hdotp, 5, 10000)

void call(const char *label, float *a, float *b, size_t count, h_dotp_t func)
{
    if (!PTEST_SUPPORTED(func))
        return;

    char buf[80];
    sprintf(buf, "%s x %d", label, int(count));
    printf("Testing %s numbers...\n", buf);

    PTEST_LOOP(buf,
        func(a, b, count);
    );
}

PTEST_MAIN
{
    size_t buf_size = 1 << MAX_RANK;
    uint8_t *data   = NULL;
    float *a        = alloc_aligned<float>(data, buf_size * 2, 64);
    float *b        = &a[buf_size];

    randomize_sign(a, buf_size * 2);

    #define CALL(func) \
        call(#func, a, b, count, func)

    for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
    {
        size_t count = 1 << i;

        CALL(native::h_dotp);
        IF_ARCH_X86(CALL(sse::h_dotp));
        IF_ARCH_X86(CALL(avx::h_dotp));
        IF_ARCH_ARM(CALL(neon_d32::h_dotp));
        IF_ARCH_AARCH64(CALL(asimd::h_dotp));
        PTEST_SEPARATOR;

        CALL(native::h_sqr_dotp);
        IF_ARCH_X86(CALL(sse::h_sqr_dotp));
        IF_ARCH_X86(CALL(avx::h_sqr_dotp));
        IF_ARCH_ARM(CALL(neon_d32::h_sqr_dotp));
        IF_ARCH_AARCH64(CALL(asimd::h_sqr_dotp));
        PTEST_SEPARATOR;

        CALL(native::h_abs_dotp);
        IF_ARCH_X86(CALL(sse::h_abs_dotp));
        IF_ARCH_X86(CALL(avx::h_abs_dotp));
        IF_ARCH_ARM(CALL(neon_d32::h_abs_dotp));
        IF_ARCH_AARCH64(CALL(asimd::h_abs_dotp));
        PTEST_SEPARATOR2;
    }

    free_aligned(data);
}

PTEST_END



