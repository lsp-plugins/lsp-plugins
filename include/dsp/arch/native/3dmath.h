/*
 * 3dmath.h
 *
 *  Created on: 9 апр. 2017 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_NATIVE_3DMATH_H_
#define DSP_ARCH_NATIVE_3DMATH_H_

#ifndef __DSP_NATIVE_IMPL
    #error "This header should not be included directly"
#endif /* __DSP_NATIVE_IMPL */

namespace native
{
    inline void init_point_xyz(point3d_t *p, float x, float y, float z)
    {
        p->x        = x;
        p->y        = y;
        p->z        = z;
        p->w        = 1.0f;
    }

    void init_point(point3d_t *p, const point3d_t *s)
    {
        *p          = *s;
    }

    void normalize_point(point3d_t *p)
    {
        float w     = sqrtf(p->x * p->x + p->y * p->y + p->z * p->z);
        if (w == 0.0f)
            return;
        w           = 1.0f / w;
        p->x       *= w;
        p->y       *= w;
        p->z       *= w;
        p->w        = 1.0f;
    }

    void scale_point1(point3d_t *p, float r)
    {
        float w     = sqrtf(p->x * p->x + p->y * p->y + p->z * p->z);
        if (w == 0.0f)
            return;
        w           = r / w;
        p->x       *= w;
        p->y       *= w;
        p->z       *= w;
        p->w        = 1.0f;
    }

    void scale_point2(point3d_t *p, const point3d_t *s, float r)
    {
        float w     = sqrtf(s->x * s->x + s->y * s->y + s->z * s->z);
        if (w == 0.0f)
        {
            p->x        = s->x;
            p->y        = s->y;
            p->z        = s->z;
            p->w        = 1.0f;
            return;
        }
        w           = r / w;
        p->x        = s->x * w;
        p->y        = s->y * w;
        p->z        = s->z * w;
        p->w        = 1.0f;
    }

    void init_vector_dxyz(vector3d_t *v, float dx, float dy, float dz)
    {
        v->dx       = dx;
        v->dy       = dy;
        v->dz       = dz;
        v->dw       = 0.0f;
    }

    void init_vector_p2(vector3d_t *v, const point3d_t *p1, const point3d_t *p2)
    {
        v->dx       = p2->x - p1->x;
        v->dy       = p2->y - p1->y;
        v->dz       = p2->z - p1->z;
        v->dw       = 0.0f;
    }

    void init_vector_pv(vector3d_t *v, const point3d_t *pv)
    {
        v->dx       = pv[1].x - pv[0].x;
        v->dy       = pv[1].y - pv[0].y;
        v->dz       = pv[1].z - pv[0].z;
        v->dw       = 0.0f;
    }

    void init_vector(vector3d_t *p, const vector3d_t *s)
    {
        *p          = *s;
    }

    void normalize_vector(vector3d_t *v)
    {
        float w     = sqrtf(v->dx * v->dx + v->dy * v->dy + v->dz * v->dz);
        if (w == 0.0f)
            return;
        w           = 1.0f / w;
        v->dx      *= w;
        v->dy      *= w;
        v->dz      *= w;
        v->dw       = 0.0f;
    }

    void scale_vector1(vector3d_t *v, float r)
    {
        float w     = sqrtf(v->dx * v->dx + v->dy * v->dy + v->dz * v->dz);
        if (w == 0.0f)
            return;
        w           = r / w;
        v->dx      *= w;
        v->dy      *= w;
        v->dz      *= w;
        v->dw       = 0.0f;
    }

    void scale_vector2(vector3d_t *v, const vector3d_t *s, float r)
    {
        float w     = sqrtf(s->dx * s->dx + s->dy * s->dy + s->dz * s->dz);
        if (w == 0.0f)
        {
            v->dx       = s->dx;
            v->dy       = s->dy;
            v->dz       = s->dz;
            v->dw       = 0.0f;
            return;
        }
        w           = r / w;
        v->dx       = s->dx * w;
        v->dy       = s->dy * w;
        v->dz       = s->dz * w;
        v->dw       = 0.0f;
    }


    void init_normal3d_xyz(vector3d_t *v, float x1, float y1, float z1, float x2, float y2, float z2)
    {
        v->dx       = x2 - x1;
        v->dy       = y2 - y1;
        v->dz       = z2 - z1;
        v->dw       = 0.0f;
        normalize_vector(v);
    }

    void init_normal3d_dxyz(vector3d_t *v, float dx, float dy, float dz)
    {
        v->dx       = dx;
        v->dy       = dy;
        v->dz       = dz;
        v->dw       = 0.0f;
        normalize_vector(v);
    }

    void init_normal3d(vector3d_t *p, const vector3d_t *s)
    {
        *p          = *s;
        normalize_vector(p);
    }

    void init_ray_xyz(ray3d_t *l,
        float x0, float y0, float z0,
        float x1, float y1, float z1
    )
    {
        l->z.x      = x0;
        l->z.y      = y0;
        l->z.z      = z0;
        l->z.w      = 1.0f;

        l->v.dx     = (x1 - x0);
        l->v.dy     = (y1 - y0);
        l->v.dz     = (z1 - z0);
        l->v.dw     = 0.0f;
    }

    void init_ray_dxyz(ray3d_t *l,
        float x0, float y0, float z0,
        float dx, float dy, float dz
    )
    {
        l->z.x      = x0;
        l->z.y      = y0;
        l->z.z      = z0;
        l->z.w      = 1.0f;

        l->v.dx     = dx;
        l->v.dy     = dy;
        l->v.dz     = dz;
        l->v.dw     = 0.0f;
    }

    void init_ray_pdv(ray3d_t *l, const point3d_t *p, const vector3d_t *v)
    {
        l->z        = *p;
        l->v        = *v;
    }

    void init_ray_p2(ray3d_t *l, const point3d_t *p1, const point3d_t *p2)
    {
        l->z        = *p1;
        l->v.dx     = (p2->x - p1->x);
        l->v.dx     = (p2->y - p1->y);
        l->v.dx     = (p2->z - p1->z);
        l->v.dw     = 0.0f;
    }

    void init_ray_pv(ray3d_t *l, const point3d_t *p)
    {
        l->z        = p[0];
        l->v.dx     = (p[1].x - p[0].x);
        l->v.dx     = (p[1].y - p[0].y);
        l->v.dx     = (p[1].z - p[0].z);
        l->v.dw     = 0.0f;
    }

    void init_ray(ray3d_t *l, const ray3d_t *r)
    {
        *l          = *r;
    }

    void calc_ray_xyz(ray3d_t *l,
        float x0, float y0, float z0,
        float x1, float y1, float z1
    )
    {
        l->z.x      = x0;
        l->z.y      = y0;
        l->z.z      = z0;
        l->z.w      = 1.0f;

        l->v.dx     = (x1 - x0);
        l->v.dy     = (y1 - y0);
        l->v.dz     = (z1 - z0);
        l->v.dw     = 0.0f;

        normalize_vector(&l->v);
    }

    void calc_ray_dxyz(ray3d_t *l,
        float x0, float y0, float z0,
        float dx, float dy, float dz
    )
    {
        l->z.x      = x0;
        l->z.y      = y0;
        l->z.z      = z0;
        l->z.w      = 0.0f;

        l->v.dx     = dx;
        l->v.dy     = dy;
        l->v.dz     = dz;
        l->v.dw     = 0.0f;
        normalize_vector(&l->v);
    }

    void calc_ray_pdv(ray3d_t *l, const point3d_t *p, const vector3d_t *v)
    {
        l->z        = *p;
        l->v        = *v;
        normalize_vector(&l->v);
    }

    void calc_ray_p2(ray3d_t *l, const point3d_t *p1, const point3d_t *p2)
    {
        l->z        = *p1;
        l->v.dx     = (p2->x - p1->x);
        l->v.dx     = (p2->y - p1->y);
        l->v.dx     = (p2->z - p1->z);
        l->v.dw     = 0.0f;
        normalize_vector(&l->v);
    }

    void calc_ray_pv(ray3d_t *l, const point3d_t *p)
    {
        l->z        = p[0];
        l->v.dx     = (p[1].x - p[0].x);
        l->v.dx     = (p[1].y - p[0].y);
        l->v.dx     = (p[1].z - p[0].z);
        l->v.dw     = 0.0f;
        normalize_vector(&l->v);
    }

    void calc_ray(ray3d_t *l, const ray3d_t *r)
    {
        *l          = *r;
        normalize_vector(&l->v);
    }

    void init_segment_xyz(segment3d_t *s,
        float x0, float y0, float z0,
        float x1, float y1, float z1
    )
    {
        s->p[0].x       = x0;
        s->p[0].y       = y0;
        s->p[0].z       = z0;
        s->p[0].w       = 0.0f;

        s->p[1].x       = x1;
        s->p[1].y       = y1;
        s->p[1].z       = z1;
        s->p[1].w       = 0.0f;
    }

    void init_segment_p2(segment3d_t *s, const point3d_t *p1, const point3d_t *p2)
    {
        s->p[0]         = *p1;
        s->p[1]         = *p2;
    }

    void init_segment_pv(segment3d_t *s, const point3d_t *p)
    {
        s->p[0]         = p[0];
        s->p[1]         = p[1];
    }

