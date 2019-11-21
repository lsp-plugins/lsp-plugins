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

    /** Calculate absolute values: dst[i] = src1[i] + abs(src2[i])
     *
     * @param dst destination vector
     * @param src1 source vector 1
     * @param src2 source vector 2
     * @param count number of elements
     */
    extern void (* abs_add3)(float *dst, const float *src1, const float *src2, size_t count);

    /** Calculate absolute values: dst[i] = src1[i] - abs(src2[i])
     *
     * @param dst destination vector
     * @param src1 source vector 1
     * @param src2 source vector 2
     * @param count number of elements
     */
    extern void (* abs_sub3)(float *dst, const float *src1, const float *src2, size_t count);

    /** Calculate absolute values: dst[i] = src1[i] * abs(src2[i])
     *
     * @param dst destination vector
     * @param src1 source vector 1
     * @param src2 source vector 2
     * @param count number of elements
     */
    extern void (* abs_mul3)(float *dst, const float *src1, const float *src2, size_t count);

    /** Calculate absolute values: dst[i] = src1[i] / abs(src2[i])
     *
     * @param dst destination vector
     * @param src1 source vector 1
     * @param src2 source vector 2
     * @param count number of elements
     */
    extern void (* abs_div3)(float *dst, const float *src1, const float *src2, size_t count);

    /**
     * Compute dst[i] = exp(dst[i])
     * @param dst destination
     * @param count number of elements in destination
     */
    extern void (* exp1)(float *dst, size_t count);

    /**
     * Compute dst[i] = exp(src[i])
     * @param dst destination
     * @param src source
     * @param count number of elements in source
     */
    extern void (* exp2)(float *dst, const float *src, size_t count);

    /**
     * Compute binary logarithm: dst[i] = log(2, dst[i])
     * @param dst destination
     * @param count number of elements in destination
     */
    extern void (* logb1)(float *dst, size_t count);

    /**
     * Compute binary logarithm: dst[i] = log(2, src[i])
     * @param dst destination
     * @param src source
     * @param count number of elements in source
     */
    extern void (* logb2)(float *dst, const float *src, size_t count);

    /**
     * Compute natural logarithm: dst[i] = log(E, dst[i])
     * @param dst destination
     * @param count number of elements in destination
     */
    extern void (* loge1)(float *dst, size_t count);

    /**
     * Compute natural logarithm: dst[i] = log(E, src[i])
     * @param dst destination
     * @param src source
     * @param count number of elements in source
     */
    extern void (* loge2)(float *dst, const float *src, size_t count);

    /**
     * Compute decimal logarithm: dst[i] = log(10, dst[i])
     * @param dst destination
     * @param count number of elements in destination
     */
    extern void (* logd1)(float *dst, size_t count);

    /**
     * Compute decimal logarithm: dst[i] = log(10, src[i])
     * @param dst destination
     * @param src source
     * @param count number of elements in source
     */
    extern void (* logd2)(float *dst, const float *src, size_t count);

    /**
     * Compute v[i] = c ^ v[i], the value to be raised should be non-negative
     * @param v power array
     * @param c value to be raised
     * @param count number of elements in array
     */
    extern void (* powcv1)(float *v, float c, size_t count);

    /**
     * Compute dst[i] = c ^ v[i], the value to be raised should be non-negative
     * @param dst output array
     * @param v power array
     * @param c value to be raised
     * @param count number of elements in array
     */
    extern void (* powcv2)(float *dst, const float *v, float c, size_t count);

    /**
     * Compute v[i] = v[i] ^ c, the value to be raised should be non-negative
     * @param v values to be raised
     * @param c power value
     * @param count number of elements in array
     */
    extern void (* powvc1)(float *c, float v, size_t count);

    /**
     * Compute dst[i] = v[i] ^ c, the value to be raised should be non-negative
     * @param dst output array
     * @param v values to be raised
     * @param c power value
     * @param count number of elements in array
     */
    extern void (* powvc2)(float *dst, const float *c, float v, size_t count);

    /**
     * Compute v[i] = v[i] ^ x[i], the value to be raised should be non-negative
     * @param v values to be raised
     * @param x power values
     * @param count number of elements in array
     */
    extern void (* powvx1)(float *v, const float *x, size_t count);

    /**
     * Compute dst[i] = v[i] ^ x[i], the value to be raised should be non-negative
     * @param dst output array
     * @param v values to be raised
     * @param x power values
     * @param count number of elements in array
     */
    extern void (* powvx2)(float *dst, const float *v, const float *x, size_t count);

}

#endif /* DSP_COMMON_PMATH_H_ */
