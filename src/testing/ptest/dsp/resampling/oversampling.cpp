/*
 * oversampling.cpp
 *
 *  Created on: 21 авг. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>

#define RTEST_BUF_SIZE  0x1000

namespace native
{
    void lanczos_resample_2x2(float *dst, const float *src, size_t count);
    void lanczos_resample_2x3(float *dst, const float *src, size_t count);
    void lanczos_resample_3x2(float *dst, const float *src, size_t count);
    void lanczos_resample_3x3(float *dst, const float *src, size_t count);
    void lanczos_resample_4x2(float *dst, const float *src, size_t count);
    void lanczos_resample_4x3(float *dst, const float *src, size_t count);
    void lanczos_resample_6x2(float *dst, const float *src, size_t count);
    void lanczos_resample_6x3(float *dst, const float *src, size_t count);
    void lanczos_resample_8x2(float *dst, const float *src, size_t count);
    void lanczos_resample_8x3(float *dst, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void lanczos_resample_2x2(float *dst, const float *src, size_t count);
        void lanczos_resample_2x3(float *dst, const float *src, size_t count);
        void lanczos_resample_3x2(float *dst, const float *src, size_t count);
        void lanczos_resample_3x3(float *dst, const float *src, size_t count);
        void lanczos_resample_4x2(float *dst, const float *src, size_t count);
        void lanczos_resample_4x3(float *dst, const float *src, size_t count);
        void lanczos_resample_6x2(float *dst, const float *src, size_t count);
        void lanczos_resample_6x3(float *dst, const float *src, size_t count);
        void lanczos_resample_8x2(float *dst, const float *src, size_t count);
        void lanczos_resample_8x3(float *dst, const float *src, size_t count);
    }

    namespace avx
    {
        void lanczos_resample_2x2(float *dst, const float *src, size_t count);
        void lanczos_resample_2x3(float *dst, const float *src, size_t count);
        void lanczos_resample_3x2(float *dst, const float *src, size_t count);
        void lanczos_resample_3x3(float *dst, const float *src, size_t count);
        void lanczos_resample_4x2(float *dst, const float *src, size_t count);
        void lanczos_resample_4x3(float *dst, const float *src, size_t count);
        void lanczos_resample_6x2(float *dst, const float *src, size_t count);
        void lanczos_resample_6x3(float *dst, const float *src, size_t count);
        void lanczos_resample_8x2(float *dst, const float *src, size_t count);
        void lanczos_resample_8x3(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void lanczos_resample_2x2(float *dst, const float *src, size_t count);
        void lanczos_resample_2x3(float *dst, const float *src, size_t count);
        void lanczos_resample_3x2(float *dst, const float *src, size_t count);
        void lanczos_resample_3x3(float *dst, const float *src, size_t count);
        void lanczos_resample_4x2(float *dst, const float *src, size_t count);
        void lanczos_resample_4x3(float *dst, const float *src, size_t count);
        void lanczos_resample_6x2(float *dst, const float *src, size_t count);
        void lanczos_resample_6x3(float *dst, const float *src, size_t count);
        void lanczos_resample_8x2(float *dst, const float *src, size_t count);
        void lanczos_resample_8x3(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_AARCH64(
    namespace asimd
    {
        void lanczos_resample_2x2(float *dst, const float *src, size_t count);
        void lanczos_resample_2x3(float *dst, const float *src, size_t count);
        void lanczos_resample_3x2(float *dst, const float *src, size_t count);
        void lanczos_resample_3x3(float *dst, const float *src, size_t count);
        void lanczos_resample_4x2(float *dst, const float *src, size_t count);
        void lanczos_resample_4x3(float *dst, const float *src, size_t count);
        void lanczos_resample_6x2(float *dst, const float *src, size_t count);
        void lanczos_resample_6x3(float *dst, const float *src, size_t count);
        void lanczos_resample_8x2(float *dst, const float *src, size_t count);
        void lanczos_resample_8x3(float *dst, const float *src, size_t count);
    }
)

//-----------------------------------------------------------------------------
// Performance test for lanczos resampling
PTEST_BEGIN("dsp.resampling", oversampling, 5, 1000)

    void call(float *out, const float *in, size_t count, size_t times, const char *text, resampling_function_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        printf("Testing %s oversampling for %d -> %d samples ...\n", text, int(count), int(count * times));
        size_t zeros = count*times + RESAMPLING_RESERVED_SAMPLES;

        PTEST_LOOP(text,
            dsp::fill_zero(out, zeros);
            func(out, in, count);
        );
    }

    PTEST_MAIN
    {
        float *out          = new float[RTEST_BUF_SIZE*8 + RESAMPLING_RESERVED_SAMPLES];
        float *in           = new float[RTEST_BUF_SIZE];

        // Prepare data
        for (size_t i=0; i<RTEST_BUF_SIZE; ++i)
            in[i]               = (i % 1) ? 1.0f : -1.0f;
        dsp::fill_zero(out, RTEST_BUF_SIZE * 8 + RESAMPLING_RESERVED_SAMPLES);

        #define CALL(func, n) \
            call(out, in, RTEST_BUF_SIZE, n, #func, func)

        // Do tests
        CALL(native::lanczos_resample_2x2, 2);
        IF_ARCH_X86(CALL(sse::lanczos_resample_2x2, 2));
        IF_ARCH_X86(CALL(avx::lanczos_resample_2x2, 2));
        IF_ARCH_ARM(CALL(neon_d32::lanczos_resample_2x2, 2));
        IF_ARCH_AARCH64(CALL(asimd::lanczos_resample_2x2, 2));
        PTEST_SEPARATOR;

        CALL(native::lanczos_resample_2x3, 2);
        IF_ARCH_X86(CALL(sse::lanczos_resample_2x3, 2));
        IF_ARCH_X86(CALL(avx::lanczos_resample_2x3, 2));
        IF_ARCH_ARM(CALL(neon_d32::lanczos_resample_2x3, 2));
        IF_ARCH_AARCH64(CALL(asimd::lanczos_resample_2x3, 2));
        PTEST_SEPARATOR;

        CALL(native::lanczos_resample_3x2, 3);
        IF_ARCH_X86(CALL(sse::lanczos_resample_3x2, 3));
        IF_ARCH_X86(CALL(avx::lanczos_resample_3x2, 3));
        IF_ARCH_ARM(CALL(neon_d32::lanczos_resample_3x2, 3));
        IF_ARCH_AARCH64(CALL(asimd::lanczos_resample_3x2, 3));
        PTEST_SEPARATOR;

        CALL(native::lanczos_resample_3x3, 3);
        IF_ARCH_X86(CALL(sse::lanczos_resample_3x3, 3));
        IF_ARCH_X86(CALL(avx::lanczos_resample_3x3, 3));
        IF_ARCH_ARM(CALL(neon_d32::lanczos_resample_3x3, 3));
        IF_ARCH_AARCH64(CALL(asimd::lanczos_resample_3x3, 3));
        PTEST_SEPARATOR;

        CALL(native::lanczos_resample_4x2, 4);
        IF_ARCH_X86(CALL(sse::lanczos_resample_4x2, 4));
        IF_ARCH_X86(CALL(avx::lanczos_resample_4x2, 4));
        IF_ARCH_ARM(CALL(neon_d32::lanczos_resample_4x2, 4));
        IF_ARCH_AARCH64(CALL(asimd::lanczos_resample_4x2, 4));
        PTEST_SEPARATOR;

        CALL(native::lanczos_resample_4x3, 4);
        IF_ARCH_X86(CALL(sse::lanczos_resample_4x3, 4));
        IF_ARCH_X86(CALL(avx::lanczos_resample_4x3, 4));
        IF_ARCH_ARM(CALL(neon_d32::lanczos_resample_4x3, 4));
        IF_ARCH_AARCH64(CALL(asimd::lanczos_resample_4x3, 4));
        PTEST_SEPARATOR;

        CALL(native::lanczos_resample_6x2, 6);
        IF_ARCH_X86(CALL(sse::lanczos_resample_6x2, 6));
        IF_ARCH_X86(CALL(avx::lanczos_resample_6x2, 6));
        IF_ARCH_ARM(CALL(neon_d32::lanczos_resample_6x2, 6));
        IF_ARCH_AARCH64(CALL(asimd::lanczos_resample_6x2, 6));
        PTEST_SEPARATOR;

        CALL(native::lanczos_resample_6x3, 6);
        IF_ARCH_X86(CALL(sse::lanczos_resample_6x3, 6));
        IF_ARCH_X86(CALL(avx::lanczos_resample_6x3, 6));
        IF_ARCH_ARM(CALL(neon_d32::lanczos_resample_6x3, 6));
        IF_ARCH_AARCH64(CALL(asimd::lanczos_resample_6x3, 6));
        PTEST_SEPARATOR;

        CALL(native::lanczos_resample_8x2, 8);
        IF_ARCH_X86(CALL(sse::lanczos_resample_8x2, 8));
        IF_ARCH_X86(CALL(avx::lanczos_resample_8x2, 8));
        IF_ARCH_ARM(CALL(neon_d32::lanczos_resample_8x2, 8));
        IF_ARCH_AARCH64(CALL(asimd::lanczos_resample_8x2, 8));
        PTEST_SEPARATOR;

        CALL(native::lanczos_resample_8x3, 8);
        IF_ARCH_X86(CALL(sse::lanczos_resample_8x3, 8));
        IF_ARCH_X86(CALL(avx::lanczos_resample_8x3, 8));
        IF_ARCH_ARM(CALL(neon_d32::lanczos_resample_8x3, 8));
        IF_ARCH_AARCH64(CALL(asimd::lanczos_resample_8x3, 8));
        PTEST_SEPARATOR;

        delete [] out;
        delete [] in;
    }

PTEST_END


