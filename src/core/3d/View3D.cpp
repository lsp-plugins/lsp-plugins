/*
 * View.cpp
 *
 *  Created on: 24 дек. 2018 г.
 *      Author: sadko
 */

#include <core/3d/View3D.h>

namespace lsp
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
        if (flags & V3D_VERTEXES2)
            vVertexes2.flush();
    }

    void View3D::swap(View3D *dst)
    {
        vVertexes.swap(&dst->vVertexes);
        vRays.swap(&dst->vRays);
        vSegments.swap(&dst->vSegments);
        vPoints.swap(&dst->vPoints);
        vVertexes2.swap(&dst->vVertexes2);
    }

    void View3D::add_all(const View3D *src)
    {
        vVertexes.add_all(&src->vVertexes);
        vVertexes2.add_all(&src->vVertexes2);
        vRays.add_all(&src->vRays);
        vSegments.add_all(&src->vSegments);
        vPoints.add_all(&src->vPoints);
    }

    bool View3D::add_ray(const v_ray3d_t *r)
    {
        return vRays.append(r);
    }

    bool View3D::add_point(const v_point3d_t *p)
    {
        return vPoints.append(p);
    }

    bool View3D::add_point(const point3d_t *p, const color3d_t *c)
    {
        v_point3d_t xp;
        xp.p = *p;
        xp.c = *c;
        return vPoints.append(&xp);
    }

    bool View3D::add_segment(const v_segment3d_t *s)
    {
        return vSegments.append(s);
    }

    bool View3D::add_segment(const rtm_edge_t *s, const color3d_t *c)
    {
        v_segment3d_t xs;
        xs.p[0]     = *(s->v[0]);
        xs.p[1]     = *(s->v[1]);
        xs.c[0]     = *c;
        xs.c[1]     = *c;

        return vSegments.append(&xs);
    }

    bool View3D::add_segment(const rtm_edge_t *s, const color3d_t *c1, const color3d_t *c2)
    {
        v_segment3d_t xs;
        xs.p[0]     = *(s->v[0]);
        xs.p[1]     = *(s->v[1]);
        xs.c[0]     = *c1;
        xs.c[1]     = *c2;

        return vSegments.append(&xs);
    }

    bool View3D::add_segment(const point3d_t *p1, const point3d_t *p2, const color3d_t *c)
    {
        v_segment3d_t xs;
        xs.p[0]     = *p1;
        xs.p[1]     = *p2;
        xs.c[0]     = *c;
        xs.c[1]     = *c;

        return vSegments.append(&xs);
    }

    bool View3D::add_segment(const rt_split_t *s, const color3d_t *c)
    {
        v_segment3d_t xs;
        xs.p[0]     = s->p[0];
        xs.p[1]     = s->p[1];
        xs.c[0]     = *c;
        xs.c[1]     = *c;

        return vSegments.append(&xs);
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

    bool View3D::add_triangle(const bsp_triangle_t *t)
    {
        v_vertex3d_t *v = vVertexes.append_n(3);
        if (v == NULL)
            return false;

        v[0].p  = t->v[0];
        v[0].n  = t->n[0];
        v[0].c  = t->c;

        v[1].p  = t->v[1];
        v[1].n  = t->n[1];
        v[1].c  = t->c;

        v[2].p  = t->v[2];
        v[2].n  = t->n[2];
        v[2].c  = t->c;

        return true;
    }

    bool View3D::add_triangle(const bsp_triangle_t *t, const color3d_t *c)
    {
        v_vertex3d_t *v = vVertexes.append_n(3);
        if (v == NULL)
            return false;

        v[0].p  = t->v[0];
        v[0].n  = t->n[0];
        v[0].c  = *c;

        v[1].p  = t->v[1];
        v[1].n  = t->n[1];
        v[1].c  = *c;

        v[2].p  = t->v[2];
        v[2].n  = t->n[2];
        v[2].c  = *c;

        return true;
    }

    bool View3D::add_triangle(const obj_triangle_t *vi, const color3d_t *c0, const color3d_t *c1, const color3d_t *c2)
    {
        v_vertex3d_t *v = vVertexes.append_n(3);
        if (v == NULL)
            return false;


        v[0].p  = *(vi->v[0]);
        v[0].n  = *(vi->n[0]);
        v[0].c  = *c0;

        v[1].p  = *(vi->v[1]);
        v[1].n  = *(vi->n[1]);
        v[1].c  = *c1;

        v[2].p  = *(vi->v[2]);
        v[2].n  = *(vi->n[2]);
        v[2].c  = *c2;

        return true;
    }

    bool View3D::add_triangle_1c(const v_triangle3d_t *t, const color3d_t *c)
    {
        v_vertex3d_t *v = vVertexes.append_n(3);
        if (v == NULL)
            return false;

        v[0].p      = t->p[0];
        v[0].n      = t->n[0];
        v[0].c      = *c;

        v[1].p      = t->p[1];
        v[1].n      = t->n[1];
        v[1].c      = *c;

        v[2].p      = t->p[2];
        v[2].n      = t->n[2];
        v[2].c      = *c;

        return true;
    }

    bool View3D::add_triangle_pvnc1(const point3d_t *t, const vector3d_t *n, const color3d_t *c)
    {
        v_vertex3d_t *v = vVertexes.append_n(3);
        if (v == NULL)
            return false;

        v[0].p      = t[0];
        v[0].n      = *n;
        v[0].c      = *c;

        v[1].p      = t[1];
        v[1].n      = *n;
        v[1].c      = *c;

        v[2].p      = t[2];
        v[2].n      = *n;
        v[2].c      = *c;

        return true;
    }

    bool View3D::add_triangle_pvnc3(const point3d_t *t, const vector3d_t *n, const color3d_t *c0, const color3d_t *c1, const color3d_t *c2)
    {
        v_vertex3d_t *v = vVertexes.append_n(3);
        if (v == NULL)
            return false;

        v[0].p      = t[0];
        v[0].n      = *n;
        v[0].c      = *c0;

        v[1].p      = t[1];
        v[1].n      = *n;
        v[1].c      = *c1;

        v[2].p      = t[2];
        v[2].n      = *n;
        v[2].c      = *c2;

        return true;
    }

    bool View3D::add_triangle_3c(const v_triangle3d_t *t, const color3d_t *c0, const color3d_t *c1, const color3d_t *c2)
    {
        v_vertex3d_t *v = vVertexes.append_n(3);
        if (v == NULL)
            return false;

        v[0].p      = t->p[0];
        v[0].n      = t->n[0];
        v[0].c      = *c0;

        v[1].p      = t->p[1];
        v[1].n      = t->n[1];
        v[1].c      = *c1;

        v[2].p      = t->p[2];
        v[2].n      = t->n[2];
        v[2].c      = *c2;

        return true;
    }

    bool View3D::add_triangle_3c(const obj_triangle_t *t, const color3d_t *c0, const color3d_t *c1, const color3d_t *c2)
    {
        v_vertex3d_t *v = vVertexes.append_n(3);
        if (v == NULL)
            return false;

        v[0].p          = *(t->v[0]);
        v[0].n          = *(t->n[0]);
        v[0].c          = *c0;

        v[1].p          = *(t->v[1]);
        v[1].n          = *(t->n[1]);
        v[1].c          = *c1;

        v[2].p          = *(t->v[2]);
        v[2].n          = *(t->n[2]);
        v[2].c          = *c2;

        return true;
    }

    bool View3D::add_triangle_1c(const obj_triangle_t *t, const color3d_t *c)
    {
        v_vertex3d_t *v = vVertexes.append_n(3);
        if (v == NULL)
            return false;

        v[0].p          = *(t->v[0]);
        v[0].n          = *(t->n[0]);
        v[0].c          = *c;

        v[1].p          = *(t->v[1]);
        v[1].n          = *(t->n[1]);
        v[1].c          = *c;

        v[2].p          = *(t->v[2]);
        v[2].n          = *(t->n[2]);
        v[2].c          = *c;

        return true;
    }

    bool View3D::add_triangle_1c(const rt_triangle_t *t, const color3d_t *c)
    {
        v_vertex3d_t *v = vVertexes.append_n(3);
        if (v == NULL)
            return false;

        v[0].p          = t->v[0];
        v[0].n          = t->n;
        v[0].n.dw       = 0.0f;
        v[0].c          = *c;

        v[1].p          = t->v[1];
        v[1].n          = t->n;
        v[1].n.dw       = 0.0f;
        v[1].c          = *c;

        v[2].p          = t->v[2];
        v[2].n          = t->n;
        v[2].n.dw       = 0.0f;
        v[2].c          = *c;

        return true;
    }

    bool View3D::add_triangle_3c(const rtm_triangle_t *t, const color3d_t *c0, const color3d_t *c1, const color3d_t *c2)
    {
        v_vertex3d_t *v = vVertexes.append_n(3);
        if (v == NULL)
            return false;

        v[0].p          = *(t->v[0]);
        v[0].n          = t->n;
        v[0].c          = *c0;

        v[1].p          = *(t->v[1]);
        v[1].n          = t->n;
        v[1].c          = *c1;

        v[2].p          = *(t->v[2]);
        v[2].n          = t->n;
        v[2].c          = *c2;

        return true;
    }

    bool View3D::add_triangle_3c(const rt_triangle_t *t, const color3d_t *c0, const color3d_t *c1, const color3d_t *c2)
    {
        v_vertex3d_t *v = vVertexes.append_n(3);
        if (v == NULL)
            return false;

        v[0].p          = t->v[0];
        v[0].n          = t->n;
        v[0].n.dw       = 0.0f;
        v[0].c          = *c0;

        v[1].p          = t->v[1];
        v[1].n          = t->n;
        v[1].n.dw       = 0.0f;
        v[1].c          = *c1;

        v[2].p          = t->v[2];
        v[2].n          = t->n;
        v[2].n.dw       = 0.0f;
        v[2].c          = *c2;

        return true;
    }

    bool View3D::add_triangle_1c(const rtm_triangle_t *t, const color3d_t *c)
    {
        v_vertex3d_t *v = vVertexes.append_n(3);
        if (v == NULL)
            return false;

        v[0].p          = *(t->v[0]);
        v[0].n          = t->n;
        v[0].c          = *c;

        v[1].p          = *(t->v[1]);
        v[1].n          = t->n;
        v[1].c          = *c;

        v[2].p          = *(t->v[2]);
        v[2].n          = t->n;
        v[2].c          = *c;

        return true;
    }

    bool View3D::add_plane_3p1c(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3, const color3d_t *c)
    {
        point3d_t t[3];
        t[0] = *p1;
        t[1] = *p2;
        t[2] = *p3;
        return add_plane_pv1c(t, c);
    }

    bool View3D::add_plane_3pn1c(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3, const vector3d_t *n, const color3d_t *c)
    {
        point3d_t t[3];
        t[0] = *p1;
        t[1] = *p2;
        t[2] = *p3;
        return add_plane_pvn1c(t, n, c);
    }

    void View3D::add_plane_2pn1c(const point3d_t *p1, const point3d_t *p2, const vector3d_t *n, const color3d_t *c)
    {
        point3d_t pt[3], xp[3], pb[3];
        vector3d_t d, v;

        xp[0]       = *p1;
        xp[1].x     = 0.5f * (p1->x + p2->x);
        xp[1].y     = 0.5f * (p1->y + p2->y);
        xp[1].z     = 0.5f * (p1->z + p2->z);
        xp[1].w     = 1.0f;
        xp[2]       = *p2;

        v.dx        = p2->x - p1->x;
        v.dy        = p2->y - p1->y;
        v.dz        = p2->z - p1->z;
        v.dw        = 0.0f;

        dsp::calc_normal3d_v2(&d, &v, n);

        for (size_t i=0; i<3; ++i)
        {
            pt[i].x     = xp[i].x + 0.5f * d.dx;
            pt[i].y     = xp[i].y + 0.5f * d.dy;
            pt[i].z     = xp[i].z + 0.5f * d.dz;
            pt[i].w     = 1.0f;

            pb[i].x     = xp[i].x - 0.5f * d.dx;
            pb[i].y     = xp[i].y - 0.5f * d.dy;
            pb[i].z     = xp[i].z - 0.5f * d.dz;
            pb[i].w     = 1.0f;
        }

        add_segment(&pt[0], &pb[0], c);
        add_segment(&pt[1], &pb[1], c);
        add_segment(&pt[2], &pb[2], c);

        add_segment(&pt[0], &pt[2], c);
        add_segment(&xp[0], &xp[2], c);
        add_segment(&pb[0], &pb[2], c);

        add_segment(&pt[0], &pb[2], c);
        add_segment(&pt[2], &pb[0], c);

        v_ray3d_t ray;
        ray.p       = xp[1];
        ray.v       = *n;
        ray.v.dw    = 0.0f;
        ray.c       = *c;
        add_ray(&ray);
    }

    bool View3D::add_plane_sp3p1c(const point3d_t *sp, const point3d_t *p1, const point3d_t *p2, const point3d_t *p3, const color3d_t *c)
    {
        vector3d_t n;
        point3d_t t[3];
        t[0] = *p1;
        t[1] = *p2;
        t[2] = *p3;
        dsp::calc_oriented_plane_p3(&n, sp, p1, p2, p3);
        return add_plane_pvn1c(t, &n, c);
    }

    bool View3D::add_plane_pv1c(const point3d_t *t, const color3d_t *c)
    {
        v_ray3d_t *r = vRays.append();
        if (r == NULL)
            return false;
        v_segment3d_t *v = vSegments.append_n(6);
        if (v == NULL)
        {
            vRays.remove_last();
            return false;
        }

        v[0].p[0]   = t[0];
        v[1].p[0]   = t[1];
        v[2].p[0]   = t[2];

        v[0].p[1]   = t[1];
        v[1].p[1]   = t[2];
        v[2].p[1]   = t[0];

        v[0].c[0]   = *c;
        v[0].c[1]   = *c;
        v[1].c[0]   = *c;
        v[1].c[1]   = *c;
        v[2].c[0]   = *c;
        v[2].c[1]   = *c;

        point3d_t mp[3];
        mp[0].x     = 0.5f * (t[1].x + t[2].x);
        mp[0].y     = 0.5f * (t[1].y + t[2].y);
        mp[0].z     = 0.5f * (t[1].z + t[2].z);

        mp[1].x     = 0.5f * (t[2].x + t[0].x);
        mp[1].y     = 0.5f * (t[2].y + t[0].y);
        mp[1].z     = 0.5f * (t[2].z + t[0].z);

        mp[2].x     = 0.5f * (t[0].x + t[1].x);
        mp[2].y     = 0.5f * (t[0].y + t[1].y);
        mp[2].z     = 0.5f * (t[0].z + t[1].z);

        v[3].p[0]   = t[0];
        v[4].p[0]   = t[1];
        v[5].p[0]   = t[2];

        v[3].p[1]   = mp[0];
        v[4].p[1]   = mp[1];
        v[5].p[1]   = mp[2];

        v[3].c[0]   = *c;
        v[3].c[1]   = *c;
        v[4].c[0]   = *c;
        v[4].c[1]   = *c;
        v[5].c[0]   = *c;
        v[5].c[1]   = *c;

        r->p.x      = (t[0].x + t[1].x + t[2].x)/ 3.0f;
        r->p.y      = (t[0].y + t[1].y + t[2].y)/ 3.0f;
        r->p.z      = (t[0].z + t[1].z + t[2].z)/ 3.0f;
        r->p.w      = 1.0f;

        r->c        = *c;

        dsp::calc_normal3d_pv(&r->v, t);

        return true;
    }

    bool View3D::add_plane_pvn1c(const point3d_t *t, const vector3d_t *n, const color3d_t *c)
    {
        v_ray3d_t *r = vRays.append();
        if (r == NULL)
            return false;
        v_segment3d_t *v = vSegments.append_n(6);
        if (v == NULL)
        {
            vRays.remove_last();
            return false;
        }

        v[0].p[0]   = t[0];
        v[1].p[0]   = t[1];
        v[2].p[0]   = t[2];

        v[0].p[1]   = t[1];
        v[1].p[1]   = t[2];
        v[2].p[1]   = t[0];

        v[0].c[0]   = *c;
        v[0].c[1]   = *c;
        v[1].c[0]   = *c;
        v[1].c[1]   = *c;
        v[2].c[0]   = *c;
        v[2].c[1]   = *c;

        point3d_t mp[3];
        mp[0].x     = 0.5f * (t[1].x + t[2].x);
        mp[0].y     = 0.5f * (t[1].y + t[2].y);
        mp[0].z     = 0.5f * (t[1].z + t[2].z);

        mp[1].x     = 0.5f * (t[2].x + t[0].x);
        mp[1].y     = 0.5f * (t[2].y + t[0].y);
        mp[1].z     = 0.5f * (t[2].z + t[0].z);

        mp[2].x     = 0.5f * (t[0].x + t[1].x);
        mp[2].y     = 0.5f * (t[0].y + t[1].y);
        mp[2].z     = 0.5f * (t[0].z + t[1].z);

        v[3].p[0]   = t[0];
        v[4].p[0]   = t[1];
        v[5].p[0]   = t[2];

        v[3].p[1]   = mp[0];
        v[4].p[1]   = mp[1];
        v[5].p[1]   = mp[2];

        v[3].c[0]   = *c;
        v[3].c[1]   = *c;
        v[4].c[0]   = *c;
        v[4].c[1]   = *c;
        v[5].c[0]   = *c;
        v[5].c[1]   = *c;

        r->p.x      = (t[0].x + t[1].x + t[2].x)/ 3.0f;
        r->p.y      = (t[0].y + t[1].y + t[2].y)/ 3.0f;
        r->p.z      = (t[0].z + t[1].z + t[2].z)/ 3.0f;
        r->p.w      = 1.0f;

        r->c        = *c;
        r->v        = *n;
        r->v.dw     = 0.0f;

        return true;
    }

    bool View3D::add_triangle_pv1c(const point3d_t *pv, const color3d_t *c)
    {
        v_vertex3d_t *v = vVertexes.append_n(3);
        if (v == NULL)
            return false;

        vector3d_t n;
        dsp::calc_normal3d_pv(&n, pv);

        v[0].p      = pv[0];
        v[0].n      = n;
        v[0].c      = *c;

        v[1].p      = pv[1];
        v[1].n      = n;
        v[1].c      = *c;

        v[2].p      = pv[2];
        v[2].n      = n;
        v[2].c      = *c;

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

    void View3D::add_view_1c(const rt_view_t *v, const color3d_t *c)
    {
        add_view_3c(v, c, c, c);
    }

    /**
     * Add context view
     * @param v context view to add
     */
    void View3D::add_view_3c(const rt_view_t *v, const color3d_t *c0, const color3d_t *c1, const color3d_t *c2)
    {
        v_ray3d_t r;
        v_segment3d_t s, ms;

        // State
        dsp::init_vector_p2(&r.v, &v->s, &v->p[0]);
        r.p     = v->p[0];
        r.c     = *c0;
        s.p[0]  = v->p[0];
        s.p[1]  = v->p[1];
        s.c[0]  = *c0;
        s.c[1]  = *c0;
        ms.p[0] = v->s;
        ms.p[1] = v->p[0];
        ms.c[0] = *c0;
        ms.c[1] = *c0;
        add_ray(&r);
        add_segment(&s);
        add_segment(&ms);

        dsp::init_vector_p2(&r.v, &v->s, &v->p[1]);
        r.p     = v->p[1];
        r.c     = *c1;
        s.p[0]  = v->p[1];
        s.p[1]  = v->p[2];
        s.c[0]  = *c1;
        s.c[1]  = *c1;
        ms.p[0] = v->s;
        ms.p[1] = v->p[1];
        ms.c[0]   = *c1;
        ms.c[1]   = *c1;
        add_ray(&r);
        add_segment(&s);
        add_segment(&ms);

        dsp::init_vector_p2(&r.v, &v->s, &v->p[2]);
        r.p     = v->p[2];
        r.c     = *c2;
        s.p[0]  = v->p[2];
        s.p[1]  = v->p[0];
        s.c[0]    = *c2;
        s.c[1]    = *c2;
        ms.p[0] = v->s;
        ms.p[1] = v->p[2];
        ms.c[0]   = *c2;
        ms.c[1]   = *c2;
        add_ray(&r);
        add_segment(&s);
        add_segment(&ms);
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

    v_vertex3d_t *View3D::get_vertex2(size_t index)
    {
        return vVertexes2.get(index);
    }

    void View3D::dump(rt_plan_t *plan, const color3d_t *c)
    {
        v_segment3d_t *xs;
        color3d_t xc;

        size_t n = plan->items.size();
        if (n == 0)
            return;

        float k = 0.75f / n;

        for (size_t i=0; i<n; ++i)
        {
            rt_split_t *s = plan->items.get(i);
            xs = vSegments.add();
            if (xs == NULL)
                return;

            float d     = 0.25f + (n - i)*k;
            xc.r        = c->r * d;
            xc.g        = c->g * d;
            xc.b        = c->b * d;
            xc.a        = 0.0f;

            xs->p[0]    = s->p[0];
            xs->p[1]    = s->p[1];
            xs->c[0]    = xc;
            xs->c[1]    = xc;
        }
    }

} /* namespace mtest */
