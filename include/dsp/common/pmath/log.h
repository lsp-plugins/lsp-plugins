/*
 * log.h
 *
 *  Created on: 2 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_COMMON_PMATH_LOG_H_
#define DSP_COMMON_PMATH_LOG_H_

#ifndef __DSP_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __DSP_DSP_DEFS */

namespace dsp
{
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

}

#endif /* DSP_COMMON_PMATH_LOG_H_ */
