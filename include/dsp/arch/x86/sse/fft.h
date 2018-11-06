/*
 * fft.h
 *
 *  Created on: 05 окт. 2015 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE_FFT_H_
#define DSP_ARCH_X86_SSE_FFT_H_

#ifndef DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE_IMPL */

#include <dsp/arch/x86/sse/fft/const.h>

namespace sse
{
    // Make set of butterfly implementations
    #define FFT_BUTTERFLY_DIRECT_NAME       butterfly_direct_aa
    #define FFT_BUTTERFLY_REVERSE_NAME      butterfly_reverse_aa
    #define LS_RE                           "movaps"
    #define LS_IM                           "movaps"
    #include <dsp/arch/x86/sse/fft/butterfly.h>

    #define FFT_BUTTERFLY_DIRECT_NAME       butterfly_direct_au
    #define FFT_BUTTERFLY_REVERSE_NAME      butterfly_reverse_au
    #define LS_RE                           "movaps"
    #define LS_IM                           "movups"
    #include <dsp/arch/x86/sse/fft/butterfly.h>

    #define FFT_BUTTERFLY_DIRECT_NAME       butterfly_direct_ua
    #define FFT_BUTTERFLY_REVERSE_NAME      butterfly_reverse_ua
    #define LS_RE                           "movups"
    #define LS_IM                           "movaps"
    #include <dsp/arch/x86/sse/fft/butterfly.h>

    #define FFT_BUTTERFLY_DIRECT_NAME       butterfly_direct_uu
    #define FFT_BUTTERFLY_REVERSE_NAME      butterfly_reverse_uu
    #define LS_RE                           "movups"
    #define LS_IM                           "movups"
    #include <dsp/arch/x86/sse/fft/butterfly.h>

    #define FFT_BUTTERFLY_DIRECT_NAME       packed_butterfly_direct_a
    #define FFT_BUTTERFLY_REVERSE_NAME      packed_butterfly_reverse_a
    #define LS_RE                           "movaps"
    #include <dsp/arch/x86/sse/fft/p_butterfly.h>

    #define FFT_BUTTERFLY_DIRECT_NAME       packed_butterfly_direct_u
    #define FFT_BUTTERFLY_REVERSE_NAME      packed_butterfly_reverse_u
    #define LS_RE                           "movups"
    #include <dsp/arch/x86/sse/fft/p_butterfly.h>

    // Make set of scramble implementations
    // Use 8-bit reversive algorithm
    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct8_aa
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse8_aa
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct8_aa
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse8_aa
    #define FFT_TYPE                        uint8_t
    #define LS_RE                           "movaps"
    #define LS_IM                           "movaps"
    #include <dsp/arch/x86/sse/fft/scramble.h>

    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct8_au
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse8_au
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct8_au
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse8_au
    #define FFT_TYPE                        uint8_t
    #define LS_RE                           "movaps"
    #define LS_IM                           "movups"
    #include <dsp/arch/x86/sse/fft/scramble.h>

    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct8_ua
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse8_ua
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct8_ua
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse8_ua
    #define FFT_TYPE                        uint8_t
    #define LS_RE                           "movups"
    #define LS_IM                           "movaps"
    #include <dsp/arch/x86/sse/fft/scramble.h>

    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct8_uu
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse8_uu
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct8_uu
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse8_uu
    #define FFT_TYPE                        uint8_t
    #define LS_RE                           "movups"
    #define LS_IM                           "movups"
    #include <dsp/arch/x86/sse/fft/scramble.h>

    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   packed_scramble_self_direct8_a
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  packed_scramble_self_reverse8_a
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   packed_scramble_copy_direct8_a
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  packed_scramble_copy_reverse8_a
    #define FFT_TYPE                        uint8_t
    #define LS_RE                           "movaps"
    #include <dsp/arch/x86/sse/fft/p_scramble.h>

    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   packed_scramble_self_direct8_u
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  packed_scramble_self_reverse8_u
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   packed_scramble_copy_direct8_u
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  packed_scramble_copy_reverse8_u
    #define FFT_TYPE                        uint8_t
    #define LS_RE                           "movups"
    #include <dsp/arch/x86/sse/fft/p_scramble.h>

