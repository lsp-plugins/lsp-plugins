/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 2 янв. 2020 г.
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

#ifndef DSP_ARCH_NATIVE_FILTERS_TRANSFER_H_
#define DSP_ARCH_NATIVE_FILTERS_TRANSFER_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

namespace native
{
    void filter_transfer_calc_ri(float *re, float *im, const f_cascade_t *c, const float *freq, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            float f         = freq[i];
            float f2        = f * f;

            // Calculate top and bottom transfer parts
            float t_re      = c->t[0] - f2 * c->t[2];
            float t_im      = c->t[1]*f;
            float b_re      = c->b[0] - f2 * c->b[2];
            float b_im      = c->b[1]*f;

            // Calculate top / bottom
            float w         = 1.0 / (b_re * b_re + b_im * b_im);
            re[i]           = (t_re * b_re + t_im * b_im) * w;
            im[i]           = (t_im * b_re - t_re * b_im) * w;
        }
    }

    void filter_transfer_apply_ri(float *re, float *im, const f_cascade_t *c, const float *freq, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            float f         = freq[i];
            float f2        = f * f;

            // Calculate top and bottom transfer parts
            float t_re      = c->t[0] - f2 * c->t[2];
            float t_im      = c->t[1]*f;
            float b_re      = c->b[0] - f2 * c->b[2];
            float b_im      = c->b[1]*f;

            // Calculate top / bottom
            float w         = 1.0 / (b_re * b_re + b_im * b_im);
            float w_re      = (t_re * b_re + t_im * b_im) * w;
            float w_im      = (t_im * b_re - t_re * b_im) * w;

            // Update transfer function
            b_re            = re[i]*w_re - im[i]*w_im;
            b_im            = re[i]*w_im + im[i]*w_re;

            // Commit result
            re[i]           = b_re;
            im[i]           = b_im;
        }
    }

    void filter_transfer_calc_pc(float *dst, const f_cascade_t *c, const float *freq, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            float f         = freq[i];
            float *x        = &dst[i << 1];
            float f2        = f * f;

            // Calculate top and bottom transfer parts
            float t_re      = c->t[0] - f2 * c->t[2];
            float t_im      = c->t[1]*f;
            float b_re      = c->b[0] - f2 * c->b[2];
            float b_im      = c->b[1]*f;

            // Calculate top / bottom
            float w         = 1.0 / (b_re * b_re + b_im * b_im);
            x[0]            = (t_re * b_re + t_im * b_im) * w;
            x[1]            = (t_im * b_re - t_re * b_im) * w;
        }
    }

    void filter_transfer_apply_pc(float *dst, const f_cascade_t *c, const float *freq, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            float f         = freq[i];
            float *x        = &dst[i << 1];
            float f2        = f * f;

            // Calculate top and bottom transfer parts
            float t_re      = c->t[0] - f2 * c->t[2];
            float t_im      = c->t[1]*f;
            float b_re      = c->b[0] - f2 * c->b[2];
            float b_im      = c->b[1]*f;

            // Calculate top / bottom
            float w         = 1.0 / (b_re * b_re + b_im * b_im);
            float w_re      = (t_re * b_re + t_im * b_im) * w;
            float w_im      = (t_im * b_re - t_re * b_im) * w;

            // Update transfer function
            b_re            = x[0]*w_re - x[1]*w_im;
            b_im            = x[0]*w_im + x[1]*w_re;

            // Commit result
            x[0]            = b_re;
            x[1]            = b_im;
        }
    }
}


#endif /* DSP_ARCH_NATIVE_FILTERS_TRANSFER_H_ */
