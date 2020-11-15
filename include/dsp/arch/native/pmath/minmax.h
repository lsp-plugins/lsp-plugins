/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 5 июл. 2020 г.
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
