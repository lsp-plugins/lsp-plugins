/*
 * helpers.cpp
 *
 *  Created on: 30 авг. 2018 г.
 *      Author: sadko
 */


#include <test/utest/dsp/3d/helpers.h>

namespace test
{
    bool point3d_ck(const point3d_t *p1, const point3d_t *p2)
    {
        return
            (float_equals_relative(p1->x, p2->x)) &&
            (float_equals_relative(p1->y, p2->y)) &&
            (float_equals_relative(p1->z, p2->z));
    }

    bool point3d_sck(const point3d_t *p1, const point3d_t *p2)
    {
        return
            (float_equals_relative(p1->x, p2->x)) &&
            (float_equals_relative(p1->y, p2->y)) &&
            (float_equals_relative(p1->z, p2->z)) &&
            (float_equals_relative(p1->w, p2->w));
    }

    bool vector3d_sck(const vector3d_t *v1, const vector3d_t *v2)
    {
        return
            (float_equals_relative(v1->dx, v2->dx)) &&
            (float_equals_relative(v1->dy, v2->dy)) &&
            (float_equals_relative(v1->dz, v2->dz)) &&
            (float_equals_relative(v1->dw, v2->dw));
    }

    bool matrix3d_ck(const matrix3d_t *m1, const matrix3d_t *m2)
    {
        for (size_t i=0; i<16; ++i)
            if (!float_equals_relative(m1->m[i], m2->m[i]))
                return false;
        return true;
    }
}

