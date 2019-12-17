/*
 * hsum.h
 *
 *  Created on: 29 нояб. 2019 г.
 *      Author: sadko
 */

#ifndef INCLUDE_DSP_COMMON_HMATH_HSUM_H_
#define INCLUDE_DSP_COMMON_HMATH_HSUM_H_

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

}

#endif /* INCLUDE_DSP_COMMON_HMATH_HSUM_H_ */
