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

    void fmmod3(float *dst, const float *a, const float *b, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            float v     = dst[i];
            float d     = a[i] * b[i];
            int32_t r   = v / d;
            dst[i]      = v - d * r;
        }
    }

    void fmrmod3(float *dst, const float *a, const float *b, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            float v     = a[i] * b[i];
            float d     = dst[i];
            int32_t r   = v / d;
            dst[i]      = v - d * r;
        }
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

    void fmmod4(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            float v     = a[i];
            float d     = b[i] * c[i];
            int32_t r   = v / d;
            dst[i]      = v - d * r;
        }
    }

    void fmrmod4(float *dst, const float *a, const float *b, const float *c, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            float v     = b[i] * c[i];
            float d     = a[i];
            int32_t r   = v / d;
            dst[i]      = v - d * r;
        }
    }
}

#endif /* DSP_ARCH_NATIVE_PMATH_FMOP_VV_H_ */
