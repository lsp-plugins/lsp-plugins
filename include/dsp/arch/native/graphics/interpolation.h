/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 26 нояб. 2018 г.
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

#ifndef DSP_ARCH_NATIVE_GRAPHICS_INTERPOLATION_H_
#define DSP_ARCH_NATIVE_GRAPHICS_INTERPOLATION_H_

namespace native
{
    void smooth_cubic_linear(float *dst, float start, float stop, size_t count)
    {
        float dy = stop - start;
        float nx = 1.0f / (count + 1); // Normalizing x

        for (size_t i=0; i<count; ++i)
        {
            float x = i * nx;
            *(dst++) = start + dy * x*x * (3.0f - 2.0f * x);
        }
    }

    void smooth_cubic_log(float *dst, float start, float stop, size_t count)
    {
        float dy = logf(stop/start);
        float nx = 1.0f / (count + 1); // Normalizing x

        for (size_t i=0; i<count; ++i)
        {
            float x = i * nx;
            *(dst++) = start * expf(dy * x*x * (3.0f - 2.0f * x));
        }
    }
}

#endif /* DSP_ARCH_NATIVE_GRAPHICS_INTERPOLATION_H_ */
