/*
 * fastconv.h
 *
 *  Created on: 5 мар. 2017 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE_FASTCONV_H_
#define DSP_ARCH_X86_SSE_FASTCONV_H_

#ifndef DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE_IMPL */

namespace sse
{
    #define DSP_ARCH_X86_SSE_FASTCONV_H_IMPL

    // Parse implementation
    #define PARSE_IMPL              fastconv_parse_aa
    #define PARSE_INTERNAL_IMPL     fastconv_parse_internal_aa
    #define MV_DST                  "movaps"
    #define MV_SRC                  "movaps"
    #include <dsp/arch/x86/sse/fastconv/parse.h>

    #define PARSE_IMPL              fastconv_parse_au
    #define PARSE_INTERNAL_IMPL     fastconv_parse_internal_au
    #define MV_DST                  "movaps"
    #define MV_SRC                  "movups"
    #include <dsp/arch/x86/sse/fastconv/parse.h>

    #define PARSE_IMPL              fastconv_parse_ua
    #define PARSE_INTERNAL_IMPL     fastconv_parse_internal_ua
    #define MV_DST                  "movups"
    #define MV_SRC                  "movaps"
    #include <dsp/arch/x86/sse/fastconv/parse.h>

    #define PARSE_IMPL              fastconv_parse_uu
    #define PARSE_INTERNAL_IMPL     fastconv_parse_internal_uu
    #define MV_DST                  "movups"
    #define MV_SRC                  "movups"
    #include <dsp/arch/x86/sse/fastconv/parse.h>

    // Internal apply implementation
    #define APPLY_INTERNAL_IMPL     fastconv_parse_apply_internal_aa
    #define MV_TMP                  "movaps"
    #define MV_CONV                 "movaps"
    #include <dsp/arch/x86/sse/fastconv/papply.h>

    #define APPLY_INTERNAL_IMPL     fastconv_parse_apply_internal_au
    #define MV_TMP                  "movaps"
    #define MV_CONV                 "movups"
    #include <dsp/arch/x86/sse/fastconv/papply.h>

    #define APPLY_INTERNAL_IMPL     fastconv_parse_apply_internal_ua
    #define MV_TMP                  "movups"
    #define MV_CONV                 "movaps"
    #include <dsp/arch/x86/sse/fastconv/papply.h>

    #define APPLY_INTERNAL_IMPL     fastconv_parse_apply_internal_uu
    #define MV_TMP                  "movups"
    #define MV_CONV                 "movups"
    #include <dsp/arch/x86/sse/fastconv/papply.h>

    // Internal apply implementation
    #define APPLY_IMPL              fastconv_apply_internal_aaa
    #define MV_TMP                  "movaps"
    #define MV_C1                   "movaps"
    #define MV_C2                   "movaps"
    #include <dsp/arch/x86/sse/fastconv/apply.h>

    #define APPLY_IMPL              fastconv_apply_internal_aau
    #define MV_TMP                  "movaps"
    #define MV_C1                   "movaps"
    #define MV_C2                   "movups"
    #include <dsp/arch/x86/sse/fastconv/apply.h>

    #define APPLY_IMPL              fastconv_apply_internal_aua
    #define MV_TMP                  "movaps"
    #define MV_C1                   "movups"
    #define MV_C2                   "movaps"
    #include <dsp/arch/x86/sse/fastconv/apply.h>

    #define APPLY_IMPL              fastconv_apply_internal_auu
    #define MV_TMP                  "movaps"
    #define MV_C1                   "movups"
    #define MV_C2                   "movups"
    #include <dsp/arch/x86/sse/fastconv/apply.h>

    #define APPLY_IMPL              fastconv_apply_internal_uaa
    #define MV_TMP                  "movups"
    #define MV_C1                   "movaps"
    #define MV_C2                   "movaps"
    #include <dsp/arch/x86/sse/fastconv/apply.h>

    #define APPLY_IMPL              fastconv_apply_internal_uau
    #define MV_TMP                  "movups"
    #define MV_C1                   "movaps"
    #define MV_C2                   "movups"
    #include <dsp/arch/x86/sse/fastconv/apply.h>

    #define APPLY_IMPL              fastconv_apply_internal_uua
    #define MV_TMP                  "movups"
    #define MV_C1                   "movups"
    #define MV_C2                   "movaps"
    #include <dsp/arch/x86/sse/fastconv/apply.h>

    #define APPLY_IMPL              fastconv_apply_internal_uuu
    #define MV_TMP                  "movups"
    #define MV_C1                   "movups"
    #define MV_C2                   "movups"
    #include <dsp/arch/x86/sse/fastconv/apply.h>

    // Internal restore implementation
    #define RESTORE_INTERNAL_IMPL   fastconv_restore_internal_aa
    #define MV_DST                  "movaps"
    #define MV_TMP                  "movaps"
    #include <dsp/arch/x86/sse/fastconv/irestore.h>

    #define RESTORE_INTERNAL_IMPL   fastconv_restore_internal_au
    #define MV_DST                  "movaps"
    #define MV_TMP                  "movups"
    #include <dsp/arch/x86/sse/fastconv/irestore.h>

    #define RESTORE_INTERNAL_IMPL   fastconv_restore_internal_ua
    #define MV_DST                  "movups"
    #define MV_TMP                  "movaps"
    #include <dsp/arch/x86/sse/fastconv/irestore.h>

