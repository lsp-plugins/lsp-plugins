/*
 * abs_vv.h
 *
 *  Created on: 26 нояб. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_NATIVE_PMATH_ABS_VV_H_
#define DSP_ARCH_NATIVE_PMATH_ABS_VV_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

namespace native
{
    void abs1(float *dst, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i]      = fabs(dst[i]);
    }

    void abs2(float *dst, const float *src, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i]      = fabs(src[i]);
    }

    void abs_add2(float *dst, const float *src, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i]     += fabs(src[i]);
    }

    void abs_sub2(float *dst, const float *src, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i]     -= fabs(src[i]);
    }

    void abs_rsub2(float *dst, const float *src, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i]     = fabs(src[i]) - dst[i];
    }

    void abs_mul2(float *dst, const float *src, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i]     *= fabs(src[i]);
    }

    void abs_div2(float *dst, const float *src, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i]     /= fabs(src[i]);
    }

    void abs_rdiv2(float *dst, const float *src, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i]      = fabs(src[i]) / dst[i];
    }

    void abs_add3(float *dst, const float *src1, const float *src2, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i]      = src1[i] + fabs(src2[i]);
    }

    void abs_sub3(float *dst, const float *src1, const float *src2, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i]      = src1[i] - fabs(src2[i]);
    }

    void abs_rsub3(float *dst, const float *src1, const float *src2, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i]      = fabs(src2[i]) - src1[i];
    }

    void abs_mul3(float *dst, const float *src1, const float *src2, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i]      = src1[i] * fabs(src2[i]);
    }

    void abs_div3(float *dst, const float *src1, const float *src2, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i]      = src1[i] / fabs(src2[i]);
    }

    void abs_rdiv3(float *dst, const float *src1, const float *src2, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i]      = fabs(src2[i]) / src1[i];
    }
}


#endif /* INCLUDE_DSP_ARCH_NATIVE_PMATH_ABS_VV_H_ */
