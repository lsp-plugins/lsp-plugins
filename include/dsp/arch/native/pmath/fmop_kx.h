/*
 * fmop_kx.h
 *
 *  Created on: 20 нояб. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_NATIVE_PMATH_FMOP_KX_H_
#define DSP_ARCH_NATIVE_PMATH_FMOP_KX_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

namespace native
{
    void fmadd_k3(float *dst, const float *src, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] += src[i] * k;
    }

    void fmsub_k3(float *dst, const float *src, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] -= src[i] * k;
    }

    void fmrsub_k3(float *dst, const float *src, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = src[i] * k - dst[i];
    }

    void fmmul_k3(float *dst, const float *src, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] *= src[i] * k;
    }

    void fmdiv_k3(float *dst, const float *src, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] /= src[i] * k;
    }

    void fmrdiv_k3(float *dst, const float *src, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = (src[i] * k) / dst[i];
    }

    void fmadd_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = src1[i] + src2[i] * k;
    }

    void fmsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = src1[i] - src2[i] * k;
    }

    void fmrsub_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = src2[i] * k - src1[i];
    }

    void fmmul_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = src1[i] * src2[i] * k;
    }

    void fmdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = src1[i] / (src2[i] * k);
    }

    void fmrdiv_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = (src2[i] * k) / src1[i];
    }
}

#endif /* DSP_ARCH_NATIVE_PMATH_FMOP_KX_H_ */
