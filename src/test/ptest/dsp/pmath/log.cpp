/*
 * log.cpp
 *
 *  Created on: 11 дек. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>
#include <core/sugar.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace native
{
    void logb1(float *dst, size_t count);
    void logb2(float *dst, const float *src, size_t count);
    void loge1(float *dst, size_t count);
    void loge2(float *dst, const float *src, size_t count);
    void logd1(float *dst, size_t count);
    void logd2(float *dst, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse2
    {
        void logb1(float *dst, size_t count);
        void logb2(float *dst, const float *src, size_t count);
        void loge1(float *dst, size_t count);
        void loge2(float *dst, const float *src, size_t count);
        void logd1(float *dst, size_t count);
        void logd2(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_X86_64(
    namespace avx2
    {
        void x64_logb1(float *dst, size_t count);
        void x64_logb2(float *dst, const float *src, size_t count);
        void x64_loge1(float *dst, size_t count);
        void x64_loge2(float *dst, const float *src, size_t count);
        void x64_logd1(float *dst, size_t count);
        void x64_logd2(float *dst, const float *src, size_t count);

        void x64_logb1_fma3(float *dst, size_t count);
        void x64_logb2_fma3(float *dst, const float *src, size_t count);
        void x64_loge1_fma3(float *dst, size_t count);
        void x64_loge2_fma3(float *dst, const float *src, size_t count);
        void x64_logd1_fma3(float *dst, size_t count);
        void x64_logd2_fma3(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void logb1(float *dst, size_t count);
        void logb2(float *dst, const float *src, size_t count);
        void loge1(float *dst, size_t count);
        void loge2(float *dst, const float *src, size_t count);
        void logd1(float *dst, size_t count);
        void logd2(float *dst, const float *src, size_t count);
    }
)

typedef void (* log1_t)(float *dst, size_t count);
typedef void (* log2_t)(float *dst, const float *src, size_t count);

//-----------------------------------------------------------------------------
// Performance test
PTEST_BEGIN("dsp.pmath", log, 5, 1000)

    void call(const char *label, float *dst, const float *src, size_t count, log1_t func)
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

    void call(const char *label, float *dst, const float *src, size_t count, log2_t func)
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

            CALL("native::logb1", dst, src, count, native::logb1);
            IF_ARCH_X86(CALL("sse2::logb1", dst, src, count, sse2::logb1));
            IF_ARCH_X86_64(CALL("avx2::x64_logb1", dst, src, count, avx2::x64_logb1));
            IF_ARCH_X86_64(CALL("avx2::x64_logb1_fma3", dst, src, count, avx2::x64_logb1_fma3));
            IF_ARCH_ARM(CALL("neon_d32::logb1", dst, src, count, neon_d32::logb1));
            PTEST_SEPARATOR;

            CALL("native::logb2", dst, src, count, native::logb2);
            IF_ARCH_X86(CALL("sse2::logb2", dst, src, count, sse2::logb2));
            IF_ARCH_X86_64(CALL("avx2::x64_logb2", dst, src, count, avx2::x64_logb2));
            IF_ARCH_X86_64(CALL("avx2::x64_logb2_fma3", dst, src, count, avx2::x64_logb2_fma3));
            IF_ARCH_ARM(CALL("neon_d32::logb2", dst, src, count, neon_d32::logb2));
            PTEST_SEPARATOR;

            CALL("native::loge1", dst, src, count, native::loge1);
            IF_ARCH_X86(CALL("sse2::loge1", dst, src, count, sse2::loge1));
            IF_ARCH_X86_64(CALL("avx2::x64_loge1", dst, src, count, avx2::x64_loge1));
            IF_ARCH_X86_64(CALL("avx2::x64_loge1_fma3", dst, src, count, avx2::x64_loge1_fma3));
            IF_ARCH_ARM(CALL("neon_d32::loge1", dst, src, count, neon_d32::loge1));
            PTEST_SEPARATOR;

            CALL("native::loge2", dst, src, count, native::loge2);
            IF_ARCH_X86(CALL("sse2::loge2", dst, src, count, sse2::loge2));
            IF_ARCH_X86_64(CALL("avx2::x64_loge2", dst, src, count, avx2::x64_loge2));
            IF_ARCH_X86_64(CALL("avx2::x64_loge2_fma3", dst, src, count, avx2::x64_loge2_fma3));
            IF_ARCH_ARM(CALL("neon_d32::loge2", dst, src, count, neon_d32::loge2));
            PTEST_SEPARATOR;

            CALL("native::logd1", dst, src, count, native::logd1);
            IF_ARCH_X86(CALL("sse2::logd1", dst, src, count, sse2::logd1));
            IF_ARCH_X86_64(CALL("avx2::x64_logd1", dst, src, count, avx2::x64_logd1));
            IF_ARCH_X86_64(CALL("avx2::x64_logd1_fma3", dst, src, count, avx2::x64_logd1_fma3));
            IF_ARCH_ARM(CALL("neon_d32::logd1", dst, src, count, neon_d32::logd1));
            PTEST_SEPARATOR;

            CALL("native::logd2", dst, src, count, native::logd2);
            IF_ARCH_X86(CALL("sse2::logd2", dst, src, count, sse2::logd2));
            IF_ARCH_X86_64(CALL("avx2::x64_logd2", dst, src, count, avx2::x64_logd2));
            IF_ARCH_X86_64(CALL("avx2::x64_logd2_fma3", dst, src, count, avx2::x64_logd2_fma3));
            IF_ARCH_ARM(CALL("neon_d32::logd2", dst, src, count, neon_d32::logd2));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }
PTEST_END



