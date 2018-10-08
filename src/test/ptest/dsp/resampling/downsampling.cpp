/*
 * downsampling.cpp
 *
 *  Created on: 23 авг. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>

#define RTEST_BUF_SIZE  0x1000

namespace native
{
    void downsample_2x(float *dst, const float *src, size_t count);
    void downsample_3x(float *dst, const float *src, size_t count);
    void downsample_4x(float *dst, const float *src, size_t count);
    void downsample_6x(float *dst, const float *src, size_t count);
    void downsample_8x(float *dst, const float *src, size_t count);
}

IF_ARCH_X86(
    namespace sse
    {
        void downsample_2x(float *dst, const float *src, size_t count);
        void downsample_3x(float *dst, const float *src, size_t count);
        void downsample_4x(float *dst, const float *src, size_t count);
        void downsample_6x(float *dst, const float *src, size_t count);
        void downsample_8x(float *dst, const float *src, size_t count);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void downsample_2x(float *dst, const float *src, size_t count);
        void downsample_3x(float *dst, const float *src, size_t count);
        void downsample_4x(float *dst, const float *src, size_t count);
        void downsample_6x(float *dst, const float *src, size_t count);
        void downsample_8x(float *dst, const float *src, size_t count);
    }
)

typedef void (* downsample_t)(float *dst, const float *src, size_t count);

//-----------------------------------------------------------------------------
// Performance test for lanczos resampling
PTEST_BEGIN("dsp.resampling", downsampling, 5, 1000)

    void call(float *out, const float *in, size_t count, size_t times, const char *text, downsample_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        printf("Testing %s downsampling for %d -> %d samples ...\n", text, int(count * times), int(count));

        PTEST_LOOP(text,
            func(out, in, count);
        );
    }

    PTEST_MAIN
    {
        float *out          = new float[RTEST_BUF_SIZE];
        float *in           = new float[RTEST_BUF_SIZE*8];

        // Prepare data
        for (size_t i=0; i<RTEST_BUF_SIZE; ++i)
            in[i]               = (i % 1) ? 1.0f : -1.0f;

        // Do tests
        call(out, in, RTEST_BUF_SIZE, 2, "native:downsample_2x", native::downsample_2x);
        IF_ARCH_X86(call(out, in, RTEST_BUF_SIZE, 2, "sse:downsample_2x", sse::downsample_2x));
        IF_ARCH_ARM(call(out, in, RTEST_BUF_SIZE, 2, "neon_d32:downsample_2x", neon_d32::downsample_2x));
        PTEST_SEPARATOR;

        call(out, in, RTEST_BUF_SIZE, 3, "native:downsample_3x", native::downsample_3x);
        IF_ARCH_X86(call(out, in, RTEST_BUF_SIZE, 3, "sse:downsample_3x", sse::downsample_3x));
        IF_ARCH_ARM(call(out, in, RTEST_BUF_SIZE, 3, "neon_d32:downsample_3x", neon_d32::downsample_3x));
        PTEST_SEPARATOR;

        call(out, in, RTEST_BUF_SIZE, 4, "native:downsample_4x", native::downsample_4x);
        IF_ARCH_X86(call(out, in, RTEST_BUF_SIZE, 4, "sse:downsample_4x", sse::downsample_4x));
        IF_ARCH_ARM(call(out, in, RTEST_BUF_SIZE, 4, "neon_d32:downsample_4x", neon_d32::downsample_4x));
        PTEST_SEPARATOR;

        call(out, in, RTEST_BUF_SIZE, 6, "native:downsample_6x", native::downsample_6x);
        IF_ARCH_X86(call(out, in, RTEST_BUF_SIZE, 6, "sse:downsample_6x", sse::downsample_6x));
        IF_ARCH_ARM(call(out, in, RTEST_BUF_SIZE, 6, "neon_d32:downsample_6x", neon_d32::downsample_6x));
        PTEST_SEPARATOR;

        call(out, in, RTEST_BUF_SIZE, 8, "native:downsample_8x", native::downsample_8x);
        IF_ARCH_X86(call(out, in, RTEST_BUF_SIZE, 8, "sse:downsample_8x", sse::downsample_8x));
        IF_ARCH_ARM(call(out, in, RTEST_BUF_SIZE, 8, "neon_d32:downsample_8x", neon_d32::downsample_8x));
        PTEST_SEPARATOR;

        delete [] out;
        delete [] in;
    }

PTEST_END