    void calc_triangle3d_params(triangle3d_t *t)
    {
        // Calculate edge parameters
        vector3d_t d[3];
        d[0].dx     = t->p[1].x - t->p[0].x;
        d[0].dy     = t->p[1].y - t->p[0].y;
        d[0].dz     = t->p[1].z - t->p[0].z;
        d[0].dw     = t->p[1].w - t->p[0].w;

        d[1].dx     = t->p[2].x - t->p[1].x;
        d[1].dy     = t->p[2].y - t->p[1].y;
        d[1].dz     = t->p[2].z - t->p[1].z;
        d[1].dw     = t->p[2].w - t->p[1].w;

        d[2].dx     = t->p[2].x - t->p[0].x;
        d[2].dy     = t->p[2].y - t->p[0].y;
        d[2].dz     = t->p[2].z - t->p[0].z;
        d[2].dw     = t->p[2].w - t->p[0].w;

        // Do vector multiplication to calculate the normal vector
        t->n.dx     = + d[0].dy*d[2].dz - d[0].dz*d[2].dy;
        t->n.dy     = - d[0].dx*d[2].dz + d[0].dz*d[2].dx;
        t->n.dz     = + d[0].dx*d[2].dy - d[0].dy*d[2].dx;
        t->n.dw     = - ( t->n.dx * t->p[0].x + t->n.dy * t->p[0].y + t->n.dz * t->p[0].z); // Parameter for the plane equation

        // Calculate lengths of edges and (additionally) length of normal
        float l[4];
        l[0]        = sqrtf(d[0].dx*d[0].dx + d[0].dy*d[0].dy + d[0].dz*d[0].dz);
        l[1]        = sqrtf(d[1].dx*d[1].dx + d[1].dy*d[1].dy + d[1].dz*d[1].dz);
        l[2]        = sqrtf(d[2].dx*d[2].dx + d[2].dy*d[2].dy + d[2].dz*d[2].dz);
        l[3]        = sqrtf(t->n.dx*t->n.dx + t->n.dy*t->n.dy + t->n.dz*t->n.dz);

        // Calculate normal length and normalize normal
        t->n.dx    /= l[3];
        t->n.dy    /= l[3];
        t->n.dz    /= l[3];
        t->n.dw    /= l[3];

        // Additionally, calculate the length of edges
        t->p[0].w   = l[0];
        t->p[1].w   = l[1];
        t->p[2].w   = l[2];
    }

    void init_triangle3d_xyz(triangle3d_t *t,
            float x0, float y0, float z0,
            float x1, float y1, float z1,
            float x2, float y2, float z2
        )
    {
        // Initalize points
        t->p[0].x   = x0;
        t->p[0].y   = y0;
        t->p[0].z   = z0;
        t->p[0].w   = 0.0f;

        t->p[1].x   = x1;
        t->p[1].y   = y1;
        t->p[1].z   = z1;
        t->p[1].w   = 0.0f;

        t->p[2].x   = x2;
        t->p[2].y   = y2;
        t->p[2].z   = z2;
        t->p[2].w   = 0.0f;

        // Init normal
        t->n.dx     = 0.0f;
        t->n.dy     = 0.0f;
        t->n.dz     = 0.0f;
        t->n.dw     = 0.0f;
    }

    void init_triangle3d_p3(
            triangle3d_t *t,
            const point3d_t *p1,
            const point3d_t *p2,
            const point3d_t *p3
        )
    {
        // Initalize points
        t->p[0]     = *p1;
        t->p[1]     = *p2;
        t->p[2]     = *p3;

        // Init normal
        t->n.dx     = 0.0f;
        t->n.dy     = 0.0f;
        t->n.dz     = 0.0f;
        t->n.dw     = 0.0f;
    }

    void init_triangle3d_pv(
            triangle3d_t *t,
            const point3d_t *p
        )
    {
        // Initalize points
        t->p[0]     = p[0];
        t->p[1]     = p[1];
        t->p[2]     = p[2];

        // Init normal
        t->n.dx     = 0.0f;
        t->n.dy     = 0.0f;
        t->n.dz     = 0.0f;
        t->n.dw     = 0.0f;
    }

    void init_triangle3d(triangle3d_t *dst, const triangle3d_t *src)
    {
        dst->p[0]   = src->p[0];
        dst->p[1]   = src->p[1];
        dst->p[2]   = src->p[2];
        dst->n      = src->n;
    }

    void calc_triangle3d_xyz(triangle3d_t *t,
            float x0, float y0, float z0,
            float x1, float y1, float z1,
            float x2, float y2, float z2
        )
    {
        // Initalize points
        t->p[0].x   = x0;
        t->p[0].y   = y0;
        t->p[0].z   = z0;
        t->p[0].w   = 0.0f;

        t->p[1].x   = x1;
        t->p[1].y   = y1;
        t->p[1].z   = z1;
        t->p[1].w   = 0.0f;

        t->p[2].x   = x2;
        t->p[2].y   = y2;
        t->p[2].z   = z2;
        t->p[2].w   = 0.0f;

        // Calc parameters
        calc_triangle3d_params(t);
    }

    void calc_triangle3d_p3(
            triangle3d_t *t,
            const point3d_t *p1,
            const point3d_t *p2,
            const point3d_t *p3
        )
    {
        // Initalize points
        t->p[0]     = *p1;
        t->p[1]     = *p2;
        t->p[2]     = *p3;

        // Calc parameters
        calc_triangle3d_params(t);
    }

    void calc_triangle3d_pv(
            triangle3d_t *t,
            const point3d_t *p
        )
    {
        // Initalize points
        t->p[0]     = p[0];
        t->p[1]     = p[1];
        t->p[2]     = p[2];

        // Calc parameters
        calc_triangle3d_params(t);
    }

    void calc_triangle3d(triangle3d_t *dst, const triangle3d_t *src)
    {
        dst->p[0]   = src->p[0];
        dst->p[1]   = src->p[1];
        dst->p[2]   = src->p[2];
        calc_triangle3d_params(dst);
    }

    void init_intersection3d(intersection3d_t *is)
    {
        // Initialize intersection as not found
        is->p.x     = 0.0f;
        is->p.y     = 0.0f;
        is->p.z     = 0.0f;
        is->p.w     = DSP_3D_MAXVALUE;

        is->n       = 0;
    }

    void init_raytrace3d(raytrace3d_t *rt, const raytrace3d_t *r)
    {
        rt->r           = r->r;
        rt->x.p         = r->x.p;
        rt->amplitude   = r->amplitude;
        rt->delay       = r->delay;

        for (size_t i=0; i<r->x.n; ++i)
        {
            rt->x.t[i]      = r->x.t[i];
            rt->x.m[i]      = r->x.m[i];
        }
        rt->x.n         = r->x.n;
    }

    void init_raytrace3d_r(raytrace3d_t *rt, const ray3d_t *r)
    {
        rt->r           = *r;
        rt->amplitude   = 1.0f;
        rt->delay       = 0.0f;
        init_intersection3d(&rt->x);
    }

    void init_raytrace3d_ix(raytrace3d_t *rt, const ray3d_t *r, const intersection3d_t *ix)
    {
        rt->r           = *r;
        rt->x.p         = ix->p;
        rt->amplitude   = 1.0f;
        rt->delay       = 0.0f;

        for (size_t i=0; i<ix->n; ++i)
        {
            rt->x.t[i]  = ix->t[i];
            rt->x.m[i]  = ix->m[i];
        }
        rt->x.n     = ix->n;
    }

    static inline void swap_vectors(vector3d_t *a, vector3d_t *b)
    {
        vector3d_t  t;
        t.dx         = a->dx;
        t.dy         = a->dy;
        t.dz         = a->dz;
        t.dw         = a->dw;

        a->dx        = b->dx;
        a->dy        = b->dy;
        a->dz        = b->dz;
        a->dw        = b->dw;

        b->dx        = t.dx;
        b->dy        = t.dy;
        b->dz        = t.dz;
        b->dw        = t.dw;
    }

    static inline bool is_zero(float x)
    {
        return fabs(x) < DSP_3D_TOLERANCE;
    }

    static inline bool has_triangle(const intersection3d_t *is, const triangle3d_t *t)
    {
        for (size_t i=0; i<is->n; ++i)
            if (is->t[i] == t)
                return true;
        return false;
    }

    void init_matrix3d(matrix3d_t *dst, const matrix3d_t *src)
    {
        *dst        = *src;
    }

    void init_matrix3d_zero(matrix3d_t *m)
    {
        float *v    = m->m;

        v[0]        = 0.0f;
        v[1]        = 0.0f;
        v[2]        = 0.0f;
        v[3]        = 0.0f;

        v[4]        = 0.0f;
        v[5]        = 0.0f;
        v[6]        = 0.0f;
        v[7]        = 0.0f;

        v[8]        = 0.0f;
        v[9]        = 0.0f;
        v[10]       = 0.0f;
        v[11]       = 0.0f;

        v[12]       = 0.0f;
        v[13]       = 0.0f;
        v[14]       = 0.0f;
        v[15]       = 0.0f;
    }

    void init_matrix3d_one(matrix3d_t *m)
    {
        float *v    = m->m;

        v[0]        = 1.0f;
        v[1]        = 1.0f;
        v[2]        = 1.0f;
        v[3]        = 1.0f;

        v[4]        = 1.0f;
        v[5]        = 1.0f;
        v[6]        = 1.0f;
        v[7]        = 1.0f;

        v[8]        = 1.0f;
        v[9]        = 1.0f;
        v[10]       = 1.0f;
        v[11]       = 1.0f;

        v[12]       = 1.0f;
        v[13]       = 1.0f;
        v[14]       = 1.0f;
        v[15]       = 1.0f;
    }

    void init_matrix3d_identity(matrix3d_t *m)
    {
        float *v    = m->m;

        v[0]        = 1.0f;
        v[1]        = 0.0f;
        v[2]        = 0.0f;
        v[3]        = 0.0f;

        v[4]        = 0.0f;
        v[5]        = 1.0f;
        v[6]        = 0.0f;
        v[7]        = 0.0f;

        v[8]        = 0.0f;
        v[9]        = 0.0f;
        v[10]       = 1.0f;
        v[11]       = 0.0f;

        v[12]       = 0.0f;
        v[13]       = 0.0f;
        v[14]       = 0.0f;
        v[15]       = 1.0f;
    }

    void init_matrix3d_translate(matrix3d_t *m, float dx, float dy, float dz)
    {
        float *v    = m->m;

        v[0]        = 1.0f;
        v[1]        = 0.0f;
        v[2]        = 0.0f;
        v[3]        = 0.0f;

        v[4]        = 0.0f;
        v[5]        = 1.0f;
        v[6]        = 0.0f;
        v[7]        = 0.0f;

        v[8]        = 0.0f;
        v[9]        = 0.0f;
        v[10]       = 1.0f;
        v[11]       = 0.0f;

        v[12]       = dx;
        v[13]       = dy;
        v[14]       = dz;
        v[15]       = 1.0;
    }

