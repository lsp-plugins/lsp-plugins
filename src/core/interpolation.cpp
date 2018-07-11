/*
 * interpolation.cpp
 *
 *  Created on: 23 сент. 2016 г.
 *      Author: sadko
 */

#include <core/interpolation.h>

namespace lsp
{
    namespace interpolation
    {
        void hermite_quadratic(float *p, float x0, float y0, float k0, float x1, float k1)
        {
            p[0]    = (k0-k1) * 0.5f / (x0 - x1);
            p[1]    = k0 - 2.0f*p[0]*x0;
            p[2]    = y0 - p[0]*x0*x0 - p[1]*x0;
        }
    }
}


