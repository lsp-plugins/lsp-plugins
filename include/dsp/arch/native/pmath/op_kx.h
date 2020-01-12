/*
 * op_kx.h
 *
 *  Created on: 20 нояб. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_NATIVE_PMATH_OP_KX_H_
#define DSP_ARCH_NATIVE_PMATH_OP_KX_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

namespace native
{
    void add_k2(float *dst, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] += k;
    };

    void sub_k2(float *dst, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] -= k;
    };

    void rsub_k2(float *dst, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = k - dst[i];
    };

    void mul_k2(float *dst, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] *= k;
    };

    void div_k2(float *dst, float k, size_t count)
    {
        k = 1.0f / k;
        for (size_t i=0; i<count; ++i)
            dst[i] *= k;
    };

    void rdiv_k2(float *dst, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = k / dst[i];
    };

    void mod_k2(float *dst, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            float v     = dst[i];
            int32_t r   = v / k;
            dst[i]      = v - k * r;
        }
    }

    void rmod_k2(float *dst, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            float v     = dst[i];
            int32_t r   = k / v;
            dst[i]      = k - v * r;
        }
    }

    void add_k3(float *dst, const float *src, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = src[i] + k;
    };

    void sub_k3(float *dst, const float *src, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = src[i] - k;
    };

    void rsub_k3(float *dst, const float *src, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = k - src[i];
    };

    void mul_k3(float *dst, const float *src, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = src[i] * k;
    };

    void div_k3(float *dst, const float *src, float k, size_t count)
    {
        k = 1.0f / k;
        for (size_t i=0; i<count; ++i)
            dst[i] = src[i] * k;
    };

    void rdiv_k3(float *dst, const float *src, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = k / src[i];
    };

    void mod_k3(float *dst, const float *src, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            float v     = src[i];
            int32_t r   = v / k;
            dst[i]      = v - k * r;
        }
    }

    void rmod_k3(float *dst, const float *src, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            float v     = src[i];
            int32_t r   = k / v;
            dst[i]      = k - v * r;
        }
    }
}

#endif /* DSP_ARCH_NATIVE_PMATH_OP_KX_H_ */
