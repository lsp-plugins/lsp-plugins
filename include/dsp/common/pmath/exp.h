/*
 * exp.h
 *
 *  Created on: 2 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_COMMON_PMATH_EXP_H_
#define DSP_COMMON_PMATH_EXP_H_

#ifndef __DSP_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __DSP_DSP_DEFS */

namespace dsp
{
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
}

#endif /* DSP_COMMON_PMATH_EXP_H_ */
