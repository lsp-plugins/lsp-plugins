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

    namespace sse3
    {
        void x64_biquad_process_x8(float *dst, const float *src, size_t count, biquad_t *f);
    }

    namespace avx
    {
        void biquad_process_x1(float *dst, const float *src, size_t count, biquad_t *f);
        void biquad_process_x1_fma3(float *dst, const float *src, size_t count, biquad_t *f);

        void biquad_process_x2(float *dst, const float *src, size_t count, biquad_t *f);
        void biquad_process_x2_fma3(float *dst, const float *src, size_t count, biquad_t *f);

        void biquad_process_x4(float *dst, const float *src, size_t count, biquad_t *f);
        void biquad_process_x4_fma3(float *dst, const float *src, size_t count, biquad_t *f);

        void x64_biquad_process_x8(float *dst, const float *src, size_t count, biquad_t *f);
        void biquad_process_x8_fma3(float *dst, const float *src, size_t count, biquad_t *f);
    }
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

IF_ARCH_AARCH64(
    namespace asimd
    {
        void biquad_process_x1(float *dst, const float *src, size_t count, biquad_t *f);
        void biquad_process_x2(float *dst, const float *src, size_t count, biquad_t *f);
        void biquad_process_x4(float *dst, const float *src, size_t count, biquad_t *f);
        void biquad_process_x8(float *dst, const float *src, size_t count, biquad_t *f);
    }
)

typedef void (* biquad_process_t)(float *dst, const float *src, size_t count, biquad_t *f);

static biquad_x1_t bq_normal = {
    1.0, 2.0, 1.0,
    -2.0, -1.0,
    0.0, 0.0, 0.0
};

//-----------------------------------------------------------------------------
// Performance test for static biquad processing
PTEST_BEGIN("dsp.filters", static, 10, 1000)

    void process_8x1(const char *text, float *out, const float *in, size_t count, biquad_process_t process)
    {
        if (!PTEST_SUPPORTED(process))
            return;

        printf("Testing %s static filters on input buffer of %d samples ...\n", text, int(count));

        biquad_t f __lsp_aligned64;

        f.x1 = bq_normal;

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

        // Filters x 2
        for (size_t i=0; i<2; ++i)
        {
            f.x2.b0[i]      = bq_normal.b0;
            f.x2.b1[i]      = bq_normal.b1;
            f.x2.b2[i]      = bq_normal.b2;
            f.x2.a1[i]      = bq_normal.a1;
            f.x2.a2[i]      = bq_normal.a2;
            f.x2.p[i]       = 0.0f;
        }

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
        for (size_t i=0; i<4; ++i)
        {
            f.x4.b0[i]     = bq_normal.b0;
            f.x4.b1[i]     = bq_normal.b1;
            f.x4.b2[i]     = bq_normal.b2;
            f.x4.a1[i]     = bq_normal.a1;
            f.x4.a2[i]     = bq_normal.a2;
        }

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
        // Filters x 8
        for (size_t i=0; i<8; ++i)
        {
            f.x8.b0[i]     = bq_normal.b0;
            f.x8.b1[i]     = bq_normal.b1;
            f.x8.b2[i]     = bq_normal.b2;
            f.x8.a1[i]     = bq_normal.a1;
            f.x8.a2[i]     = bq_normal.a2;
        }

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
        IF_ARCH_X86(process_8x1("avx::biquad_process_x1 x8", out, in, FTEST_BUF_SIZE, avx::biquad_process_x1));
        IF_ARCH_X86(process_8x1("avx::biquad_process_x1_fma3 x8", out, in, FTEST_BUF_SIZE, avx::biquad_process_x1_fma3));
        IF_ARCH_ARM(process_8x1("neon_d32::biquad_process_x1 x8", out, in, FTEST_BUF_SIZE, neon_d32::biquad_process_x1));
        IF_ARCH_AARCH64(process_8x1("asimd::biquad_process_x1 x8", out, in, FTEST_BUF_SIZE, asimd::biquad_process_x1));
        PTEST_SEPARATOR;

        process_4x2("native::biquad_process_x2 x4", out, in, FTEST_BUF_SIZE, native::biquad_process_x2);
        IF_ARCH_X86(process_4x2("sse::biquad_process_x2 x4", out, in, FTEST_BUF_SIZE, sse::biquad_process_x2));
        IF_ARCH_X86(process_4x2("avx::biquad_process_x2 x4", out, in, FTEST_BUF_SIZE, avx::biquad_process_x2));
        IF_ARCH_X86(process_4x2("avx::biquad_process_x2_fma3 x4", out, in, FTEST_BUF_SIZE, avx::biquad_process_x2_fma3));
        IF_ARCH_ARM(process_4x2("neon_d32::biquad_process_x2 x4", out, in, FTEST_BUF_SIZE, neon_d32::biquad_process_x2));
        IF_ARCH_AARCH64(process_4x2("asimd::biquad_process_x2 x4", out, in, FTEST_BUF_SIZE, asimd::biquad_process_x2));
        PTEST_SEPARATOR;

        process_2x4("native::biquad_process_x4 x2", out, in, FTEST_BUF_SIZE, native::biquad_process_x4);
        IF_ARCH_X86(process_2x4("sse::biquad_process_x4 x2", out, in, FTEST_BUF_SIZE, sse::biquad_process_x4));
        IF_ARCH_X86(process_2x4("avx::biquad_process_x4 x2", out, in, FTEST_BUF_SIZE, avx::biquad_process_x4));
        IF_ARCH_X86(process_2x4("avx::biquad_process_x4_fma3 x2", out, in, FTEST_BUF_SIZE, avx::biquad_process_x4_fma3));
        IF_ARCH_ARM(process_2x4("neon_d32::biquad_process_x4 x2", out, in, FTEST_BUF_SIZE, neon_d32::biquad_process_x4));
        IF_ARCH_AARCH64(process_2x4("asimd::biquad_process_x4 x2", out, in, FTEST_BUF_SIZE, asimd::biquad_process_x4));
        PTEST_SEPARATOR;

        process_1x8("native::biquad_process_x8 x1", out, in, FTEST_BUF_SIZE, native::biquad_process_x8);
        IF_ARCH_X86(process_1x8("sse::biquad_process_x8 x1", out, in, FTEST_BUF_SIZE, sse::biquad_process_x8));
        IF_ARCH_X86(process_1x8("sse3::x64_biquad_process_x8 x1", out, in, FTEST_BUF_SIZE, sse3::x64_biquad_process_x8));
        IF_ARCH_X86(process_1x8("avx::x64_biquad_process_x8 x1", out, in, FTEST_BUF_SIZE, avx::x64_biquad_process_x8));
        IF_ARCH_X86(process_1x8("avx::biquad_process_x8_fma3 x1", out, in, FTEST_BUF_SIZE, avx::biquad_process_x8_fma3));
        IF_ARCH_ARM(process_1x8("neon_d32::biquad_process_x8 x1", out, in, FTEST_BUF_SIZE, neon_d32::biquad_process_x8));
        IF_ARCH_AARCH64(process_1x8("asimd::biquad_process_x8 x1", out, in, FTEST_BUF_SIZE, asimd::biquad_process_x8));
        PTEST_SEPARATOR;

        delete [] out;
        delete [] in;
    }

PTEST_END

