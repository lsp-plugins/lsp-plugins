/*
 * fft.h
 *
 *  Created on: 05 окт. 2015 г.
 *      Author: sadko
 */

#ifndef CORE_X86_SSE_FFT_H_
#define CORE_X86_SSE_FFT_H_

// Make set of butterfly implementations
#define FFT_BUTTERFLY_DIRECT_NAME       butterfly_direct_aa
#define FFT_BUTTERFLY_REVERSE_NAME      butterfly_reverse_aa
#define LS_RE                           "movaps"
#define LS_IM                           "movaps"
#include <core/x86/sse/fft/butterfly.h>

#define FFT_BUTTERFLY_DIRECT_NAME       butterfly_direct_au
#define FFT_BUTTERFLY_REVERSE_NAME      butterfly_reverse_au
#define LS_RE                           "movaps"
#define LS_IM                           "movups"
#include <core/x86/sse/fft/butterfly.h>

#define FFT_BUTTERFLY_DIRECT_NAME       butterfly_direct_ua
#define FFT_BUTTERFLY_REVERSE_NAME      butterfly_reverse_ua
#define LS_RE                           "movups"
#define LS_IM                           "movaps"
#include <core/x86/sse/fft/butterfly.h>

#define FFT_BUTTERFLY_DIRECT_NAME       butterfly_direct_uu
#define FFT_BUTTERFLY_REVERSE_NAME      butterfly_reverse_uu
#define LS_RE                           "movups"
#define LS_IM                           "movups"
#include <core/x86/sse/fft/butterfly.h>

// Make set of scramble implementations
// Use 8-bit reversive algorithm
#define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct8_aa
#define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse8_aa
#define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct8_aa
#define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse8_aa
#define FFT_TYPE                        uint8_t
#define LS_RE                           "movaps"
#define LS_IM                           "movaps"
#include <core/x86/sse/fft/scramble.h>

#define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct8_au
#define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse8_au
#define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct8_au
#define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse8_au
#define FFT_TYPE                        uint8_t
#define LS_RE                           "movaps"
#define LS_IM                           "movups"
#include <core/x86/sse/fft/scramble.h>

#define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct8_ua
#define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse8_ua
#define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct8_ua
#define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse8_ua
#define FFT_TYPE                        uint8_t
#define LS_RE                           "movups"
#define LS_IM                           "movaps"
#include <core/x86/sse/fft/scramble.h>

#define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct8_uu
#define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse8_uu
#define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct8_uu
#define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse8_uu
#define FFT_TYPE                        uint8_t
#define LS_RE                           "movups"
#define LS_IM                           "movups"
#include <core/x86/sse/fft/scramble.h>

// Use 16-bit reversive algorithm
#define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct16_aa
#define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse16_aa
#define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct16_aa
#define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse16_aa
#define FFT_TYPE                        uint16_t
#define LS_RE                           "movaps"
#define LS_IM                           "movaps"
#include <core/x86/sse/fft/scramble.h>

#define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct16_au
#define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse16_au
#define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct16_au
#define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse16_au
#define FFT_TYPE                        uint16_t
#define LS_RE                           "movaps"
#define LS_IM                           "movups"
#include <core/x86/sse/fft/scramble.h>

#define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct16_ua
#define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse16_ua
#define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct16_ua
#define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse16_ua
#define FFT_TYPE                        uint16_t
#define LS_RE                           "movups"
#define LS_IM                           "movaps"
#include <core/x86/sse/fft/scramble.h>

#define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct16_uu
#define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse16_uu
#define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct16_uu
#define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse16_uu
#define FFT_TYPE                        uint16_t
#define LS_RE                           "movups"
#define LS_IM                           "movups"
#include <core/x86/sse/fft/scramble.h>

// Use 32-bit reversive algorithm
#define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct32_aa
#define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse32_aa
#define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct32_aa
#define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse32_aa
#define FFT_TYPE                        uint32_t
#define LS_RE                           "movaps"
#define LS_IM                           "movaps"
#include <core/x86/sse/fft/scramble.h>

