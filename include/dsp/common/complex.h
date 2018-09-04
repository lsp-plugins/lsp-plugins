/*
 * complex.h
 *
 *  Created on: 10 авг. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_COMMON_COMPLEX_H_
#define DSP_COMMON_COMPLEX_H_

#ifndef __DSP_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __DSP_DSP_DEFS */

//-----------------------------------------------------------------------
// DSP complex number functions
namespace dsp
{
    /** Calculate complex multiplication
     *
     * @param dst_re destination real part
     * @param dst_im destination imaginary part
     * @param src1_re source 1 real part
     * @param src1_im source 1 imaginary part
     * @param src2_re source 2 real part
     * @param src2_im source 2 imaginary part
     * @param count number of multiplications
     */
    extern void (* complex_mul)(
            float *dst_re, float *dst_im,
            const float *src1_re, const float *src1_im,
            const float *src2_re, const float *src2_im,
            size_t count
        );

    /** Calculate complex reciprocal: 1 / (re + j * im)
     *
     * @param dst_re source, destination real part
     * @param dst_im source, destination imaginary part
     * @param count number of multiplications
     */
    extern void (* complex_rcp1)(
            float *dst_re, float *dst_im,
            size_t count
        );

    /** Calculate complex reciprocal: 1 / (re + j * im)
     *
     * @param dst_re destination real part
     * @param dst_im destination imaginary part
     * @param src_re source real part
     * @param src_im source imaginary part
     * @param count number of multiplications
     */
    extern void (* complex_rcp2)(
            float *dst_re, float *dst_im,
            const float *src_re, const float *src_im,
            size_t count
        );

    /** Calculate packed complex multiplication
     *
     * @param dst destination to store complex numbers
     * @param src1 source 1
     * @param src2 source 2
     * @param count number of multiplications
     */
    extern void (* pcomplex_mul)(float *dst, const float *src1, const float *src2, size_t count);

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

    /** Convert real+imaginary complex number to polar form
     *
     * @param dst_mod module of the complex number
     * @param dst_arg argument of the complex number
     * @param src_re real part of complex number
     * @param src_im imaginary part of complex number
     * @param count number of elements to process
     */
    extern void (* complex_cvt2modarg)(
            float *dst_mod, float *dst_arg,
            const float *src_re, const float *src_im,
            size_t count
        );

    /** Get module for complex numbers
     *
     * @param dst_mod array to sore module
     * @param src_re real part of complex number
     * @param src_im imaginary part of complex number
     * @param count number of elements to process
     */
    extern void (* complex_mod)(
            float *dst_mod,
            const float *src_re, const float *src_im,
            size_t count
        );

    /** Get module for complex numbers
     *
     * @param dst_mod array to sore module
     * @param src packed complex number data
     * @param count count number of elements to process
     */
    extern void (* pcomplex_mod)(float *dst_mod, const float *src, size_t count);

    /** Convert polar-form of complex number to real+imaginary
     *
     * @param dst_re real part of complex number
     * @param dst_im imaginary part of complex number
     * @param src_mod module of the complex number
     * @param src_arg argument of the complex number
     * @param count number of elements to process
     */
    extern void (* complex_cvt2reim)(
            float *dst_re, float *dst_im,
            const float *src_mod, const float *src_arg,
            size_t count
        );

}

#endif /* DSP_COMMON_COMPLEX_H_ */
