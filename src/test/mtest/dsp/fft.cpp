/*
 * fft.cpp
 *
 *  Created on: 25 окт. 2018 г.
 *      Author: sadko
 */

#include <dsp/bits.h>
#include <test/mtest.h>
#include <test/FloatBuffer.h>

#define RANK        4
#define BUF_SIZE    (1 << RANK)

static void scramble_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
{
    size_t items    = 1 << rank;

    // Scramble the order of samples
    if ((dst_re != src_re) && (dst_im != src_im))
    {
        for (size_t i = 0; i < items; ++i)
        {
            size_t j    = reverse_bits(i, rank);
            dst_re[i]   = src_re[j];
            dst_im[i]   = src_im[j];
        }
    }
    else
    {
        for (size_t i = 1; i < (items - 1); ++i)
        {
            size_t j = reverse_bits(i, rank);
            if (i >= j)
                continue;

            float re    = dst_re[i];
            float im    = dst_im[i];
            dst_re[i]   = dst_re[j];
            dst_im[i]   = dst_im[j];
            dst_re[j]   = re;
            dst_im[j]   = im;
        }
    }
}

static void start_direct_fft(float *dst_re, float *dst_im, size_t rank)
{
    size_t iterations    = 1 << (rank - 2);
    while (iterations--)
    {
        // Perform 4-calculations
        // s0' = s0 + s1
        // s1' = s0 - s1
        // s2' = s2 + s3
        // s3' = s2 - s3
        // s0'' = s0' + s2'
        // s1'' = s1' - j * s3'
        // s2'' = s0' - s2'
        // s3'' = s1' + j * s3'
        float s0_re     = dst_re[0] + dst_re[1];
        float s1_re     = dst_re[0] - dst_re[1];
        float s2_re     = dst_re[2] + dst_re[3];
        float s3_re     = dst_re[2] - dst_re[3];

        float s0_im     = dst_im[0] + dst_im[1];
        float s1_im     = dst_im[0] - dst_im[1];
        float s2_im     = dst_im[2] + dst_im[3];
        float s3_im     = dst_im[2] - dst_im[3];

        dst_re[0]       = s0_re + s2_re;
        dst_re[1]       = s1_re + s3_im;
        dst_re[2]       = s0_re - s2_re;
        dst_re[3]       = s1_re - s3_im;

        dst_im[0]       = s0_im + s2_im;
        dst_im[1]       = s1_im - s3_re;
        dst_im[2]       = s0_im - s2_im;
        dst_im[3]       = s1_im + s3_re;

        // Move pointers
        dst_re         += 4;
        dst_im         += 4;
    }
}

static void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
{
    scramble_fft(dst_re, dst_im, src_re, src_im, rank);
    start_direct_fft(dst_re, dst_im, rank);
}

IF_ARCH_ARM(
    namespace neon_d32
    {
        void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
    }
)

MTEST_BEGIN("dsp", fft)

    MTEST_MAIN
    {
        FloatBuffer src1r(BUF_SIZE, 64);
        FloatBuffer src1i(BUF_SIZE, 64);
        FloatBuffer src2r(BUF_SIZE, 64);
        FloatBuffer src2i(BUF_SIZE, 64);
        FloatBuffer dst1r(BUF_SIZE, 64);
        FloatBuffer dst1i(BUF_SIZE, 64);
        FloatBuffer dst2r(BUF_SIZE, 64);
        FloatBuffer dst2i(BUF_SIZE, 64);

        // Prepare data
        for (size_t i=0; i<BUF_SIZE; ++i)
        {
            src1r[i]            = i;
            src1i[i]            = i * 0.1f;
        }
        src2r.copy(src1r);
        src2i.copy(src1i);

        // Test
        src1r.dump("src1r");
        src1i.dump("src1i");

        direct_fft(dst1r, dst1i, src1r, src1i, RANK);
        dst1r.dump("dst1r");
        dst1i.dump("dst1i");

        direct_fft(src1r, src1i, src1r, src1i, RANK);
        src1r.dump("src1r");
        src1i.dump("src1i");

        IF_ARCH_ARM(
            src2r.dump("src2r");
            src2i.dump("src2i");

            neon_d32::direct_fft(dst2r, dst2i, src2r, src2i, RANK);
            dst2r.dump("dst2r");
            dst2i.dump("dst2i");

            neon_d32::direct_fft(src2r, src2i, src2r, src2i, RANK);
            src2r.dump("src2r");
            src2i.dump("src2i");
        );

    }
MTEST_END
