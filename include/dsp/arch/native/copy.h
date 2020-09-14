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

#ifndef DSP_ARCH_NATIVE_COPY_H_
#define DSP_ARCH_NATIVE_COPY_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

namespace native
{
    void copy(float *dst, const float *src, size_t count)
    {
        if (dst == src)
            return;
        while (count--)
            *(dst++)    = *(src++);
    }

    void move(float *dst, const float *src, size_t count)
    {
        if (dst == src)
            return;
        if (dst < src)
        {
            while (count--)
                *(dst++)    = *(src++);
        }
        else
        {
            dst += count;
            src += count;
            while (count--)
                *(--dst)    = *(--src);
        }
    }

    void fill(float *dst, float value, size_t count)
    {
        while (count--)
            *(dst++) = value;
    }

    void fill_zero(float *dst, size_t count)
    {
        while (count--)
            *(dst++) = 0.0f;
    }

    void fill_one(float *dst, size_t count)
    {
        while (count--)
            *(dst++) = 1.0f;
    }

    void fill_minus_one(float *dst, size_t count)
    {
        while (count--)
            *(dst++) = -1.0f;
    }

    void reverse1(float *dst, size_t count)
    {
        float *src      = &dst[count];
        count >>= 1;
        while (count--)
        {
            float   tmp = *dst;
            *(dst++)    = *(--src);
            *src        = tmp;
        }
    }

    void reverse2(float *dst, const float *src, size_t count)
    {
        if (dst != src)
        {
            src         = &src[count];
            while (count--)
                *(dst++)    = *(--src);
        }
        else
            reverse1(dst, count);
    }
}

#endif /* DSP_ARCH_NATIVE_COPY_H_ */
