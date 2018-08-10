/*
 * graphics.h
 *
 *  Created on: 10 авг. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_COMMON_GRAPHICS_H_
#define DSP_COMMON_GRAPHICS_H_

#ifndef __DSP_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __DSP_DSP_DEFS */

namespace lsp
{
    //-----------------------------------------------------------------------
    // DSP different graphical functions
    namespace dsp
    {
        /** Do logarithmic vector apply:
         *  x[i] = x[i] + norm_x * logf(absf(v[i]*zero))
         *  y[i] = y[i] + norm_y * logf(absf(v[i]*zero))
         *
         * @param x destination vector for X coordinate
         * @param y destination vector for Y coordinate
         * @param v delta vector to apply
         * @param zero graphics zero point
         * @param norm_x X norming factor
         * @param norm_y Y norming factor
         */
        extern void (* axis_apply_log)(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count);

        /** Convert RGBA32 -> BGRA32 color
         *
         * @param dst target buffer
         * @param src source buffer
         * @param count number of samples to process
         */
        extern void (* rgba32_to_bgra32)(void *dst, const void *src, size_t count);
    }
}

#endif /* DSP_COMMON_GRAPHICS_H_ */