#define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct32_au
#define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse32_au
#define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct32_au
#define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse32_au
#define FFT_TYPE                        uint32_t
#define LS_RE                           "movaps"
#define LS_IM                           "movups"
#include <core/x86/sse/fft/scramble.h>

#define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct32_ua
#define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse32_ua
#define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct32_ua
#define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse32_ua
#define FFT_TYPE                        uint32_t
#define LS_RE                           "movups"
#define LS_IM                           "movaps"
#include <core/x86/sse/fft/scramble.h>

#define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct32_uu
#define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse32_uu
#define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct32_uu
#define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse32_uu
#define FFT_TYPE                        uint32_t
#define LS_RE                           "movups"
#define LS_IM                           "movups"
#include <core/x86/sse/fft/scramble.h>

// Use 64-bit reversive algorithm
#ifdef __x86_64__
    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct64_aa
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse64_aa
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct64_aa
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse64_aa
    #define FFT_TYPE                        uint64_t
    #define LS_RE                           "movaps"
    #define LS_IM                           "movaps"
    #include <core/x86/sse/fft/scramble.h>

    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct64_au
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse64_au
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct64_au
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse64_au
    #define FFT_TYPE                        uint64_t
    #define LS_RE                           "movaps"
    #define LS_IM                           "movups"
    #include <core/x86/sse/fft/scramble.h>

    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct64_ua
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse64_ua
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct64_ua
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse64_ua
    #define FFT_TYPE                        uint64_t
    #define LS_RE                           "movups"
    #define LS_IM                           "movaps"
    #include <core/x86/sse/fft/scramble.h>

    #define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct64_uu
    #define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse64_uu
    #define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct64_uu
    #define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse64_uu
    #define FFT_TYPE                        uint64_t
    #define LS_RE                           "movups"
    #define LS_IM                           "movups"
    #include <core/x86/sse/fft/scramble.h>
#endif /* __x86_64__ */

// Make set of scramble-switch implementations
#define FFT_SCRAMBLE_SELF_DIRECT_NAME       scramble_self_direct
#define FFT_SCRAMBLE_COPY_DIRECT_NAME       scramble_copy_direct
#define FFT_SCRAMBLE_SELF_REVERSE_NAME      scramble_self_reverse
#define FFT_SCRAMBLE_COPY_REVERSE_NAME      scramble_copy_reverse
#define FFT_SCRAMBLE_DIRECT_NAME            scramble_direct
#define FFT_SCRAMBLE_REVERSE_NAME           scramble_reverse
#define FFT_MODE                            aa
#include <core/x86/sse/fft/switch.h>

#define FFT_SCRAMBLE_SELF_DIRECT_NAME       scramble_self_direct
#define FFT_SCRAMBLE_COPY_DIRECT_NAME       scramble_copy_direct
#define FFT_SCRAMBLE_SELF_REVERSE_NAME      scramble_self_reverse
#define FFT_SCRAMBLE_COPY_REVERSE_NAME      scramble_copy_reverse
#define FFT_SCRAMBLE_DIRECT_NAME            scramble_direct
#define FFT_SCRAMBLE_REVERSE_NAME           scramble_reverse
#define FFT_MODE                            au
#include <core/x86/sse/fft/switch.h>

#define FFT_SCRAMBLE_SELF_DIRECT_NAME       scramble_self_direct
#define FFT_SCRAMBLE_COPY_DIRECT_NAME       scramble_copy_direct
#define FFT_SCRAMBLE_SELF_REVERSE_NAME      scramble_self_reverse
#define FFT_SCRAMBLE_COPY_REVERSE_NAME      scramble_copy_reverse
#define FFT_SCRAMBLE_DIRECT_NAME            scramble_direct
#define FFT_SCRAMBLE_REVERSE_NAME           scramble_reverse
#define FFT_MODE                            ua
#include <core/x86/sse/fft/switch.h>

