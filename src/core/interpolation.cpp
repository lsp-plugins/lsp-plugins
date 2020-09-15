/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 23 сент. 2016 г.
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

#include <core/interpolation.h>
#include <math.h>

namespace lsp
{
    namespace interpolation
    {
        void hermite_quadratic(float *p, float x0, float y0, float k0, float x1, float k1)
        {
            // y = p[0]*x^2 + p[1]*x + p[2]
            p[0]    = (k0 - k1)*0.5f / (x0 - x1);
            p[1]    = k0 - 2.0f*p[0]*x0;
            p[2]    = y0 - p[0]*x0*x0 - p[1]*x0;
        }

        void hermite_cubic(float *p, float x0, float y0, float k0, float x1, float y1, float k1)
        {
            // y = p[0]*x^3 + p[1]*x^2 + p[2]*x + p[3]
            // dy/dx = 3*p[0]*x^2 + 2*p[1]*x + p[2]
            double dx    = x1 - x0;
            double dy    = y1 - y0;
            double kx    = dy / dx;
            double xx1   = x1*x1;
            double xx2   = x0 + x1;

            double a     = ((k0 + k1)*dx - 2.0f*dy) / (dx*dx*dx);
            double b     = ((kx - k0) + a*((2.0f*x0-x1)*x0 - xx1))/dx;
            double c     = kx - a*(xx1+xx2*x0) - b*xx2;
            double d     = y0 - x0*(c+x0*(b+x0*a));

            p[0]    = a;
            p[1]    = b;
            p[2]    = c;
            p[3]    = d;
        }

        void exponent(float *p, float x0, float y0, float x1, float y1, float k)
        {
            double e        = exp(k*(x0 - x1));
            p[0]            = (y0 - e*y1) / (1.0 - e);
            p[1]            = (y0 - p[0]) / exp(k*x0);
            p[2]            = k;
        }

        void linear(float *p, float x0, float y0, float x1, float y1)
        {
            p[0]            = (y1 - y0) / (x1 - x0);
            p[1]            = y0 - p[0]*x0;
        }
    }
}


