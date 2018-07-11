/*
 * RayTrace3D.cpp
 *
 *  Created on: 9 апр. 2017 г.
 *      Author: sadko
 */

#include <core/alloc.h>
#include <core/3d/RayTrace3D.h>

namespace lsp
{
    RayTrace3D::RayTrace3D()
    {
    }

    RayTrace3D::~RayTrace3D()
    {
        destroy();
    }

    void RayTrace3D::destroy()
    {
        sRays.flush();
    }

    bool RayTrace3D::push(const ray3d_t *r)
    {
        raytrace3d_t *rt = sRays.append();
        if (rt == NULL)
            return false;

        dsp::init_raytrace3d_r(rt, r);
        return true;
    }

    bool RayTrace3D::push(const ray3d_t *r, const intersection3d_t *ix)
    {
        raytrace3d_t *rt = sRays.append();
        if (rt == NULL)
            return false;

        dsp::init_raytrace3d_ix(rt, r, ix);
        return true;
    }

    bool RayTrace3D::push(const raytrace3d_t *r)
    {
        raytrace3d_t *rt = sRays.append();
        if (rt == NULL)
            return false;

        dsp::init_raytrace3d(rt, r);
        return true;
    }

    bool RayTrace3D::pop(raytrace3d_t *r)
    {
        return sRays.pop(r);
    }

} /* namespace lsp */
