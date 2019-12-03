/*
 * mix.cpp
 *
 *  Created on: 2 дек. 2019 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>
#include <test/helpers.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace native
{
    void mix2(float *dst, const float *src, float k1, float k2, size_t count);
    void mix3(float *dst, const float *src1, const float *src2, float k1, float k2, float k3, size_t count);
    void mix4(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, float k4, size_t count);

    void mix_copy2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);
    void mix_copy3(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, size_t count);
    void mix_copy4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count);

    void mix_add2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);
    void mix_add3(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, size_t count);
    void mix_add4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void mix2(float *dst, const float *src, float k1, float k2, size_t count);
        void mix3(float *dst, const float *src1, const float *src2, float k1, float k2, float k3, size_t count);
        void mix4(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, float k4, size_t count);

        void mix_copy2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);
        void mix_copy3(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, size_t count);
        void mix_copy4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count);

        void mix_add2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);
        void mix_add3(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, size_t count);
        void mix_add4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count);
    }

    namespace avx
    {
        void mix2(float *dst, const float *src, float k1, float k2, size_t count);
        void mix3(float *dst, const float *src1, const float *src2, float k1, float k2, float k3, size_t count);
        void mix4(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, float k4, size_t count);

        void mix_copy2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);
        void mix_copy3(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, size_t count);
        void mix_copy4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count);

        void mix_add2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);
        void mix_add3(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, size_t count);
        void mix_add4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void mix2(float *dst, const float *src, float k1, float k2, size_t count);
        void mix3(float *dst, const float *src1, const float *src2, float k1, float k2, float k3, size_t count);
        void mix4(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, float k4, size_t count);

        void mix_copy2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);
        void mix_copy3(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, size_t count);
        void mix_copy4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count);

        void mix_add2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);
        void mix_add3(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, size_t count);
        void mix_add4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void mix2(float *dst, const float *src, float k1, float k2, size_t count);
        void mix3(float *dst, const float *src1, const float *src2, float k1, float k2, float k3, size_t count);
        void mix4(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, float k4, size_t count);

        void mix_copy2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);
        void mix_copy3(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, size_t count);
        void mix_copy4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count);

        void mix_add2(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);
        void mix_add3(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, size_t count);
        void mix_add4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count);
    }
)

typedef void (* mix2_t)(float *dst, const float *src, float k1, float k2, size_t count);
typedef void (* mix3_t)(float *dst, const float *src1, const float *src2, float k1, float k2, float k3, size_t count);
typedef void (* mix4_t)(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, float k4, size_t count);

typedef void (* mix_dst2_t)(float *dst, const float *src1, const float *src2, float k1, float k2, size_t count);
typedef void (* mix_dst3_t)(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, size_t count);
typedef void (* mix_dst4_t)(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count);

PTEST_BEGIN("dsp", mix, 5, 1000)

void call(const char *label, float **src, mix2_t func, size_t count)
{
    if (!PTEST_SUPPORTED(func))
        return;

    char buf[80];
    sprintf(buf, "%s x %d", label, int(count));
    printf("Testing %s numbers...\n", buf);

    PTEST_LOOP(buf,
        func(src[0], src[1], 1.0f, 1.1f, count);
        func(src[2], src[3], 1.2f, 1.3f, count);
        func(src[4], src[5], 1.4f, 1.5f, count);
        func(src[6], src[7], 1.6f, 1.7f, count);
        func(src[8], src[9], 1.8f, 1.9f, count);
        func(src[10], src[11], 2.1f, 2.2f, count);
    );
}

void call(const char *label, float **src, mix3_t func, size_t count)
{
    if (!PTEST_SUPPORTED(func))
        return;

    char buf[80];
    sprintf(buf, "%s x %d", label, int(count));
    printf("Testing %s numbers...\n", buf);

    PTEST_LOOP(buf,
        func(src[0], src[1], src[2], 1.0f, 1.1f, 1.2f, count);
        func(src[3], src[4], src[5], 1.3f, 1.4f, 1.5f, count);
        func(src[6], src[7], src[8], 1.6f, 1.7f, 1.8f, count);
        func(src[9], src[10], src[11], 1.9f, 2.1f, 2.2f, count);
    );
}

void call(const char *label, float **src, mix4_t func, size_t count)
{
    if (!PTEST_SUPPORTED(func))
        return;

    char buf[80];
    sprintf(buf, "%s x %d", label, int(count));
    printf("Testing %s numbers...\n", buf);

    PTEST_LOOP(buf,
        func(src[0], src[1], src[2], src[3], 1.0f, 1.1f, 1.2f, 1.3f, count);
        func(src[4], src[5], src[6], src[7], 1.4f, 1.5f, 1.6f, 1.7f, count);
        func(src[8], src[9], src[10], src[11], 1.8f, 1.9f, 2.1f, 2.2f, count);
    );
}

void call(const char *label, float **src, mix_dst2_t func, size_t count)
{
    if (!PTEST_SUPPORTED(func))
        return;

    char buf[80];
    sprintf(buf, "%s x %d", label, int(count));
    printf("Testing %s numbers...\n", buf);

    PTEST_LOOP(buf,
        func(src[0], src[0], src[1], 1.0f, 1.1f, count);
        func(src[2], src[2], src[3], 1.2f, 1.3f, count);
        func(src[4], src[4], src[5], 1.4f, 1.5f, count);
        func(src[6], src[6], src[7], 1.6f, 1.7f, count);
        func(src[8], src[8], src[9], 1.8f, 1.9f, count);
        func(src[10], src[10], src[11], 2.1f, 2.2f, count);
    );
}

void call(const char *label, float **src, mix_dst3_t func, size_t count)
{
    if (!PTEST_SUPPORTED(func))
        return;

    char buf[80];
    sprintf(buf, "%s x %d", label, int(count));
    printf("Testing %s numbers...\n", buf);

    PTEST_LOOP(buf,
        func(src[0], src[0], src[1], src[2], 1.0f, 1.1f, 1.2f, count);
        func(src[3], src[3], src[4], src[5], 1.3f, 1.4f, 1.5f, count);
        func(src[6], src[6], src[7], src[8], 1.6f, 1.7f, 1.8f, count);
        func(src[9], src[9], src[10], src[11], 1.9f, 2.1f, 2.2f, count);
    );
}

void call(const char *label, float **src, mix_dst4_t func, size_t count)
{
    if (!PTEST_SUPPORTED(func))
        return;

    char buf[80];
    sprintf(buf, "%s x %d", label, int(count));
    printf("Testing %s numbers...\n", buf);

    PTEST_LOOP(buf,
        func(src[0], src[0], src[1], src[2], src[3], 1.0f, 1.1f, 1.2f, 1.3f, count);
        func(src[4], src[4], src[5], src[6], src[7], 1.4f, 1.5f, 1.6f, 1.7f, count);
        func(src[8], src[8], src[9], src[10], src[11], 1.8f, 1.9f, 2.1f, 2.2f, count);
    );
}

PTEST_MAIN
{
    size_t buf_size = 1 << MAX_RANK;
    uint8_t *data   = NULL;
    float *src      = alloc_aligned<float>(data, buf_size*12*2, 64);
    float *buf[12];

    for (size_t i=0; i < 12; ++i)
    {
        buf[i]          = src;
        src            += buf_size;
    }
    randomize(src, buf_size*12, -10.0f, 10.0f);

    #define CALL(func) \
        dsp::copy(buf[0], src, buf_size*12); \
        call(#func, buf, func, count)

    for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
    {
        size_t count = 1 << i;

        CALL(native::mix2);
        CALL(native::mix3);
        CALL(native::mix4);
        IF_ARCH_X86(CALL(sse::mix2));
        IF_ARCH_X86(CALL(sse::mix3));
        IF_ARCH_X86(CALL(sse::mix4));
        IF_ARCH_X86(CALL(avx::mix2));
        IF_ARCH_X86(CALL(avx::mix3));
        IF_ARCH_X86(CALL(avx::mix4));
        IF_ARCH_ARM(CALL(neon_d32::mix2));
        IF_ARCH_ARM(CALL(neon_d32::mix3));
        IF_ARCH_ARM(CALL(neon_d32::mix4));
        IF_ARCH_AARCH64(CALL(asimd::mix2));
        IF_ARCH_AARCH64(CALL(asimd::mix3));
        IF_ARCH_AARCH64(CALL(asimd::mix4));
        PTEST_SEPARATOR;

        CALL(native::mix_copy2);
        CALL(native::mix_copy3);
        CALL(native::mix_copy4);
        IF_ARCH_X86(CALL(sse::mix_copy2));
        IF_ARCH_X86(CALL(sse::mix_copy3));
        IF_ARCH_X86(CALL(sse::mix_copy4));
        IF_ARCH_X86(CALL(avx::mix_copy2));
        IF_ARCH_X86(CALL(avx::mix_copy3));
        IF_ARCH_X86(CALL(avx::mix_copy4));
        IF_ARCH_ARM(CALL(neon_d32::mix_copy2));
        IF_ARCH_ARM(CALL(neon_d32::mix_copy3));
        IF_ARCH_ARM(CALL(neon_d32::mix_copy4));
        IF_ARCH_AARCH64(CALL(asimd::mix_copy2));
        IF_ARCH_AARCH64(CALL(asimd::mix_copy3));
        IF_ARCH_AARCH64(CALL(asimd::mix_copy4));
        PTEST_SEPARATOR;

        CALL(native::mix_add2);
        CALL(native::mix_add3);
        CALL(native::mix_add4);
        IF_ARCH_X86(CALL(sse::mix_add2));
        IF_ARCH_X86(CALL(sse::mix_add3));
        IF_ARCH_X86(CALL(sse::mix_add4));
        IF_ARCH_X86(CALL(avx::mix_add2));
        IF_ARCH_X86(CALL(avx::mix_add3));
        IF_ARCH_X86(CALL(avx::mix_add4));
        IF_ARCH_ARM(CALL(neon_d32::mix_add2));
        IF_ARCH_ARM(CALL(neon_d32::mix_add3));
        IF_ARCH_ARM(CALL(neon_d32::mix_add4));
        IF_ARCH_AARCH64(CALL(asimd::mix_add2));
        IF_ARCH_AARCH64(CALL(asimd::mix_add3));
        IF_ARCH_AARCH64(CALL(asimd::mix_add4));
        PTEST_SEPARATOR2;
    }

    free_aligned(data);
}

PTEST_END