    // Use 16-bit reversive algorithm
    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct16_aa
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse16_aa
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct16_aa
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse16_aa
    #define FFT_TYPE                        uint16_t
    #define LS_RE                           "movaps"
    #define LS_IM                           "movaps"
    #include <dsp/arch/x86/sse/fft/scramble.h>

    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct16_au
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse16_au
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct16_au
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse16_au
    #define FFT_TYPE                        uint16_t
    #define LS_RE                           "movaps"
    #define LS_IM                           "movups"
    #include <dsp/arch/x86/sse/fft/scramble.h>

    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct16_ua
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse16_ua
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct16_ua
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse16_ua
    #define FFT_TYPE                        uint16_t
    #define LS_RE                           "movups"
    #define LS_IM                           "movaps"
    #include <dsp/arch/x86/sse/fft/scramble.h>

    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct16_uu
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse16_uu
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct16_uu
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse16_uu
    #define FFT_TYPE                        uint16_t
    #define LS_RE                           "movups"
    #define LS_IM                           "movups"
    #include <dsp/arch/x86/sse/fft/scramble.h>

    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   packed_scramble_self_direct16_a
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  packed_scramble_self_reverse16_a
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   packed_scramble_copy_direct16_a
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  packed_scramble_copy_reverse16_a
    #define FFT_TYPE                        uint16_t
    #define LS_RE                           "movaps"
    #include <dsp/arch/x86/sse/fft/p_scramble.h>

    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   packed_scramble_self_direct16_u
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  packed_scramble_self_reverse16_u
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   packed_scramble_copy_direct16_u
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  packed_scramble_copy_reverse16_u
    #define FFT_TYPE                        uint16_t
    #define LS_RE                           "movups"
    #include <dsp/arch/x86/sse/fft/p_scramble.h>

    //// Use 32-bit reversive algorithm
    //#define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct32_aa
    //#define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse32_aa
    //#define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct32_aa
    //#define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse32_aa
    //#define FFT_TYPE                        uint32_t
    //#define LS_RE                           "movaps"
    //#define LS_IM                           "movaps"
    //#include <dsp/arch/x86/sse/fft/scramble.h>
    //
    //#define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct32_au
    //#define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse32_au
    //#define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct32_au
    //#define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse32_au
    //#define FFT_TYPE                        uint32_t
    //#define LS_RE                           "movaps"
    //#define LS_IM                           "movups"
    //#include <dsp/arch/x86/sse/fft/scramble.h>
    //
    //#define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct32_ua
    //#define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse32_ua
    //#define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct32_ua
    //#define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse32_ua
    //#define FFT_TYPE                        uint32_t
    //#define LS_RE                           "movups"
    //#define LS_IM                           "movaps"
    //#include <dsp/arch/x86/sse/fft/scramble.h>
    //
    //#define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct32_uu
    //#define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse32_uu
    //#define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct32_uu
    //#define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse32_uu
    //#define FFT_TYPE                        uint32_t
    //#define LS_RE                           "movups"
    //#define LS_IM                           "movups"
    //#include <dsp/arch/x86/sse/fft/scramble.h>
    //
    //// Use 64-bit reversive algorithm
    //#ifdef __x86_64__
    //    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct64_aa
    //    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse64_aa
    //    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct64_aa
    //    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse64_aa
    //    #define FFT_TYPE                        uint64_t
    //    #define LS_RE                           "movaps"
    //    #define LS_IM                           "movaps"
    //    #include <dsp/arch/x86/sse/fft/scramble.h>
    //
    //    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct64_au
    //    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse64_au
    //    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct64_au
    //    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse64_au
    //    #define FFT_TYPE                        uint64_t
    //    #define LS_RE                           "movaps"
    //    #define LS_IM                           "movups"
    //    #include <dsp/arch/x86/sse/fft/scramble.h>
    //
    //    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct64_ua
    //    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse64_ua
    //    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct64_ua
    //    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse64_ua
    //    #define FFT_TYPE                        uint64_t
    //    #define LS_RE                           "movups"
    //    #define LS_IM                           "movaps"
    //    #include <dsp/arch/x86/sse/fft/scramble.h>
    //
    //    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct64_uu
    //    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse64_uu
    //    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct64_uu
    //    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse64_uu
    //    #define FFT_TYPE                        uint64_t
    //    #define LS_RE                           "movups"
    //    #define LS_IM                           "movups"
    //    #include <dsp/arch/x86/sse/fft/scramble.h>
    //#endif /* __x86_64__ */

