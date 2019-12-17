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

#define DSP_ARCH_X86_SSE_FASTCONV_H_IMPL

#include <dsp/arch/x86/sse/fastconv/parse.h>
#include <dsp/arch/x86/sse/fastconv/papply.h>
#include <dsp/arch/x86/sse/fastconv/apply.h>
#include <dsp/arch/x86/sse/fastconv/irestore.h>
#include <dsp/arch/x86/sse/fastconv/restore.h>

namespace sse
{
    void fastconv_parse_apply(float *dst, float *tmp, const float *c, const float *src, size_t rank)
    {
        // Do direct FFT
        fastconv_parse_internal(tmp, src, rank);

        // Apply complex convolution
        fastconv_parse_apply_internal(tmp, c, rank);

        // Do reverse FFT
        fastconv_restore_internal(dst, tmp, rank);
    }

    void fastconv_apply(float *dst, float *tmp, const float *c1, const float *c2, size_t rank)
    {
        // Apply complex convolution
        fastconv_apply_internal(tmp, c1, c2, rank);

        // Do reverse FFT
        fastconv_restore_internal(dst, tmp, rank);
    }
}

#undef DSP_ARCH_X86_SSE_FASTCONV_H_IMPL

#endif /* DSP_ARCH_X86_SSE_FASTCONV_H_ */
