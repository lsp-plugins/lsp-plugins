/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 05 окт. 2015 г.
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

#ifndef DSP_ARCH_NATIVE_MSMATRIX_H_
#define DSP_ARCH_NATIVE_MSMATRIX_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

namespace native
{
    void lr_to_ms(float *m, float *s, const float *l, const float *r, size_t count)
    {
        while (count--)
        {
            float lv        = *(l++);
            float rv        = *(r++);
            *(m++)          = (lv + rv) * 0.5f;
            *(s++)          = (lv - rv) * 0.5f;
        }
    }

    void lr_to_mid(float *m, const float *l, const float *r, size_t count)
    {
        while (count--)
        {
            float lv        = *(l++);
            float rv        = *(r++);
            *(m++)          = (lv + rv) * 0.5f;
        }
    }

    void lr_to_side(float *s, const float *l, const float *r, size_t count)
    {
        while (count--)
        {
            float lv        = *(l++);
            float rv        = *(r++);
            *(s++)          = (lv - rv) * 0.5f;
        }
    }

    void ms_to_lr(float *l, float *r, const float *m, const float *s, size_t count)
    {
        while (count--)
        {
            float mv        = *(m++);
            float sv        = *(s++);
            *(l++)          = mv + sv;
            *(r++)          = mv - sv;
        }
    }

    void ms_to_left(float *l, const float *m, const float *s, size_t count)
    {
        while (count--)
            *(l++)          = *(m++) + *(s++);
    }

    void ms_to_right(float *r, const float *m, const float *s, size_t count)
    {
        while (count--)
            *(r++)          = *(m++) - *(s++);
    }
}

#endif /* DSP_ARCH_NATIVE_MSMATRIX_H_ */