    // Make set of scramble-switch implementations
    #define FFT_SCRAMBLE_SELF_DIRECT_NAME       scramble_self_direct
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME       scramble_copy_direct
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME      scramble_self_reverse
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME      scramble_copy_reverse
    #define FFT_SCRAMBLE_DIRECT_NAME            scramble_direct
    #define FFT_SCRAMBLE_REVERSE_NAME           scramble_reverse
    #define FFT_MODE                            aa
    #include <dsp/arch/x86/sse/fft/switch.h>

    #define FFT_SCRAMBLE_SELF_DIRECT_NAME       scramble_self_direct
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME       scramble_copy_direct
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME      scramble_self_reverse
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME      scramble_copy_reverse
    #define FFT_SCRAMBLE_DIRECT_NAME            scramble_direct
    #define FFT_SCRAMBLE_REVERSE_NAME           scramble_reverse
    #define FFT_MODE                            au
    #include <dsp/arch/x86/sse/fft/switch.h>

    #define FFT_SCRAMBLE_SELF_DIRECT_NAME       scramble_self_direct
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME       scramble_copy_direct
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME      scramble_self_reverse
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME      scramble_copy_reverse
    #define FFT_SCRAMBLE_DIRECT_NAME            scramble_direct
    #define FFT_SCRAMBLE_REVERSE_NAME           scramble_reverse
    #define FFT_MODE                            ua
    #include <dsp/arch/x86/sse/fft/switch.h>

    #define FFT_SCRAMBLE_SELF_DIRECT_NAME       scramble_self_direct
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME       scramble_copy_direct
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME      scramble_self_reverse
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME      scramble_copy_reverse
    #define FFT_SCRAMBLE_DIRECT_NAME            scramble_direct
    #define FFT_SCRAMBLE_REVERSE_NAME           scramble_reverse
    #define FFT_MODE                            uu
    #include <dsp/arch/x86/sse/fft/switch.h>

    #define FFT_SCRAMBLE_SELF_DIRECT_NAME       packed_scramble_self_direct
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME       packed_scramble_copy_direct
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME      packed_scramble_self_reverse
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME      packed_scramble_copy_reverse
    #define FFT_SCRAMBLE_DIRECT_NAME            packed_scramble_direct
    #define FFT_SCRAMBLE_REVERSE_NAME           packed_scramble_reverse
    #define FFT_REPACK                          packed_fft_repack
    #define FFT_REPACK_NORMALIZE                packed_fft_repack_normalize
    #define LS_RE                               "movaps"
    #define FFT_MODE                            a
    #include <dsp/arch/x86/sse/fft/p_switch.h>

    #define FFT_SCRAMBLE_SELF_DIRECT_NAME       packed_scramble_self_direct
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME       packed_scramble_copy_direct
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME      packed_scramble_self_reverse
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME      packed_scramble_copy_reverse
    #define FFT_SCRAMBLE_DIRECT_NAME            packed_scramble_direct
    #define FFT_SCRAMBLE_REVERSE_NAME           packed_scramble_reverse
    #define FFT_REPACK                          packed_fft_repack
    #define FFT_REPACK_NORMALIZE                packed_fft_repack_normalize
    #define LS_RE                               "movups"
    #define FFT_MODE                            u
    #include <dsp/arch/x86/sse/fft/p_switch.h>


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

