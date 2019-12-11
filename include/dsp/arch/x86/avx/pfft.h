/*
 * pfft.h
 *
 *  Created on: 11 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_AVX_PFFT_H_
#define DSP_ARCH_X86_AVX_PFFT_H_

#ifndef DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_AVX_IMPL */

#include <dsp/arch/x86/avx/fft/const.h>
#include <dsp/arch/x86/avx/fft/p_repack.h>
#include <dsp/arch/x86/avx/fft/p_butterfly.h>

// Scrambling functions
#define FFT_PSCRAMBLE_SELF_DIRECT_NAME      packed_scramble_self_direct8
#define FFT_PSCRAMBLE_SELF_REVERSE_NAME     packed_scramble_self_reverse8
#define FFT_PSCRAMBLE_COPY_DIRECT_NAME      packed_scramble_copy_direct8
#define FFT_PSCRAMBLE_COPY_REVERSE_NAME     packed_scramble_copy_reverse8
#define FFT_TYPE                            uint8_t
#define FFT_FMA(a, b)                       a
#include <dsp/arch/x86/avx/fft/p_scramble.h>

#define FFT_PSCRAMBLE_SELF_DIRECT_NAME      packed_scramble_self_direct16
#define FFT_PSCRAMBLE_SELF_REVERSE_NAME     packed_scramble_self_reverse16
#define FFT_PSCRAMBLE_COPY_DIRECT_NAME      packed_scramble_copy_direct16
#define FFT_PSCRAMBLE_COPY_REVERSE_NAME     packed_scramble_copy_reverse16
#define FFT_TYPE                            uint16_t
#define FFT_FMA(a, b)                       a
#include <dsp/arch/x86/avx/fft/p_scramble.h>

#define FFT_PSCRAMBLE_SELF_DIRECT_NAME      packed_scramble_self_direct8_fma3
#define FFT_PSCRAMBLE_SELF_REVERSE_NAME     packed_scramble_self_reverse8_fma3
#define FFT_PSCRAMBLE_COPY_DIRECT_NAME      packed_scramble_copy_direct8_fma3
#define FFT_PSCRAMBLE_COPY_REVERSE_NAME     packed_scramble_copy_reverse8_fma3
#define FFT_TYPE                            uint8_t
#define FFT_FMA(a, b)                       b
#include <dsp/arch/x86/avx/fft/p_scramble.h>

#define FFT_PSCRAMBLE_SELF_DIRECT_NAME      packed_scramble_self_direct16_fma3
#define FFT_PSCRAMBLE_SELF_REVERSE_NAME     packed_scramble_self_reverse16_fma3
#define FFT_PSCRAMBLE_COPY_DIRECT_NAME      packed_scramble_copy_direct16_fma3
#define FFT_PSCRAMBLE_COPY_REVERSE_NAME     packed_scramble_copy_reverse16_fma3
#define FFT_TYPE                            uint16_t
#define FFT_FMA(a, b)                       b
#include <dsp/arch/x86/avx/fft/p_scramble.h>

namespace avx
{
    static void packed_small_direct_fft(float *dst, const float *src, size_t rank)
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
    }

    static void packed_small_reverse_fft(float *dst, const float *src, size_t rank)
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

    void packed_direct_fft(float *dst, const float *src, size_t rank)
    {
        if (rank <= 2)
        {
            packed_small_direct_fft(dst, src, rank);
            return;
        }

        if ((dst == src) || (rank < 4))
        {
            dsp::move(dst, src, 2 << rank); // 1 << rank + 1
            if (rank <= 8)
                packed_scramble_self_direct8(dst, rank);
            else
                packed_scramble_self_direct16(dst, rank);
        }
        else
        {
            if (rank <= 12)
                packed_scramble_copy_direct8(dst, src, rank-4);
            else
                packed_scramble_copy_direct16(dst, src, rank-4);
        }

        for (size_t i=3; i < rank; ++i)
            packed_butterfly_direct8p(dst, i, 1 << (rank - i - 1));

        packed_fft_repack(dst, rank);
    }

    void packed_reverse_fft(float *dst, const float *src, size_t rank)
    {
        if (rank <= 2)
        {
            packed_small_reverse_fft(dst, src, rank);
            return;
        }

        if ((dst == src) || (rank < 4))
        {
            dsp::move(dst, src, 2 << rank); // 1 << rank + 1
            if (rank <= 8)
                packed_scramble_self_reverse8(dst, rank);
            else
                packed_scramble_self_reverse16(dst, rank);
        }
        else
        {
            if (rank <= 12)
                packed_scramble_copy_reverse8(dst, src, rank-4);
            else
                packed_scramble_copy_reverse16(dst, src, rank-4);
        }

        for (size_t i=3; i < rank; ++i)
            packed_butterfly_reverse8p(dst, i, 1 << (rank - i - 1));

        packed_fft_repack_normalize(dst, rank);
    }

    void packed_direct_fft_fma3(float *dst, const float *src, size_t rank)
    {
        if (rank <= 2)
        {
            packed_small_direct_fft(dst, src, rank);
            return;
        }

        if ((dst == src) || (rank < 4))
        {
            dsp::move(dst, src, 2 << rank); // 1 << rank + 1
            if (rank <= 8)
                packed_scramble_self_direct8_fma3(dst, rank);
            else
                packed_scramble_self_direct16_fma3(dst, rank);
        }
        else
        {
            if (rank <= 12)
                packed_scramble_copy_direct8_fma3(dst, src, rank-4);
            else
                packed_scramble_copy_direct16_fma3(dst, src, rank-4);
        }

        for (size_t i=3; i < rank; ++i)
            packed_butterfly_direct8p_fma3(dst, i, 1 << (rank - i - 1));

        packed_fft_repack(dst, rank);
    }

    void packed_reverse_fft_fma3(float *dst, const float *src, size_t rank)
    {
        if (rank <= 2)
        {
            packed_small_reverse_fft(dst, src, rank);
            return;
        }

        if ((dst == src) || (rank < 4))
        {
            dsp::move(dst, src, 2 << rank); // 1 << rank + 1
            if (rank <= 8)
                packed_scramble_self_reverse8_fma3(dst, rank);
            else
                packed_scramble_self_reverse16_fma3(dst, rank);
        }
        else
        {
            if (rank <= 12)
                packed_scramble_copy_reverse8_fma3(dst, src, rank-4);
            else
                packed_scramble_copy_reverse16_fma3(dst, src, rank-4);
        }

        for (size_t i=3; i < rank; ++i)
            packed_butterfly_reverse8p_fma3(dst, i, 1 << (rank - i - 1));

        packed_fft_repack_normalize(dst, rank);
    }
}

#endif /* DSP_ARCH_X86_AVX_PFFT_H_ */
