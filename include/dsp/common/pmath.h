/*
 * pmath.h
 *
 *  Created on: 10 авг. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_COMMON_PMATH_H_
#define DSP_COMMON_PMATH_H_

#ifndef __DSP_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __DSP_DSP_DEFS */

//-----------------------------------------------------------------------
// DSP parallel mathematical functions
namespace dsp
{
    /** Calculate absolute values: dst[i] = abs(dst[i])
     *
     * @param dst destination vector
     * @param count number of elements
     */
    extern void (* abs1)(float *dst, size_t count);

    /** Calculate absolute values: dst[i] = abs(src[i])
     *
     * @param dst destination vector
     * @param src source vector
     * @param count number of elements
     */
    extern void (* abs2)(float *dst, const float *src, size_t count);

    /** Calculate absolute values: dst[i] = dst[i] + abs(src[i])
     *
     * @param dst destination vector
     * @param src source vector
     * @param count number of elements
     */
    extern void (* abs_add2)(float *dst, const float *src, size_t count);

    /** Calculate absolute values: dst[i] = dst[i] - abs(src[i])
     *
     * @param dst destination vector
     * @param src source vector
     * @param count number of elements
     */
    extern void (* abs_sub2)(float *dst, const float *src, size_t count);

    /** Calculate absolute values: dst[i] = dst[i] * abs(src[i])
     *
     * @param dst destination vector
     * @param src source vector
     * @param count number of elements
     */
    extern void (* abs_mul2)(float *dst, const float *src, size_t count);

    /** Calculate absolute values: dst[i] = dst[i] / abs(src[i])
     *
     * @param dst destination vector
     * @param src source vector
     * @param count number of elements
     */
    extern void (* abs_div2)(float *dst, const float *src, size_t count);

    /** Calculate dst[i] = dst[i] + src[i] * k
     *
     * @param dst destination array
     * @param src source array
     * @param k multiplier
     * @param count number of elements
     */
    extern void (* scale_add3)(float *dst, const float *src, float k, size_t count);

    /** Calculate dst[i] = dst[i] - src[i] * k
     *
     * @param dst destination array
     * @param src source array
     * @param k multiplier
     * @param count number of elements
     */
    extern void (* scale_sub3)(float *dst, const float *src, float k, size_t count);

    /** Calculate dst[i] = dst[i] * src[i] * k
     *
     * @param dst destination array
     * @param src source array
     * @param k multiplier
     * @param count number of elements
     */
    extern void (* scale_mul3)(float *dst, const float *src, float k, size_t count);

    /** Calculate dst[i] = dst[i] / (src[i] * k)
     *
     * @param dst destination array
     * @param src source array
     * @param k multiplier
     * @param count number of elements
     */
    extern void (* scale_div3)(float *dst, const float *src, float k, size_t count);

    /** Calculate dst[i] = src1[i] + src2[i] * k
     *
     * @param dst destination array
     * @param src1 source array 1
     * @param src2 source array 2
     * @param k multiplier for elements of array 2
     * @param count number of elements
     */
    extern void (* scale_add4)(float *dst, const float *src1, const float *src2, float k, size_t count);

    /** Calculate dst[i] = src1[i] - src2[i] * k
     *
     * @param dst destination array
     * @param src1 source array 1
     * @param src2 source array 2
     * @param k multiplier for elements of array 2
     * @param count number of elements
     */
    extern void (* scale_sub4)(float *dst, const float *src1, const float *src2, float k, size_t count);

    /** Calculate dst[i] = src1[i] * src2[i] * k
     *
     * @param dst destination array
     * @param src1 source array 1
     * @param src2 source array 2
     * @param k multiplier for elements of array 2
     * @param count number of elements
     */
    extern void (* scale_mul4)(float *dst, const float *src1, const float *src2, float k, size_t count);

    /** Calculate dst[i] = src1[i] / (src2[i] * k)
     *
     * @param dst destination array
     * @param src1 source array 1
     * @param src2 source array 2
     * @param k multiplier for elements of array 2
     * @param count number of elements
     */
    extern void (* scale_div4)(float *dst, const float *src1, const float *src2, float k, size_t count);

    /** Calculate dst[i] = dst[i] + src[i]
     *
     * @param dst destination array
     * @param src source array
     * @param count number of elements
     */
    extern void (* add2)(float *dst, const float *src, size_t count);

    /** Calculate dst[i] = dst[i] - src[i]
     *
     * @param dst destination array
     * @param src source array
     * @param count number of elements
     */
    extern void (* sub2)(float *dst, const float *src, size_t count);

    /** Multiply: dst[i] = dst[i] * src[i]
     *
     * @param dst destination
     * @param src first source
     * @param count number of elements
     */
    extern void (* mul2)(float *dst, const float *src, size_t count);

    /** Divide: dst[i] = dst[i] / src[i]
     *
     * @param dst destination
     * @param src first source
     * @param count number of elements
     */
    extern void (* div2)(float *dst, const float *src, size_t count);

    /** Scale: dst[i] = dst[i] * k
     *
     * @param dst destination
     * @param src source
     * @param k multiplier
     * @param count number of elements
     */
    extern void (* scale2)(float *dst, float k, size_t count);

    /** Calculate dst[i] = src1[i] + src2[i]
     *
     * @param dst destination array
     * @param src source array
     * @param count number of elements
     */
    extern void (* add3)(float *dst, const float *src1, const float *src2, size_t count);

    /** Calculate dst[i] = src1[i] - src2[i]
     *
     * @param dst destination array
     * @param src source array
     * @param count number of elements
     */
    extern void (* sub3)(float *dst, const float *src1, const float *src2, size_t count);

    /** Multiply: dst[i] = src1[i] * src2[i]
     *
     * @param dst destination
     * @param src1 first source
     * @param src2 second source
     * @param count number of elements
     */
    extern void (* mul3)(float *dst, const float *src1, const float *src2, size_t count);

    /** Divide: dst[i] = src1[i] / src2[i]
     *
     * @param dst destination
     * @param src1 first source
     * @param src2 second source
     * @param count number of elements
     */
    extern void (* div3)(float *dst, const float *src1, const float *src2, size_t count);

    /** Scale: dst[i] = src[i] * k
     *
     * @param dst destination
     * @param src source
     * @param k multiplier
     * @param count number of elements
     */
    extern void (* scale3)(float *dst, const float *src, float k, size_t count);

}

#endif /* DSP_COMMON_PMATH_H_ */
