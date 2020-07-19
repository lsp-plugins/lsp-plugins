/*
 * minmax.h
 *
 *  Created on: 5 июл. 2020 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_NATIVE_PMATH_MINMAX_H_
#define DSP_ARCH_NATIVE_PMATH_MINMAX_H_

namespace native
{
    void pmin2(float *dst, const float *src, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i]  = (dst[i] < src[i]) ? dst[i] : src[i];
    }

    void psmin2(float *dst, const float *src, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i]  = (fabs(dst[i]) < fabs(src[i])) ? dst[i] : src[i];
    }

    void pamin2(float *dst, const float *src, size_t count)
    {
        float xa, xb;
        for (size_t i=0; i<count; ++i)
        {
            xa = fabs(dst[i]);
            xb = fabs(src[i]);
            dst[i]  = (xa < xb) ? xa : xb;
        }
    }

    void pmax2(float *dst, const float *src, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i]  = (dst[i] < src[i]) ? src[i] : dst[i];
    }

    void psmax2(float *dst, const float *src, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i]  = (fabs(dst[i]) < fabs(src[i])) ? src[i] : dst[i];
    }

    void pamax2(float *dst, const float *src, size_t count)
    {
        float xa, xb;
        for (size_t i=0; i<count; ++i)
        {
            xa = fabs(dst[i]);
            xb = fabs(src[i]);
            dst[i]  = (xa < xb) ? xb : xa;
        }
    }

    void pmin3(float *dst, const float *a, const float *b, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i]  = (a[i] < b[i]) ? a[i] : b[i];
    }

    void psmin3(float *dst, const float *a, const float *b, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i]  = (fabs(a[i]) < fabs(b[i])) ? a[i] : b[i];
    }

    void pamin3(float *dst, const float *a, const float *b, size_t count)
    {
        float xa, xb;
        for (size_t i=0; i<count; ++i)
        {
            xa = fabs(a[i]);
            xb = fabs(b[i]);
            dst[i]  = (xa < xb) ? xa : xb;
        }
    }

    void pmax3(float *dst, const float *a, const float *b, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i]  = (a[i] < b[i]) ? b[i] : a[i];
    }

    void psmax3(float *dst, const float *a, const float *b, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i]  = (fabs(a[i]) < fabs(b[i])) ? b[i] : a[i];
    }

    void pamax3(float *dst, const float *a, const float *b, size_t count)
    {
        float xa, xb;
        for (size_t i=0; i<count; ++i)
        {
            xa = fabs(a[i]);
            xb = fabs(b[i]);
            dst[i]  = (xa < xb) ? xb : xa;
        }
    }

}

#endif /* DSP_ARCH_NATIVE_PMATH_MINMAX_H_ */
