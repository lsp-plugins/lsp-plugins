/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 05 авг. 2016 г.
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

#ifndef DSP_ARCH_NATIVE_FLOAT_H_
#define DSP_ARCH_NATIVE_FLOAT_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

namespace native
{
    void copy_saturated(float *dst, const float *src, size_t count)
    {
        while (count--)
        {
            float v = *(src++);
            if (isnan(v))
                v       =   FLOAT_SAT_P_NAN;
            else if (isinf(v))
                v       =   (v < 0.0f) ? FLOAT_SAT_N_INF : FLOAT_SAT_P_INF;

            *(dst++)    = v;
        }
    }

    void saturate(float *dst, size_t count)
    {
        while (count--)
        {
            float v = *dst;
            if (isnan(v))
                *(dst++)    = FLOAT_SAT_P_NAN;
            else if (isinf(v))
                *(dst++)    = (v < 0.0f) ? FLOAT_SAT_N_INF : FLOAT_SAT_P_INF;
            else
                dst++;
        }
    }

    void limit_saturate1(float *dst, size_t count)
    {
        while (count--)
        {
            float v     = *dst;
            if (isnan(v))
                v       =   FLOAT_SAT_P_NAN;
            else if (isinf(v))
                v       =   (v < 0.0f) ? -1.0f : 1.0f;
            else if (v > 1.0f)
                v       =   1.0f;
            else if (v < -1.0f)
                v       =  -1.0f;

            *(dst++)    = v;
        }
    }

    void limit_saturate2(float *dst, const float *src, size_t count)
    {
        while (count--)
        {
            float v     = *(src++);
            if (isnan(v))
                v       =   FLOAT_SAT_P_NAN;
            else if (isinf(v))
                v       =   (v < 0.0f) ? -1.0f : 1.0f;
            else if (v > 1.0f)
                v       =   1.0f;
            else if (v < -1.0f)
                v       =  -1.0f;

            *(dst++)    = v;
        }
    }

    void limit1(float *dst, float min, float max, size_t count)
    {
        while (count--)
        {
            float v     = *dst;
            if (isnan(v))
                v       =   min;
            else if (isinf(v))
                v       =   (v < 0.0f) ? min : max;
            else if (v > max)
                v       =   max;
            else if (v < min)
                v       =  min;

            *(dst++)    = v;
        }
    }

    void limit2(float *dst, const float *src, float min, float max, size_t count)
    {
        while (count--)
        {
            float v     = *(src++);
            if (isnan(v))
                v       =   min;
            else if (isinf(v))
                v       =   (v < 0.0f) ? min : max;
            else if (v > max)
                v       =   max;
            else if (v < min)
                v       =  min;

            *(dst++)    = v;
        }
    }

    void sanitize1(float *dst, size_t count)
    {
        uint32_t *dptr = reinterpret_cast<uint32_t *>(dst);

        for (size_t i=0; i<count; ++i)
        {
            uint32_t v      = dptr[i];
            uint32_t a      = v & 0x7fffffff;   // Absolute value
            uint32_t s      = v & 0x80000000;   // Sign
            dptr[i]         = ((a > 0x007fffff) && (a <= 0x7f7fffff)) ? v : s;
        }
    }

    void sanitize2(float *dst, const float *src, size_t count)
    {
        uint32_t *dptr          = reinterpret_cast<uint32_t *>(dst);
        const uint32_t *sptr    = reinterpret_cast<const uint32_t *>(src);

        for (size_t i=0; i<count; ++i)
        {
            uint32_t v      = sptr[i];
            uint32_t a      = v & 0x7fffffff;   // Absolute value
            uint32_t s      = v & 0x80000000;   // Sign
            dptr[i]         = ((a > 0x007fffff) && (a <= 0x7f7fffff)) ? v : s;
        }
    }
}

#endif /* DSP_ARCH_NATIVE_FLOAT_H_ */
