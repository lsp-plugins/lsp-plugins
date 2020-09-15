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

#ifndef CORE_INTERPOLATION_H_
#define CORE_INTERPOLATION_H_

#include <core/types.h>

namespace lsp
{
    namespace interpolation
    {
        /** Perform quadratic Hermite interpolation
         *
         * @param p destination (3 floats) to store the final hermite polynom
         * @param x0 x-coordinate of first point used for interpolation
         * @param y0 y-coordinate of first point used for interpolation
         * @param k0 the tangent angle of the line at first point
         * @param x1 x-coordinate of second point used for interpolation
         * @param k1 the tangent angle of the line at second point
         */
        void hermite_quadratic(float *p, float x0, float y0, float k0, float x1, float k1);

        /** Perform cubic Hermite interpolation
         *
         * @param p destination (4 floats) to store the final hermite polynom
         * @param x0 x-coordinate of first point used for interpolation
         * @param y0 y-coordinate of first point used for interpolation
         * @param k0 the tangent angle of the line at first point
         * @param x1 x-coordinate of second point used for interpolation
         * @param y1 y-coordinate of second point used for interpolation
         * @param k1 the tangent angle of the line at second point
         */
        void hermite_cubic(float *p, float x0, float y0, float k0, float x1, float y1, float k1);

        /** Perform exponent interpolation
         *
         * @param p destination (3 floats) to store the formula: y(x) = p[0] + p[1] * exp(p[2] * x)
         * @param x0 x-coordinate of first point used for interpolation
         * @param y0 y-coordinate of first point used for interpolation
         * @param x1 x-coordinate of second point used for interpolation
         * @param y1 y-coordinate of second point used for interpolation
         * @param k growing/lowering coefficient
         */
        void exponent(float *p, float x0, float y0, float x1, float y1, float k);

        /** Perform linear interpolation
         *
         * @param p destination (2 floats) to store the formula: y(x) = p[0]*x + p[1]
         * @param x0 x-coordinate of first point used for interpolation
         * @param y0 y-coordinate of first point used for interpolation
         * @param x1 x-coordinate of second point used for interpolation
         * @param y1 y-coordinate of second point used for interpolation
         */
        void linear(float *p, float x0, float y0, float x1, float y1);
    }
}

#endif /* CORE_INTERPOLATION_H_ */