    void init_matrix3d_scale(matrix3d_t *m, float sx, float sy, float sz)
    {
        float *v    = m->m;

        v[0]        = sx;
        v[1]        = 0.0f;
        v[2]        = 0.0f;
        v[3]        = 0.0f;

        v[4]        = 0.0f;
        v[5]        = sy;
        v[6]        = 0.0f;
        v[7]        = 0.0f;

        v[8]        = 0.0f;
        v[9]        = 0.0f;
        v[10]       = sz;
        v[11]       = 0.0f;

        v[12]       = 0.0f;
        v[13]       = 0.0f;
        v[14]       = 0.0f;
        v[15]       = 1.0f;
    }

    void init_matrix3d_rotate_x(matrix3d_t *m, float angle)
    {
        float s     = sinf(angle);
        float c     = cosf(angle);
        float *M    = m->m;

        M[0]        = 1.0f;
        M[1]        = 0.0f;
        M[2]        = 0.0f;
        M[3]        = 0.0f;
        M[4]        = 0.0f;
        M[5]        = c;
        M[6]        = s;
        M[7]        = 0.0f;
        M[8]        = 0.0f;
        M[9]        = -s;
        M[10]       = c;
        M[11]       = 0.0f;
        M[12]       = 0.0f;
        M[13]       = 0.0f;
        M[14]       = 0.0f;
        M[15]       = 1.0f;
    }

    void init_matrix3d_rotate_y(matrix3d_t *m, float angle)
    {
        float s     = sinf(angle);
        float c     = cosf(angle);
        float *M    = m->m;

        M[0]        = c;
        M[1]        = 0.0f;
        M[2]        = -s;
        M[3]        = 0.0f;
        M[4]        = 0.0f;
        M[5]        = 1.0f;
        M[6]        = 0.0f;
        M[7]        = 0.0f;
        M[8]        = s;
        M[9]        = 0.0f;
        M[10]       = c;
        M[11]       = 0.0f;
        M[12]       = 0.0f;
        M[13]       = 0.0f;
        M[14]       = 0.0f;
        M[15]       = 1.0f;
    }

    void init_matrix3d_rotate_z(matrix3d_t *m, float angle)
    {
        float s     = sinf(angle);
        float c     = cosf(angle);
        float *M    = m->m;

        M[0]        = c;
        M[1]        = s;
        M[2]        = 0.0f;
        M[3]        = 0.0f;
        M[4]        = -s;
        M[5]        = c;
        M[6]        = 0.0f;
        M[7]        = 0.0f;
        M[8]        = 0.0f;
        M[9]        = 0.0f;
        M[10]       = 1.0f;
        M[11]       = 0.0f;
        M[12]       = 0.0f;
        M[13]       = 0.0f;
        M[14]       = 0.0f;
        M[15]       = 1.0f;
    }

    void init_matrix3d_rotate_xyz(matrix3d_t *m, float x, float y, float z, float angle)
    {
        if (x == 0.0f)
        {
            if (y == 0.0f)
            {
                if (z > 0.0f)
                    init_matrix3d_rotate_z(m, angle);
                else if (z < 0.0f)
                    init_matrix3d_rotate_z(m, -angle);
                else // (z == 0.0)
                    init_matrix3d_identity(m);
            }
            else if (z == 0.0f)
            {
                if (y > 0.0f)
                    init_matrix3d_rotate_y(m, angle);
                else if (y < 0.0f)
                    init_matrix3d_rotate_y(m, -angle);
            }
            return;
        }
        else if ((y == 0.0) && (z == 0.0))
        {
            if (x > 0.0f)
                init_matrix3d_rotate_x(m, angle);
            else
                init_matrix3d_rotate_x(m, -angle);
        }

        float s = sinf(angle);
        float c = cosf(angle);

        float xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c;
        float mag = sqrtf(x*x + y*y + z*z);

    //            if (mag <= 1.0e-4)
    //                return *this;
        x          /= mag;
        y          /= mag;
        z          /= mag;

        xx          = x * x;
        yy          = y * y;
        zz          = z * z;
        xy          = x * y;
        yz          = y * z;
        zx          = z * x;
        xs          = x * s;
        ys          = y * s;
        zs          = z * s;
        one_c       = 1.0f - c;

        float *M    = m->m;
        M[0]        = (one_c * xx) + c;
        M[1]        = (one_c * xy) + zs;
        M[2]        = (one_c * zx) - ys;
        M[3]        = 0.0f;
        M[4]        = (one_c * xy) - zs;
        M[5]        = (one_c * yy) + c;
        M[6]        = (one_c * yz) + xs;
        M[7]        = 0.0f;
        M[8]        = (one_c * zx) + ys;
        M[9]        = (one_c * yz) - xs;
        M[10]       = (one_c * zz) + c;
        M[11]       = 0.0f;
        M[12]       = 0.0f;
        M[13]       = 0.0f;
        M[14]       = 0.0f;
        M[15]       = 0.0f;
    }

    void apply_matrix3d_mv2(vector3d_t *r, const vector3d_t *v, const matrix3d_t *m)
    {
        const float *M = m->m;
        r->dx       = M[0] * v->dx + M[4] * v->dy + M[8]  * v->dz; // + M[12] * v->dw;
        r->dy       = M[1] * v->dx + M[5] * v->dy + M[9]  * v->dz; // + M[13] * v->dw;
        r->dz       = M[2] * v->dx + M[6] * v->dy + M[10] * v->dz; // + M[14] * v->dw;
        r->dw       = M[3] * v->dx + M[7] * v->dy + M[11] * v->dz; // + M[15] * v->dw;

        // Homogenize vector
        if (r->dw == 0.0f)
            return;
        r->dx      /= r->dw;
        r->dy      /= r->dw;
        r->dz      /= r->dw;
        r->dw      /= r->dw;
    }

    void apply_matrix3d_mv1(vector3d_t *r, const matrix3d_t *m)
    {
        vector3d_t  tmp;
        apply_matrix3d_mv2(&tmp, r, m);
        *r          = tmp;
    }

    void apply_matrix3d_mp2(point3d_t *r, const point3d_t *p, const matrix3d_t *m)
    {
        const float *M = m->m;
        r->x        = M[0] * p->x + M[4] * p->y + M[8]  * p->z + M[12];
        r->y        = M[1] * p->x + M[5] * p->y + M[9]  * p->z + M[13];
        r->z        = M[2] * p->x + M[6] * p->y + M[10] * p->z + M[14];
        r->w        = M[3] * p->x + M[7] * p->y + M[11] * p->z + M[15];

        // Homogenize vector
        if (r->w == 0.0f)
            return;
        r->x       /= r->w;
        r->y       /= r->w;
        r->z       /= r->w;
        r->w       /= r->w;
    }

    void apply_matrix3d_mp1(point3d_t *r, const matrix3d_t *m)
    {
        point3d_t tmp;
        apply_matrix3d_mp2(&tmp, r, m);
        *r          = tmp;
    }

    void apply_matrix3d_mm2(matrix3d_t *r, const matrix3d_t *s, const matrix3d_t *m)
    {
        const float *A      = s->m;
        const float *B      = m->m;
        float *R            = r->m;

    //            for (int i = 0; i < 4; i++)
    //            {
    //                ai[0] = A[i + 0];
    //                ai[1] = A[i + 4];
    //                ai[2] = A[i + 8];
    //                ai[3] = A[i + 12];
    //
    //                m[i + 0]    = ai[0] * B[0] + ai[1] * B[1] + ai[2] * B[2] + ai[3] * B[3];
    //                m[i + 4]    = ai[0] * B[4] + ai[1] * B[5] + ai[2] * B[6] + ai[3] * B[7];
    //                m[i + 8]    = ai[0] * B[8] + ai[1] * B[9] + ai[2] * B[10] + ai[3] * B[11];
    //                m[i + 12]   = ai[0] * B[12] + ai[1] * B[13] + ai[2] * B[14] + ai[3] * B[15];
    //            }


        R[0]    = A[0] * B[0]   + A[4] * B[1]   + A[8] * B[2]   + A[12] * B[3];
        R[1]    = A[1] * B[0]   + A[5] * B[1]   + A[9] * B[2]   + A[13] * B[3];
        R[2]    = A[2] * B[0]   + A[6] * B[1]   + A[10] * B[2]  + A[14] * B[3];
        R[3]    = A[3] * B[0]   + A[7] * B[1]   + A[11] * B[2]  + A[15] * B[3];

        R[4]    = A[0] * B[4]   + A[4] * B[5]   + A[8] * B[6]   + A[12] * B[7];
        R[5]    = A[1] * B[4]   + A[5] * B[5]   + A[9] * B[6]   + A[13] * B[7];
        R[6]    = A[2] * B[4]   + A[6] * B[5]   + A[10] * B[6]  + A[14] * B[7];
        R[7]    = A[3] * B[4]   + A[7] * B[5]   + A[11] * B[6]  + A[15] * B[7];

        R[8]    = A[0] * B[8]   + A[4] * B[9]   + A[8] * B[10]  + A[12] * B[11];
        R[9]    = A[1] * B[8]   + A[5] * B[9]   + A[9] * B[10]  + A[13] * B[11];
        R[10]   = A[2] * B[8]   + A[6] * B[9]   + A[10] * B[10] + A[14] * B[11];
        R[11]   = A[3] * B[8]   + A[7] * B[9]   + A[11] * B[10] + A[15] * B[11];

        R[12]   = A[0] * B[12]  + A[4] * B[13]  + A[8] * B[14]  + A[12] * B[15];
        R[13]   = A[1] * B[12]  + A[5] * B[13]  + A[9] * B[14]  + A[13] * B[15];
        R[14]   = A[2] * B[12]  + A[6] * B[13]  + A[10] * B[14] + A[14] * B[15];
        R[15]   = A[3] * B[12]  + A[7] * B[13]  + A[11] * B[14] + A[15] * B[15];
    }

