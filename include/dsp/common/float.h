/*
 * float.h
 *
 *  Created on: 10 авг. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_COMMON_FLOAT_H_
#define DSP_COMMON_FLOAT_H_

#ifndef __DSP_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __DSP_DSP_DEFS */

//-----------------------------------------------------------------------
// DSP floating-point manipulations
namespace dsp
{
    /** Copy data: dst[i] = saturate(src[i])
     *
     * @param dst destination pointer
     * @param src source pointer
     * @param count number of elements
     */
    extern void (*copy_saturated)(float *dst, const float *src, size_t count);

    /** Saturate data: dst[i] = saturate(src[i])
     *
     * @param dst destination pointer
     * @param src source pointer
     * @param count number of elements
     */
    extern void (* saturate)(float *dst, size_t count);

    /** Avoid denormal values
     *
     * @param dst destination buffer
     * @param src source buffer
     * @param count number of samples
     */
    extern void (* avoid_denormals)(float *dst, const float *src, size_t count);
}

#endif /* DSP_COMMON_FLOAT_H_ */