#define FFT_SCRAMBLE_SELF_DIRECT_NAME       scramble_self_direct
#define FFT_SCRAMBLE_COPY_DIRECT_NAME       scramble_copy_direct
#define FFT_SCRAMBLE_SELF_REVERSE_NAME      scramble_self_reverse
#define FFT_SCRAMBLE_COPY_REVERSE_NAME      scramble_copy_reverse
#define FFT_SCRAMBLE_DIRECT_NAME            scramble_direct
#define FFT_SCRAMBLE_REVERSE_NAME           scramble_reverse
#define FFT_MODE                            uu
#include <core/x86/sse/fft/switch.h>

namespace lsp
{
    namespace sse
    {
        static void normalize_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);

        static void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
        {
            // Check bounds
            if (rank <= 1)
            {
                if (rank == 1)
                {
                    // s0' = s0 + s1
                    // s1' = s0 - s1
                    float s1_re     = dst_re[1];
                    float s1_im     = dst_im[1];
                    dst_re[1]       = dst_re[0] - s1_re;
                    dst_im[1]       = dst_im[0] - s1_im;
                    dst_re[0]       = dst_re[0] + s1_re;
                    dst_im[0]       = dst_im[0] + s1_im;
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
                        butterfly_direct_aa(dst_re, dst_im, 1 << i, 1 << (rank - i - 1));
                }
                else
                {
                    scramble_direct_au(dst_re, dst_im, src_re, src_im, rank);

                    for (size_t i=2; i < rank; ++i)
                        butterfly_direct_au(dst_re, dst_im, 1 << i, 1 << (rank - i - 1));
                }
            }
            else
            {
                if (sse_aligned(dst_im))
                {
                    scramble_direct_ua(dst_re, dst_im, src_re, src_im, rank);

                    for (size_t i=2; i < rank; ++i)
                        butterfly_direct_ua(dst_re, dst_im, 1 << i, 1 << (rank - i - 1));
                }
                else
                {
                    scramble_direct_uu(dst_re, dst_im, src_re, src_im, rank);

                    for (size_t i=2; i < rank; ++i)
                        butterfly_direct_uu(dst_re, dst_im, 1 << i, 1 << (rank - i - 1));
                }
            }
        }

        static void reverse_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
        {
            // Check bounds
            if (rank <= 1)
            {
                if (rank == 1)
                {
                    // s0' = s0 + s1
                    // s1' = s0 - s1
                    float s1_re     = dst_re[1];
                    float s1_im     = dst_im[1];
                    dst_re[1]       = (dst_re[0] - s1_re) * 0.5f;
                    dst_im[1]       = (dst_im[0] - s1_im) * 0.5f;
                    dst_re[0]       = (dst_re[0] + s1_re) * 0.5f;
                    dst_im[0]       = (dst_im[0] + s1_im) * 0.5f;
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
                        butterfly_reverse_aa(dst_re, dst_im, 1 << i, 1 << (rank - i - 1));
                }
                else
                {
                    scramble_reverse_au(dst_re, dst_im, src_re, src_im, rank);

                    for (size_t i=2; i < rank; ++i)
                        butterfly_reverse_au(dst_re, dst_im, 1 << i, 1 << (rank - i - 1));
                }
            }
            else
            {
                if (sse_aligned(dst_im))
                {
                    scramble_reverse_ua(dst_re, dst_im, src_re, src_im, rank);

                    for (size_t i=2; i < rank; ++i)
                        butterfly_reverse_ua(dst_re, dst_im, 1 << i, 1 << (rank - i - 1));
                }
                else
                {
                    scramble_reverse_uu(dst_re, dst_im, src_re, src_im, rank);

                    for (size_t i=2; i < rank; ++i)
                        butterfly_reverse_uu(dst_re, dst_im, 1 << i, 1 << (rank - i - 1));
                }
            }

            normalize_fft(dst_re, dst_im, dst_re, dst_im, rank);
        }
    }
}

#endif /* CORE_X86_SSE_FFT_H_ */