    void apply_matrix3d_mm1(matrix3d_t *r, const matrix3d_t *m)
    {
        matrix3d_t  tmp;
        apply_matrix3d_mm2(&tmp, r, m);
        *r          = tmp;
    }

    void transpose_matrix3d1(matrix3d_t *r)
    {
        float T;
        float *R        = r->m;

        #define MX_SWAP(i0, i1) T = R[i0]; R[i0] = R[i1]; R[i1] = T;
        MX_SWAP(1, 4)
        MX_SWAP(2, 8)
        MX_SWAP(3, 12)
        MX_SWAP(6, 9)
        MX_SWAP(7, 13)
        MX_SWAP(11, 14)
        #undef MX_SWAP
    }

    void transpose_matrix3d2(matrix3d_t *r, const matrix3d_t *m)
    {
        float *R        = r->m;
        const float *M  = m->m;

        R[0]            = M[0];
        R[1]            = M[4];
        R[2]            = M[8];
        R[3]            = M[12];
        R[4]            = M[1];
        R[5]            = M[5];
        R[6]            = M[9];
        R[7]            = M[13];
        R[8]            = M[2];
        R[9]            = M[6];
        R[10]           = M[10];
        R[11]           = M[14];
        R[12]           = M[3];
        R[13]           = M[7];
        R[14]           = M[11];
        R[15]           = M[15];
    }

    float check_triplet3d_p3n(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3, const vector3d_t *n)
    {
        // Calculate two vectors
        vector3d_t  v[3];
        v[0].dx             = p2->x - p1->x;
        v[0].dy             = p2->y - p1->y;
        v[0].dz             = p2->z - p1->z;

        v[1].dx             = p3->x - p2->x;
        v[1].dy             = p3->y - p2->y;
        v[1].dz             = p3->z - p2->z;

        // Calculate vector multiplication
        v[2].dx             = v[0].dy * v[1].dz - v[0].dz * v[1].dy;
        v[2].dy             = v[0].dz * v[1].dx - v[0].dx * v[1].dz;
        v[2].dz             = v[0].dx * v[1].dy - v[0].dy * v[1].dx;

        // Return result of scalar multiplication to the normal
        return n->dx * v[2].dx + n->dy * v[2].dy + n->dz * v[2].dz;
    }

    float check_triplet3d_pvn(const point3d_t *pv, const vector3d_t *n)
    {
        // Calculate two vectors
        vector3d_t  v[3];
        v[0].dx             = pv[1].x - pv[0].x;
        v[0].dy             = pv[1].y - pv[0].y;
        v[0].dz             = pv[1].z - pv[0].z;

        v[1].dx             = pv[2].x - pv[1].x;
        v[1].dy             = pv[2].y - pv[1].y;
        v[1].dz             = pv[2].z - pv[1].z;

        // Calculate vector multiplication
        v[2].dx             = v[0].dy * v[1].dz - v[0].dz * v[1].dy;
        v[2].dy             = v[0].dz * v[1].dx - v[0].dx * v[1].dz;
        v[2].dz             = v[0].dx * v[1].dy - v[0].dy * v[1].dx;

        // Return result of scalar multiplication to the normal
        return n->dx * v[2].dx + n->dy * v[2].dy + n->dz * v[2].dz;
    }

    float check_triplet3d_v2n(const vector3d_t *v1, const vector3d_t *v2, const vector3d_t *n)
    {
        // Calculate two vectors
        vector3d_t  v;

        // Calculate vector multiplication
        v.dx                = v1->dy * v2->dz - v1->dz * v2->dy;
        v.dy                = v1->dz * v2->dx - v1->dx * v2->dz;
        v.dz                = v1->dx * v2->dy - v1->dy * v2->dx;

        // Return result of scalar multiplication to the normal
        return n->dx * v.dx + n->dy * v.dy + n->dz * v.dz;
    }

    float check_triplet3d_vvn(const vector3d_t *v, const vector3d_t *n)
    {
        // Calculate two vectors
        vector3d_t  vt;

        // Calculate vector multiplication
        vt.dx               = v[0].dy * v[1].dz - v[0].dz * v[1].dy;
        vt.dy               = v[0].dz * v[1].dx - v[0].dx * v[1].dz;
        vt.dz               = v[0].dx * v[1].dy - v[0].dy * v[1].dx;

        // Return result of scalar multiplication to the normal
        return n->dx * vt.dx + n->dy * vt.dy + n->dz * vt.dz;
    }

    float check_triplet3d_vv(const vector3d_t *v)
    {
        // Calculate two vectors
        vector3d_t  vt;

        // Calculate vector multiplication
        vt.dx               = v[0].dy * v[1].dz - v[0].dz * v[1].dy;
        vt.dy               = v[0].dz * v[1].dx - v[0].dx * v[1].dz;
        vt.dz               = v[0].dx * v[1].dy - v[0].dy * v[1].dx;

        // Return result of scalar multiplication to the normal
        return v[2].dx * vt.dx + v[2].dy * vt.dy + v[2].dz * vt.dz;
    }

    float check_triplet3d_t(const triangle3d_t *t)
    {
        // Calculate two vectors
        vector3d_t  v[3];
        v[0].dx             = t->p[1].x - t->p[0].x;
        v[0].dy             = t->p[1].y - t->p[0].y;
        v[0].dz             = t->p[1].z - t->p[0].z;

        v[1].dx             = t->p[2].x - t->p[1].x;
        v[1].dy             = t->p[2].y - t->p[1].y;
        v[1].dz             = t->p[2].z - t->p[1].z;

        // Calculate vector multiplication
        v[2].dx             = v[0].dy * v[1].dz - v[0].dz * v[1].dy;
        v[2].dy             = v[0].dz * v[1].dx - v[0].dx * v[1].dz;
        v[2].dz             = v[0].dx * v[1].dy - v[0].dy * v[1].dx;

        // Return result of scalar multiplication to the normal
        return t->n.dx * v[2].dx + t->n.dy * v[2].dy + t->n.dz * v[2].dz;
    }

    float check_triplet3d_tn(const triangle3d_t *t, const vector3d_t *n)
    {
        // Calculate two vectors
        vector3d_t  v[3];
        v[0].dx             = t->p[1].x - t->p[0].x;
        v[0].dy             = t->p[1].y - t->p[0].y;
        v[0].dz             = t->p[1].z - t->p[0].z;

        v[1].dx             = t->p[2].x - t->p[1].x;
        v[1].dy             = t->p[2].y - t->p[1].y;
        v[1].dz             = t->p[2].z - t->p[1].z;

        // Calculate vector multiplication
        v[2].dx             = v[0].dy * v[1].dz - v[0].dz * v[1].dy;
        v[2].dy             = v[0].dz * v[1].dx - v[0].dx * v[1].dz;
        v[2].dz             = v[0].dx * v[1].dy - v[0].dy * v[1].dx;

        // Return result of scalar multiplication to the normal
        return n->dx * v[2].dx + n->dy * v[2].dy + n->dz * v[2].dz;
    }

    //        float check_point3d_location_tp(const triangle3d_t *t, const point3d_t *p)
    //        {
    //            vector3d_t  v[3];
    //            v[0].dx             = p->x - t->p[0].x;
    //            v[0].dy             = p->y - t->p[0].y;
    //            v[0].dz             = p->z - t->p[0].z;
    //
    //            v[1].dx             = p->x - t->p[1].x;
    //            v[1].dy             = p->y - t->p[1].y;
    //            v[1].dz             = p->z - t->p[1].z;
    //
    //            v[2].dx             = p->x - t->p[2].x;
    //            v[2].dy             = p->y - t->p[2].y;
    //            v[2].dz             = p->z - t->p[2].z;
    //
    //            vector3d_t  m[3];
    //            m[0].dx             = v[0].dy * v[1].dz - v[0].dz * v[1].dy;
    //            m[0].dy             = v[0].dz * v[1].dx - v[0].dx * v[1].dz;
    //            m[0].dz             = v[0].dx * v[1].dy - v[0].dy * v[1].dx;
    //
    //            m[1].dx             = v[1].dy * v[2].dz - v[1].dz * v[2].dy;
    //            m[1].dy             = v[1].dz * v[2].dx - v[1].dx * v[2].dz;
    //            m[1].dz             = v[1].dx * v[2].dy - v[1].dy * v[2].dx;
    //
    //            m[2].dx             = v[2].dy * v[0].dz - v[2].dz * v[0].dy;
    //            m[2].dy             = v[2].dz * v[0].dx - v[2].dx * v[0].dz;
    //            m[2].dz             = v[2].dx * v[0].dy - v[2].dy * v[0].dx;
    //
    //            float r[2];
    //            r[0]                = m[0].dx * m[1].dx + m[0].dy * m[1].dy + m[0].dz * m[1].dz;
    //            r[1]                = m[1].dx * m[2].dx + m[1].dy * m[2].dy + m[1].dz * m[2].dz;
    //
    //            return (r[0] < 0.0f) ? r[0] : r[1];
    //        }
    //
    //        float check_point3d_location_pvp(const point3d_t *t, const point3d_t *p)
    //        {
    //            vector3d_t  v[3];
    //            v[0].dx             = p->x - t[0].x;
    //            v[0].dy             = p->y - t[0].y;
    //            v[0].dz             = p->z - t[0].z;
    //
    //            v[1].dx             = p->x - t[1].x;
    //            v[1].dy             = p->y - t[1].y;
    //            v[1].dz             = p->z - t[1].z;
    //
    //            v[2].dx             = p->x - t[2].x;
    //            v[2].dy             = p->y - t[2].y;
    //            v[2].dz             = p->z - t[2].z;
    //
    //            vector3d_t  m[3];
    //            m[0].dx             = v[0].dy * v[1].dz - v[0].dz * v[1].dy;
    //            m[0].dy             = v[0].dz * v[1].dx - v[0].dx * v[1].dz;
    //            m[0].dz             = v[0].dx * v[1].dy - v[0].dy * v[1].dx;
    //
    //            m[1].dx             = v[1].dy * v[2].dz - v[1].dz * v[2].dy;
    //            m[1].dy             = v[1].dz * v[2].dx - v[1].dx * v[2].dz;
    //            m[1].dz             = v[1].dx * v[2].dy - v[1].dy * v[2].dx;
    //
    //            m[2].dx             = v[2].dy * v[0].dz - v[2].dz * v[0].dy;
    //            m[2].dy             = v[2].dz * v[0].dx - v[2].dx * v[0].dz;
    //            m[2].dz             = v[2].dx * v[0].dy - v[2].dy * v[0].dx;
    //
    //            float r[2];
    //            r[0]                = m[0].dx * m[1].dx + m[0].dy * m[1].dy + m[0].dz * m[1].dz;
    //            r[1]                = m[1].dx * m[2].dx + m[1].dy * m[2].dy + m[1].dz * m[2].dz;
    //
    //            return (r[0] < 0.0f) ? r[0] : r[1];
    //        }
    //
    //        float check_point3d_location_p3p(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3, const point3d_t *p)
    //        {
    //            vector3d_t  v[3];
    //            v[0].dx             = p->x - p1->x;
    //            v[0].dy             = p->y - p1->y;
    //            v[0].dz             = p->z - p1->z;
    //
    //            v[1].dx             = p->x - p2->x;
    //            v[1].dy             = p->y - p2->y;
    //            v[1].dz             = p->z - p2->z;
    //
    //            v[2].dx             = p->x - p3->x;
    //            v[2].dy             = p->y - p3->y;
    //            v[2].dz             = p->z - p3->z;
    //
    //            vector3d_t  m[3];
    //            m[0].dx             = v[0].dy * v[1].dz - v[0].dz * v[1].dy;
    //            m[0].dy             = v[0].dz * v[1].dx - v[0].dx * v[1].dz;
    //            m[0].dz             = v[0].dx * v[1].dy - v[0].dy * v[1].dx;
    //
    //            m[1].dx             = v[1].dy * v[2].dz - v[1].dz * v[2].dy;
    //            m[1].dy             = v[1].dz * v[2].dx - v[1].dx * v[2].dz;
    //            m[1].dz             = v[1].dx * v[2].dy - v[1].dy * v[2].dx;
    //
    //            m[2].dx             = v[2].dy * v[0].dz - v[2].dz * v[0].dy;
    //            m[2].dy             = v[2].dz * v[0].dx - v[2].dx * v[0].dz;
    //            m[2].dz             = v[2].dx * v[0].dy - v[2].dy * v[0].dx;
    //
    //            float r[2];
    //            r[0]                = m[0].dx * m[1].dx + m[0].dy * m[1].dy + m[0].dz * m[1].dz;
    //            r[1]                = m[1].dx * m[2].dx + m[1].dy * m[2].dy + m[1].dz * m[2].dz;
    //
    //            return (r[0] < 0.0f) ? r[0] : r[1];
    //        }

