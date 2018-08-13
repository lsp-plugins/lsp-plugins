/*
 * hmath.h
 *
 *  Created on: 10 авг. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_COMMON_HMATH_H_
#define DSP_COMMON_HMATH_H_

#ifndef __DSP_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __DSP_DSP_DEFS */

//-----------------------------------------------------------------------
// DSP horizontal mathematical functions
namespace dsp
{
    /** Calculate horizontal sum: result = sum (i) from 0 to count-1 src[i]
     *
     * @param src vector to summarize
     * @param count number of elements
     * @return status of operation
     */
    extern float (* h_sum)(const float *src, size_t count);

    /** Calculate horizontal sum: result = sum (i) from 0 to count-1 sqr(src[i])
     *
     * @param src vector to summarize
     * @param count number of elements
     * @return status of operation
     */
    extern float (* h_sqr_sum)(const float *src, size_t count);

    /** Calculate horizontal sum of absolute values: result = sum (i) from 0 to count-1 abs(src[i])
     *
     * @param src vector to summarize
     * @param count number of elements
     * @return status of operation
     */
    extern float (* h_abs_sum)(const float *src, size_t count);

    /** Calculate sum {from 0 to count-1} (a[i] * b[i])
     *
     * @param a first vector
     * @param b second vector
     * @param count number of elements
     * @return scalar multiplication
     */
    extern float (* scalar_mul)(const float *a, const float *b, size_t count);

}

#endif /* DSP_COMMON_HMATH_H_ */
