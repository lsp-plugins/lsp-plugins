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

        // Do tests
        call(out, in, RTEST_BUF_SIZE, 2, "native:2x2", native::lanczos_resample_2x2);
        IF_ARCH_X86(call(out, in, RTEST_BUF_SIZE, 2, "sse:2x2", sse::lanczos_resample_2x2));
        IF_ARCH_ARM(call(out, in, RTEST_BUF_SIZE, 2, "neon_d32:2x2", neon_d32::lanczos_resample_2x2));
        PTEST_SEPARATOR;

        call(out, in, RTEST_BUF_SIZE, 2, "native:2x3", native::lanczos_resample_2x3);
        IF_ARCH_X86(call(out, in, RTEST_BUF_SIZE, 2, "sse:2x3", sse::lanczos_resample_2x3));
        IF_ARCH_ARM(call(out, in, RTEST_BUF_SIZE, 2, "neon_d32:2x3", neon_d32::lanczos_resample_2x3));
        PTEST_SEPARATOR;

        call(out, in, RTEST_BUF_SIZE, 3, "native:3x2", native::lanczos_resample_3x2);
        IF_ARCH_X86(call(out, in, RTEST_BUF_SIZE, 3, "sse:3x2", sse::lanczos_resample_3x2));
        IF_ARCH_ARM(call(out, in, RTEST_BUF_SIZE, 3, "neon_d32:3x2", neon_d32::lanczos_resample_3x2));
        PTEST_SEPARATOR;

        call(out, in, RTEST_BUF_SIZE, 3, "native:3x3", native::lanczos_resample_3x3);
        IF_ARCH_X86(call(out, in, RTEST_BUF_SIZE, 3, "sse:3x3", sse::lanczos_resample_3x3));
        IF_ARCH_ARM(call(out, in, RTEST_BUF_SIZE, 3, "neon_d32:3x3", neon_d32::lanczos_resample_3x3));
        PTEST_SEPARATOR;

        call(out, in, RTEST_BUF_SIZE, 4, "native:4x2", native::lanczos_resample_4x2);
        IF_ARCH_X86(call(out, in, RTEST_BUF_SIZE, 4, "sse:4x2", sse::lanczos_resample_4x2));
        IF_ARCH_ARM(call(out, in, RTEST_BUF_SIZE, 4, "neon_d32:4x2", neon_d32::lanczos_resample_4x2));
        PTEST_SEPARATOR;

        call(out, in, RTEST_BUF_SIZE, 4, "native:4x3", native::lanczos_resample_4x3);
        IF_ARCH_X86(call(out, in, RTEST_BUF_SIZE, 4, "sse:4x3", sse::lanczos_resample_4x3));
        IF_ARCH_ARM(call(out, in, RTEST_BUF_SIZE, 4, "neon_d32:4x3", neon_d32::lanczos_resample_4x3));
        PTEST_SEPARATOR;

        call(out, in, RTEST_BUF_SIZE, 6, "native:6x2", native::lanczos_resample_6x2);
        IF_ARCH_X86(call(out, in, RTEST_BUF_SIZE, 6, "sse:6x2", sse::lanczos_resample_6x2));
        IF_ARCH_ARM(call(out, in, RTEST_BUF_SIZE, 6, "neon_d32:6x2", neon_d32::lanczos_resample_6x2));
        PTEST_SEPARATOR;

        call(out, in, RTEST_BUF_SIZE, 6, "native:6x3", native::lanczos_resample_6x3);
        IF_ARCH_X86(call(out, in, RTEST_BUF_SIZE, 6, "sse:6x3", sse::lanczos_resample_6x3));
        IF_ARCH_ARM(call(out, in, RTEST_BUF_SIZE, 6, "neon_d32:6x3", neon_d32::lanczos_resample_6x3));
        PTEST_SEPARATOR;

        call(out, in, RTEST_BUF_SIZE, 8, "native:8x2", native::lanczos_resample_8x2);
        IF_ARCH_X86(call(out, in, RTEST_BUF_SIZE, 8, "sse:8x2", sse::lanczos_resample_8x2));
        IF_ARCH_ARM(call(out, in, RTEST_BUF_SIZE, 8, "neon_d32:8x2", neon_d32::lanczos_resample_8x2));
        PTEST_SEPARATOR;

        call(out, in, RTEST_BUF_SIZE, 8, "native:8x3", native::lanczos_resample_8x3);
        IF_ARCH_X86(call(out, in, RTEST_BUF_SIZE, 8, "sse:8x3", sse::lanczos_resample_8x3));
        IF_ARCH_ARM(call(out, in, RTEST_BUF_SIZE, 8, "neon_d32:8x3", neon_d32::lanczos_resample_8x3));
        PTEST_SEPARATOR;

        delete [] out;
        delete [] in;
    }

PTEST_END


