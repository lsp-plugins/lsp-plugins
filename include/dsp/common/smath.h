/*
 * smath.h
 *
 *  Created on: 10 авг. 2018 г.
 *      Author: sadko
 */

#ifndef DSP_COMMON_SMATH_H_
#define DSP_COMMON_SMATH_H_

#ifndef __DSP_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __DSP_DSP_DEFS */

//-----------------------------------------------------------------------
// DSP scalar mathematical functions
namespace dsp
{
    /** Power of floating-point value by integer constant
     *
     * @param x value to power
     * @param deg the power degree
     * @return result of x^deg calculation
     */
    extern float (* ipowf)(float x, int deg);

    /** Calculate the integer root of value
     *
     * @param x the value to calculate
     * @param deg the root degree, should be positive
     * @return the deg'th root of x
     */
    extern float (* irootf)(float x, int deg);
}

#endif /* DSP_COMMON_SMATH_H_ */
