/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 12 авг. 2018 г.
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

#ifndef DSP_ARCH_NATIVE_PMATH_H_
#define DSP_ARCH_NATIVE_PMATH_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

namespace native
{
    void abs_normalized(float *dst, const float *src, size_t count)
    {
        // Calculate absolute values
        dsp::abs2(dst, src, count);

        // Find the maximum value
        float max = dsp::max(dst, count);

        // Divide if it is possible
        if (max != 0.0f)
            dsp::mul_k2(dst, 1.0f / max, count);
    }

    void normalize(float *dst, const float *src, size_t count)
    {
        // Find minimum and maximum
        float max = 0.0f, min = 0.0f;
        dsp::minmax(src, count, &min, &max);

        // Determine maximum possible value
        max = (max > 0.0f) ? max : -max;
        min = (min > 0.0f) ? min : -min;
        if (max < min)
            max = min;

        // Normalize OR copy
        if (max > 0.0f)
            dsp::mul_k3(dst, src, 1.0f / max, count);
        else
            dsp::copy(dst, src, count);
    }
}

#endif /* DSP_ARCH_NATIVE_PMATH_H_ */
