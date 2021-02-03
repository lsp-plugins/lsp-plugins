/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 6 дек. 2020 г.
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

#ifndef DSP_ARCH_NATIVE_CROSSFADE_H_
#define DSP_ARCH_NATIVE_CROSSFADE_H_

namespace native
{
    void lin_xfade2(float *dst, const float *src, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n)
    {
        float dy    = (y1 - y0) / float(x1 - x0);
        x          -= x0;

        for (size_t i=0; i<n; ++i, ++x)
        {
            float d     = x * dy;
            dst[i]      = dst[i]*(y0 + d) + src[i]*(y1 - d);
        }
    }

    void lin_xfade3(float *dst, const float *a, const float *b, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n)
    {
        float dy    = (y1 - y0) / float(x1 - x0);
        x          -= x0;

        for (size_t i=0; i<n; ++i, ++x)
        {
            float d     = x * dy;
            dst[i]      = a[i]*(y0 + d) + b[i]*(y1 - d);
        }
    }

    void lin_xfade_add3(float *dst, const float *a, const float *b, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n)
    {
        float dy    = (y1 - y0) / float(x1 - x0);
        x          -= x0;

        for (size_t i=0; i<n; ++i, ++x)
        {
            float d     = x * dy;
            dst[i]     += a[i]*(y0 + d) + b[i]*(y1 - d);
        }
    }
}

#endif /* DSP_ARCH_NATIVE_CROSSFADE_H_ */
