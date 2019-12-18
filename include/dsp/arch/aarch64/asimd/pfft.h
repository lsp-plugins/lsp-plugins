/*
 * pfft.h
 *
 *  Created on: 18 дек. 2019 г.
 *      Author: sadko
 */

#ifndef INCLUDE_DSP_ARCH_AARCH64_ASIMD_PFFT_H_
#define INCLUDE_DSP_ARCH_AARCH64_ASIMD_PFFT_H_

#ifndef DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_AARCH64_ASIMD_IMPL */

#include <dsp/arch/aarch64/asimd/fft/const.h>
#include <dsp/arch/aarch64/asimd/fft/pscramble.h>
#include <dsp/arch/aarch64/asimd/fft/pbutterfly.h>

namespace asimd
{
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

        if (dst == src)
            packed_scramble_self_direct(dst, rank);
        else
            packed_scramble_copy_direct(dst, src, rank);
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

        if (dst == src)
            packed_scramble_self_reverse(dst, rank);
        else
            packed_scramble_copy_reverse(dst, src, rank);
        packed_reverse_butterfly_rank3(dst, 1 << (rank-3));

        for (size_t i=4; i <= rank; ++i)
            packed_reverse_butterfly_rank4p(dst, i, 1 << (rank - i));

        packed_unscramble_reverse(dst, rank);
    }
}



#endif /* INCLUDE_DSP_ARCH_AARCH64_ASIMD_PFFT_H_ */
