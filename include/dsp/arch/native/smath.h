/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 1 апр. 2018 г.
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

#ifndef DSP_ARCH_NATIVE_SMATH_H_
#define DSP_ARCH_NATIVE_SMATH_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

namespace native
{
    float ipowf(float x, int deg)
    {
        float res = 1.0f;
        if (!deg)
            return res;

        if (deg < 0)
        {
            deg = -deg;

            do
            {
                if (deg & 1)
                {
                    --deg;
                    res *= x;
                }
                else
                {
                    deg >>= 1;
                    x *= x;
                }
            }
            while (deg);

            res = 1.0f / res;
        }
        else
        {
            do
            {
                if (deg & 1)
                {
                    --deg;
                    res *= x;
                }
                else
                {
                    deg >>= 1;
                    x *= x;
                }
            }
            while (deg);
        }

        return res;
    }

    float ipospowf(float x, int deg)
    {
        if (deg <= 1)
            return x;

        float res = 1.0f;

        do
        {
            if (deg & 1)
            {
                --deg;
                res *= x;
            }
            else
            {
                deg >>= 1;
                x *= x;
            }
        }
        while (deg);

        return res;
    }

    float irootf(float x, int deg)
    {
        // Check validity of arguments
        if (deg <= 1)
            return x;

        // While root is odd, simple calc square root
        if (!(deg & 1))
        {
            do
            {
                x = sqrtf(x);
                deg >>= 1;
            } while (!(deg & 1));

            if (deg <= 1)
                return x;
        }

        // Newton method
        float xp;
        float a = 1.0f / float(deg);
        float k = float(--deg) * a;
        a *= x;

        do
        {
            xp      = x;
            x       = k*x + a / ipospowf(x, deg);
        } while (fabs(x - xp) > fabs(1e-5 * x));

        return x;
    }
}

#endif /* DSP_ARCH_NATIVE_SMATH_H_ */
