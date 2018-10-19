/*
 * dynamic.cpp
 *
 *  Created on: 22 авг. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>
#include <core/sugar.h>

#define FTEST_BUF_SIZE 0x200

namespace native
{
    void dyn_biquad_process_x1(float *dst, const float *src, float *d, size_t count, const biquad_x1_t *f);
    void dyn_biquad_process_x2(float *dst, const float *src, float *d, size_t count, const biquad_x2_t *f);
    void dyn_biquad_process_x4(float *dst, const float *src, float *d, size_t count, const biquad_x4_t *f);
    void dyn_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const biquad_x8_t *f);
}

IF_ARCH_X86(
    namespace sse
    {
        void dyn_biquad_process_x1(float *dst, const float *src, float *d, size_t count, const biquad_x1_t *f);
        void dyn_biquad_process_x2(float *dst, const float *src, float *d, size_t count, const biquad_x2_t *f);
        void dyn_biquad_process_x4(float *dst, const float *src, float *d, size_t count, const biquad_x4_t *f);
        void dyn_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const biquad_x8_t *f);
    }

    IF_ARCH_X86_64(
        namespace sse3
        {
            void x64_dyn_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const biquad_x8_t *f);
        }

        namespace avx
        {
            void x64_dyn_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const biquad_x8_t *f);
            void x64_dyn_biquad_process_x8_fma3(float *dst, const float *src, float *d, size_t count, const biquad_x8_t *f);
        }
    )
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void dyn_biquad_process_x1(float *dst, const float *src, float *d, size_t count, const biquad_x1_t *f);
        void dyn_biquad_process_x2(float *dst, const float *src, float *d, size_t count, const biquad_x2_t *f);
        void dyn_biquad_process_x4(float *dst, const float *src, float *d, size_t count, const biquad_x4_t *f);
        void dyn_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const biquad_x8_t *f);
    }
)

typedef void (* dyn_biquad_process_x1_t)(float *dst, const float *src, float *d, size_t count, const biquad_x1_t *f);
typedef void (* dyn_biquad_process_x2_t)(float *dst, const float *src, float *d, size_t count, const biquad_x2_t *f);
typedef void (* dyn_biquad_process_x4_t)(float *dst, const float *src, float *d, size_t count, const biquad_x4_t *f);
typedef void (* dyn_biquad_process_x8_t)(float *dst, const float *src, float *d, size_t count, const biquad_x8_t *f);

static biquad_x1_t bq_normal = {
    { 1.0, 1.0, 2.0, 1.0 },
    {-2.0, -1.0, 0.0, 0.0 }
};

//-----------------------------------------------------------------------------
// Performance test for dynamic biquad processing
PTEST_BEGIN("dsp.filters", dynamic, 30, 10000)

    void process_8x1(const char *text, float *out, const float *in, size_t count, dyn_biquad_process_x1_t process)
    {
        if (!PTEST_SUPPORTED(process))
            return;

        printf("Testing %s dynamic filters on input buffer of %d samples ...\n", text, int(count));

        float d[16] __lsp_aligned64;
        for (size_t i=0; i<16; ++i)
            d[i]     = 0.0;

        void *ptr = NULL;
        biquad_x1_t *f = alloc_aligned<biquad_x1_t>(ptr, count, 64);
        for (size_t i=0; i<count; ++i)
            f[i]        = bq_normal;

        PTEST_LOOP(text,
            process(out, in, &d[0], count, f);
            process(out, out, &d[2], count, f);
            process(out, out, &d[4], count, f);
            process(out, out, &d[6], count, f);
            process(out, out, &d[8], count, f);
            process(out, out, &d[10], count, f);
            process(out, out, &d[12], count, f);
            process(out, out, &d[14], count, f);
        );

        free_aligned(ptr);
    }

    void process_4x2(const char *text, float *out, const float *in, size_t count, dyn_biquad_process_x2_t process)
    {
        if (!PTEST_SUPPORTED(process))
            return;
        printf("Testing %s dynamic filters on input buffer of %d samples ...\n", text, int(count));

        float d[16] __lsp_aligned64;
        for (size_t i=0; i<16; ++i)
            d[i]     = 0.0;

        void *ptr = NULL;
        biquad_x2_t *f = alloc_aligned<biquad_x2_t>(ptr, count+1, 64);
        for (size_t i=0; i<(count+1); ++i)
        {
            f[i].a[0]   = bq_normal.a[0];
            f[i].a[1]   = bq_normal.a[1];
            f[i].a[2]   = bq_normal.a[2];
            f[i].a[3]   = bq_normal.a[3];
            f[i].a[4]   = bq_normal.a[0];
            f[i].a[5]   = bq_normal.a[1];
            f[i].a[6]   = bq_normal.a[2];
            f[i].a[7]   = bq_normal.a[3];

            f[i].b[0]   = bq_normal.b[0];
            f[i].b[1]   = bq_normal.b[1];
            f[i].b[2]   = bq_normal.b[2];
            f[i].b[3]   = bq_normal.b[3];
            f[i].b[4]   = bq_normal.b[0];
            f[i].b[5]   = bq_normal.b[1];
            f[i].b[6]   = bq_normal.b[2];
            f[i].b[7]   = bq_normal.b[3];
        }

        PTEST_LOOP(text,
            process(out, in, &d[0], count, f);
            process(out, out, &d[4], count, f);
            process(out, out, &d[8], count, f);
            process(out, out, &d[12], count, f);
        );

        free_aligned(ptr);
    }

    void process_2x4(const char *text, float *out, const float *in, size_t count, dyn_biquad_process_x4_t process)
    {
        if (!PTEST_SUPPORTED(process))
            return;
        printf("Testing %s dynamic filters on input buffer of %d samples ...\n", text, int(count));

        float d[16] __lsp_aligned64;
        for (size_t i=0; i<16; ++i)
            d[i]     = 0.0;

        void *ptr = NULL;
        biquad_x4_t *f = alloc_aligned<biquad_x4_t>(ptr, count+3, 64);
        for (size_t i=0; i<(count+3); ++i)
        {
            f[i].a0[0]  = f[i].a0[1] = f[i].a0[2] = f[i].a0[3] = bq_normal.a[0];
            f[i].a1[0]  = f[i].a1[1] = f[i].a1[2] = f[i].a1[3] = bq_normal.a[1];
            f[i].a2[0]  = f[i].a2[1] = f[i].a2[2] = f[i].a2[3] = bq_normal.a[2];
            f[i].b1[0]  = f[i].b1[1] = f[i].b1[2] = f[i].b1[3] = bq_normal.b[1];
            f[i].b2[0]  = f[i].b2[1] = f[i].b2[2] = f[i].b2[3] = bq_normal.b[2];
        }

        PTEST_LOOP(text,
            process(out, in, &d[0], count, f);
            process(out, out, &d[8], count, f);
        );

        free_aligned(ptr);
    }

    void process_1x8(const char *text, float *out, const float *in, size_t count, dyn_biquad_process_x8_t process)
    {
        if (!PTEST_SUPPORTED(process))
            return;
        printf("Testing %s dynamic filters on input buffer of %d samples ...\n", text, int(count));

        float d[16] __lsp_aligned64;
        for (size_t i=0; i<16; ++i)
            d[i]     = 0.0;

        void *ptr = NULL;
        biquad_x8_t *f = alloc_aligned<biquad_x8_t>(ptr, count+7, 64);
        for (size_t i=0; i<(count+7); ++i)
        {
            f[i].a0[0]  = f[i].a0[1] = f[i].a0[2] = f[i].a0[3] =
            f[i].a0[4]  = f[i].a0[5] = f[i].a0[6] = f[i].a0[7] = bq_normal.a[0];
            f[i].a1[0]  = f[i].a1[1] = f[i].a1[2] = f[i].a1[3] =
            f[i].a1[4]  = f[i].a1[5] = f[i].a1[6] = f[i].a1[7] = bq_normal.a[1];
            f[i].a2[0]  = f[i].a2[1] = f[i].a2[2] = f[i].a2[3] =
            f[i].a2[4]  = f[i].a2[5] = f[i].a2[6] = f[i].a2[7] = bq_normal.a[2];
            f[i].b1[0]  = f[i].b1[1] = f[i].b1[2] = f[i].b1[3] =
            f[i].b1[4]  = f[i].b1[5] = f[i].b1[6] = f[i].b1[7] = bq_normal.b[1];
            f[i].b2[0]  = f[i].b2[1] = f[i].b2[2] = f[i].b2[3] =
            f[i].b2[4]  = f[i].b2[5] = f[i].b2[6] = f[i].b2[7] = bq_normal.b[2];
        }

        PTEST_LOOP(text,
            process(out, in, d, count, f);
        );

        free_aligned(ptr);
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

        process_8x1("native::dyn_biquad_process_x1 x8", out, in, FTEST_BUF_SIZE, native::dyn_biquad_process_x1);
        IF_ARCH_X86(process_8x1("sse::dyn_biquad_process_x1 x8", out, in, FTEST_BUF_SIZE, sse::dyn_biquad_process_x1));
        IF_ARCH_ARM(process_8x1("neon_d32::dyn_biquad_process_x1 x8", out, in, FTEST_BUF_SIZE, neon_d32::dyn_biquad_process_x1));
        PTEST_SEPARATOR;

        process_4x2("native::dyn_biquad_process_x2 x4", out, in, FTEST_BUF_SIZE, native::dyn_biquad_process_x2);
        IF_ARCH_X86(process_4x2("sse::dyn_biquad_process_x2 x4", out, in, FTEST_BUF_SIZE, sse::dyn_biquad_process_x2));
        IF_ARCH_ARM(process_4x2("neon_d32::dyn_biquad_process_x2 x4", out, in, FTEST_BUF_SIZE, neon_d32::dyn_biquad_process_x2));
        PTEST_SEPARATOR;

        process_2x4("native::dyn_biquad_process_x4 x2", out, in, FTEST_BUF_SIZE, native::dyn_biquad_process_x4);
        IF_ARCH_X86(process_2x4("sse::dyn_biquad_process_x4 x2", out, in, FTEST_BUF_SIZE, sse::dyn_biquad_process_x4));
        IF_ARCH_ARM(process_2x4("neon_d32::dyn_biquad_process_x4 x2", out, in, FTEST_BUF_SIZE, neon_d32::dyn_biquad_process_x4));
        PTEST_SEPARATOR;

        process_1x8("native::dyn_biquad_process_x8 x1", out, in, FTEST_BUF_SIZE, native::dyn_biquad_process_x8);
        IF_ARCH_X86(process_1x8("sse::dyn_biquad_process_x8 x1", out, in, FTEST_BUF_SIZE, sse::dyn_biquad_process_x8));
        IF_ARCH_X86_64(process_1x8("sse3::x64_dyn_biquad_process_x8 x1", out, in, FTEST_BUF_SIZE, sse3::x64_dyn_biquad_process_x8));
        IF_ARCH_X86_64(process_1x8("avx::x64_dyn_biquad_process_x8 x1", out, in, FTEST_BUF_SIZE, avx::x64_dyn_biquad_process_x8));
        IF_ARCH_X86_64(process_1x8("avx::x64_dyn_biquad_process_x8_fma3 x1", out, in, FTEST_BUF_SIZE, avx::x64_dyn_biquad_process_x8_fma3));
        IF_ARCH_ARM(process_1x8("neon_d32::dyn_biquad_process_x8 x1", out, in, FTEST_BUF_SIZE, neon_d32::dyn_biquad_process_x8));
        PTEST_SEPARATOR;

        delete [] out;
        delete [] in;
    }

PTEST_END


