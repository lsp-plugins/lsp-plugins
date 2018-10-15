/*
 * static.cpp
 *
 *  Created on: 21 авг. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>

#define FTEST_BUF_SIZE 0x200

namespace native
{
    void biquad_process_x1(float *dst, const float *src, size_t count, biquad_t *f);
    void biquad_process_x2(float *dst, const float *src, size_t count, biquad_t *f);
    void biquad_process_x4(float *dst, const float *src, size_t count, biquad_t *f);
    void biquad_process_x8(float *dst, const float *src, size_t count, biquad_t *f);
}

IF_ARCH_X86(
    namespace sse
    {
        void biquad_process_x1(float *dst, const float *src, size_t count, biquad_t *f);
        void biquad_process_x2(float *dst, const float *src, size_t count, biquad_t *f);
        void biquad_process_x4(float *dst, const float *src, size_t count, biquad_t *f);
        void biquad_process_x8(float *dst, const float *src, size_t count, biquad_t *f);
    }

    IF_ARCH_X86_64(
        namespace sse3
        {
            void x64_biquad_process_x2(float *dst, const float *src, size_t count, biquad_t *f);
            void x64_biquad_process_x8(float *dst, const float *src, size_t count, biquad_t *f);
        }

        namespace avx
        {
            void x64_biquad_process_x8(float *dst, const float *src, size_t count, biquad_t *f);
            void x64_biquad_process_x8_fma3(float *dst, const float *src, size_t count, biquad_t *f);
        }
    )
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void biquad_process_x1(float *dst, const float *src, size_t count, biquad_t *f);
        void biquad_process_x2(float *dst, const float *src, size_t count, biquad_t *f);
        void biquad_process_x4(float *dst, const float *src, size_t count, biquad_t *f);
        void biquad_process_x8(float *dst, const float *src, size_t count, biquad_t *f);
    }
)

typedef void (* biquad_process_t)(float *dst, const float *src, size_t count, biquad_t *f);

//-----------------------------------------------------------------------------
// Performance test for static biquad processing
PTEST_BEGIN("dsp.filters", static, 30, 10000)

    void process_8x1(const char *text, float *out, const float *in, size_t count, biquad_process_t process)
    {
        if (!PTEST_SUPPORTED(process))
            return;

        printf("Testing %s static filters on input buffer of %d samples ...\n", text, int(count));

        biquad_t f __lsp_aligned64;

        f.x1.a[0]      = 1.0f;
        f.x1.a[1]      = 1.0f;
        f.x1.a[2]      = 0.0f;
        f.x1.a[3]      = 0.0f;
        f.x1.b[0]      = 0.0f;
        f.x1.b[1]      = 0.0f;
        f.x1.b[2]      = 0.0f;
        f.x1.b[3]      = 0.0f;

        PTEST_LOOP(text,
            process(out, in, count, &f);
            process(out, out, count, &f);
            process(out, out, count, &f);
            process(out, out, count, &f);
            process(out, out, count, &f);
            process(out, out, count, &f);
            process(out, out, count, &f);
            process(out, out, count, &f);
        );
    }

    void process_4x2(const char *text, float *out, const float *in, size_t count, biquad_process_t process)
    {
        if (!PTEST_SUPPORTED(process))
            return;
        printf("Testing %s static filters on input buffer of %d samples ...\n", text, int(count));

        biquad_t f __lsp_aligned64;

        // Filter 1
        f.x2.a[0]      = 1.0f;
        f.x2.a[1]      = 1.0f;
        f.x2.a[2]      = 0.0f;
        f.x2.a[3]      = 0.0f;
        f.x2.b[0]      = 0.0f;
        f.x2.b[1]      = 0.0f;
        f.x2.b[2]      = 0.0f;
        f.x2.b[3]      = 0.0f;

        // Filter 2
        f.x2.a[4]      = 1.0f;
        f.x2.a[5]      = 1.0f;
        f.x2.a[6]      = 0.0f;
        f.x2.a[7]      = 0.0f;
        f.x2.b[4]      = 0.0f;
        f.x2.b[5]      = 0.0f;
        f.x2.b[6]      = 0.0f;
        f.x2.b[7]      = 0.0f;

        for (size_t i=0; i<8; ++i)
            f.d[i]          = 0.0f;

        PTEST_LOOP(text,
            process(out, in, count, &f);
            process(out, out, count, &f);
            process(out, out, count, &f);
            process(out, out, count, &f);
        );
    }

    void process_2x4(const char *text, float *out, const float *in, size_t count, biquad_process_t process)
    {
        if (!PTEST_SUPPORTED(process))
            return;
        printf("Testing %s static filters on input buffer of %d samples ...\n", text, int(count));

        biquad_t f __lsp_aligned64;

        // Filters x 4
        f.x4.a0[0]     = 1.0f;
        f.x4.a0[1]     = 1.0f;
        f.x4.a0[2]     = 1.0f;
        f.x4.a0[3]     = 1.0f;

        f.x4.a1[0]     = 0.0f;
        f.x4.a1[1]     = 0.0f;
        f.x4.a1[2]     = 0.0f;
        f.x4.a1[3]     = 0.0f;

        f.x4.a2[0]     = 0.0f;
        f.x4.a2[1]     = 0.0f;
        f.x4.a2[2]     = 0.0f;
        f.x4.a2[3]     = 0.0f;

        f.x4.b1[0]     = 0.0f;
        f.x4.b1[1]     = 0.0f;
        f.x4.b1[2]     = 0.0f;
        f.x4.b1[3]     = 0.0f;

        f.x4.b2[0]     = 0.0f;
        f.x4.b2[1]     = 0.0f;
        f.x4.b2[2]     = 0.0f;
        f.x4.b2[3]     = 0.0f;

        for (size_t i=0; i<8; ++i)
            f.d[i]          = 0.0f;

        PTEST_LOOP(text,
            process(out, in, count, &f);
            process(out, out, count, &f);
        );
    }

    void process_1x8(const char *text, float *out, const float *in, size_t count, biquad_process_t process)
    {
        if (!PTEST_SUPPORTED(process))
            return;
        printf("Testing %s static filters on input buffer of %d samples ...\n", text, int(count));

        biquad_t f __lsp_aligned64;
        bzero(&f, sizeof(biquad_t));

        // Filters x 8
        f.x8.a0[0]     = 1.0f;
        f.x8.a0[1]     = 1.0f;
        f.x8.a0[2]     = 1.0f;
        f.x8.a0[3]     = 1.0f;
        f.x8.a0[4]     = 1.0f;
        f.x8.a0[5]     = 1.0f;
        f.x8.a0[6]     = 1.0f;
        f.x8.a0[7]     = 1.0f;

        for (size_t i=0; i<8; ++i)
            f.d[i]          = 0.0f;

        PTEST_LOOP(text,
            process(out, in, count, &f);
        );
    }

    PTEST_MAIN
    {
        float *out          = new float[FTEST_BUF_SIZE];
        float *in           = new float[FTEST_BUF_SIZE];

        for (size_t i=0; i<FTEST_BUF_SIZE; ++i)
        {
            in[i]               = (i % 1) ? 1.0f : -1.0f;
            out[i]              = 0.0f;
        }

        process_8x1("native::biquad_process_x1 x8", out, in, FTEST_BUF_SIZE, native::biquad_process_x1);
        IF_ARCH_X86(process_8x1("sse::biquad_process_x1 x8", out, in, FTEST_BUF_SIZE, sse::biquad_process_x1));
        IF_ARCH_ARM(process_8x1("neon_d32::biquad_process_x1 x8", out, in, FTEST_BUF_SIZE, neon_d32::biquad_process_x1));
        PTEST_SEPARATOR;

        process_4x2("native::biquad_process_x2 x4", out, in, FTEST_BUF_SIZE, native::biquad_process_x2);
        IF_ARCH_X86(process_4x2("sse::biquad_process_x2 x4", out, in, FTEST_BUF_SIZE, sse::biquad_process_x2));
        IF_ARCH_X86_64(process_4x2("sse3::x64_biquad_process_x2 x4", out, in, FTEST_BUF_SIZE, sse3::x64_biquad_process_x2));
        IF_ARCH_ARM(process_4x2("neon_d32::biquad_process_x2 x4", out, in, FTEST_BUF_SIZE, neon_d32::biquad_process_x2));
        PTEST_SEPARATOR;

        process_2x4("native::biquad_process_x4 x2", out, in, FTEST_BUF_SIZE, native::biquad_process_x4);
        IF_ARCH_X86(process_2x4("sse::biquad_process_x4 x2", out, in, FTEST_BUF_SIZE, sse::biquad_process_x4));
        IF_ARCH_ARM(process_2x4("neon_d32::biquad_process_x4 x2", out, in, FTEST_BUF_SIZE, neon_d32::biquad_process_x4));
        PTEST_SEPARATOR;

        process_1x8("native::biquad_process_x8 x1", out, in, FTEST_BUF_SIZE, native::biquad_process_x8);
        IF_ARCH_X86(process_1x8("sse::biquad_process_x8 x1", out, in, FTEST_BUF_SIZE, sse::biquad_process_x8));
        IF_ARCH_X86_64(process_1x8("sse3::x64_biquad_process_x8 x1", out, in, FTEST_BUF_SIZE, sse3::x64_biquad_process_x8));
        IF_ARCH_X86_64(process_1x8("avx::x64_biquad_process_x8 x1", out, in, FTEST_BUF_SIZE, avx::x64_biquad_process_x8));
        IF_ARCH_X86_64(process_1x8("avx::x64_biquad_process_x8_fma3 x1", out, in, FTEST_BUF_SIZE, avx::x64_biquad_process_x8_fma3));
        IF_ARCH_ARM(process_1x8("neon_d32::biquad_process_x8 x1", out, in, FTEST_BUF_SIZE, neon_d32::biquad_process_x8));
        PTEST_SEPARATOR;

        delete [] out;
        delete [] in;
    }

PTEST_END