    float check_point3d_on_triangle_p3p(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3, const point3d_t *p)
    {
        // Vector parameters
        vector3d_t  v[3];
        v[0].dx             = p1->x - p->x;
        v[0].dy             = p1->y - p->y;
        v[0].dz             = p1->z - p->z;

        v[1].dx             = p2->x - p->x;
        v[1].dy             = p2->y - p->y;
        v[1].dz             = p2->z - p->z;

        v[2].dx             = p3->x - p->x;
        v[2].dy             = p3->y - p->y;
        v[2].dz             = p3->z - p->z;

        // Do logic
        vector3d_t  m[3];
        float r[3];

        // Check 1: 2 vector multiplications and 1 scalar
        m[0].dx             = v[0].dy * v[1].dz - v[0].dz * v[1].dy;
        m[0].dy             = v[0].dz * v[1].dx - v[0].dx * v[1].dz;
        m[0].dz             = v[0].dx * v[1].dy - v[0].dy * v[1].dx;

        m[1].dx             = v[1].dy * v[2].dz - v[1].dz * v[2].dy;
        m[1].dy             = v[1].dz * v[2].dx - v[1].dx * v[2].dz;
        m[1].dz             = v[1].dx * v[2].dy - v[1].dy * v[2].dx;

        r[0]                = m[0].dx * m[1].dx + m[0].dy * m[1].dy + m[0].dz * m[1].dz;
        if (r[0] < 0.0f)
            return r[0];

        // Check 2: 1 vector multiplication and 1 scalar
        m[2].dx             = v[2].dy * v[0].dz - v[2].dz * v[0].dy;
        m[2].dy             = v[2].dz * v[0].dx - v[2].dx * v[0].dz;
        m[2].dz             = v[2].dx * v[0].dy - v[2].dy * v[0].dx;

        r[1]                = m[1].dx * m[2].dx + m[1].dy * m[2].dy + m[1].dz * m[2].dz;
        if (r[1] < 0.0f)
            return r[1];

        // Check 3: 1 scalar multiplication
        r[2]                = m[2].dx * m[0].dx + m[2].dy * m[0].dy + m[2].dz * m[0].dz;
        if (r[2] < 0.0f)
            return r[2];

        // Check 4
        r[2]                = r[0]*r[1]*r[2];
        if (r[2] != 0.0f)
            return r[2];

        // Edge check: 3 scalar multiplications
        r[0]                = v[0].dx * v[1].dx + v[0].dy * v[1].dy + v[0].dz * v[1].dz;
        r[1]                = v[1].dx * v[2].dx + v[1].dy * v[2].dy + v[1].dz * v[2].dz;
        r[2]                = v[2].dx * v[0].dx + v[2].dy * v[0].dy + v[2].dz * v[0].dz;

        return r[0]*r[1]*r[2];
    }

    float check_point3d_on_triangle_pvp(const point3d_t *pv, const point3d_t *p)
    {
        // Vector parameters
        vector3d_t  v[3];
        v[0].dx             = pv[0].x - p->x;
        v[0].dy             = pv[0].y - p->y;
        v[0].dz             = pv[0].z - p->z;

        v[1].dx             = pv[1].x - p->x;
        v[1].dy             = pv[1].y - p->y;
        v[1].dz             = pv[1].z - p->z;

        v[2].dx             = pv[2].x - p->x;
        v[2].dy             = pv[2].y - p->y;
        v[2].dz             = pv[2].z - p->z;

        // Do logic
        vector3d_t  m[3];
        float r[3];

        // Check 1: 2 vector multiplications and 1 scalar
        m[0].dx             = v[0].dy * v[1].dz - v[0].dz * v[1].dy;
        m[0].dy             = v[0].dz * v[1].dx - v[0].dx * v[1].dz;
        m[0].dz             = v[0].dx * v[1].dy - v[0].dy * v[1].dx;

        m[1].dx             = v[1].dy * v[2].dz - v[1].dz * v[2].dy;
        m[1].dy             = v[1].dz * v[2].dx - v[1].dx * v[2].dz;
        m[1].dz             = v[1].dx * v[2].dy - v[1].dy * v[2].dx;

        r[0]                = m[0].dx * m[1].dx + m[0].dy * m[1].dy + m[0].dz * m[1].dz;
        if (r[0] < 0.0f)
            return r[0];

        // Check 2: 1 vector multiplication and 1 scalar
        m[2].dx             = v[2].dy * v[0].dz - v[2].dz * v[0].dy;
        m[2].dy             = v[2].dz * v[0].dx - v[2].dx * v[0].dz;
        m[2].dz             = v[2].dx * v[0].dy - v[2].dy * v[0].dx;

        r[1]                = m[1].dx * m[2].dx + m[1].dy * m[2].dy + m[1].dz * m[2].dz;
        if (r[1] < 0.0f)
            return r[1];

        // Check 3: 1 scalar multiplication
        r[2]                = m[2].dx * m[0].dx + m[2].dy * m[0].dy + m[2].dz * m[0].dz;
        if (r[2] < 0.0f)
            return r[2];

        // Check 4
        r[2]                = r[0]*r[1]*r[2];
        if (r[2] != 0.0f)
            return r[2];

        // Edge check: 3 scalar multiplications
        r[0]                = v[0].dx * v[1].dx + v[0].dy * v[1].dy + v[0].dz * v[1].dz;
        r[1]                = v[1].dx * v[2].dx + v[1].dy * v[2].dy + v[1].dz * v[2].dz;
        r[2]                = v[2].dx * v[0].dx + v[2].dy * v[0].dy + v[2].dz * v[0].dz;

        return r[0]*r[1]*r[2];
    }

    inline float check_point3d_on_triangle_tp(const triangle3d_t *t, const point3d_t *p)
    {
        // Vector parameters
        vector3d_t  v[3];
        v[0].dx             = t->p[0].x - p->x;
        v[0].dy             = t->p[0].y - p->y;
        v[0].dz             = t->p[0].z - p->z;

        v[1].dx             = t->p[1].x - p->x;
        v[1].dy             = t->p[1].y - p->y;
        v[1].dz             = t->p[1].z - p->z;

        v[2].dx             = t->p[2].x - p->x;
        v[2].dy             = t->p[2].y - p->y;
        v[2].dz             = t->p[2].z - p->z;

        // Do logic
        vector3d_t  m[3];
        float r[3];

        // Check 1: 2 vector multiplications and 1 scalar
        m[0].dx             = v[0].dy * v[1].dz - v[0].dz * v[1].dy;
        m[0].dy             = v[0].dz * v[1].dx - v[0].dx * v[1].dz;
        m[0].dz             = v[0].dx * v[1].dy - v[0].dy * v[1].dx;

        m[1].dx             = v[1].dy * v[2].dz - v[1].dz * v[2].dy;
        m[1].dy             = v[1].dz * v[2].dx - v[1].dx * v[2].dz;
        m[1].dz             = v[1].dx * v[2].dy - v[1].dy * v[2].dx;

        r[0]                = m[0].dx * m[1].dx + m[0].dy * m[1].dy + m[0].dz * m[1].dz;
        if (r[0] < 0.0f)
            return r[0];

        // Check 2: 1 vector multiplication and 1 scalar
        m[2].dx             = v[2].dy * v[0].dz - v[2].dz * v[0].dy;
        m[2].dy             = v[2].dz * v[0].dx - v[2].dx * v[0].dz;
        m[2].dz             = v[2].dx * v[0].dy - v[2].dy * v[0].dx;

        r[1]                = m[1].dx * m[2].dx + m[1].dy * m[2].dy + m[1].dz * m[2].dz;
        if (r[1] < 0.0f)
            return r[1];

        // Check 3: 1 scalar multiplication
        r[2]                = m[2].dx * m[0].dx + m[2].dy * m[0].dy + m[2].dz * m[0].dz;
        if (r[2] < 0.0f)
            return r[2];

        // Check 4
        r[2]                = r[0]*r[1]*r[2];
        if (r[2] != 0.0f)
            return r[2];

        // Edge check: 3 scalar multiplications
        r[0]                = v[0].dx * v[1].dx + v[0].dy * v[1].dy + v[0].dz * v[1].dz;
        r[1]                = v[1].dx * v[2].dx + v[1].dy * v[2].dy + v[1].dz * v[2].dz;
        r[2]                = v[2].dx * v[0].dx + v[2].dy * v[0].dy + v[2].dz * v[0].dz;

        return r[0]*r[1]*r[2];
    }

