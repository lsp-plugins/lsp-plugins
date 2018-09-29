/*
 * misc.h
 *
 *  Created on: 10 авг. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_COMMON_MISC_H_
#define DSP_COMMON_MISC_H_

//-----------------------------------------------------------------------
// DSP miscellaneous (uncategorized) functions
namespace dsp
{
    /** Calculate absolute normalized values: dst[i] = abs(src[i]) / max { abs(src) }
     *
     * @param dst destination vector
     * @param src source vector
     * @param count number of elements
     */
    extern void (* abs_normalized)(float *dst, const float *src, size_t count);

    /** Calculate normalized values: dst[i] = src[i] / (max { abs(src) })
     *
     * @param dst destination vector
     * @param src source vector
     * @param count number of elements
     */
    extern void (* normalize)(float *dst, const float *src, size_t count);
}

#endif /* DSP_COMMON_MISC_H_ */
