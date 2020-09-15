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

#ifndef DSP_ARCH_NATIVE_HMATH_H_
#define DSP_ARCH_NATIVE_HMATH_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

namespace native
{
    float scalar_mul(const float *a, const float *b, size_t count)
    {
        float result = 0;
        while (count--)
            result += *(a++) * *(b++);
        return result;
    }

    float h_sum(const float *src, size_t count)
    {
        float result    = 0.0f;
        while (count--)
            result         += *(src++);
        return result;
    }

    float h_sqr_sum(const float *src, size_t count)
    {
        float result    = 0.0f;
        while (count--)
        {
            float tmp       = *(src++);
            result         += tmp * tmp;
        }
        return result;
    }

    float h_abs_sum(const float *src, size_t count)
    {
        float result    = 0.0f;
        while (count--)
        {
            float tmp       = *(src++);
            if (tmp < 0.0f)
                result         -= tmp;
            else
                result         += tmp;
        }
        return result;
    }
}

#endif /* DSP_ARCH_NATIVE_HMATH_H_ */
