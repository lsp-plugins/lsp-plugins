/*
 * fastconv.cpp
 *
 *  Created on: 29 авг. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>
#include <core/sugar.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace native
{
    void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
    void reverse_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
    void complex_mul3(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count);
    void add2(float *dst, const float *src, size_t count);

    void fastconv_parse(float *dst, const float *src, size_t rank);
    void fastconv_parse_apply(float *dst, float *tmp, const float *c, const float *src, size_t rank);
}

IF_ARCH_X86(
    namespace sse
    {
        void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        void reverse_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        void complex_mul3(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count);
        void add2(float *dst, const float *src, size_t count);

        void fastconv_parse(float *dst, const float *src, size_t rank);
        void fastconv_parse_apply(float *dst, float *tmp, const float *c, const float *src, size_t rank);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        void reverse_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        void complex_mul3(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count);
        void add2(float *dst, const float *src, size_t count);

        void fastconv_parse(float *dst, const float *src, size_t rank);
        void fastconv_parse_apply(float *dst, float *tmp, const float *c, const float *src, size_t rank);
    }
)

typedef void (* direct_fft_t)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
typedef void (* reverse_fft_t)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
typedef void (* complex_mul3_t)(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count);
typedef void (* add2_t)(float *dst, const float *src, size_t count);

typedef void (* fastconv_parse_t)(float *dst, const float *src, size_t rank);
typedef void (* fastconv_parse_apply_t)(float *dst, float *tmp, const float *c, const float *src, size_t rank);

//-----------------------------------------------------------------------------
// Performance test for complex multiplication
PTEST_BEGIN("dsp.fft", fastconv, 30, 1000)

    void call(
            const char *label,
            float *out, float *tmp, float *tmp2, float *conv, const float *in, const float *cv, size_t rank,
            direct_fft_t direct, complex_mul3_t cmul, reverse_fft_t reverse, add2_t add2
        )
    {
        if (!(PTEST_SUPPORTED(direct) && (PTEST_SUPPORTED(cmul)) && (PTEST_SUPPORTED(reverse)) && (PTEST_SUPPORTED(add2))))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(1 << rank));
        printf("Testing %s samples (rank = %d)...\n", buf, int(rank));
        size_t bin_size = 1 << rank;

        // Prepare data
        dsp::fill_zero(out, bin_size);
        direct(conv, &conv[bin_size], cv, &cv[bin_size], rank);

        // Run test
        PTEST_LOOP(buf,
            direct(tmp, &tmp[bin_size], in, &in[bin_size], rank);
            cmul(tmp, &tmp[bin_size], tmp, &tmp[bin_size], conv, &conv[bin_size], bin_size);
            reverse(tmp2, &tmp2[bin_size], tmp, &tmp[bin_size], rank);
            add2(out, tmp2, bin_size);
        )
    }

    void call(
            const char *label,
            float *out, float *tmp, float *conv, const float *in, const float *cv, size_t rank,
            fastconv_parse_t parse, fastconv_parse_apply_t apply
            )
    {
        if (!(PTEST_SUPPORTED(parse) && (PTEST_SUPPORTED(apply))))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(1 << rank));
        printf("Testing %s samples (rank = %d)...\n", buf, int(rank));

        // Prepare data
        dsp::fill_zero(out, 1 << rank);
        parse(conv, cv, rank);

        PTEST_LOOP(buf,
            apply(out, tmp, conv, in, rank);
        );
    }

    PTEST_MAIN
    {
        size_t fft_size = 1 << MAX_RANK;
        size_t alloc    = fft_size * 2 * 2 // in + cv size
                        + fft_size * 2 // conv size
                        + fft_size * 2 // tmp size
                        + fft_size * 2 // tmp2 size
                        + fft_size;     // out size

        uint8_t *data   = NULL;
        float *in       = alloc_aligned<float>(data, alloc, 64);
        dsp::fill_zero(in, alloc);

        float *cv       = &in[fft_size * 2];
        float *conv     = &cv[fft_size * 2];
        float *tmp      = &conv[fft_size * 2];
        float *tmp2     = &tmp[fft_size * 2];
        float *out      = &tmp2[fft_size * 2];

        for (size_t i=0; i < (fft_size / 2); ++i)
        {
            in[i]       = float(rand()) / RAND_MAX;
            cv[i]       = float(rand()) / RAND_MAX;
        }

        for (size_t rank=MIN_RANK; rank <= MAX_RANK; ++rank)
        {
            call("native::fft", out, tmp, tmp2, conv, in, cv, rank,
                    native::direct_fft, native::complex_mul3, native::reverse_fft, native::add2);
            call("native::fastconv_fft", out, tmp, conv, in, cv, rank,
                    native::fastconv_parse, native::fastconv_parse_apply);

            IF_ARCH_X86(
                call("sse::fft", out, tmp, tmp2, conv, in, cv, rank,
                    sse::direct_fft, sse::complex_mul3, sse::reverse_fft, sse::add2);
                call("sse::fastconv_fft", out, tmp, conv, in, cv, rank,
                    sse::fastconv_parse, sse::fastconv_parse_apply);
            )

            IF_ARCH_ARM(
                call("neon_d32::fft", out, tmp, tmp2, conv, in, cv, rank,
                    neon_d32::direct_fft, neon_d32::complex_mul3, neon_d32::reverse_fft, neon_d32::add2);
                call("neon_d32::fastconv_fft", out, tmp, conv, in, cv, rank,
                    neon_d32::fastconv_parse, neon_d32::fastconv_parse_apply);
            )

            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }

PTEST_END


