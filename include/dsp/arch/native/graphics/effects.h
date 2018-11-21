/*
 * effects.h
 *
 *  Created on: 21 нояб. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_NATIVE_GRAPHICS_EFFECTS_H_
#define DSP_ARCH_NATIVE_GRAPHICS_EFFECTS_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

namespace native
{
    void eff_hsla_hue(float *dst, const float *v, const dsp::hsla_hue_eff_t *eff, size_t count)
    {
        float value, hue, alpha;
        float t     = 1.0f - eff->thresh;
        float kt    = 1.0f / eff->thresh;

        for (size_t i=0; i<count; ++i, dst += 4)
        {
            value   = v[i];
            value   = (value >= 0.0f) ? 1.0f - value : 1.0f + value;

            if (value < t)
            {
                hue         = eff->h + value;
                alpha       = 0.0f;
            }
            else
            {
                hue         = eff->h + t;
                alpha       = ((value - t) * kt);
            }

            dst[0]      = (hue > 1.0f) ? hue - 1.0f : hue;
            dst[1]      = eff->s;
            dst[2]      = eff->l;
            dst[3]      = alpha;
        }
    }
}

#endif /* DSP_ARCH_NATIVE_GRAPHICS_EFFECTS_H_ */
