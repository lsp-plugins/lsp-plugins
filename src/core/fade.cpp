/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 24 марта 2016 г.
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

#include <core/fade.h>

namespace lsp
{
    void fade_in(float *dst, const float *src, size_t fade_len, size_t buf_len)
    {
        if ((fade_len <= 0) || (buf_len <= 0))
            return;

        float k = 1.0f / fade_len;
        if (fade_len > buf_len)
            fade_len = buf_len;

        for (size_t i=0; i < fade_len; ++i)
            dst[i] = src[i] * i * k;
    }

    void fade_out(float *dst, const float *src, size_t fade_len, size_t buf_len)
    {
        if ((fade_len <= 0) || (buf_len <= 0))
            return;

        float k = 1.0f / fade_len;
        if (fade_len > buf_len)
            fade_len = buf_len;
        else
        {
            src     = &src[buf_len - fade_len];
            dst     = &dst[buf_len - fade_len];
        }

        for (size_t i=fade_len; i > 0; )
            *(dst++) = *(src++) * ((--i) * k);
    }
}