    #define RESTORE_INTERNAL_IMPL   fastconv_restore_internal_uu
    #define MV_DST                  "movups"
    #define MV_TMP                  "movups"
    #include <dsp/arch/x86/sse/fastconv/irestore.h>

    // Restore implementation
    #define RESTORE_IMPL            fastconv_restore_aa
    #define MV_DST                  "movaps"
    #define MV_TMP                  "movaps"
    #include <dsp/arch/x86/sse/fastconv/restore.h>

    #define RESTORE_IMPL            fastconv_restore_au
    #define MV_DST                  "movaps"
    #define MV_TMP                  "movups"
    #include <dsp/arch/x86/sse/fastconv/restore.h>

    #define RESTORE_IMPL            fastconv_restore_ua
    #define MV_DST                  "movups"
    #define MV_TMP                  "movaps"
    #include <dsp/arch/x86/sse/fastconv/restore.h>

    #define RESTORE_IMPL            fastconv_restore_uu
    #define MV_DST                  "movups"
    #define MV_TMP                  "movups"
    #include <dsp/arch/x86/sse/fastconv/restore.h>


    #undef DSP_ARCH_X86_SSE_FASTCONV_H_IMPL

    void fastconv_parse(float *dst, const float *src, size_t rank)
    {
        if (sse_aligned(dst))
        {
            if (sse_aligned(src))
                fastconv_parse_aa(dst, src, rank);
            else
                fastconv_parse_au(dst, src, rank);
        }
        else
        {
            if (sse_aligned(src))
                fastconv_parse_ua(dst, src, rank);
            else
                fastconv_parse_uu(dst, src, rank);
        }
    }

    void fastconv_parse_apply(float *dst, float *tmp, const float *c, const float *src, size_t rank)
    {
        // Parse convolution data
        if (sse_aligned(tmp))
        {
            // Do direct FFT
            if (sse_aligned(src))
                fastconv_parse_internal_aa(tmp, src, rank);
            else
                fastconv_parse_internal_au(tmp, src, rank);

            // Apply complex convolution
            if (sse_aligned(c))
                fastconv_parse_apply_internal_aa(tmp, c, rank);
            else
                fastconv_parse_apply_internal_au(tmp, c, rank);

            // Do reverse FFT
            if (sse_aligned(dst))
                fastconv_restore_internal_aa(dst, tmp, rank);
            else
                fastconv_restore_internal_ua(dst, tmp, rank);
        }
        else
        {
            // Do direct FFT
            if (sse_aligned(src))
                fastconv_parse_internal_ua(tmp, src, rank);
            else
                fastconv_parse_internal_uu(tmp, src, rank);

            // Apply complex convolution
            if (sse_aligned(c))
                fastconv_parse_apply_internal_ua(tmp, c, rank);
            else
                fastconv_parse_apply_internal_uu(tmp, c, rank);

            // Do reverse FFT
            if (sse_aligned(dst))
                fastconv_restore_internal_au(dst, tmp, rank);
            else
                fastconv_restore_internal_uu(dst, tmp, rank);
        }
    }

    void fastconv_apply(float *dst, float *tmp, const float *c1, const float *c2, size_t rank)
    {
        // Parse convolution data
        if (sse_aligned(tmp))
        {
            // Apply complex convolution
            if (sse_aligned(c1))
            {
                if (sse_aligned(c2))
                    fastconv_apply_internal_aaa(tmp, c1, c2, rank);
                else
                    fastconv_apply_internal_aau(tmp, c1, c2, rank);
            }
            else
            {
                if (sse_aligned(c2))
                    fastconv_apply_internal_aua(tmp, c1, c2, rank);
                else
                    fastconv_apply_internal_auu(tmp, c1, c2, rank);
            }

            // Do reverse FFT
            if (sse_aligned(dst))
                fastconv_restore_internal_aa(dst, tmp, rank);
            else
                fastconv_restore_internal_ua(dst, tmp, rank);
        }
        else
        {
            // Apply complex convolution
            if (sse_aligned(c1))
            {
                if (sse_aligned(c2))
                    fastconv_apply_internal_uaa(tmp, c1, c2, rank);
                else
                    fastconv_apply_internal_uau(tmp, c1, c2, rank);
            }
            else
            {
                if (sse_aligned(c2))
                    fastconv_apply_internal_uua(tmp, c1, c2, rank);
                else
                    fastconv_apply_internal_uuu(tmp, c1, c2, rank);
            }

            // Do reverse FFT
            if (sse_aligned(dst))
                fastconv_restore_internal_au(dst, tmp, rank);
            else
                fastconv_restore_internal_uu(dst, tmp, rank);
        }
    }

    void fastconv_restore(float *dst, float *src, size_t rank)
    {
        if (sse_aligned(dst))
        {
            if (sse_aligned(src))
                fastconv_restore_aa(dst, src, rank);
            else
                fastconv_restore_au(dst, src, rank);
        }
        else
        {
            if (sse_aligned(src))
                fastconv_restore_ua(dst, src, rank);
            else
                fastconv_restore_uu(dst, src, rank);
        }
    }
}

#endif /* DSP_ARCH_X86_SSE_FASTCONV_H_ */