    float check_point3d_on_edge_p2p(const point3d_t *p1, const point3d_t *p2, const point3d_t *p)
    {
        vector3d_t  v[3];
        v[0].dx             = p->x - p1->x;
        v[0].dy             = p->y - p1->y;
        v[0].dz             = p->z - p1->z;

        v[1].dx             = p->x - p2->x;
        v[1].dy             = p->y - p2->y;
        v[1].dz             = p->z - p2->z;

        v[2].dx             = p2->x - p1->x;
        v[2].dy             = p2->y - p1->y;
        v[2].dz             = p2->z - p1->z;

        float  c[3];
        c[0]                = sqrtf(v[0].dx * v[0].dx + v[0].dy * v[0].dy + v[0].dz * v[0].dz);
        c[1]                = sqrtf(v[1].dx * v[1].dx + v[1].dy * v[1].dy + v[1].dz * v[1].dz);
        c[2]                = sqrtf(v[2].dx * v[2].dx + v[2].dy * v[2].dy + v[2].dz * v[2].dz);

        return c[2] - c[1] - c[0];
    }

    float check_point3d_on_edge_pvp(const point3d_t *pv, const point3d_t *p)
    {
        vector3d_t  v[3];
        v[0].dx             = p->x - pv[0].x;
        v[0].dy             = p->y - pv[0].y;
        v[0].dz             = p->z - pv[0].z;

        v[1].dx             = p->x - pv[1].x;
        v[1].dy             = p->y - pv[1].y;
        v[1].dz             = p->z - pv[1].z;

        v[2].dx             = pv[1].x - pv[0].x;
        v[2].dy             = pv[1].y - pv[0].y;
        v[2].dz             = pv[1].z - pv[0].z;

        float  c[3];
        c[0]                = sqrtf(v[0].dx * v[0].dx + v[0].dy * v[0].dy + v[0].dz * v[0].dz);
        c[1]                = sqrtf(v[1].dx * v[1].dx + v[1].dy * v[1].dy + v[1].dz * v[1].dz);
        c[2]                = sqrtf(v[2].dx * v[2].dx + v[2].dy * v[2].dy + v[2].dz * v[2].dz);

        return c[2] - c[1] - c[0];
    }

    size_t longest_edge3d_p3(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3)
    {
        vector3d_t  v[3];
        v[0].dx             = p2->x - p1->x;
        v[0].dy             = p2->y - p1->y;
        v[0].dz             = p2->z - p1->z;

        v[1].dx             = p3->x - p2->x;
        v[1].dy             = p3->y - p2->y;
        v[1].dz             = p3->z - p2->z;

        v[2].dx             = p1->x - p3->x;
        v[2].dy             = p1->y - p3->y;
        v[2].dz             = p1->z - p3->z;

        float       r[4];
        r[0]                = v[0].dx * v[0].dx + v[0].dy * v[0].dy + v[0].dz * v[0].dz;
        r[1]                = v[1].dx * v[1].dx + v[1].dy * v[1].dy + v[1].dz * v[1].dz;
        r[2]                = v[2].dx * v[2].dx + v[2].dy * v[2].dy + v[2].dz * v[2].dz;

        if (r[0] > r[1])
            return (r[0] > r[2]) ? 0 : 2;
        return (r[1] > r[2]) ? 1 : 2;
    }

    size_t longest_edge3d_pv(const point3d_t *p)
    {
        vector3d_t  v[3];
        v[0].dx             = p[1].x - p[0].x;
        v[0].dy             = p[1].y - p[0].y;
        v[0].dz             = p[1].z - p[0].z;

        v[1].dx             = p[2].x - p[1].x;
        v[1].dy             = p[2].y - p[1].y;
        v[1].dz             = p[2].z - p[1].z;

        v[2].dx             = p[0].x - p[2].x;
        v[2].dy             = p[0].y - p[2].y;
        v[2].dz             = p[0].z - p[2].z;

        float       r[4];
        r[0]                = v[0].dx * v[0].dx + v[0].dy * v[0].dy + v[0].dz * v[0].dz;
        r[1]                = v[1].dx * v[1].dx + v[1].dy * v[1].dy + v[1].dz * v[1].dz;
        r[2]                = v[2].dx * v[2].dx + v[2].dy * v[2].dy + v[2].dz * v[2].dz;

        if (r[0] > r[1])
            return (r[0] > r[2]) ? 0 : 2;
        return (r[1] > r[2]) ? 1 : 2;
    }

    float find_intersection3d_rt(point3d_t *ip, const ray3d_t *l, const triangle3d_t *t)
    {
        point3d_t   ix; // intersection point

        //---------------------------------------------------
        // PART 0: perform simple culling
        if (l->v.dx >= 0.0f)
        {
            if ((t->p[0].x < l->z.x) && (t->p[1].x < l->z.x) && (t->p[2].x < l->z.x))
                return -1.0f;
        }
        else
        {
            if ((t->p[0].x >= l->z.x) && (t->p[1].x >= l->z.x) && (t->p[2].x >= l->z.x))
                return -1.0f;
        }

        if (l->v.dy >= 0.0f)
        {
            if ((t->p[0].y < l->z.y) && (t->p[1].y < l->z.y) && (t->p[2].y < l->z.y))
                return -1.0f;
        }
        else
        {
            if ((t->p[0].y >= l->z.y) && (t->p[1].y >= l->z.y) && (t->p[2].y >= l->z.y))
                return -1.0f;
        }

        if (l->v.dz >= 0.0f)
        {
            if ((t->p[0].z < l->z.z) && (t->p[1].z < l->z.z) && (t->p[2].z < l->z.z))
                return -1.0f;
        }
        else
        {
            if ((t->p[0].z >= l->z.z) && (t->p[1].z >= l->z.z) && (t->p[2].z >= l->z.z))
                return -1.0f;
        }

        //---------------------------------------------------
        // PART 1: check intersection with plane
        // Form equations
        // Form equations
        /*

         Nx*x0 + Ny*y0 + Nz*z0    + W               = 0
       - dz*x0 +         dx*z0    + (dz*xs - dx*zs) = 0
         dy*x0 - dx*y0            + (dx*ys - dy*xs) = 0
                 dz*y0 - dy*z0    + (dy*zs - dz*ys) = 0

         */
        vector3d_t  m[4];

        m[0].dx      = t->n.dx;
        m[0].dy      = t->n.dy;
        m[0].dz      = t->n.dz;
        m[0].dw      = t->n.dw;

        m[1].dx      = -l->v.dz;
        m[1].dy      = 0.0f;
        m[1].dz      = l->v.dx;
        m[1].dw      = l->v.dz * l->z.x - l->v.dx * l->z.z;

        m[2].dx      = l->v.dy;
        m[2].dy      = -l->v.dx;
        m[2].dz      = 0.0f;
        m[2].dw      = l->v.dx * l->z.y - l->v.dy * l->z.x;

        m[3].dx      = 0.0f;
        m[3].dy      = l->v.dz;
        m[3].dz      = -l->v.dy;
        m[3].dw      = l->v.dy * l->z.z - l->v.dz * l->z.y;

        if (is_zero(m[0].dx))
        {
            if (!is_zero(m[1].dx))
                swap_vectors(&m[0], &m[1]);
            else if (!is_zero(m[2].dx))
                swap_vectors(&m[0], &m[2]);
            else
                return -1.0f;
        }

        if (!is_zero(m[1].dx))
        {
            float k      = m[1].dx / m[0].dx;
            m[1].dx      = m[1].dx - m[0].dx * k;
            m[1].dy      = m[1].dy - m[0].dy * k;
            m[1].dz      = m[1].dz - m[0].dz * k;
            m[1].dw      = m[1].dw - m[0].dw * k;
        }
        if (!is_zero(m[2].dx))
        {
            float k      = m[2].dx / m[0].dx;
            m[2].dx      = m[2].dx - m[0].dx * k;
            m[2].dy      = m[2].dy - m[0].dy * k;
            m[2].dz      = m[2].dz - m[0].dz * k;
            m[2].dw      = m[2].dw - m[0].dw * k;
        }

        // Solve equations, step 1
        if (is_zero(m[1].dy))
        {
            if (!is_zero(m[2].dy))
                swap_vectors(&m[1], &m[2]);
            else if (!is_zero(m[3].dy))
                swap_vectors(&m[1], &m[3]);
            else
                return -1.0f;
        }

        if (!is_zero(m[2].dy))
        {
            float k      = m[2].dy / m[1].dy;
            m[2].dx      = m[2].dx - m[1].dx * k;
            m[2].dy      = m[2].dy - m[1].dy * k;
            m[2].dz      = m[2].dz - m[1].dz * k;
            m[2].dw      = m[2].dw - m[1].dw * k;
        }
        if (!is_zero(m[3].dy))
        {
            float k      = m[3].dy / m[1].dy;
            m[3].dx      = m[3].dx - m[1].dx * k;
            m[3].dy      = m[3].dy - m[1].dy * k;
            m[3].dz      = m[3].dz - m[1].dz * k;
            m[3].dw      = m[3].dw - m[1].dw * k;
        }

        // Check that matrix form is right
        if (is_zero(m[2].dz))
        {
            if (!is_zero(m[3].dz))
                swap_vectors(&m[2], &m[3]);
            else
                return -1.0f;
        }

        // Now solve matrix into intersection point
        ix.z        = - m[2].dw / m[2].dz;
        ix.y        = - (m[1].dw + m[1].dz * ix.z) / m[1].dy;
        ix.x        = - (m[0].dw + m[0].dy * ix.y + m[0].dz * ix.z) / m[0].dx;
        ix.w        = 0.0f;

        //---------------------------------------------------
        // PART 2: check that point lies on the ray
        vector3d_t  pv; // Projection vector
        pv.dx       = ix.x - l->z.x;
        pv.dy       = ix.y - l->z.y;
        pv.dz       = ix.z - l->z.z;

        float proj  = pv.dx*l->v.dx + pv.dy*l->v.dy + pv.dz*l->v.dz; // Projection on the ray, can be also interpreted as a distance
        if (proj < 0.0f)
            return -1.0f;

        //---------------------------------------------------
        // PART 3: check that point lies within a triangle
        if (check_point3d_on_triangle_tp(t, &ix) < 0.0f)
            return -1.0f;

        // Return point
        *ip         = ix;
        return proj;
    }

