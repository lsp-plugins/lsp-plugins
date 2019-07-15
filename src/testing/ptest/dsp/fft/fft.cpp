/*
 * fft.cpp
 *
 *  Created on: 27 авг. 2018 г.
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
    void packed_direct_fft(float *dst, const float *src, size_t rank);
}

IF_ARCH_X86(
    namespace sse
    {
        void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        void packed_direct_fft(float *dst, const float *src, size_t rank);
    }
)

IF_ARCH_ARM(
    namespace neon_d32
    {
        void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        void packed_direct_fft(float *dst, const float *src, size_t rank);
    }
)


typedef void (* direct_fft_t) (float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
typedef void (* conv_direct_fft_t) (float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
typedef void (* packed_direct_fft_t) (float *dst, const float *src, size_t rank);

//-----------------------------------------------------------------------------
// Performance test for complex multiplication
PTEST_BEGIN("dsp.fft", fft, 30, 1000)

    void call(const char *label, float *fft_re, float *fft_im, const float *sig_re, const float *sig_im, size_t rank, direct_fft_t fft)
    {
        if (!PTEST_SUPPORTED(fft))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(1 << rank));
        printf("Testing %s samples (rank = %d) ...\n", buf, int(rank));

        PTEST_LOOP(buf,
            fft(fft_re, fft_im, sig_re, sig_im, rank);
        )
    }

    void call(const char *label, float *dst, const float *src, size_t rank, packed_direct_fft_t fft)
    {
        if (!PTEST_SUPPORTED(fft))
            return;

        char buf[80];
        sprintf(buf, "%s x %d", label, int(1 << rank));
        printf("Testing %s samples (rank = %d) ...\n", buf, int(rank));

        PTEST_LOOP(buf,
            fft(dst, src, rank);
        )
    }

    PTEST_MAIN
    {
        size_t fft_size = 1 << MAX_RANK;

        uint8_t *data   = NULL;

        float *sig_re   = alloc_aligned<float>(data, fft_size * 4, 64);
        float *sig_im   = &sig_re[fft_size];
        float *fft_re   = &sig_im[fft_size];
        float *fft_im   = &fft_re[fft_size];

        for (size_t i=0; i < (1 << MAX_RANK); ++i)
        {
            sig_re[i]       = float(rand()) / RAND_MAX;
            sig_im[i]       = 0.0f;
        }

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            call("native::direct_fft", fft_re, fft_im, sig_re, sig_im, i, native::direct_fft);
            call("native::packed_direct_fft", fft_re, sig_re, i, native::packed_direct_fft);

            IF_ARCH_X86(call("sse::direct_fft", fft_re, fft_im, sig_re, sig_im, i, sse::direct_fft));
            IF_ARCH_X86(call("sse::packed_direct_fft", fft_re, sig_re, i, sse::packed_direct_fft));

            IF_ARCH_ARM(call("neon_d32::direct_fft", fft_re, fft_im, sig_re, sig_im, i, neon_d32::direct_fft));
            IF_ARCH_ARM(call("neon_d32::packed_direct_fft", fft_re, sig_re, i, neon_d32::packed_direct_fft));

            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END





