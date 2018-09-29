/*
 * native.h
 *
 *  Created on: 05 окт. 2015 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_X86_SSE_NATIVE_H_
#define DSP_ARCH_X86_SSE_NATIVE_H_

#ifndef DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* DSP_ARCH_X86_SSE_IMPL */

namespace sse
{
    void normalize(float *dst, const float *src, size_t count)
    {
        // Find minimum and maximum
        float max = 0.0f, min = 0.0f;
        minmax(src, count, &min, &max);

        // Determine maximum possible value
        if (max < 0.0f)
            max     = - max;
        if (min < 0.0f)
            min     = - min;
        if (max < min)
            max = min;

        // Normalize OR copy
        if (max > 0.0f)
            scale3(dst, src, 1.0f / max, count);
        else
            copy(dst, src, count);
    }

    void abs_normalized(float *dst, const float *src, size_t count)
    {
        // Calculate absolute values
        abs2(dst, src, count);

        // Find the maximum value
        float maxv = max(dst, count);

        // Divide if it is possible
        if (maxv != 0.0f)
            scale2(dst, 1.0f / maxv, count);
    }

    void normalize_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
    {
        rank            = 1 << rank;
        float k         = 1.0f / rank;
        scale3(dst_re, src_re, k, rank);
        scale3(dst_im, src_im, k, rank);
    }
}

#endif /* DSP_ARCH_X86_SSE_NATIVE_H_ */