    void reflect_ray(raytrace3d_t *rt, raytrace3d_t *rf, const intersection3d_t *ix)
    {
        material3d_t    m;  // Compiled material
        vector3d_t vn, vt, dv; // Tangent and normal vectors

        // Calculate normal of intersecting triangles
        m.speed         = 0.0f;
        m.damping       = 0.0f;
        m.absorption    = 0.0f;
        m.transparency  = 0.0f;
        m.refraction    = 0.0f;
        m.reflection    = 0.0f;
        m.diffuse       = 0.0f;

        vn              = ix->t[0]->n;
        ray3d_t *r1     = &rt->r;

        // Compute normal and material
        for (size_t i=0; i<ix->n; ++i)
        {
            const triangle3d_t *pt = ix->t[i];
            const material3d_t *mt = ix->m[i];

            vn.dx          += pt->n.dx;
            vn.dy          += pt->n.dy;
            vn.dz          += pt->n.dz;

    //                float smul      = r1->v.dx*pt->n.dx + r1->v.dy*pt->n.dy + r1->v.dz*pt->n.dz; // TODO
    //
    //                if (smul >= 0) // TODO
    //                {
                m.speed        += mt->speed;
                m.damping      += mt->damping;
                m.absorption   += mt->absorption;
                m.transparency += mt->transparency;
                m.refraction   += mt->refraction;
                m.reflection   += mt->reflection;
                m.diffuse      += mt->diffuse;
    //                }
    //                else // TODO
    //                {
    //                    m.speed        += mt->speed;
    //                    m.damping      += mt->damping;
    //                    m.absorption   += mt->absorption;
    //                    m.transparency += (1.0f - mt->transparency);        // !!!
    //                    m.refraction   += mt->reflection;                   // !!!
    //                    m.reflection   += mt->refraction;                   // !!!
    //                    m.diffuse      += mt->diffuse;
    //                }
        }

        // Normalize normal
        float l     = sqrtf(vn.dx * vn.dx + vn.dy * vn.dy + vn.dz * vn.dz);
        if (l > 0.0f)
        {
            vn.dx       /= l;
            vn.dy       /= l;
            vn.dz       /= l;
        }

        // Calculate average material properties
        l               = 1.0f / ix->n;
        m.speed        *= l;
        m.damping      *= l;
        m.absorption   *= l;
        m.transparency *= l;
        m.refraction   *= l;
        m.reflection   *= l;
        m.diffuse      *= l;

        // Calculate the length of projected vector to the normal
        ray3d_t *r2     = &rf->r;
        float proj      = r1->v.dx*vn.dx + r1->v.dy*vn.dy + r1->v.dz*vn.dz;

        // Calculate the tangent and normal vectors
        vn.dx          *= proj;
        vn.dy          *= proj;
        vn.dz          *= proj;

        vt.dx           = r1->v.dx - vn.dx;
        vt.dy           = r1->v.dy - vn.dy;
        vt.dz           = r1->v.dz - vn.dz;

        // Form the reflected and refracted ray parameters
        r1->v.dx        = vt.dx / m.reflection - vn.dx * m.reflection;
        r1->v.dy        = vt.dy / m.reflection - vn.dy * m.reflection;
        r1->v.dz        = vt.dz / m.reflection - vn.dz * m.reflection;

        r2->v.dx        = vt.dx / m.refraction + vn.dx * m.refraction;
        r2->v.dy        = vt.dy / m.refraction + vn.dy * m.refraction;
        r2->v.dz        = vt.dz / m.refraction + vn.dz * m.refraction;

        // Update energy parameters
        dv.dx           = r1->z.x - ix->p.x;
        dv.dy           = r1->z.y - ix->p.y;
        dv.dz           = r1->z.z - ix->p.z;

        r1->z           = ix->p;
        r2->z           = ix->p;

        float dist      = sqrtf(dv.dx*dv.dx + dv.dy*dv.dy + dv.dz*dv.dz);
        float amplitude = rt->amplitude * (1.0f - m.absorption) * expf(m.damping*dist);
        float delay     = rt->delay + dist / m.speed;

        rf->amplitude   = amplitude * m.transparency;
        rt->amplitude   = amplitude * m.transparency - amplitude; // Amplitude will be negated
        rf->delay       = delay;
        rt->delay       = delay;

        rf->x           = *ix;
        rt->x           = *ix;
    }

    float calc_angle3d_v2(const vector3d_t *v1, const vector3d_t *v2)
    {
        float w         =
            sqrtf(v1->dx*v1->dx + v1->dy*v1->dy + v1->dz*v1->dz) *
            sqrtf(v2->dx*v2->dx + v2->dy*v2->dy + v2->dz*v2->dz);

        float wl        = v1->dx*v2->dx + v1->dy*v2->dy + v1->dz*v2->dz;
        if (w != 0.0f)
            wl             /= w;
        if (wl > 1.0f)
            wl = 1.0f;
        else if (wl < -1.0f)
            wl = -1.0f;

        return wl;
    }

    float calc_angle3d_vv(const vector3d_t *v)
    {
        float w         =
            sqrtf(v[0].dx*v[0].dx + v[0].dy*v[0].dy + v[0].dz*v[0].dz) *
            sqrtf(v[1].dx*v[1].dx + v[1].dy*v[1].dy + v[1].dz*v[1].dz);

        float wl        = v[0].dx*v[1].dx + v[0].dy*v[1].dy + v[0].dz*v[1].dz;
        if (w != 0.0f)
            wl             /= w;
        if (wl > 1.0f)
            wl = 1.0f;
        else if (wl < -1.0f)
            wl = -1.0f;

        return wl;
    }

    inline void calc_normal3d_v2(vector3d_t *n, const vector3d_t *v1, const vector3d_t *v2)
    {
        // Calculate vector multiplication
        n->dx       = v1->dy * v2->dz - v1->dz * v2->dy;
        n->dy       = v1->dz * v2->dx - v1->dx * v2->dz;
        n->dz       = v1->dx * v2->dy - v1->dy * v2->dx;

        float l     = sqrtf(n->dx*n->dx + n->dy*n->dy + n->dz*n->dz);
        if (l > 0.0f)
        {
            l           = 1.0f / l;
            n->dx      *= l;
            n->dy      *= l;
            n->dz      *= l;
        }
    }

    inline void calc_normal3d_vv(vector3d_t *n, const vector3d_t *vv)
    {
        // Calculate vector multiplication
        n->dx       = vv[0].dy * vv[1].dz - vv[0].dz * vv[1].dy;
        n->dy       = vv[0].dz * vv[1].dx - vv[0].dx * vv[1].dz;
        n->dz       = vv[0].dx * vv[1].dy - vv[0].dy * vv[1].dx;

        float l     = sqrtf(n->dx*n->dx + n->dy*n->dy + n->dz*n->dz);
        if (l > 0.0f)
        {
            l           = 1.0f / l;
            n->dx      *= l;
            n->dy      *= l;
            n->dz      *= l;
        }
    }

    void calc_normal3d_p3(vector3d_t *n, const point3d_t *p1, const point3d_t *p2, const point3d_t *p3)
    {
        // Calculate edge parameters
        vector3d_t d[2];
        d[0].dx     = p2->x - p1->x;
        d[0].dy     = p2->y - p1->y;
        d[0].dz     = p2->z - p1->z;

        d[1].dx     = p3->x - p2->x;
        d[1].dy     = p3->y - p2->y;
        d[1].dz     = p3->z - p2->z;

        calc_normal3d_vv(n, d);
    }

    void calc_normal3d_pv(vector3d_t *n, const point3d_t *pv)
    {
        // Calculate edge parameters
        vector3d_t d[2];
        d[0].dx     = pv[1].x - pv[0].x;
        d[0].dy     = pv[1].y - pv[0].y;
        d[0].dz     = pv[1].z - pv[0].z;

        d[1].dx     = pv[2].x - pv[1].x;
        d[1].dy     = pv[2].y - pv[1].y;
        d[1].dz     = pv[2].z - pv[1].z;

        calc_normal3d_vv(n, d);
    }

    inline void vector_mul_v2(vector3d_t *r, const vector3d_t *v1, const vector3d_t *v2)
    {
        vector3d_t x;
        x.dx        = v1->dy * v2->dz - v1->dz * v2->dy;
        x.dy        = v1->dz * v2->dx - v1->dx * v2->dz;
        x.dz        = v1->dx * v2->dy - v1->dy * v2->dx;
        *r          = x;
    }

