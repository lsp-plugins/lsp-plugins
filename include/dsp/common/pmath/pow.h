/*
 * pow.h
 *
 *  Created on: 2 дек. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_COMMON_PMATH_POW_H_
#define DSP_COMMON_PMATH_POW_H_

#ifndef __DSP_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __DSP_DSP_DEFS */

namespace dsp
{
    /**
     * Compute v[i] = c ^ v[i], the value to be raised should be non-negative
     * @param v power array
     * @param c value to be raised
     * @param count number of elements in array
     */
    extern void (* powcv1)(float *v, float c, size_t count);

    /**
     * Compute dst[i] = c ^ v[i], the value to be raised should be non-negative
     * @param dst output array
     * @param v power array
     * @param c value to be raised
     * @param count number of elements in array
     */
    extern void (* powcv2)(float *dst, const float *v, float c, size_t count);

    /**
     * Compute v[i] = v[i] ^ c, the value to be raised should be non-negative
     * @param v values to be raised
     * @param c power value
     * @param count number of elements in array
     */
    extern void (* powvc1)(float *c, float v, size_t count);

    /**
     * Compute dst[i] = v[i] ^ c, the value to be raised should be non-negative
     * @param dst output array
     * @param v values to be raised
     * @param c power value
     * @param count number of elements in array
     */
    extern void (* powvc2)(float *dst, const float *c, float v, size_t count);

    /**
     * Compute v[i] = v[i] ^ x[i], the value to be raised should be non-negative
     * @param v values to be raised
     * @param x power values
     * @param count number of elements in array
     */
    extern void (* powvx1)(float *v, const float *x, size_t count);

    /**
     * Compute dst[i] = v[i] ^ x[i], the value to be raised should be non-negative
     * @param dst output array
     * @param v values to be raised
     * @param x power values
     * @param count number of elements in array
     */
    extern void (* powvx2)(float *dst, const float *v, const float *x, size_t count);

}

#endif /* DSP_COMMON_PMATH_POW_H_ */
