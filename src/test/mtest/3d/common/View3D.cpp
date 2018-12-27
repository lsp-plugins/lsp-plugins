/*
 * View.cpp
 *
 *  Created on: 24 дек. 2018 г.
 *      Author: sadko
 */

#include <test/mtest/3d/common/View3D.h>

namespace mtest
{
    
    View3D::View3D()
    {
    }
    
    View3D::~View3D()
    {
    }

    void View3D::clear(size_t flags)
    {
        if (flags & V3D_VERTEXES)
            vVertexes.flush();
        if (flags & V3D_RAYS)
            vRays.flush();
        if (flags & V3D_SEGMENTS)
            vSegments.flush();
        if (flags & V3D_POINTS)
            vPoints.flush();
    }

    bool View3D::add_ray(const v_ray3d_t *r)
    {
        return vRays.append(r);
    }

    bool View3D::add_point(const v_point3d_t *p)
    {
        return vPoints.append(p);
    }

    bool View3D::add_segment(const v_segment3d_t *s)
    {
        return vSegments.append(s);
    }

    bool View3D::add_triangle(const v_vertex3d_t *vi)
    {
        v_vertex3d_t *v = vVertexes.append_n(3);
        if (v == NULL)
            return false;

        v[0] = vi[0];
        v[1] = vi[1];
        v[2] = vi[2];

        return true;
    }

    bool View3D::add_triangle(const v_triangle3d_t *t)
    {
        v_vertex3d_t *v = vVertexes.append_n(3);
        if (v == NULL)
            return false;

        v[0].p      = t->p[0];
        v[0].n      = t->n[0];
        v[0].c      = t->c[0];

        v[1].p      = t->p[1];
        v[1].n      = t->n[1];
        v[1].c      = t->c[1];

        v[2].p      = t->p[2];
        v[2].n      = t->n[2];
        v[2].c      = t->c[2];

        return true;
    }

    bool View3D::add_triangle(const v_vertex3d_t *v1, const v_vertex3d_t *v2, const v_vertex3d_t *v3)
    {
        v_vertex3d_t *v = vVertexes.append_n(3);
        if (v == NULL)
            return false;

        v[0] = *v1;
        v[1] = *v2;
        v[2] = *v3;
        return true;
    }

    v_ray3d_t *View3D::get_ray(size_t index)
    {
        return vRays.get(index);
    }

    v_point3d_t *View3D::get_point(size_t index)
    {
        return vPoints.get(index);
    }

    v_segment3d_t *View3D::get_segment(size_t index)
    {
        return vSegments.get(index);
    }

    v_vertex3d_t *View3D::get_vertex(size_t index)
    {
        return vVertexes.get(index);
    }

} /* namespace mtest */
