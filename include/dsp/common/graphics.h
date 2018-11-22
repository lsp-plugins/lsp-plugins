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

//-----------------------------------------------------------------------
// DSP different graphical functions
namespace dsp
{
#pragma pack(push, 1)
    typedef struct hsla_hue_eff_t
    {
        float h, s, l, a;   // Hue, saturation, lightness, alpha
        float thresh;       // alpha threshold
    } hsla_hue_eff_t;

    typedef struct hsla_alpha_eff_t
    {
        float h, s, l, a;   // Hue, saturation, lightness, alpha
    } hsla_alpha_eff_t;

    typedef struct hsla_sat_eff_t
    {
        float h, s, l, a;   // Hue, saturation, lightness, alpha
        float thresh;       // alpha threshold
    } hsla_sat_eff_t;

    typedef struct hsla_light_eff_t
    {
        float h, s, l, a;   // Hue, saturation, lightness, alpha
        float thresh;       // alpha threshold
    } hsla_light_eff_t;
#pragma pack(pop)



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

    /**
     * Fill array with RGBA colors
     * @param dst array to store RGBA
     * @param r red amount
     * @param g green amount
     * @param b blue amount
     * @param a alpha amount
     * @param count number of elements
     */
    extern void (* fill_rgba)(float *dst, float r, float g, float b, float a, size_t count);

    /**
     * Fill array with HSLA colors
     * @param dst array to store HSLA
     * @param h hue amount
     * @param s saturation amount
     * @param l lightness amount
     * @param a alpha amount
     * @param count number of elements
     */
    extern void (* fill_hsla)(float *dst, float h, float s, float l, float a, size_t count);

    /**
     * Convert RGBA to HSLA color
     * @param dst array to store HSLA
     * @param src array containing RGBA
     * @param count number of structures to convert
     */
    extern void (* rgba_to_hsla)(float *dst, const float *src, size_t count);

    /**
     * Convert HSLA to RGBA color
     * @param dst array to store RGBA
     * @param src array containing HSLA
     * @param count number of structures to convert
     */
    extern void (* hsla_to_rgba)(float *dst, const float *src, size_t count);

    /** Convert RGBA (float) -> BGRA32 color
     * Alpha color is applied to result by formula:
     *   A' = (1 - A) * 255
     *   R' = R * (1 - A) * 255
     *   G' = G * (1 - A) * 255
     *   B' = B * (1 - A) * 255
     *
     * @param dst target buffer (4 bytes per pixel)
     * @param src source buffer (4 floats per pixel)
     * @param count number of samples to process
     */
    extern void (* rgba_to_bgra32)(void *dst, const float *src, size_t count);

    /**
     * Generate set of pixels with applied hue shift effect
     * @param dst target array to store pixels
     * @param v hue shift in range (-1 .. +1)
     * @param eff effect parameters
     * @param count number of points to process
     */
    extern void (* eff_hsla_hue)(float *dst, const float *v, const hsla_hue_eff_t *eff, size_t count);

    /**
     * Generate set of pixels with applied saturation shift effect
     * @param dst target array to store pixels
     * @param v saturation shift in range (-1 .. +1)
     * @param eff effect parameters
     * @param count number of points to process
     */
    extern void (* eff_hsla_sat)(float *dst, const float *v, const hsla_sat_eff_t *eff, size_t count);

    /**
     * Generate set of pixels with applied lighness shift effect
     * @param dst target array to store pixels
     * @param v lightness shift in range (-1 .. +1)
     * @param eff effect parameters
     * @param count number of points to process
     */
    extern void (* eff_hsla_light)(float *dst, const float *v, const hsla_light_eff_t *eff, size_t count);

    /**
     * Generate set of pixels with applied alpha effect
     * @param dst target array to store pixels
     * @param v alpha shift in range (-1 .. +1)
     * @param eff effect parameters
     * @param count number of points to process
     */
    extern void (* eff_hsla_alpha)(float *dst, const float *v, const hsla_alpha_eff_t *eff, size_t count);
}

#endif /* DSP_COMMON_GRAPHICS_H_ */
