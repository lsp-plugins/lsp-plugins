/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 20 нояб. 2019 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
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

    void fmmod_k3(float *dst, const float *src, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            float v     = dst[i];
            float d     = src[i] * k;
            int32_t r   = v / d;
            dst[i]      = v - d * r;
        }
    }

    void fmrmod_k3(float *dst, const float *src, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            float v     = src[i] * k;
            float d     = dst[i];
            int32_t r   = v / d;
            dst[i]      = v - d * r;
        }
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

    void fmmod_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            float v     = src1[i];
            float d     = src2[i] * k;
            int32_t r   = v / d;
            dst[i]      = v - d * r;
        }
    }

    void fmrmod_k4(float *dst, const float *src1, const float *src2, float k, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            float v     = src2[i] * k;
            float d     = src1[i];
            int32_t r   = v / d;
            dst[i]      = v - d * r;
        }
    }
}

#endif /* DSP_ARCH_NATIVE_PMATH_FMOP_KX_H_ */
