/*
 * graphics.h
 *
 *  Created on: 07 авг. 2016 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_NATIVE_GRAPHICS_H_
#define DSP_ARCH_NATIVE_GRAPHICS_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

void axis_apply_log(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count)
{
    for (size_t i=0; i<count; ++i)
    {
        float vec    = v[i];
        if (vec < 0.0f)
            vec     = -vec;
        if (vec < AMPLIFICATION_THRESH)
            vec     = AMPLIFICATION_THRESH;
        float k     = logf(vec * zero);
        x[i]       += norm_x * k;
        y[i]       += norm_y * k;
    }
}

void rgba32_to_bgra32(void *dst, const void *src, size_t count)
{
    const uint32_t *s   = reinterpret_cast<const uint32_t *>(src);
    uint32_t *d         = reinterpret_cast<uint32_t *>(dst);

    for (size_t i=0; i<count; ++i)
    {
        uint32_t c      = s[i];
        d[i]            = ((c&0xff0000) >> 16) | ((c&0xff)<<16) | (c&0xff00ff00);
    }
}


#endif /* DSP_ARCH_NATIVE_GRAPHICS_H_ */
