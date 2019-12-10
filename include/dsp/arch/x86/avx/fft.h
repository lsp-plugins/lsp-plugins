/*
 * fft.h
 *
 *  Created on: 9 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX_FFT_H_
#define DSP_ARCH_X86_AVX_FFT_H_

#ifndef DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_AVX_IMPL */

#include <dsp/arch/x86/avx/fft/const.h>
#include <dsp/arch/x86/avx/fft/butterfly.h>

#define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct8
#define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse8
#define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct8
#define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse8
#define FFT_TYPE                        uint8_t
#include <dsp/arch/x86/avx/fft/scramble.h>

#define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct16
#define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse16
#define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct16
#define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse16
#define FFT_TYPE                        uint16_t
#include <dsp/arch/x86/avx/fft/scramble.h>

namespace avx
{
    // Make set of scramble implementations
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

        if ((dst_re == src_re) || (dst_im == src_im) || (rank < 4))
        {
            dsp::copy(dst_re, src_re, 1 << rank);
            dsp::copy(dst_im, src_im, 1 << rank);
            if (rank <= 8)
                scramble_self_direct8(dst_re, dst_im, rank);
            else
                scramble_self_direct16(dst_re, dst_im, rank);
        }
        else
        {
            if (rank <= 12)
                scramble_copy_direct8(dst_re, dst_im, src_re, src_im, rank-4);
            else
                scramble_copy_direct16(dst_re, dst_im, src_re, src_im, rank-4);
        }

        size_t i=2;
        if (i++ < rank)
            butterfly_direct4(dst_re, dst_im, 1 << (rank - 3));
//
//        for (size_t i=2; i < rank; ++i)
//            butterfly_direct(dst_re, dst_im, i, 1 << (rank - i - 1));
    }

    // Make set of scramble implementations
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

        if ((dst_re == src_re) || (dst_im == src_im) || (rank < 4))
        {
            dsp::copy(dst_re, src_re, 1 << rank);
            dsp::copy(dst_im, src_im, 1 << rank);
            if (rank <= 8)
                scramble_self_reverse8(dst_re, dst_im, rank);
            else
                scramble_self_reverse16(dst_re, dst_im, rank);
        }
        else
        {
            if (rank <= 12)
                scramble_copy_reverse8(dst_re, dst_im, src_re, src_im, rank-4);
            else
                scramble_copy_reverse16(dst_re, dst_im, src_re, src_im, rank-4);
        }
//
//        for (size_t i=2; i < rank; ++i)
//            butterfly_direct(dst_re, dst_im, i, 1 << (rank - i - 1));
        dsp::normalize_fft2(dst_re, dst_im, rank);
    }
}

#endif /* DSP_ARCH_X86_AVX_FFT_H_ */
