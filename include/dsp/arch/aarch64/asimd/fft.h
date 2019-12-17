/*
 * fft.h
 *
 *  Created on: 17 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_AARCH64_ASIMD_FFT_H_
#define DSP_ARCH_AARCH64_ASIMD_FFT_H_

#ifndef DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_AARCH64_ASIMD_IMPL */

#include <dsp/arch/aarch64/asimd/fft/const.h>
#include <dsp/arch/aarch64/asimd/fft/scramble.h>
#include <dsp/arch/aarch64/asimd/fft/butterfly.h>
#include <dsp/arch/aarch64/asimd/fft/normalize.h>

namespace asimd
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
        if ((dst_re == src_re) || (dst_im == src_im))
        {
            dsp::move(dst_re, src_re, 1 << rank);
            dsp::move(dst_im, src_im, 1 << rank);
            scramble_self_direct(dst_re, dst_im, rank);
        }
        else
            scramble_copy_direct(dst_re, dst_im, src_re, src_im, rank);

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
        if ((dst_re == src_re) || (dst_im == src_im))
        {
            dsp::move(dst_re, src_re, 1 << rank);
            dsp::move(dst_im, src_im, 1 << rank);
            scramble_self_reverse(dst_re, dst_im, rank);
        }
        else
            scramble_copy_reverse(dst_re, dst_im, src_re, src_im, rank);

        reverse_butterfly_rank3(dst_re, dst_im, 1 << (rank-3));

        for (size_t i=4; i <= rank; ++i)
            reverse_butterfly_rank4p(dst_re, dst_im, i, 1 << (rank - i));

        dsp::normalize_fft2(dst_re, dst_im, rank);
    }
}

#endif /* DSP_ARCH_AARCH64_ASIMD_FFT_H_ */
