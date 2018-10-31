/*
 * fft.h
 *
 *  Created on: 24 окт. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_ARM_NEON_D32_FFT_H_
#define DSP_ARCH_ARM_NEON_D32_FFT_H_

#ifndef DSP_ARCH_ARM_NEON_32_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_ARM_NEON_32_IMPL */

#include <dsp/arch/arm/neon-d32/fft/const.h>
#include <dsp/arch/arm/neon-d32/fft/scramble.h>
#include <dsp/arch/arm/neon-d32/fft/pscramble.h>
#include <dsp/arch/arm/neon-d32/fft/butterfly.h>
#include <dsp/arch/arm/neon-d32/fft/pbutterfly.h>

namespace neon_d32
{
    void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
    {
        // Check bounds
        if (rank <= 2)
        {
            if (rank == 2)
            {
                float s0_re     = src_re[0] + src_re[1];
                float s1_re     = src_re[0] - src_re[1];
                float s2_re     = src_re[2] + src_re[3];
                float s3_re     = src_re[2] - src_re[3];

                float s0_im     = src_im[0] + src_im[1];
                float s1_im     = src_im[0] - src_im[1];
                float s2_im     = src_im[2] + src_im[3];
                float s3_im     = src_im[2] - src_im[3];

                dst_re[0]       = s0_re + s2_re;
                dst_re[1]       = s1_re + s3_im;
                dst_re[2]       = s0_re - s2_re;
                dst_re[3]       = s1_re - s3_im;

                dst_im[0]       = s0_im + s2_im;
                dst_im[1]       = s1_im - s3_re;
                dst_im[2]       = s0_im - s2_im;
                dst_im[3]       = s1_im + s3_re;
            }
            else if (rank == 1)
            {
                // s0' = s0 + s1
                // s1' = s0 - s1
                float s1_re     = src_re[1];
                float s1_im     = src_im[1];
                dst_re[1]       = src_re[0] - s1_re;
                dst_im[1]       = src_im[0] - s1_im;
                dst_re[0]       = src_re[0] + s1_re;
                dst_im[0]       = src_im[0] + s1_im;
            }
            else
            {
                dst_re[0]       = src_re[0];
                dst_im[0]       = src_im[0];
            }
            return;
        }

        // Scramble data
        scramble_direct(dst_re, dst_im, src_re, src_im, rank);
        direct_butterfly_rank3(dst_re, dst_im, 1 << (rank-3));

        for (size_t i=4; i <= rank; ++i)
            direct_butterfly_rank4p(dst_re, dst_im, i, 1 << (rank - i));
    }

    void reverse_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
    {
        // Check bounds
        if (rank <= 2)
        {
            if (rank == 2)
            {
                float s0_re     = src_re[0] + src_re[1];
                float s1_re     = src_re[0] - src_re[1];
                float s2_re     = src_re[2] + src_re[3];
                float s3_re     = src_re[2] - src_re[3];

                float s0_im     = src_im[0] + src_im[1];
                float s1_im     = src_im[0] - src_im[1];
                float s2_im     = src_im[2] + src_im[3];
                float s3_im     = src_im[2] - src_im[3];

                dst_re[0]       = (s0_re + s2_re)*0.25f;
                dst_re[1]       = (s1_re - s3_im)*0.25f;
                dst_re[2]       = (s0_re - s2_re)*0.25f;
                dst_re[3]       = (s1_re + s3_im)*0.25f;

                dst_im[0]       = (s0_im + s2_im)*0.25f;
                dst_im[1]       = (s1_im + s3_re)*0.25f;
                dst_im[2]       = (s0_im - s2_im)*0.25f;
                dst_im[3]       = (s1_im - s3_re)*0.25f;
            }
            else if (rank == 1)
            {
                // s0' = s0 + s1
                // s1' = s0 - s1
                float s1_re     = src_re[1];
                float s1_im     = src_im[1];
                dst_re[1]       = (src_re[0] - s1_re) * 0.5f;
                dst_im[1]       = (src_im[0] - s1_im) * 0.5f;
                dst_re[0]       = (src_re[0] + s1_re) * 0.5f;
                dst_im[0]       = (src_im[0] + s1_im) * 0.5f;
            }
            else
            {
                dst_re[0]       = src_re[0];
                dst_im[0]       = src_im[0];
            }
            return;
        }

        // Scramble data
        scramble_reverse(dst_re, dst_im, src_re, src_im, rank);
        reverse_butterfly_rank3(dst_re, dst_im, 1 << (rank-3));

        for (size_t i=4; i <= rank; ++i)
            reverse_butterfly_rank4p(dst_re, dst_im, i, 1 << (rank - i));

        dsp::normalize_fft2(dst_re, dst_im, rank);
    }

