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
    /** Copy saturated data from source to destination.
     * Avoid +Inf, -Inf values by replacing with large values
     * Avoid +NaN, -NaN values by replacing with zeros
     *
     * @param dst destination pointer
     * @param src source pointer
     * @param count number of elements
     */
    extern void (*copy_saturated)(float *dst, const float *src, size_t count);

    /** Saturate floating-point data in buffer
     * Avoid +Inf, -Inf values by replacing with large values
     * Avoid +NaN, -NaN values by replacing with zeros
     *
     * @param dst destination pointer
     * @param count number of elements
     */
    extern void (* saturate)(float *dst, size_t count);

    /** Limit floating-point data in buffer
     * Avoid +Inf, -Inf values by replacing with 1.0 and -1.0
     * Avoid +NaN, -NaN values by replacing with zeros
     * Replace values greater than 1 with 1.0
     * Replace values less than -1 with -1.0
     *
     * @param dst destination pointer
     * @param src source pointer
     * @param count number of elements
     */
    extern void (* limit_saturate1)(float *dst, size_t count);

    /** Limit floating-point data in buffer and copy do destination
     * Avoid +Inf, -Inf values by replacing with 1.0 and -1.0
     * Avoid +NaN, -NaN values by replacing with zeros
     * Replace values greater than 1 with 1.0
     * Replace values less than -1 with -1.0
     *
     * @param dst destination pointer
     * @param src source pointer
     * @param count number of elements
     */
    extern void (* limit_saturate2)(float *dst, const float *src, size_t count);

    /** Avoid denormal values
     *
     * @param dst destination buffer
     * @param src source buffer
     * @param count number of samples
     */
    extern void (* avoid_denormals)(float *dst, const float *src, size_t count);

    /**
     * Limit floating-point data in buffer
     * Replace +Inf with max, -Inf, +NaN, -NaN with min
     * Replace values greater than max with max,
     * Replace values less than min with min
     *
     * @param dst destination buffer
     * @param min minimum value
     * @param max maximum value
     * @param count number of samples
     */
    extern void (* limit1)(float *dst, float min, float max, size_t count);

    /**
     * Limit floating-point data in buffer
     * Replace +Inf with max, -Inf, +NaN, -NaN with min
     * Replace values greater than max with max,
     * Replace values less than min with min
     *
     * @param dst destination buffer
     * @param src source buffer
     * @param min minimum value
     * @param max maximum value
     * @param count number of samples
     */
    extern void (* limit2)(float *dst, const float *src, float min, float max, size_t count);
}

#endif /* DSP_COMMON_FLOAT_H_ */