    inline void vector_mul_vv(vector3d_t *r, const vector3d_t *vv)
    {
        vector3d_t x;
        x.dx        = vv[0].dy * vv[1].dz - vv[0].dz * vv[1].dy;
        x.dy        = vv[0].dz * vv[1].dx - vv[0].dx * vv[1].dz;
        x.dz        = vv[0].dx * vv[1].dy - vv[0].dy * vv[1].dx;
        *r          = x;
    }

    void move_point3d_p2(point3d_t *p, const point3d_t *p1, const point3d_t *p2, float k)
    {
        point3d_t tp;
        tp.x        = p1->x + (p2->x - p1->x) * k;
        tp.y        = p1->y + (p2->y - p1->y) * k;
        tp.z        = p1->z + (p2->z - p1->z) * k;
        tp.w        = 1.0f;
        *p          = tp;
    }

    void move_point3d_pv(point3d_t *p, const point3d_t *pv, float k)
    {
        point3d_t tp;
        tp.x        = pv[0].x + (pv[1].x - pv[0].x) * k;
        tp.y        = pv[0].y + (pv[1].y - pv[0].y) * k;
        tp.z        = pv[0].z + (pv[1].z - pv[0].z) * k;
        tp.w        = 1.0f;
        *p          = tp;
    }

    void init_octant3d_v(octant3d_t *o, const point3d_t *t, size_t n)
    {
        if (n == 0)
        {
            dsp::fill_zero(&o->min.x, sizeof(octant3d_t)/sizeof(float));
            return;
        }
        o->min  = *t;
        o->max  = *t;

        while (--n)
        {
            ++t;

            // Find minimum
            if (o->min.x > t->x)
                o->min.x    = t->x;
            if (o->min.y > t->y)
                o->min.y    = t->y;
            if (o->min.z > t->z)
                o->min.z    = t->z;

            // Find maximum
            if (o->max.x < t->x)
                o->max.x    = t->x;
            if (o->max.y < t->y)
                o->max.y    = t->y;
            if (o->max.z < t->z)
                o->max.z    = t->z;
        }

        // Complete the octant
        init_point_xyz(&o->bounds[0x00], o->max.x, o->max.y, o->max.z);
        init_point_xyz(&o->bounds[0x01], o->min.x, o->max.y, o->max.z);
        init_point_xyz(&o->bounds[0x02], o->max.x, o->min.y, o->max.z);
        init_point_xyz(&o->bounds[0x03], o->min.x, o->min.y, o->max.z);
        init_point_xyz(&o->bounds[0x04], o->max.x, o->max.y, o->min.z);
        init_point_xyz(&o->bounds[0x05], o->min.x, o->max.y, o->min.z);
        init_point_xyz(&o->bounds[0x06], o->max.x, o->min.y, o->min.z);
        init_point_xyz(&o->bounds[0x07], o->min.x, o->min.y, o->min.z);
    }

    bool check_octant3d_rv(const octant3d_t *o, const ray3d_t *r)
    {
        size_t index    = (r->v.dx < 0.0f) ? 0x01 : 0x00;
        if (r->v.dy < 0.0f)
            index          |= 0x02;
        if (r->v.dz < 0.0f)
            index          |= 0x04;

        const point3d_t *p  = &o->bounds[index];
        vector3d_t dv;
        dv.dx               = (p->x - r->z.x) * r->v.dx;
        dv.dy               = (p->y - r->z.y) * r->v.dy;
        dv.dz               = (p->z - r->z.z) * r->v.dz;

        return (dv.dx >= 0.0f) && (dv.dy >= 0.0f) && (dv.dz >= 0.0f);
    }

    inline void calc_tetrahedron_normals(tetra3d_t *t)
    {
        calc_normal3d_vv(&t->n[0], &t->r[0]); // Calculate between r[0] and r[1]
        calc_normal3d_vv(&t->n[1], &t->r[1]); // Calculate between r[1] and r[2]
        calc_normal3d_v2(&t->n[2], &t->r[2], &t->r[0]); // Calculate between r[2] and r[0]

        // Calculate plane equations
        t->r[0].dw  = - ( t->n[0].dx * t->s.x + t->n[0].dy * t->s.y + t->n[0].dz * t->s.z);
        t->r[1].dw  = - ( t->n[1].dx * t->s.x + t->n[1].dy * t->s.y + t->n[1].dz * t->s.z);
        t->r[2].dw  = - ( t->n[2].dx * t->s.x + t->n[2].dy * t->s.y + t->n[2].dz * t->s.z);
    }

    void calc_tetra3d_pv(tetra3d_t *t, const point3d_t *p)
    {
        t->s        = p[0];

        t->r[0].dx  = p[1].x - p[0].x;
        t->r[0].dy  = p[1].y - p[0].y;
        t->r[0].dz  = p[1].z - p[0].z;

        t->r[1].dx  = p[2].x - p[0].x;
        t->r[1].dy  = p[2].y - p[0].y;
        t->r[1].dz  = p[2].z - p[0].z;

        t->r[2].dx  = p[3].x - p[0].x;
        t->r[2].dy  = p[3].y - p[0].y;
        t->r[2].dz  = p[3].z - p[0].z;

        calc_tetrahedron_normals(t);
    }

    void calc_tetra3d_pv3(tetra3d_t *t, const point3d_t *p, const vector3d_t *v1, const vector3d_t *v2, const vector3d_t *v3)
    {
        t->s        = *p;
        t->r[0]     = *v1;
        t->r[1]     = *v2;
        t->r[2]     = *v3;

        calc_tetrahedron_normals(t);
    }

    void calc_tetra3d_pvv(tetra3d_t *t, const point3d_t *p, const vector3d_t *v)
    {
        t->s        = *p;
        t->r[0]     = v[0];
        t->r[1]     = v[1];
        t->r[2]     = v[2];

        calc_tetrahedron_normals(t);
    }

    float find_tetra3d_intersections(ray3d_t *r, const tetra3d_t *t, const triangle3d_t *tr)
    {
        // Ignore all bad cases
        float x[3];
        // 1: all points of triangle lay outside of each side plane of tetrahedron
        for (size_t i=0; i<3; ++i)
        {
            x[0]        = t->n[i].dx*tr->p[0].x + t->n[i].dy*tr->p[0].y + t->n[i].dz*tr->p[0].z + t->n[0].dw;
            x[1]        = t->n[i].dx*tr->p[1].x + t->n[i].dy*tr->p[1].y + t->n[i].dz*tr->p[1].z + t->n[0].dw;
            x[2]        = t->n[i].dx*tr->p[2].x + t->n[i].dy*tr->p[2].y + t->n[i].dz*tr->p[2].z + t->n[0].dw;
            if ((x[0] > 0.0f) && (x[1] > 0.0f) && (x[2] > 0.0f))
                return -1.0f;
        }
        // 2: All three projections of triangle points on the ray vector are negative
        vector3d_t dv[3];
        for (size_t i=0; i<3; ++i)
        {
            dv[0].dx    = tr->p[0].x - t->s.x;
            dv[0].dy    = tr->p[0].y - t->s.y;
            dv[0].dz    = tr->p[0].z - t->s.z;

            dv[1].dx    = tr->p[1].x - t->s.x;
            dv[1].dy    = tr->p[1].y - t->s.y;
            dv[1].dz    = tr->p[1].z - t->s.z;

            dv[2].dx    = tr->p[2].x - t->s.x;
            dv[2].dy    = tr->p[2].y - t->s.y;
            dv[2].dz    = tr->p[2].z - t->s.z;

            x[0]        = t->r[i].dx*dv[0].dx + t->r[i].dy*dv[0].dy + t->r[i].dz*dv[0].dz;
            x[1]        = t->r[i].dx*dv[1].dx + t->r[i].dy*dv[1].dy + t->r[i].dz*dv[1].dz;
            x[2]        = t->r[i].dx*dv[2].dx + t->r[i].dy*dv[2].dy + t->r[i].dz*dv[2].dz;
            if ((x[0] < 0.0f) && (x[1] < 0.0f) && (x[2] < 0.0f))
                return -1.0f;
        }

        // Calculate the vector that describes intersection between planes
        vector_mul_v2(&r[0].v, &t->n[0], &tr->n);
        vector_mul_v2(&r[1].v, &t->n[1], &tr->n);
        vector_mul_v2(&r[2].v, &t->n[2], &tr->n);

        r[0].v.dw   = - (r[0].v.dx * r[0].v.dx + r[0].v.dy*r[0].v.dy + r[0].v.dz * r[0].v.dz);
        r[1].v.dw   = - (r[1].v.dx * r[1].v.dx + r[1].v.dy*r[1].v.dy + r[1].v.dz * r[1].v.dz);
        r[2].v.dw   = - (r[2].v.dx * r[2].v.dx + r[2].v.dy*r[2].v.dy + r[2].v.dz * r[2].v.dz);

        vector3d_t m;
        m.dx = 0; // TODO: this is returned currently to avoid GCC warnings

        for (size_t i=0; i<3; ++i)
        {
            // Consider there's not exceptional situation
            if (is_zero(r[0].v.dw))
            {
                r[0].z.x        = 0.0f;
                r[0].z.y        = 0.0f;
                r[0].z.z        = 0.0f;

                r[0].v.dx       = 0.0f;
                r[0].v.dy       = 0.0f;
                r[0].v.dz       = 0.0f;

                if (r[0].v.dw == tr->n.dw)
                {
                    r[0].z.w        = -1.0f;
                    r[0].v.dw       = -1.0f;
                }
                else
                {
                    r[0].z.w        = 0.0f;
                    r[0].v.dw       = 0.0f;
                }
                continue;
            }

            // Calculate minors of the plane equation
            m.dx        = r[0].v.dy*tr->n.dz - r[0].v.dz*tr->n.dy;
            m.dy        = r[0].v.dz*tr->n.dx - r[0].v.dx*tr->n.dz;
            m.dz        = r[0].v.dx*tr->n.dy - r[0].v.dy*tr->n.dx;

            // Analyze minors to solve the equation
        }

        return m.dx; // TODO: this is returned currently to avoid GCC warnings
    }
}

#endif /* DSP_ARCH_NATIVE_3DMATH_H_ */
