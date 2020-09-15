/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 2 дек. 2019 г.
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

#ifndef DSP_ARCH_NATIVE_PMATH_POW_H_
#define DSP_ARCH_NATIVE_PMATH_POW_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

namespace native
{
    void powcv1(float *v, float c, size_t count)
    {
        float C = ::logf(c);
        for (size_t i=0; i<count; ++i)
            v[i] = ::expf(v[i] * C);
    }

    void powcv2(float *dst, const float *v, float c, size_t count)
    {
        float C = ::logf(c);
        for (size_t i=0; i<count; ++i)
            dst[i] = ::expf(v[i] * C);
    }

    void powvc1(float *c, float v, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            c[i] = ::expf(v * ::logf(c[i]));
    }

    void powvc2(float *dst, const float *c, float v, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = ::expf(v * ::logf(c[i]));
    }

    void powvx1(float *v, const float *x, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            v[i] = ::expf(x[i] * ::logf(v[i]));
    }

    void powvx2(float *dst, const float *v, const float *x, size_t count)
    {
        for (size_t i=0; i<count; ++i)
            dst[i] = ::expf(x[i] * ::logf(v[i]));
    }
}

#endif /* DSP_ARCH_NATIVE_PMATH_POW_H_ */
