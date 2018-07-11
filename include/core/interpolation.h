/*
 * interpolation.h
 *
 *  Created on: 23 сент. 2016 г.
 *      Author: sadko
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
