/*
 * op_vv.h
 *
 *  Created on: 20 нояб. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_NATIVE_PMATH_OP_VV_H_
#define DSP_ARCH_NATIVE_PMATH_OP_VV_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

namespace native
{
    void add2(float *dst, const float *src, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] += src[i];
    }

    void sub2(float *dst, const float *src, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] -= src[i];
    }

    void rsub2(float *dst, const float *src, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = src[i] - dst[i];
    }

    void mul2(float *dst, const float *src, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] *= src[i];
    }

    void div2(float *dst, const float *src, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] /= src[i];
    }

    void rdiv2(float *dst, const float *src, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = src[i] / dst[i];
    }

    void mod2(float *dst, const float *src, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            float v     = dst[i];
            float s     = src[i];
            int32_t r   = v / s;
            dst[i]      = v - s * r;
        }
    }

    void rmod2(float *dst, const float *src, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            float v     = src[i];
            float s     = dst[i];
            int32_t r   = v / s;
            dst[i]      = v - s * r;
        }
    }

    void add3(float *dst, const float *src1, const float *src2, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = src1[i] + src2[i];
    }

    void sub3(float *dst, const float *src1, const float *src2, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = src1[i] - src2[i];
    }

    void mul3(float *dst, const float *src1, const float *src2, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = src1[i] * src2[i];
    }

    void div3(float *dst, const float *src1, const float *src2, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = src1[i] / src2[i];
    }

    void mod3(float *dst, const float *src1, const float *src2, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            float v     = src1[i];
            float s     = src2[i];
            int32_t r   = v / s;
            dst[i]      = v - s * r;
        }
    }
}

#endif /* DSP_ARCH_NATIVE_PMATH_OP_VV_H_ */
