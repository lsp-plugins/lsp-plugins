/*
 * pcomplex.h
 *
 *  Created on: 10 авг. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_COMMON_PCOMPLEX_H_
#define DSP_COMMON_PCOMPLEX_H_

#ifndef __DSP_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __DSP_DSP_DEFS */

//-----------------------------------------------------------------------
// DSP complex number functions
namespace dsp
{
    /** Calculate packed complex multiplication: dst = src1 * src2
     *
     * @param dst destination to store complex numbers
     * @param src1 source 1
     * @param src2 source 2
     * @param count number of multiplications
     */
    extern void (* pcomplex_mul3)(float *dst, const float *src1, const float *src2, size_t count);

    /** Calculate packed complex multiplication: dst = dst * src
     *
     * @param dst destination and source to store complex numbers
     * @param src source
     * @param count number of multiplications
     */
    extern void (* pcomplex_mul2)(float *dst, const float *src, size_t count);

    /**
     * Divide complex numbers:
     *      dst = dst / src
     *
     * @param dst destination, source top
     * @param src bottom
     * @param count number of divisions
     */
    extern void (* pcomplex_div2)(float *dst, const float *src, size_t count);

    /**
     * Divide complex numbers:
     *      dst = src / dst
     *
     * @param dst destination, source bottom
     * @param src top
     * @param count number of divisions
     */
    extern void (* pcomplex_rdiv2)(float *dst, const float *src, size_t count);

    /**
     * Divide complex numbers:
     *      dst = t / b
     *
     * @param dst destination
     * @param t top
     * @param b bottom
     * @param count number of divisions
     */
    extern void (* pcomplex_div3)(float *dst, const float *t, const float *b, size_t count);

    /** Calculate packed complex reciprocal:
     *   dst[i].{re,im} = 1 / (dst[i].re + j * dst[i].im)
     *
     * @param dst source, destination to store complex numbers
     * @param count number of multiplications
     */
    extern void (* pcomplex_rcp1)(float *dst, size_t count);

    /** Calculate packed complex reciprocal:
     *   dst[i].{re,im} = 1 / (src[i].re + j * src[i].im)
     *
     * @param dst destination to store complex numbers
     * @param src source
     * @param count number of multiplications
     */
    extern void (* pcomplex_rcp2)(float *dst, const float *src, size_t count);

    /** Fill output array with same complex numbers
     *
     * @param dst target array to fill
     * @param re real part of complex number
     * @param im imaginary part of complex number
     * @param count number of elements to fill
     */
    extern void (* pcomplex_fill_ri)(float *dst, float re, float im, size_t count);

    /** Convert real to packed complex:
     *  dst[i].re = src[i]
     *  dst[i].im = 0
     *
     * @param dst destination packed complex data
     * @param src source real data
     * @param count number of items to convert
     */
    extern void (* pcomplex_r2c)(float *dst, const float *src, size_t count);

    /** Convert packed complex to real:
     *  dst[i] = src[i].re
     *
     * @param dst destination real data
     * @param src source packed complex data
     * @param count number of items to convert
     */
    extern void (* pcomplex_c2r)(float *dst, const float *src, size_t count);

    /** Convert packed complex to real and add to destination buffer
     *
     * @param dst destination real data
     * @param src source packed complex data
     * @param count number of items to convert
     */
    extern void (* pcomplex_add_r)(float *dst, const float *src, size_t count);

    /** Get module for complex numbers: mod = sqrt(re*re + im*im)
     *
     * @param dst_mod array to sore module
     * @param src packed complex number data
     * @param count count number of elements to process
     */
    extern void (* pcomplex_mod)(float *dst_mod, const float *src, size_t count);

    /**
     * Calculate: dst[i] = dst[i] + src[i].re
     * @param dst destination real number array
     * @param src source packed complex number array
     * @param count number of elements
     */
    extern void (* pcomplex_c2r_add2)(float *dst, const float *src, size_t count);

    /**
     * Calculate: dst[i] = dst[i] - src[i].re
     * @param dst destination real number array
     * @param src source packed complex number array
     * @param count number of elements
     */
    extern void (* pcomplex_c2r_sub2)(float *dst, const float *src, size_t count);

    /**
     * Calculate: dst[i] = src[i].re - dst[i]
     * @param dst destination real number array
     * @param src source packed complex number array
     * @param count number of elements
     */
    extern void (* pcomplex_c2r_rsub2)(float *dst, const float *src, size_t count);

    /**
     * Calculate: dst[i] = dst[i] * src[i].re
     * @param dst destination real number array
     * @param src source packed complex number array
     * @param count number of elements
     */
    extern void (* pcomplex_c2r_mul2)(float *dst, const float *src, size_t count);

    /**
     * Calculate: dst[i] = dst[i] / src[i].re
     * @param dst destination real number array
     * @param src source packed complex number array
     * @param count number of elements
     */
    extern void (* pcomplex_c2r_div2)(float *dst, const float *src, size_t count);

    /**
     * Calculate: dst[i] = src[i].re / dst[i]
     * @param dst destination real number array
     * @param src source packed complex number array
     * @param count number of elements
     */
    extern void (* pcomplex_c2r_rdiv2)(float *dst, const float *src, size_t count);
}

#endif /* DSP_COMMON_PCOMPLEX_H_ */
