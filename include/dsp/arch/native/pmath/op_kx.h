/*
 * op_kx.h
 *
 *  Created on: 20 нояб. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_NATIVE_PMATH_OP_KX_H_
#define DSP_ARCH_NATIVE_PMATH_OP_KX_H_

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

    void mul_k2(float *dst, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] *= k;
    };

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

    void mul_k3(float *dst, const float *src, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = src[i] * k;
    };

}

#endif /* DSP_ARCH_NATIVE_PMATH_OP_KX_H_ */