        // Iterate butterflies
        if (sse_aligned(dst_re))
        {
            if (sse_aligned(dst_im))
            {
                scramble_direct_aa(dst_re, dst_im, src_re, src_im, rank);

                for (size_t i=2; i < rank; ++i)
                    butterfly_direct_aa(dst_re, dst_im, i /*1 << i*/, 1 << (rank - i - 1));
            }
            else
            {
                scramble_direct_au(dst_re, dst_im, src_re, src_im, rank);

                for (size_t i=2; i < rank; ++i)
                    butterfly_direct_au(dst_re, dst_im, i /*1 << i*/, 1 << (rank - i - 1));
            }
        }
        else
        {
            if (sse_aligned(dst_im))
            {
                scramble_direct_ua(dst_re, dst_im, src_re, src_im, rank);

                for (size_t i=2; i < rank; ++i)
                    butterfly_direct_ua(dst_re, dst_im, i /*1 << i*/, 1 << (rank - i - 1));
            }
            else
            {
                scramble_direct_uu(dst_re, dst_im, src_re, src_im, rank);

                for (size_t i=2; i < rank; ++i)
                    butterfly_direct_uu(dst_re, dst_im, i /*1 << i*/, 1 << (rank - i - 1));
            }
        }
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

        // Iterate butterflies
        if (sse_aligned(dst))
        {
            packed_scramble_direct_a(dst, src, rank);

            for (size_t i=2; i < rank; ++i)
                packed_butterfly_direct_a(dst, i /*1 << i*/, 1 << (rank - i - 1));

            packed_fft_repack_a(dst, rank);
        }
        else
        {
            packed_scramble_direct_u(dst, src, rank);

            for (size_t i=2; i < rank; ++i)
                packed_butterfly_direct_u(dst, i /*1 << i*/, 1 << (rank - i - 1));

            packed_fft_repack_u(dst, rank);
        }
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

        // Iterate butterflies
        if (sse_aligned(dst_re))
        {
            if (sse_aligned(dst_im))
            {
                scramble_reverse_aa(dst_re, dst_im, src_re, src_im, rank);

                for (size_t i=2; i < rank; ++i)
                    butterfly_reverse_aa(dst_re, dst_im, i, 1 << (rank - i - 1));
            }
            else
            {
                scramble_reverse_au(dst_re, dst_im, src_re, src_im, rank);

                for (size_t i=2; i < rank; ++i)
                    butterfly_reverse_au(dst_re, dst_im, i, 1 << (rank - i - 1));
            }
        }
        else
        {
            if (sse_aligned(dst_im))
            {
                scramble_reverse_ua(dst_re, dst_im, src_re, src_im, rank);

                for (size_t i=2; i < rank; ++i)
                    butterfly_reverse_ua(dst_re, dst_im, i, 1 << (rank - i - 1));
            }
            else
            {
                scramble_reverse_uu(dst_re, dst_im, src_re, src_im, rank);

                for (size_t i=2; i < rank; ++i)
                    butterfly_reverse_uu(dst_re, dst_im, i, 1 << (rank - i - 1));
            }
        }

        dsp::normalize_fft2(dst_re, dst_im, rank);
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

        // Iterate butterflies
        if (sse_aligned(dst))
        {
            packed_scramble_reverse_a(dst, src, rank);

            for (size_t i=2; i < rank; ++i)
                packed_butterfly_reverse_a(dst, i /*1 << i*/, 1 << (rank - i - 1));

            packed_fft_repack_normalize_a(dst, rank);
        }
        else
        {
            packed_scramble_reverse_u(dst, src, rank);

            for (size_t i=2; i < rank; ++i)
                packed_butterfly_reverse_u(dst, i /*1 << i*/, 1 << (rank - i - 1));

            packed_fft_repack_normalize_u(dst, rank);
        }
    }
}

#endif /* DSP_ARCH_X86_SSE_FFT_H_ */
