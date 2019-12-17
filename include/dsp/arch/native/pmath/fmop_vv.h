/*
 * fmop_vv.h
 *
 *  Created on: 21 нояб. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_NATIVE_PMATH_FMOP_VV_H_
#define DSP_ARCH_NATIVE_PMATH_FMOP_VV_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

namespace native
{
    void fmadd3(float *dst, const float *a, const float *b, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] += a[i] * b[i];
    }

    void fmsub3(float *dst, const float *a, const float *b, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] -= a[i] * b[i];
    }

    void fmrsub3(float *dst, const float *a, const float *b, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = a[i] * b[i] - dst[i];
    }

    void fmmul3(float *dst, const float *a, const float *b, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] *= a[i] * b[i];
    }

    void fmdiv3(float *dst, const float *a, const float *b, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] /= a[i] * b[i];
    }

    void fmrdiv3(float *dst, const float *a, const float *b, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = (a[i] * b[i]) / dst[i];
    }

    void fmadd4(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = a[i] + b[i] * c[i];
    }

    void fmsub4(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = a[i] - b[i] * c[i];
    }

    void fmrsub4(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = b[i] * c[i] - a[i];
    }

    void fmmul4(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = a[i] * b[i] * c[i];
    }

    void fmdiv4(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = a[i] / (b[i] * c[i]);
    }

    void fmrdiv4(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = (b[i] * c[i]) / a[i];
    }
}

#endif /* DSP_ARCH_NATIVE_PMATH_FMOP_VV_H_ */
