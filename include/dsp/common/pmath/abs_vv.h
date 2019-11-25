/*
 * abs_vv.h
 *
 *  Created on: 26 нояб. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_COMMON_PMATH_ABS_VV_H_
#define DSP_COMMON_PMATH_ABS_VV_H_

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

    /** Calculate absolute values: dst[i] = abs(src[i]) - dst[i]
     *
     * @param dst destination vector
     * @param src source vector
     * @param count number of elements
     */
    extern void (* abs_rsub2)(float *dst, const float *src, size_t count);

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

    /** Calculate absolute values: dst[i] = abs(src[i]) / dst[i]
     *
     * @param dst destination vector
     * @param src source vector
     * @param count number of elements
     */
    extern void (* abs_rdiv2)(float *dst, const float *src, size_t count);

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

    /** Calculate absolute values: dst[i] = abs(src2[i]) - src1[i]
     *
     * @param dst destination vector
     * @param src1 source vector 1
     * @param src2 source vector 2
     * @param count number of elements
     */
    extern void (* abs_rsub3)(float *dst, const float *src1, const float *src2, size_t count);

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

    /** Calculate absolute values: dst[i] = abs(src2[i]) / src1[i]
     *
     * @param dst destination vector
     * @param src1 source vector 1
     * @param src2 source vector 2
     * @param count number of elements
     */
    extern void (* abs_rdiv3)(float *dst, const float *src1, const float *src2, size_t count);
}

#endif /* INCLUDE_DSP_COMMON_PMATH_ABS_VV_H_ */
