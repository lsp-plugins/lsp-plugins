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
    /** Calculate complex multiplication: { dst_re, dst_im } = { src1_re, src1_im } * { src2_re, src2_im }
     *
     * @param dst_re destination real part
     * @param dst_im destination imaginary part
     * @param src1_re source 1 real part
     * @param src1_im source 1 imaginary part
     * @param src2_re source 2 real part
     * @param src2_im source 2 imaginary part
     * @param count number of multiplications
     */
    extern void (* complex_mul3)(
            float *dst_re, float *dst_im,
            const float *src1_re, const float *src1_im,
            const float *src2_re, const float *src2_im,
            size_t count
        );

    /** Calculate complex multiplication: { dst_re, dst_im } = { dst_re, dst_im } * { src_re, src_im }
     *
     * @param dst_re destination and source real part
     * @param dst_im destination and source imaginary part
     * @param src_re source real part
     * @param src_im source imaginary part
     * @param count number of multiplications
     */
    extern void (* complex_mul2)(
            float *dst_re, float *dst_im,
            const float *src_re, const float *src_im,
            size_t count
        );

    /**
     * Divide complex numbers:
     *      { dst_re, dst_im } = { dst_re, dst_im } / { src_re, src_im }
     *
     * @param dst_re destination real part, source top real part
     * @param dst_im destination imaginary part, source top imaginary part
     * @param src_re bottom real part
     * @param src_im bottom imaginary part
     * @param count number of divisions
     */
    extern void (* complex_div2)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count);

    /**
     * Divide complex numbers:
     *      { dst_re, dst_im } = { src_re, src_im } / { dst_re, dst_im }
     *
     * @param dst_re destination real part, source bottom real part
     * @param dst_im destination imaginary part, source bottom imaginary part
     * @param src_re top real part
     * @param src_im top imaginary part
     * @param count number of divisions
     */
    extern void (* complex_rdiv2)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count);

    /**
     * Divide complex numbers:
     *      { dst_re, dst_im } = { t_re, t_im } / { b_re, b_im }
     *
     * @param dst_re destination real part, source bottom real part
     * @param dst_im destination imaginary part, source bottom imaginary part
     * @param t_re top real part
     * @param t_im top imaginary part
     * @param b_re bottom real part
     * @param b_im bottom imaginary part
     * @param count number of divisions
     */
    extern void (* complex_div3)(float *dst_re, float *dst_im, const float *t_re, const float *t_im, const float *b_re, const float *b_im, size_t count);

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

    /** Get module for complex numbers: mod = sqrt(re*re + im*im)
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