    void packed_direct_fft(float *dst, const float *src, size_t rank)
    {
        // Check bounds
        if (rank <= 2)
        {
            if (rank == 2)
            {
                float s0_re     = dst[0] + dst[2];
                float s1_re     = dst[0] - dst[2];
                float s0_im     = dst[1] + dst[3];
                float s1_im     = dst[1] - dst[3];

                float s2_re     = dst[4] + dst[6];
                float s3_re     = dst[4] - dst[6];
                float s2_im     = dst[5] + dst[7];
                float s3_im     = dst[5] - dst[7];

                dst[0]          = s0_re + s2_re;
                dst[1]          = s0_im + s2_im;
                dst[2]          = s1_re + s3_im;
                dst[3]          = s1_im - s3_re;

                dst[4]          = s0_re - s2_re;
                dst[5]          = s0_im - s2_im;
                dst[6]          = s1_re - s3_im;
                dst[7]          = s1_im + s3_re;
            }
            else if (rank == 1)
            {
                // s0' = s0 + s1
                // s1' = s0 - s1
                float s1_re     = src[2];
                float s1_im     = src[3];
                dst[2]          = src[0] - s1_re;
                dst[3]          = src[1] - s1_im;
                dst[0]          = src[0] + s1_re;
                dst[1]          = src[1] + s1_im;
            }
            else
            {
                dst[0]          = src[0];
                dst[1]          = src[1];
            }
            return;
        }

        packed_scramble_direct(dst, src, rank);
        packed_direct_butterfly_rank3(dst, 1 << (rank-3));

        for (size_t i=4; i <= rank; ++i)
            packed_direct_butterfly_rank4p(dst, i, 1 << (rank - i));

        packed_unscramble_direct(dst, rank);
    }

    void packed_reverse_fft(float *dst, const float *src, size_t rank)
    {
        // Check bounds
        if (rank <= 2)
        {
            if (rank == 2)
            {
                float s0_re     = src[0] + src[2];
                float s1_re     = src[0] - src[2];
                float s2_re     = src[4] + src[6];
                float s3_re     = src[4] - src[6];

                float s0_im     = src[1] + src[3];
                float s1_im     = src[1] - src[3];
                float s2_im     = src[5] + src[7];
                float s3_im     = src[5] - src[7];

                dst[0]          = (s0_re + s2_re)*0.25f;
                dst[1]          = (s0_im + s2_im)*0.25f;
                dst[2]          = (s1_re - s3_im)*0.25f;
                dst[3]          = (s1_im + s3_re)*0.25f;

                dst[4]          = (s0_re - s2_re)*0.25f;
                dst[5]          = (s0_im - s2_im)*0.25f;
                dst[6]          = (s1_re + s3_im)*0.25f;
                dst[7]          = (s1_im - s3_re)*0.25f;
            }
            else if (rank == 1)
            {
                // s0' = s0 + s1
                // s1' = s0 - s1
                float s1_re     = src[2];
                float s1_im     = src[3];
                dst[2]          = src[0] - s1_re;
                dst[3]          = src[1] - s1_im;
                dst[0]          = src[0] + s1_re;
                dst[1]          = src[1] + s1_im;
            }
            else
            {
                dst[0]          = src[0];
                dst[1]          = src[1];
            }
            return;
        }

        packed_scramble_reverse(dst, src, rank);
        packed_reverse_butterfly_rank3(dst, 1 << (rank-3));

        for (size_t i=4; i <= rank; ++i)
            packed_reverse_butterfly_rank4p(dst, i, 1 << (rank - i));

        packed_unscramble_reverse(dst, rank);
    }
}

#endif /* DSP_ARCH_ARM_NEON_D32_FFT_H_ */
