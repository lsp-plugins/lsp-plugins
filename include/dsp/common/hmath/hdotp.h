/*
 * hmath.h
 *
 *  Created on: 10 авг. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_COMMON_HMATH_HDOTP_H_
#define DSP_COMMON_HMATH_HDOTP_H_

#ifndef __DSP_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __DSP_DSP_DEFS */

//-----------------------------------------------------------------------
// DSP horizontal mathematical functions
namespace dsp
{

    /** Calculate dot product: sum {from 0 to count-1} (a[i] * b[i])
     *
     * @param a first vector
     * @param b second vector
     * @param count number of elements
     * @return scalar multiplication
     */
    extern float (* h_dotp)(const float *a, const float *b, size_t count);

    /** Calculate dot product of squares: sum {from 0 to count-1} (sqr(a[i]) * sqr(b[i]))
     *
     * @param a first vector
     * @param b second vector
     * @param count number of elements
     * @return scalar multiplication
     */
    extern float (* h_sqr_dotp)(const float *a, const float *b, size_t count);

    /** Calculate dot product of absolute values: sum {from 0 to count-1} (abs(a[i]) * abs(b[i]))
     *
     * @param a first vector
     * @param b second vector
     * @param count number of elements
     * @return scalar multiplication
     */
    extern float (* h_abs_dotp)(const float *a, const float *b, size_t count);
}

#endif /* DSP_COMMON_HMATH_HDOTP_H_ */
