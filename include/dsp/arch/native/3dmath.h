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

    void normalize_vector2(vector3d_t *v, const vector3d_t *src)
    {
        float w     = sqrtf(src->dx * src->dx + src->dy * src->dy + src->dz * src->dz);
        if (w != 0.0f)
        {
            w           = 1.0f / w;
            v->dx       = src->dx * w;
            v->dy       = src->dy * w;
            v->dz       = src->dz * w;
            v->dw       = 0.0f;
        }
        else
            *v      = *src;
    }

    void flip_vector_v1(vector3d_t *v)
    {
        v->dx       = -v->dx;
        v->dy       = -v->dy;
        v->dz       = -v->dz;
    }

    void flip_vector_v2(vector3d_t *v, const vector3d_t *sv)
    {
        v->dx       = -sv->dx;
        v->dy       = -sv->dy;
        v->dz       = -sv->dz;
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

    void init_matrix3d_translate_p1(matrix3d_t *m, const point3d_t *p)
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

        v[12]       = p->x;
        v[13]       = p->y;
        v[14]       = p->z;
        v[15]       = p->w;
    }

    void init_matrix3d_translate_v1(matrix3d_t *m, const vector3d_t *vt)
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

        v[12]       = vt->dx;
        v[13]       = vt->dy;
        v[14]       = vt->dz;
        v[15]       = 1.0f;
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

    void init_matrix3d_frustum(matrix3d_t *m, float left, float right, float bottom, float top, float znear, float zfar)
    {
        float *M    = m->m;
        M[0]        = 2.0f * znear / (right - left);
        M[1]        = 0.0f;
        M[2]        = 0.0f;
        M[3]        = 0.0f;

        M[4]        = 0.0f;
        M[5]        = 2.0f * znear / (top - bottom);
        M[6]        = 0.0f;
        M[7]        = 0.0f;

        M[8]        = (right + left) / (right - left);
        M[9]        = (top + bottom) / (top - bottom);
        M[10]       = - (zfar + znear) / (zfar - znear);
        M[11]       = -1.0f;

        M[12]       = 0.0f;
        M[13]       = 0.0f;
        M[14]       = -2.0f * zfar * znear / (zfar - znear);
        M[15]       = 0.0f;
    }

    void init_matrix3d_lookat_p1v2(matrix3d_t *m, const point3d_t *pov, const vector3d_t *fwd, const vector3d_t *up)
    {
        vector3d_t f, s, u;
        float fw, fs;

        // Normalize forward vector
        fw      = sqrtf(fwd->dx*fwd->dx + fwd->dy*fwd->dy + fwd->dz*fwd->dz);
        f.dx    = fwd->dx / fw;
        f.dy    = fwd->dy / fw;
        f.dz    = fwd->dz / fw;
        f.dw    = 0.0f;

        // Compute and normalize side vector
        s.dx    = f.dy*up->dz - f.dz*up->dy;
        s.dy    = f.dz*up->dx - f.dx*up->dz;
        s.dz    = f.dx*up->dy - f.dy*up->dx;
        s.dw    = 0.0f;

        fs      = sqrtf(s.dx*s.dx + s.dy*s.dy + s.dz*s.dz);
        s.dx   /= fs;
        s.dy   /= fs;
        s.dz   /= fs;

        // Compute orthogonal up vector
        u.dx    = f.dy*s.dz - f.dz*s.dy;
        u.dy    = f.dz*s.dx - f.dx*s.dz;
        u.dz    = f.dx*s.dy - f.dy*s.dx;
        u.dw    = 0.0f;

        // Fill matrix
        float *M    = m->m;
        M[0]    =  s.dx;
        M[1]    =  u.dx;
        M[2]    =  f.dx;
        M[3]    =  0.0f;

        M[4]    =  s.dy;
        M[5]    =  u.dy;
        M[6]    =  f.dy;
        M[7]    =  0.0f;

        M[8]    =  s.dz;
        M[9]    =  u.dz;
        M[10]   =  f.dz;
        M[11]   =  0.0f;

        M[12]   = -(s.dx*pov->x + s.dy*pov->y + s.dz*pov->z);
        M[13]   = -(u.dx*pov->x + u.dy*pov->y + u.dz*pov->z);
        M[14]   = -(f.dx*pov->x + f.dy*pov->y + f.dz*pov->z);
        M[15]   =  1.0f;
    }

    void init_matrix3d_lookat_p2v1(matrix3d_t *m, const point3d_t *pov, const point3d_t *pod, const vector3d_t *up)
    {
        vector3d_t f, s, u;
        float fw, fs;

        // Normalize forward vector
        f.dx    = pov->x - pod->x;
        f.dy    = pov->y - pod->y;
        f.dz    = pov->z - pod->z;
        f.dw    = 0.0f;

        fw      = sqrtf(f.dx*f.dx + f.dy*f.dy + f.dz*f.dz);
        f.dx    = f.dx / fw;
        f.dy    = f.dy / fw;
        f.dz    = f.dz / fw;

        // Compute and normalize side vector
        s.dx    = f.dy*up->dz - f.dz*up->dy;
        s.dy    = f.dz*up->dx - f.dx*up->dz;
        s.dz    = f.dx*up->dy - f.dy*up->dx;
        s.dw    = 0.0f;

        fs      = sqrtf(s.dx*s.dx + s.dy*s.dy + s.dz*s.dz);
        s.dx   /= fs;
        s.dy   /= fs;
        s.dz   /= fs;

        // Compute orthogonal up vector
        u.dx    = f.dy*s.dz - f.dz*s.dy;
        u.dy    = f.dz*s.dx - f.dx*s.dz;
        u.dz    = f.dx*s.dy - f.dy*s.dx;
        u.dw    = 0.0f;

        // Fill matrix
        float *M    = m->m;
        M[0]    =  s.dx;
        M[1]    =  u.dx;
        M[2]    =  f.dx;
        M[3]    =  0.0f;

        M[4]    =  s.dy;
        M[5]    =  u.dy;
        M[6]    =  f.dy;
        M[7]    =  0.0f;

        M[8]    =  s.dz;
        M[9]    =  u.dz;
        M[10]   =  f.dz;
        M[11]   =  0.0f;

        M[12]   = -(s.dx*pov->x + s.dy*pov->y + s.dz*pov->z);
        M[13]   = -(u.dx*pov->x + u.dy*pov->y + u.dz*pov->z);
        M[14]   = -(f.dx*pov->x + f.dy*pov->y + f.dz*pov->z);
        M[15]   =  1.0f;
    }

    void init_matrix3d_orientation(matrix3d_t *m, axis_orientation_t orientation)
    {
        float *M    = m->m;
        M[0]        = 0.0f;
        M[1]        = 0.0f;
        M[2]        = 0.0f;
        M[3]        = 0.0f;

        M[4]        = 0.0f;
        M[5]        = 0.0f;
        M[6]        = 0.0f;
        M[7]        = 0.0f;

        M[8]        = 0.0f;
        M[9]        = 0.0f;
        M[10]       = 0.0f;
        M[11]       = 0.0f;

        M[12]       = 0.0f;
        M[13]       = 0.0f;
        M[14]       = 0.0f;
        M[15]       = 1.0f;

        size_t o    = orientation;

        // Update the forward coordinate
        switch (o)
        {
            case AO3D_POS_X_FWD_POS_Y_UP:
            case AO3D_POS_X_FWD_POS_Z_UP:
            case AO3D_POS_X_FWD_NEG_Y_UP:
            case AO3D_POS_X_FWD_NEG_Z_UP:
                M[1]    = 1.0f;
                break;

            case AO3D_NEG_X_FWD_POS_Y_UP:
            case AO3D_NEG_X_FWD_POS_Z_UP:
            case AO3D_NEG_X_FWD_NEG_Y_UP:
            case AO3D_NEG_X_FWD_NEG_Z_UP:
                M[1]    = -1.0f;
                break;

            case AO3D_POS_Y_FWD_POS_X_UP:
            case AO3D_POS_Y_FWD_POS_Z_UP:
            case AO3D_POS_Y_FWD_NEG_X_UP:
            case AO3D_POS_Y_FWD_NEG_Z_UP:
                M[5]    = 1.0f;
                break;
            case AO3D_NEG_Y_FWD_POS_X_UP:
            case AO3D_NEG_Y_FWD_POS_Z_UP:
            case AO3D_NEG_Y_FWD_NEG_X_UP:
            case AO3D_NEG_Y_FWD_NEG_Z_UP:
                M[5]    = -1.0f;
                break;

            case AO3D_POS_Z_FWD_POS_X_UP:
            case AO3D_POS_Z_FWD_POS_Y_UP:
            case AO3D_POS_Z_FWD_NEG_X_UP:
            case AO3D_POS_Z_FWD_NEG_Y_UP:
                M[9]    = 1.0f;
                break;

            case AO3D_NEG_Z_FWD_POS_X_UP:
            case AO3D_NEG_Z_FWD_POS_Y_UP:
            case AO3D_NEG_Z_FWD_NEG_X_UP:
            case AO3D_NEG_Z_FWD_NEG_Y_UP:
                M[9]    = -1.0f;
                break;

            default:
                break;
        }

        // Update the up coordinate
        switch (o)
        {
            case AO3D_POS_X_FWD_POS_Y_UP:
            case AO3D_NEG_X_FWD_POS_Y_UP:
            case AO3D_POS_Z_FWD_POS_Y_UP:
            case AO3D_NEG_Z_FWD_POS_Y_UP:
                M[6]    = 1.0f;
                break;

            case AO3D_POS_X_FWD_POS_Z_UP:
            case AO3D_NEG_X_FWD_POS_Z_UP:
            case AO3D_POS_Y_FWD_POS_Z_UP:
            case AO3D_NEG_Y_FWD_POS_Z_UP:
                M[10]   = 1.0f;
                break;

            case AO3D_POS_X_FWD_NEG_Y_UP:
            case AO3D_NEG_X_FWD_NEG_Y_UP:
            case AO3D_POS_Z_FWD_NEG_Y_UP:
            case AO3D_NEG_Z_FWD_NEG_Y_UP:
                M[6]    = -1.0f;
                break;

            case AO3D_POS_X_FWD_NEG_Z_UP:
            case AO3D_NEG_X_FWD_NEG_Z_UP:
            case AO3D_POS_Y_FWD_NEG_Z_UP:
            case AO3D_NEG_Y_FWD_NEG_Z_UP:
                M[10]   = -1.0f;
                break;

            case AO3D_POS_Y_FWD_POS_X_UP:
            case AO3D_NEG_Y_FWD_POS_X_UP:
            case AO3D_POS_Z_FWD_POS_X_UP:
            case AO3D_NEG_Z_FWD_POS_X_UP:
                M[2]    = 1.0f;
                break;

            case AO3D_POS_Y_FWD_NEG_X_UP:
            case AO3D_NEG_Y_FWD_NEG_X_UP:
            case AO3D_POS_Z_FWD_NEG_X_UP:
            case AO3D_NEG_Z_FWD_NEG_X_UP:
                M[2]    = -1.0f;
                break;

            default:
                break;
        }

        // Update the side coordinate
        switch (o)
        {
            case AO3D_POS_X_FWD_POS_Y_UP:
            case AO3D_NEG_X_FWD_NEG_Y_UP:
            case AO3D_POS_Y_FWD_NEG_X_UP:
            case AO3D_NEG_Y_FWD_POS_X_UP:
                M[8]    = 1.0f;
                break;

            case AO3D_POS_X_FWD_POS_Z_UP:
            case AO3D_NEG_X_FWD_NEG_Z_UP:
            case AO3D_POS_Z_FWD_NEG_X_UP:
            case AO3D_NEG_Z_FWD_POS_X_UP:
                M[4]    = -1.0f;
                break;

            case AO3D_POS_X_FWD_NEG_Y_UP:
            case AO3D_NEG_X_FWD_POS_Y_UP:
            case AO3D_POS_Y_FWD_POS_X_UP:
            case AO3D_NEG_Y_FWD_NEG_X_UP:
                M[8]    = -1.0f;
                break;

            case AO3D_POS_X_FWD_NEG_Z_UP:
            case AO3D_NEG_X_FWD_POS_Z_UP:
            case AO3D_POS_Z_FWD_POS_X_UP:
            case AO3D_NEG_Z_FWD_NEG_X_UP:
                M[4]    = 1.0f;
                break;

            case AO3D_POS_Y_FWD_POS_Z_UP:
            case AO3D_NEG_Y_FWD_NEG_Z_UP:
            case AO3D_POS_Z_FWD_NEG_Y_UP:
            case AO3D_NEG_Z_FWD_POS_Y_UP:
                M[0]    =  1.0f;
                break;

            case AO3D_POS_Y_FWD_NEG_Z_UP:
            case AO3D_NEG_Y_FWD_POS_Z_UP:
            case AO3D_POS_Z_FWD_POS_Y_UP:
            case AO3D_NEG_Z_FWD_NEG_Y_UP:
                M[0]    = -1.0f;
                break;

            default:
                break;
        }
    }

    void calc_matrix3d_transform_p1v1(matrix3d_t *m, const point3d_t *p, const vector3d_t *v)
    {
        matrix3d_t xm;

        // Initialize matrix with translation and scaling parameters
        float l = sqrtf(v->dx * v->dx + v->dy * v->dy + v->dz * v->dz);
        m->m[0]     = l;
        m->m[1]     = 0.0f;
        m->m[2]     = 0.0f;
        m->m[3]     = 0.0f;

        m->m[4]     = 0.0f;
        m->m[5]     = l;
        m->m[6]     = 0.0f;
        m->m[7]     = 0.0f;

        m->m[8]     = 0.0f;
        m->m[9]     = 0.0f;
        m->m[10]    = l;
        m->m[11]    = 0.0f;

        m->m[12]    = p->x;
        m->m[13]    = p->y;
        m->m[14]    = p->z;
        m->m[15]    = 1.0f;

        if (l <= 0.0f)
            return;

        // Compute normalized vector (with length = 1)
        vector3d_t tv;
        tv.dx       = v->dx / l;
        tv.dy       = v->dy / l;
        tv.dz       = v->dz / l;
        tv.dw       = 0.0f;

        // Compute theta and phi
        float sinp  = tv.dx;
        float cosp  = sqrtf(tv.dy*tv.dy + tv.dz*tv.dz);

        // Apply rotation matrix around X axis
        if (cosp > 0.0f)
        {
            float cost  = tv.dz/cosp;
            float sint  = tv.dy/cosp;

            dsp::init_matrix3d_identity(&xm);
            xm.m[5]     = cost;
            xm.m[6]     = -sint;
            xm.m[9]     = sint;
            xm.m[10]    = cost;
            dsp::apply_matrix3d_mm1(m, &xm);
        }

        // Apply rotation matrix around Y axis
        dsp::init_matrix3d_identity(&xm);
        xm.m[0]     = cosp;
        xm.m[2]     = -sinp;
        xm.m[8]     = sinp;
        xm.m[10]    = cosp;
        dsp::apply_matrix3d_mm1(m, &xm);
    }

    void calc_matrix3d_transform_r1(matrix3d_t *m, const ray3d_t *r)
    {
        matrix3d_t xm;

        // Initialize matrix with translation and scaling parameters
        float l     = sqrtf(r->v.dx * r->v.dx + r->v.dy * r->v.dy + r->v.dz * r->v.dz);
        m->m[0]     = l;
        m->m[1]     = 0.0f;
        m->m[2]     = 0.0f;
        m->m[3]     = 0.0f;

        m->m[4]     = 0.0f;
        m->m[5]     = l;
        m->m[6]     = 0.0f;
        m->m[7]     = 0.0f;

        m->m[8]     = 0.0f;
        m->m[9]     = 0.0f;
        m->m[10]    = l;
        m->m[11]    = 0.0f;

        m->m[12]    = r->z.x;
        m->m[13]    = r->z.y;
        m->m[14]    = r->z.z;
        m->m[15]    = 1.0f;

        if (l <= 0.0f)
            return;

        // Compute normalized vector (with length = 1)
        vector3d_t tv;
        tv.dx       = r->v.dx / l;
        tv.dy       = r->v.dy / l;
        tv.dz       = r->v.dz / l;
        tv.dw       = 0.0f;

        // Compute theta and phi
        float sinp  = tv.dx;
        float cosp  = sqrtf(tv.dy*tv.dy + tv.dz*tv.dz);

        // Apply rotation matrix around X axis
        if (cosp > 0.0f)
        {
            float cost  = tv.dz/cosp;
            float sint  = tv.dy/cosp;

            dsp::init_matrix3d_identity(&xm);
            xm.m[5]     = cost;
            xm.m[6]     = -sint;
            xm.m[9]     = sint;
            xm.m[10]    = cost;
            dsp::apply_matrix3d_mm1(m, &xm);
        }

        // Apply rotation matrix around Y axis
        dsp::init_matrix3d_identity(&xm);
        xm.m[0]     = cosp;
        xm.m[2]     = -sinp;
        xm.m[8]     = sinp;
        xm.m[10]    = cosp;
        dsp::apply_matrix3d_mm1(m, &xm);
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

    void calc_normal3d_v2(vector3d_t *n, const vector3d_t *v1, const vector3d_t *v2)
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
        n->dw       = 0.0f;
    }

    void calc_normal3d_vv(vector3d_t *n, const vector3d_t *vv)
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
        n->dw       = 0.0f;
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

    void vector_mul_v2(vector3d_t *r, const vector3d_t *v1, const vector3d_t *v2)
    {
        vector3d_t x;
        x.dx        = v1->dy * v2->dz - v1->dz * v2->dy;
        x.dy        = v1->dz * v2->dx - v1->dx * v2->dz;
        x.dz        = v1->dx * v2->dy - v1->dy * v2->dx;
        *r          = x;
    }

    void vector_mul_vv(vector3d_t *r, const vector3d_t *vv)
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

    void add_vector_pv1(point3d_t *p, const vector3d_t *dv)
    {
        p->x   += dv->dx;
        p->y   += dv->dy;
        p->z   += dv->dz;
        p->w   += dv->dw;
    }

    void add_vector_pv2(point3d_t *p, const point3d_t *sp, const vector3d_t *dv)
    {
        p->x    = sp->x + dv->dx;
        p->y    = sp->y + dv->dy;
        p->z    = sp->z + dv->dz;
        p->w    = sp->w + dv->dw;
    }

    void add_vector_pvk1(point3d_t *p, const vector3d_t *dv, float k)
    {
        p->x   += dv->dx * k;
        p->y   += dv->dy * k;
        p->z   += dv->dz * k;
        p->w   += dv->dw * k;
    }

    void add_vector_pvk2(point3d_t *p, const point3d_t *sp, const vector3d_t *dv, float k)
    {
        p->x    = sp->x + dv->dx * k;
        p->y    = sp->y + dv->dy * k;
        p->z    = sp->z + dv->dz * k;
        p->w    = sp->w + dv->dw * k;
    }

    void calc_bound_box(bound_box3d_t *b, const point3d_t *p, size_t n)
    {
        if (n <= 0)
        {
            for (size_t i=0; i<8; ++i)
            {
                b->p[i].x = 0.0f;
                b->p[i].y = 0.0f;
                b->p[i].z = 0.0f;
                b->p[i].w = 1.0f;
            }
            return;
        }

        for (size_t i=0; i<8; ++i)
            b->p[i] = *p;

        while (--n)
        {
            ++p;

            // Left plane
            if (b->p[0].x > p->x)
                b->p[0].x = p->x;
            if (b->p[1].x > p->x)
                b->p[1].x = p->x;
            if (b->p[4].x > p->x)
                b->p[4].x = p->x;
            if (b->p[5].x > p->x)
                b->p[5].x = p->x;

            // Right plane
            if (b->p[2].x < p->x)
                b->p[2].x = p->x;
            if (b->p[3].x < p->x)
                b->p[3].x = p->x;
            if (b->p[6].x < p->x)
                b->p[6].x = p->x;
            if (b->p[7].x < p->x)
                b->p[7].x = p->x;

            // Near plane
            if (b->p[1].y > p->y)
                b->p[1].y = p->y;
            if (b->p[2].y > p->y)
                b->p[2].y = p->y;
            if (b->p[5].y > p->y)
                b->p[5].y = p->y;
            if (b->p[6].y > p->y)
                b->p[6].y = p->y;

            // Far plane
            if (b->p[0].y < p->y)
                b->p[0].y = p->y;
            if (b->p[3].y < p->y)
                b->p[3].y = p->y;
            if (b->p[4].y < p->y)
                b->p[4].y = p->y;
            if (b->p[7].y < p->y)
                b->p[7].y = p->y;

            // Top plane
            if (b->p[0].z < p->z)
                b->p[0].z = p->z;
            if (b->p[1].z < p->z)
                b->p[1].z = p->z;
            if (b->p[2].z < p->z)
                b->p[2].z = p->z;
            if (b->p[3].z < p->z)
                b->p[3].z = p->z;

            // Bottom plane
            if (b->p[4].z > p->z)
                b->p[4].z = p->z;
            if (b->p[5].z > p->z)
                b->p[5].z = p->z;
            if (b->p[6].z > p->z)
                b->p[6].z = p->z;
            if (b->p[7].z > p->z)
                b->p[7].z = p->z;
        }
    }

    float calc_plane_p3(vector3d_t *v, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2)
    {
        // Calculate edge parameters
        vector3d_t d[2];
        d[0].dx     = p1->x - p0->x;
        d[0].dy     = p1->y - p0->y;
        d[0].dz     = p1->z - p0->z;
        d[0].dw     = p1->w - p0->w;

        d[1].dx     = p2->x - p1->x;
        d[1].dy     = p2->y - p1->y;
        d[1].dz     = p2->z - p1->z;
        d[1].dw     = p2->w - p1->w;

        // Do vector multiplication to calculate the normal vector
        v->dx       = d[0].dy*d[1].dz - d[0].dz*d[1].dy;
        v->dy       = d[0].dz*d[1].dx - d[0].dx*d[1].dz;
        v->dz       = d[0].dx*d[1].dy - d[0].dy*d[1].dx;
        v->dw       = 0.0f;

        float w     = sqrtf(v->dx * v->dx + v->dy * v->dy + v->dz * v->dz);
        if (w != 0.0f)
        {
            float kw    = 1.0f / w;
            v->dx      *= kw;
            v->dy      *= kw;
            v->dz      *= kw;
        }

        v->dw       = - ( v->dx * p0->x + v->dy * p0->y + v->dz * p0->z); // Parameter for the plane equation
        return w;
    }

    float calc_plane_pv(vector3d_t *v, const point3d_t *pv)
    {
        // Calculate edge parameters
        vector3d_t d[2];
        d[0].dx     = pv[1].x - pv[0].x;
        d[0].dy     = pv[1].y - pv[0].y;
        d[0].dz     = pv[1].z - pv[0].z;
        d[0].dw     = pv[1].w - pv[0].w;

        d[1].dx     = pv[2].x - pv[1].x;
        d[1].dy     = pv[2].y - pv[1].y;
        d[1].dz     = pv[2].z - pv[1].z;
        d[1].dw     = pv[2].w - pv[1].w;

        // Do vector multiplication to calculate the normal vector
        v->dx       = d[0].dy*d[1].dz - d[0].dz*d[1].dy;
        v->dy       = d[0].dz*d[1].dx - d[0].dx*d[1].dz;
        v->dz       = d[0].dx*d[1].dy - d[0].dy*d[1].dx;
        v->dw       = 0.0f;

        float w     = sqrtf(v->dx * v->dx + v->dy * v->dy + v->dz * v->dz);
        if (w != 0.0f)
        {
            float kw    = 1.0f / w;
            v->dx      *= kw;
            v->dy      *= kw;
            v->dz      *= kw;
        }

        v->dw       = - ( v->dx * pv[0].x + v->dy * pv[0].y + v->dz * pv[0].z); // Parameter for the plane equation

        return w;
    }

    float calc_plane_v1p2(vector3d_t *v, const vector3d_t *v0, const point3d_t *p0, const point3d_t *p1)
    {
        vector3d_t d;

        d.dx        = p1->x - p0->x;
        d.dy        = p1->y - p0->y;
        d.dz        = p1->z - p0->z;
        d.dw        = p1->w - p0->w;

        // Do vector multiplication to calculate the normal vector
        v->dx       = d.dy*v0->dz - d.dz*v0->dy;
        v->dy       = d.dz*v0->dx - d.dx*v0->dz;
        v->dz       = d.dx*v0->dy - d.dy*v0->dx;
        v->dw       = 0.0f;

        float w     = sqrtf(v->dx * v->dx + v->dy * v->dy + v->dz * v->dz);
        if (w != 0.0f)
        {
            float kw    = 1.0f / w;
            v->dx      *= kw;
            v->dy      *= kw;
            v->dz      *= kw;
        }

        v->dw       = - ( v->dx * p0->x + v->dy * p0->y + v->dz * p0->z); // Parameter for the plane equation

        return w;
    }

    float calc_oriented_plane_p3(vector3d_t *v, const point3d_t *sp, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2)
    {
        // Calculate edge parameters
        vector3d_t d[2];
        d[0].dx     = p1->x - p0->x;
        d[0].dy     = p1->y - p0->y;
        d[0].dz     = p1->z - p0->z;
        d[0].dw     = p1->w - p0->w;

        d[1].dx     = p2->x - p1->x;
        d[1].dy     = p2->y - p1->y;
        d[1].dz     = p2->z - p1->z;
        d[1].dw     = p2->w - p1->w;

        // Do vector multiplication to calculate the normal vector
        v->dx       = d[0].dy*d[1].dz - d[0].dz*d[1].dy;
        v->dy       = d[0].dz*d[1].dx - d[0].dx*d[1].dz;
        v->dz       = d[0].dx*d[1].dy - d[0].dy*d[1].dx;
        v->dw       = 0.0f;

        float w     = sqrtf(v->dx * v->dx + v->dy * v->dy + v->dz * v->dz);
        if (w == 0.0f)
            return w;

        w           = 1.0f / w;
        v->dx      *= w;
        v->dy      *= w;
        v->dz      *= w;
        v->dw       = - ( v->dx * p0->x + v->dy * p0->y + v->dz * p0->z); // Parameter for the plane equation

        // Set the valid orientation for the plane
        float a     = (sp->x * v->dx + sp->y * v->dy + sp->z * v->dz + v->dw);
        if (a > 0.0f)
        {
            v->dx       = - v->dx;
            v->dy       = - v->dy;
            v->dz       = - v->dz;
            v->dw       = - v->dw;
        }

        return w;
    }

    float calc_rev_oriented_plane_p3(vector3d_t *v, const point3d_t *sp, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2)
    {
        // Calculate edge parameters
        vector3d_t d[2];
        d[0].dx     = p1->x - p0->x;
        d[0].dy     = p1->y - p0->y;
        d[0].dz     = p1->z - p0->z;
        d[0].dw     = p1->w - p0->w;

        d[1].dx     = p2->x - p1->x;
        d[1].dy     = p2->y - p1->y;
        d[1].dz     = p2->z - p1->z;
        d[1].dw     = p2->w - p1->w;

        // Do vector multiplication to calculate the normal vector
        v->dx       = d[0].dy*d[1].dz - d[0].dz*d[1].dy;
        v->dy       = d[0].dz*d[1].dx - d[0].dx*d[1].dz;
        v->dz       = d[0].dx*d[1].dy - d[0].dy*d[1].dx;
        v->dw       = 0.0f;

        float w     = sqrtf(v->dx * v->dx + v->dy * v->dy + v->dz * v->dz);
        if (w == 0.0f)
            return w;

        w           = 1.0f / w;
        v->dx      *= w;
        v->dy      *= w;
        v->dz      *= w;
        v->dw       = - ( v->dx * p0->x + v->dy * p0->y + v->dz * p0->z); // Parameter for the plane equation

        // Set the valid orientation for the plane
        float a     = (sp->x * v->dx + sp->y * v->dy + sp->z * v->dz + v->dw);
        if (a < 0.0f)
        {
            v->dx       = - v->dx;
            v->dy       = - v->dy;
            v->dz       = - v->dz;
            v->dw       = - v->dw;
        }

        return w;
    }

    float orient_plane_v1p1(vector3d_t *v, const point3d_t *sp, const vector3d_t *pl)
    {
        float a     = (sp->x * pl->dx + sp->y * pl->dy + sp->z * pl->dz + pl->dw);
        if (a > 0.0f) // Point is above, need to flip plane
        {
            v->dx       = - pl->dx;
            v->dy       = - pl->dy;
            v->dz       = - pl->dz;
            v->dw       = - pl->dw;
            return a;
        }
        else // Point is below or on the plane, just copy plane equation
        {
            v->dx       = pl->dx;
            v->dy       = pl->dy;
            v->dz       = pl->dz;
            v->dw       = pl->dw;
            return -a;
        }
    }

    float calc_oriented_plane_pv(vector3d_t *v, const point3d_t *sp, const point3d_t *pv)
    {
        // Calculate edge parameters
        vector3d_t d[2];
        d[0].dx     = pv[1].x - pv[0].x;
        d[0].dy     = pv[1].y - pv[0].y;
        d[0].dz     = pv[1].z - pv[0].z;
        d[0].dw     = pv[1].w - pv[0].w;

        d[1].dx     = pv[2].x - pv[1].x;
        d[1].dy     = pv[2].y - pv[1].y;
        d[1].dz     = pv[2].z - pv[1].z;
        d[1].dw     = pv[2].w - pv[1].w;

        // Do vector multiplication to calculate the normal vector
        v->dx       = d[0].dy*d[1].dz - d[0].dz*d[1].dy;
        v->dy       = d[0].dz*d[1].dx - d[0].dx*d[1].dz;
        v->dz       = d[0].dx*d[1].dy - d[0].dy*d[1].dx;
        v->dw       = 0.0f;

        float w     = sqrtf(v->dx * v->dx + v->dy * v->dy + v->dz * v->dz);
        if (w != 0.0f)
        {
            w           = 1.0f / w;
            v->dx      *= w;
            v->dy      *= w;
            v->dz      *= w;
            v->dw       = 0.0f;
        }

        v->dw       = - ( v->dx * pv[0].x + v->dy * pv[0].y + v->dz * pv[0].z); // Parameter for the plane equation

        // Set the valid orientation for the plane
        float a     = (sp->x * v->dx + sp->y * v->dy + sp->z * v->dz + v->dw);
        if (a > 0.0f)
        {
            v->dx       = - v->dx;
            v->dy       = - v->dy;
            v->dz       = - v->dz;
            v->dw       = - v->dw;
        }

        return w;
    }

    float calc_rev_oriented_plane_pv(vector3d_t *v, const point3d_t *sp, const point3d_t *pv)
    {
        // Calculate edge parameters
        vector3d_t d[2];
        d[0].dx     = pv[1].x - pv[0].x;
        d[0].dy     = pv[1].y - pv[0].y;
        d[0].dz     = pv[1].z - pv[0].z;
        d[0].dw     = pv[1].w - pv[0].w;

        d[1].dx     = pv[2].x - pv[1].x;
        d[1].dy     = pv[2].y - pv[1].y;
        d[1].dz     = pv[2].z - pv[1].z;
        d[1].dw     = pv[2].w - pv[1].w;

        // Do vector multiplication to calculate the normal vector
        v->dx       = d[0].dy*d[1].dz - d[0].dz*d[1].dy;
        v->dy       = d[0].dz*d[1].dx - d[0].dx*d[1].dz;
        v->dz       = d[0].dx*d[1].dy - d[0].dy*d[1].dx;
        v->dw       = 0.0f;

        float w     = sqrtf(v->dx * v->dx + v->dy * v->dy + v->dz * v->dz);
        if (w != 0.0f)
        {
            w           = 1.0f / w;
            v->dx      *= w;
            v->dy      *= w;
            v->dz      *= w;
            v->dw       = 0.0f;
        }

        v->dw       = - ( v->dx * pv[0].x + v->dy * pv[0].y + v->dz * pv[0].z); // Parameter for the plane equation

        // Set the valid orientation for the plane
        float a     = (sp->x * v->dx + sp->y * v->dy + sp->z * v->dz + v->dw);
        if (a < 0.0f)
        {
            v->dx       = - v->dx;
            v->dy       = - v->dy;
            v->dz       = - v->dz;
            v->dw       = - v->dw;
        }

        return w;
    }

    float calc_parallel_plane_p2p2(vector3d_t *v, const point3d_t *sp, const point3d_t *pp, const point3d_t *p0, const point3d_t *p1)
    {
        // Calculate edge parameters
        vector3d_t d[2];
        d[0].dx     = sp->x - pp->x;
        d[0].dy     = sp->y - pp->y;
        d[0].dz     = sp->z - pp->z;
        d[0].dw     = 0.0f;

        d[1].dx     = p1->x - p0->x;
        d[1].dy     = p1->y - p0->y;
        d[1].dz     = p1->z - p0->z;
        d[1].dw     = p1->w - p0->w;

        // Do vector multiplication to calculate the normal vector
        v->dx       = d[0].dy*d[1].dz - d[0].dz*d[1].dy;
        v->dy       = d[0].dz*d[1].dx - d[0].dx*d[1].dz;
        v->dz       = d[0].dx*d[1].dy - d[0].dy*d[1].dx;
        v->dw       = 0.0f;

        float w     = sqrtf(v->dx * v->dx + v->dy * v->dy + v->dz * v->dz);
        if (w != 0.0f)
        {
            w           = 1.0f / w;
            v->dx      *= w;
            v->dy      *= w;
            v->dz      *= w;
            v->dw       = 0.0f;
        }

        v->dw       = - ( v->dx * pp->x + v->dy * pp->y + v->dz * pp->z); // Parameter for the plane equation

        // Set the valid orientation for the plane
        float a     = (sp->x * v->dx + sp->y * v->dy + sp->z * v->dz + v->dw);
        if (a > 0.0f)
        {
            v->dx       = - v->dx;
            v->dy       = - v->dy;
            v->dz       = - v->dz;
            v->dw       = - v->dw;
        }

        return w;
    }

    float calc_area_p3(const point3d_t *p0, const point3d_t *p1, const point3d_t *p2)
    {
        vector3d_t v[2], n;
        v[0].dx     = p1->x - p0->x;
        v[0].dy     = p1->y - p0->y;
        v[0].dz     = p1->z - p0->z;
        v[0].dw     = 0.0f;

        v[1].dx     = p2->x - p0->x;
        v[1].dy     = p2->y - p0->y;
        v[1].dz     = p2->z - p0->z;
        v[1].dw     = 0.0f;

        // Calculate vector multiplication
        n.dx        = v[0].dy * v[1].dz - v[0].dz * v[1].dy;
        n.dy        = v[0].dz * v[1].dx - v[0].dx * v[1].dz;
        n.dz        = v[0].dx * v[1].dy - v[0].dy * v[1].dx;

        return sqrtf(n.dx*n.dx + n.dy*n.dy + n.dz*n.dz);
    }

    float calc_area_pv(const point3d_t *pv)
    {
        vector3d_t v[2], n;
        v[0].dx     = pv[1].x - pv[0].x;
        v[0].dy     = pv[1].y - pv[0].y;
        v[0].dz     = pv[1].z - pv[0].z;
        v[0].dw     = 0.0f;

        v[1].dx     = pv[2].x - pv[0].x;
        v[1].dy     = pv[2].y - pv[0].y;
        v[1].dz     = pv[2].z - pv[0].z;
        v[1].dw     = 0.0f;

        // Calculate vector multiplication
        n.dx        = v[0].dy * v[1].dz - v[0].dz * v[1].dy;
        n.dy        = v[0].dz * v[1].dx - v[0].dx * v[1].dz;
        n.dz        = v[0].dx * v[1].dy - v[0].dy * v[1].dx;

        return sqrtf(n.dx*n.dx + n.dy*n.dy + n.dz*n.dz);
    }

    float calc_min_distance_p3(const point3d_t *sp, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2)
    {
        vector3d_t v[3];
        float d[3];

        v[0].dx     = sp->x - p0->x;
        v[0].dy     = sp->y - p0->y;
        v[0].dz     = sp->z - p0->z;
        v[0].dw     = 0.0f;

        v[1].dx     = sp->x - p1->x;
        v[1].dy     = sp->y - p1->y;
        v[1].dz     = sp->z - p1->z;
        v[1].dw     = 0.0f;

        v[2].dx     = sp->x - p2->x;
        v[2].dy     = sp->y - p2->y;
        v[2].dz     = sp->z - p2->z;
        v[2].dw     = 0.0f;

        d[0]        = sqrtf(v[0].dx*v[0].dx + v[0].dy*v[0].dy + v[0].dz * v[0].dz);
        d[1]        = sqrtf(v[1].dx*v[1].dx + v[1].dy*v[1].dy + v[1].dz * v[1].dz);
        d[2]        = sqrtf(v[2].dx*v[2].dx + v[2].dy*v[2].dy + v[2].dz * v[2].dz);

        if ((d[0] <= d[1]) && (d[0] <= d[2]))
            return d[0];
        return (d[1] <= d[2]) ? d[1] : d[2];
    }

    float calc_min_distance_pv(const point3d_t *sp, const point3d_t *p)
    {
        vector3d_t v[3];
        float d[3];

        v[0].dx     = sp->x - p[0].x;
        v[0].dy     = sp->y - p[0].y;
        v[0].dz     = sp->z - p[0].z;
        v[0].dw     = 0.0f;

        v[1].dx     = sp->x - p[1].x;
        v[1].dy     = sp->y - p[1].y;
        v[1].dz     = sp->z - p[1].z;
        v[1].dw     = 0.0f;

        v[2].dx     = sp->x - p[2].x;
        v[2].dy     = sp->y - p[2].y;
        v[2].dz     = sp->z - p[2].z;
        v[2].dw     = 0.0f;

        d[0]        = sqrtf(v[0].dx*v[0].dx + v[0].dy*v[0].dy + v[0].dz * v[0].dz);
        d[1]        = sqrtf(v[1].dx*v[1].dx + v[1].dy*v[1].dy + v[1].dz * v[1].dz);
        d[2]        = sqrtf(v[2].dx*v[2].dx + v[2].dy*v[2].dy + v[2].dz * v[2].dz);

        if ((d[0] <= d[1]) && (d[0] <= d[2]))
            return d[0];
        return (d[1] <= d[2]) ? d[1] : d[2];
    }

    float calc_avg_distance_p3(const point3d_t *sp, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2)
    {
        point3d_t p;
        vector3d_t d;

        p.x     = (p0->x + p1->x + p2->x) / 3.0f;
        p.y     = (p0->y + p1->y + p2->y) / 3.0f;
        p.z     = (p0->z + p1->z + p2->z) / 3.0f;
        p.w     = 1.0f;

        d.dx    = sp->x - p.x;
        d.dy    = sp->y - p.y;
        d.dz    = sp->z - p.z;
        d.dw    = 0.0f;

        return sqrtf(d.dx * d.dx + d.dy * d.dy + d.dz * d.dz);
    }

    void calc_split_point_p2v1(point3d_t *sp, const point3d_t *l0, const point3d_t *l1, const vector3d_t *pl)
    {
        vector3d_t d;
        d.dx        = l1->x - l0->x;
        d.dy        = l1->y - l0->y;
        d.dz        = l1->z - l0->z;
        d.dw        = 0.0f;

        float t     = (l0->x*pl->dx + l0->y*pl->dy + l0->z*pl->dz + pl->dw) /
                      (pl->dx*d.dx + pl->dy*d.dy + pl->dz*d.dz);

        // Compute split point
        sp->x       = l0->x - d.dx * t;
        sp->y       = l0->y - d.dy * t;
        sp->z       = l0->z - d.dz * t;
        sp->w       = 1.0f;
    }

    void calc_split_point_pvv1(point3d_t *sp, const point3d_t *lv, const vector3d_t *pl)
    {
        vector3d_t d;
        d.dx        = lv[1].x - lv[0].x;
        d.dy        = lv[1].y - lv[0].y;
        d.dz        = lv[1].z - lv[0].z;
        d.dw        = 0.0f;

        float t     = (lv[0].x*pl->dx + lv[0].y*pl->dy + lv[0].z*pl->dz + pl->dw) /
                      (pl->dx*d.dx + pl->dy*d.dy + pl->dz*d.dz);

        // Compute split point
        sp->x       = lv[0].x - d.dx * t;
        sp->y       = lv[0].y - d.dy * t;
        sp->z       = lv[0].z - d.dz * t;
        sp->w       = 1.0f;
    }

    float calc_distance_p2(const point3d_t *p1, const point3d_t *p2)
    {
        vector3d_t d;
        d.dx        = p2->x - p1->x;
        d.dy        = p2->y - p1->y;
        d.dz        = p2->z - p1->z;
        return sqrtf(d.dx*d.dx + d.dy*d.dy + d.dz*d.dz);
    }

    float calc_distance_v1(const vector3d_t *v)
    {
        return sqrtf(v->dx*v->dx + v->dy*v->dy + v->dz*v->dz);
    }

    float calc_sqr_distance_p2(const point3d_t *p1, const point3d_t *p2)
    {
        vector3d_t d;
        d.dx        = p2->x - p1->x;
        d.dy        = p2->y - p1->y;
        d.dz        = p2->z - p1->z;
        return d.dx*d.dx + d.dy*d.dy + d.dz*d.dz;
    }

    float calc_distance_pv(const point3d_t *pv)
    {
        vector3d_t d;
        d.dx        = pv[1].x - pv[0].x;
        d.dy        = pv[1].y - pv[0].y;
        d.dz        = pv[1].z - pv[0].z;
        return sqrtf(d.dx*d.dx + d.dy*d.dy + d.dz*d.dz);
    }

    float calc_sqr_distance_pv(const point3d_t *pv)
    {
        vector3d_t d;
        d.dx        = pv[1].x - pv[0].x;
        d.dy        = pv[1].y - pv[0].y;
        d.dz        = pv[1].z - pv[0].z;
        return d.dx*d.dx + d.dy*d.dy + d.dz*d.dz;
    }

    float projection_length_p2(const point3d_t *p0, const point3d_t *p1, const point3d_t *pp)
    {
        vector3d_t v[2];
        float k[2];

        v[0].dx     = p1->x - p0->x;
        v[0].dy     = p1->y - p0->y;
        v[0].dz     = p1->z - p0->z;
        v[0].dw     = 0.0f;

        v[1].dx     = pp->x - p0->x;
        v[1].dy     = pp->y - p0->y;
        v[1].dz     = pp->z - p0->z;
        v[1].dw     = 0.0f;

        k[0]        = v[0].dx * v[0].dx + v[0].dy * v[0].dy + v[0].dz * v[0].dz;
        k[1]        = v[1].dx * v[0].dx + v[1].dy * v[0].dy + v[1].dz * v[0].dz;
        return k[1] / k[0];
    }

    float projection_length_v2(const vector3d_t *v, const vector3d_t *pv)
    {
        float k[2];

        k[0]        = pv->dx * pv->dx + pv->dy * pv->dy + pv->dz * pv->dz;
        k[1]        = pv->dx * v->dx  + pv->dy * v->dy  + pv->dz * v->dz;
        return k[1] / k[0];
    }

    /**
     * Split raw triangle with plane, generates output set of triangles into out (triangles above split plane)
     * and in (triangles below split plane). For every triangle, points with indexes 1 and 2 are the points that
     * lay on the split plane, the first triangle ALWAYS has 2 common points with plane (1 and 2)
     *
     * @param out array of vertexes above plane
     * @param n_out counter of vertexes above plane (multiple of 3), should be initialized
     * @param in array of vertexes below plane
     * @param n_in counter of vertexes below plane (multiple of 3), should be initialized
     * @param pl plane equation
     * @param pv triangle to perform the split
     */
#if 0
    void split_triangle_raw(
            raw_triangle_t *out,
            size_t *n_out,
            raw_triangle_t *in,
            size_t *n_in,
            const vector3d_t *pl,
            const raw_triangle_t *pv
        )
    {
        point3d_t sp[2];    // Split point
        vector3d_t d[2];    // Delta vector
        point3d_t v[3];     // Triangle sources
        float k[3];         // Co-location of points
        float t[2];

        in     += *n_in;
        out    += *n_out;

        v[0]    = pv->v[0];
        v[1]    = pv->v[1];
        v[2]    = pv->v[2];

        k[0]    = pl->dx*v[0].x + pl->dy*v[0].y + pl->dz*v[0].z + pl->dw;
        k[1]    = pl->dx*v[1].x + pl->dy*v[1].y + pl->dz*v[1].z + pl->dw;
        k[2]    = pl->dx*v[2].x + pl->dy*v[2].y + pl->dz*v[2].z + pl->dw;

        // Check that the whole triangle lies above the plane or below the plane
        if (k[0] < 0.0f)
        {
            if ((k[1] <= 0.0f) && (k[2] <= 0.0f))
            {
                in->v[0]        = v[0];
                in->v[1]        = v[1];
                in->v[2]        = v[2];
                ++*n_in;
                return;
            }
        }
        else if (k[0] > 0.0f)
        {
            if ((k[1] >= 0.0f) && (k[2] >= 0.0f))
            {
                out->v[0]       = v[0];
                out->v[1]       = v[1];
                out->v[2]       = v[2];
                ++*n_out;
                return;
            }
        }
        else // (k[0] == 0)
        {
            if ((k[1] >= 0.0f) && (k[2] >= 0.0f))
            {
                out->v[0]       = v[0];
                out->v[1]       = v[1];
                out->v[2]       = v[2];
                ++*n_out;
                return;
            }
            else if ((k[1] <= 0.0f) && (k[2] <= 0.0f))
            {
                in->v[0]        = v[0];
                in->v[1]        = v[1];
                in->v[2]        = v[2];
                ++*n_in;
                return;
            }
        }

        // There is an intersection with plane, we need to analyze it
        // Rotate triangle until vertex 0 is above the split plane
        if (k[0] > 0.0f)
            { /* nothing */ }
        else if (k[1] > 0.0f)
        {
            // Rotate clockwise
            t[0]    = k[0];
            sp[0]   = v[0];

            k[0]    = k[1];
            v[0]    = v[1];
            k[1]    = k[2];
            v[1]    = v[2];
            k[2]    = t[0];
            v[2]    = sp[0];
        }
        else // k[2] > 0.0f
        {
            // Rotate counter-clockwise
            t[0]    = k[0];
            sp[0]   = v[0];

            k[0]    = k[2];
            v[0]    = v[2];
            k[2]    = k[1];
            v[2]    = v[1];
            k[1]    = t[0];
            v[1]    = sp[0];
        }
//        while (k[0] <= 0.0f)
//        {
//            t[0]    = k[0];
//            sp[0]   = p[0];
//
//            k[0]    = k[1];
//            p[0]    = p[1];
//            k[1]    = k[2];
//            p[1]    = p[2];
//            k[2]    = t[0];
//            p[2]    = sp[0];
//        }

        // Now we have p[0] guaranteed to be above plane, analyze p[1] and p[2]
        if (k[1] < 0.0f) // k[1] < 0
        {
            d[0].dx = v[0].x - v[1].x;
            d[0].dy = v[0].y - v[1].y;
            d[0].dz = v[0].z - v[1].z;

            t[0]    = -k[0] / (pl->dx*d[0].dx + pl->dy*d[0].dy + pl->dz*d[0].dz);

            sp[0].x = v[0].x + d[0].dx * t[0];
            sp[0].y = v[0].y + d[0].dy * t[0];
            sp[0].z = v[0].z + d[0].dz * t[0];
            sp[0].w = 1.0f;

            if (k[2] < 0.0f) // (k[1] < 0) && (k[2] < 0)
            {
                d[1].dx = v[0].x - v[2].x;
                d[1].dy = v[0].y - v[2].y;
                d[1].dz = v[0].z - v[2].z;

                t[1]    = -k[0] / (pl->dx*d[1].dx + pl->dy*d[1].dy + pl->dz*d[1].dz);

                sp[1].x = v[0].x + d[1].dx * t[1];
                sp[1].y = v[0].y + d[1].dy * t[1];
                sp[1].z = v[0].z + d[1].dz * t[1];
                sp[1].w = 1.0f;

                // 1 triangle above plane, 2 below
                out->v[0]       = v[0];
                out->v[1]       = sp[0];
                out->v[2]       = sp[1];
                ++*n_out;
                ++out;

                in->v[0]        = v[1];
                in->v[1]        = sp[1];
                in->v[2]        = sp[0];
                ++*n_in;
                ++in;

                in->v[0]        = v[2];
                in->v[1]        = sp[1];
                in->v[2]        = v[1];
                ++*n_in;
            }
            else if (k[2] > 0.0f) // (k[1] < 0) && (k[2] > 0)
            {
                d[1].dx = v[2].x - v[1].x;
                d[1].dy = v[2].y - v[1].y;
                d[1].dz = v[2].z - v[1].z;

                t[1]    = -k[2] / (pl->dx*d[1].dx + pl->dy*d[1].dy + pl->dz*d[1].dz);

                sp[1].x = v[2].x + d[1].dx * t[1];
                sp[1].y = v[2].y + d[1].dy * t[1];
                sp[1].z = v[2].z + d[1].dz * t[1];
                sp[1].w = 1.0f;

                // 2 triangles above plane, 1 below
                out->v[0]       = v[2];
                out->v[1]       = sp[0];
                out->v[2]       = sp[1];
                ++*n_out;
                ++out;

                out->v[0]       = v[0];
                out->v[1]       = sp[0];
                out->v[2]       = v[2];
                ++*n_out;

                in->v[0]        = v[1];
                in->v[1]        = sp[1];
                in->v[2]        = sp[0];
                ++*n_in;
            }
            else // (k[1] < 0) && (k[2] == 0)
            {
                // 1 triangle above plane, 1 below
                out->v[0]       = v[0];
                out->v[1]       = sp[0];
                out->v[2]       = v[2];
                ++*n_out;

                in->v[0]        = v[1];
                in->v[1]        = v[2];
                in->v[2]        = sp[0];
                ++*n_in;
            }
        }
        else // (k[1] >= 0) && (k[2] < 0)
        {
            d[0].dx = v[0].x - v[2].x;
            d[0].dy = v[0].y - v[2].y;
            d[0].dz = v[0].z - v[2].z;

            t[0]    = -k[0] / (pl->dx*d[0].dx + pl->dy*d[0].dy + pl->dz*d[0].dz);

            sp[0].x = v[0].x + d[0].dx * t[0];
            sp[0].y = v[0].y + d[0].dy * t[0];
            sp[0].z = v[0].z + d[0].dz * t[0];
            sp[0].w = 1.0f;

            if (k[1] > 0.0f) // (k[1] > 0) && (k[2] < 0)
            {
                d[1].dx = v[1].x - v[2].x;
                d[1].dy = v[1].y - v[2].y;
                d[1].dz = v[1].z - v[2].z;

                t[1]    = -k[1] / (pl->dx*d[1].dx + pl->dy*d[1].dy + pl->dz*d[1].dz);

                sp[1].x = v[1].x + d[1].dx * t[1];
                sp[1].y = v[1].y + d[1].dy * t[1];
                sp[1].z = v[1].z + d[1].dz * t[1];
                sp[1].w = 1.0f;

                // 2 triangles above plane, 1 below
                out->v[0]       = v[0];
                out->v[1]       = sp[1];
                out->v[2]       = sp[0];
                ++*n_out;
                ++out;

                out->v[0]       = v[1];
                out->v[1]       = sp[1];
                out->v[2]       = v[0];
                ++*n_out;

                in->v[0]        = v[2];
                in->v[1]        = sp[0];
                in->v[2]        = sp[1];
                ++*n_in;
            }
            else // (k[1] == 0) && (k[2] < 0)
            {
                // 1 triangle above plane, 1 triangle below plane
                out->v[0]       = v[0];
                out->v[1]       = v[1];
                out->v[2]       = sp[0];
                ++*n_out;

                in->v[0]        = v[2];
                in->v[1]        = sp[0];
                in->v[2]        = v[1];
                ++*n_in;
            }
        }
    }
#else
    void split_triangle_raw(
            raw_triangle_t *out,
            size_t *n_out,
            raw_triangle_t *in,
            size_t *n_in,
            const vector3d_t *pl,
            const raw_triangle_t *pv
        )
    {
        size_t tag;
        point3d_t sp[2];    // Split point
        vector3d_t d[2];    // Delta vector
        float k[3];         // Co-location of points
        float t[2];

        in     += *n_in;
        out    += *n_out;

        k[0]    = pl->dx*pv->v[0].x + pl->dy*pv->v[0].y + pl->dz*pv->v[0].z + pl->dw;
        k[1]    = pl->dx*pv->v[1].x + pl->dy*pv->v[1].y + pl->dz*pv->v[1].z + pl->dw;
        k[2]    = pl->dx*pv->v[2].x + pl->dy*pv->v[2].y + pl->dz*pv->v[2].z + pl->dw;

        tag     = (k[0] > DSP_3D_TOLERANCE) ? 0x00 : (k[0] < -DSP_3D_TOLERANCE) ? 0x02 : 0x01;
        tag    |= (k[1] > DSP_3D_TOLERANCE) ? 0x00 : (k[1] < -DSP_3D_TOLERANCE) ? 0x08 : 0x04;
        tag    |= (k[2] > DSP_3D_TOLERANCE) ? 0x00 : (k[2] < -DSP_3D_TOLERANCE) ? 0x20 : 0x10;

        #define STR_SPLIT_1P(P0, P1, K) \
            d[0].dx = P0.x - P1.x; \
            d[0].dy = P0.y - P1.y; \
            d[0].dz = P0.z - P1.z; \
            \
            t[0]    = K / (pl->dx*d[0].dx + pl->dy*d[0].dy + pl->dz*d[0].dz); \
            \
            sp[0].x = P0.x - d[0].dx * t[0]; \
            sp[0].y = P0.y - d[0].dy * t[0]; \
            sp[0].z = P0.z - d[0].dz * t[0]; \
            sp[0].w = 1.0f;

        #define STR_SPLIT_2P(P0, P1, P2, K) \
            d[0].dx = P0.x - P1.x; \
            d[0].dy = P0.y - P1.y; \
            d[0].dz = P0.z - P1.z; \
            d[1].dx = P0.x - P2.x; \
            d[1].dy = P0.y - P2.y; \
            d[1].dz = P0.z - P2.z; \
            \
            t[0]    = K / (pl->dx*d[0].dx + pl->dy*d[0].dy + pl->dz*d[0].dz); \
            t[1]    = K / (pl->dx*d[1].dx + pl->dy*d[1].dy + pl->dz*d[1].dz); \
            \
            sp[0].x = P0.x - d[0].dx * t[0]; \
            sp[0].y = P0.y - d[0].dy * t[0]; \
            sp[0].z = P0.z - d[0].dz * t[0]; \
            sp[0].w = 1.0f; \
            sp[1].x = P0.x - d[1].dx * t[1]; \
            sp[1].y = P0.y - d[1].dy * t[1]; \
            sp[1].z = P0.z - d[1].dz * t[1]; \
            sp[1].w = 1.0f;

        switch (tag)
        {
            // 0 intersections, triangle is above
            case 0x00:  // 0 0 0
            case 0x01:  // 0 0 1
            case 0x04:  // 0 1 0
            case 0x05:  // 0 1 1
            case 0x10:  // 1 0 0
            case 0x11:  // 1 0 1
            case 0x14:  // 1 1 0
            case 0x15:  // 1 1 1
                *out    = *pv;
                ++(*n_out);
                break;

            // 0 intersections, triangle is below
            case 0x16:  // 1 1 2
            case 0x19:  // 1 2 1
            case 0x1a:  // 1 2 2
            case 0x25:  // 2 1 1
            case 0x26:  // 2 1 2
            case 0x29:  // 2 2 1
            case 0x2a:  // 2 2 2
                *in     = *pv;
                ++(*n_in);
                break;

            // 1 intersection, 1 triangle above, 1 triangle below, counter-clockwise
            case 0x06:  // 0 1 2
                STR_SPLIT_1P(pv->v[0], pv->v[2], k[0]);
                in->v[0]        = pv->v[0];
                in->v[1]        = pv->v[1];
                in->v[2]        = sp[0];
                out->v[0]       = pv->v[2];
                out->v[1]       = sp[0];
                out->v[2]       = pv->v[1];
                ++(*n_out); ++(*n_in);
                break;
            case 0x21:  // 2 0 1
                STR_SPLIT_1P(pv->v[1], pv->v[2], k[1]);
                in->v[0]        = pv->v[2];
                in->v[1]        = pv->v[0];
                in->v[2]        = sp[0];
                out->v[0]       = pv->v[1];
                out->v[1]       = sp[0];
                out->v[2]       = pv->v[0];
                ++(*n_out); ++(*n_in);
                break;
            case 0x18:  // 1 2 0
                STR_SPLIT_1P(pv->v[0], pv->v[1], k[0]);
                in->v[0]        = pv->v[1];
                in->v[1]        = pv->v[2];
                in->v[2]        = sp[0];
                out->v[0]       = pv->v[0];
                out->v[1]       = sp[0];
                out->v[2]       = pv->v[2];
                ++(*n_out); ++(*n_in);
                break;

            // 1 intersection, 1 triangle above, 1 triangle below, clockwise
            case 0x24:  // 2 1 0
                STR_SPLIT_1P(pv->v[0], pv->v[2], k[0]);
                in->v[0]        = pv->v[2];
                in->v[1]        = sp[0];
                in->v[2]        = pv->v[1];
                out->v[0]       = pv->v[0];
                out->v[1]       = pv->v[1];
                out->v[2]       = sp[0];
                ++(*n_out); ++(*n_in);
                break;
            case 0x12:  // 1 0 2
                STR_SPLIT_1P(pv->v[0], pv->v[1], k[0]);
                in->v[0]        = pv->v[0];
                in->v[1]        = sp[0];
                in->v[2]        = pv->v[2];
                out->v[0]       = pv->v[1];
                out->v[1]       = pv->v[2];
                out->v[2]       = sp[0];
                ++(*n_out); ++(*n_in);
                break;
            case 0x09:  // 0 2 1
                STR_SPLIT_1P(pv->v[1], pv->v[2], k[1]);
                in->v[0]        = pv->v[1];
                in->v[1]        = sp[0];
                in->v[2]        = pv->v[0];
                out->v[0]       = pv->v[2];
                out->v[1]       = pv->v[0];
                out->v[2]       = sp[0];
                ++(*n_out); ++(*n_in);
                break;

            // 2 intersections, 1 triangle below, 2 triangles above
            case 0x02:  // 0 0 2
                STR_SPLIT_2P(pv->v[0], pv->v[1], pv->v[2], k[0]);
                in->v[0]        = pv->v[0];
                in->v[1]        = sp[0];
                in->v[2]        = sp[1];
                out[0].v[0]     = pv->v[1];
                out[0].v[1]     = sp[1];
                out[0].v[2]     = sp[0];
                out[1].v[0]     = pv->v[2];
                out[1].v[1]     = sp[1];
                out[1].v[2]     = pv->v[1];
                (*n_out) += 2; ++(*n_in);
                break;
            case 0x08:  // 0 2 0
                STR_SPLIT_2P(pv->v[1], pv->v[0], pv->v[2], k[1]);
                in->v[0]        = pv->v[1];
                in->v[1]        = sp[1];
                in->v[2]        = sp[0];
                out[0].v[0]     = pv->v[2];
                out[0].v[1]     = sp[0];
                out[0].v[2]     = sp[1];
                out[1].v[0]     = pv->v[0];
                out[1].v[1]     = sp[0];
                out[1].v[2]     = pv->v[2];
                (*n_out) += 2; ++(*n_in);
                break;
            case 0x20:  // 2 0 0
                STR_SPLIT_2P(pv->v[2], pv->v[0], pv->v[1], k[2]);
                in->v[0]        = pv->v[2];
                in->v[1]        = sp[0];
                in->v[2]        = sp[1];
                out[0].v[0]     = pv->v[0];
                out[0].v[1]     = sp[1];
                out[0].v[2]     = sp[0];
                out[1].v[0]     = pv->v[1];
                out[1].v[1]     = sp[1];
                out[1].v[2]     = pv->v[0];
                (*n_out) += 2; ++(*n_in);
                break;

            // 2 intersections, 1 triangle above, 2 triangles below
            case 0x28:  // 2 2 0
                STR_SPLIT_2P(pv->v[0], pv->v[1], pv->v[2], k[0]);
                in[0].v[0]      = pv->v[1];
                in[0].v[1]      = sp[1];
                in[0].v[2]      = sp[0];
                in[1].v[0]      = pv->v[2];
                in[1].v[1]      = sp[1];
                in[1].v[2]      = pv->v[1];
                out->v[0]       = pv->v[0];
                out->v[1]       = sp[0];
                out->v[2]       = sp[1];
                ++(*n_out); (*n_in) += 2;
                break;

            case 0x22:  // 2 0 2
                STR_SPLIT_2P(pv->v[1], pv->v[0], pv->v[2], k[1]);
                in[0].v[0]      = pv->v[2];
                in[0].v[1]      = sp[0];
                in[0].v[2]      = sp[1];
                in[1].v[0]      = pv->v[0];
                in[1].v[1]      = sp[0];
                in[1].v[2]      = pv->v[2];
                out->v[0]       = pv->v[1];
                out->v[1]       = sp[1];
                out->v[2]       = sp[0];
                ++(*n_out); (*n_in) += 2;
                break;

            case 0x0a:  // 0 2 2
                STR_SPLIT_2P(pv->v[2], pv->v[0], pv->v[1], k[2]);
                in[0].v[0]      = pv->v[0];
                in[0].v[1]      = sp[1];
                in[0].v[2]      = sp[0];
                in[1].v[0]      = pv->v[1];
                in[1].v[1]      = sp[1];
                in[1].v[2]      = pv->v[0];
                out->v[0]       = pv->v[2];
                out->v[1]       = sp[0];
                out->v[2]       = sp[1];
                ++(*n_out); (*n_in) += 2;
                break;

            default:
                break;
        }

        #undef STR_SPLIT_1P
        #undef STR_SPLIT_2P
    }
#endif

    void cull_triangle_raw(
            raw_triangle_t *in,
            size_t *n_in,
            const vector3d_t *pl,
            const raw_triangle_t *pv
        )
    {
        size_t tag;
        point3d_t sp[2];    // Split point
        vector3d_t d[2];    // Delta vector
        float k[3];         // Co-location of points
        float t[2];

        in     += *n_in;

        k[0]    = pl->dx*pv->v[0].x + pl->dy*pv->v[0].y + pl->dz*pv->v[0].z + pl->dw;
        k[1]    = pl->dx*pv->v[1].x + pl->dy*pv->v[1].y + pl->dz*pv->v[1].z + pl->dw;
        k[2]    = pl->dx*pv->v[2].x + pl->dy*pv->v[2].y + pl->dz*pv->v[2].z + pl->dw;

        tag     = (k[0] > DSP_3D_TOLERANCE) ? 0x00 : (k[0] < -DSP_3D_TOLERANCE) ? 0x02 : 0x01;
        tag    |= (k[1] > DSP_3D_TOLERANCE) ? 0x00 : (k[1] < -DSP_3D_TOLERANCE) ? 0x08 : 0x04;
        tag    |= (k[2] > DSP_3D_TOLERANCE) ? 0x00 : (k[2] < -DSP_3D_TOLERANCE) ? 0x20 : 0x10;

        #define STR_SPLIT_1P(P0, P1, K) \
            d[0].dx = P0.x - P1.x; \
            d[0].dy = P0.y - P1.y; \
            d[0].dz = P0.z - P1.z; \
            \
            t[0]    = K / (pl->dx*d[0].dx + pl->dy*d[0].dy + pl->dz*d[0].dz); \
            \
            sp[0].x = P0.x - d[0].dx * t[0]; \
            sp[0].y = P0.y - d[0].dy * t[0]; \
            sp[0].z = P0.z - d[0].dz * t[0]; \
            sp[0].w = 1.0f;

        #define STR_SPLIT_2P(P0, P1, P2, K) \
            d[0].dx = P0.x - P1.x; \
            d[0].dy = P0.y - P1.y; \
            d[0].dz = P0.z - P1.z; \
            d[1].dx = P0.x - P2.x; \
            d[1].dy = P0.y - P2.y; \
            d[1].dz = P0.z - P2.z; \
            \
            t[0]    = K / (pl->dx*d[0].dx + pl->dy*d[0].dy + pl->dz*d[0].dz); \
            t[1]    = K / (pl->dx*d[1].dx + pl->dy*d[1].dy + pl->dz*d[1].dz); \
            \
            sp[0].x = P0.x - d[0].dx * t[0]; \
            sp[0].y = P0.y - d[0].dy * t[0]; \
            sp[0].z = P0.z - d[0].dz * t[0]; \
            sp[0].w = 1.0f; \
            sp[1].x = P0.x - d[1].dx * t[1]; \
            sp[1].y = P0.y - d[1].dy * t[1]; \
            sp[1].z = P0.z - d[1].dz * t[1]; \
            sp[1].w = 1.0f;

        switch (tag)
        {
            // 0 intersections, triangle is above
            case 0x00:  // 0 0 0
            case 0x01:  // 0 0 1
            case 0x04:  // 0 1 0
            case 0x05:  // 0 1 1
            case 0x10:  // 1 0 0
            case 0x11:  // 1 0 1
            case 0x14:  // 1 1 0
            case 0x15:  // 1 1 1
                break;

            // 0 intersections, triangle is below
            case 0x16:  // 1 1 2
            case 0x19:  // 1 2 1
            case 0x1a:  // 1 2 2
            case 0x25:  // 2 1 1
            case 0x26:  // 2 1 2
            case 0x29:  // 2 2 1
            case 0x2a:  // 2 2 2
                *in             = *pv;
                ++(*n_in);
                break;

            // 1 intersection, 1 triangle above, 1 triangle below, counter-clockwise
            case 0x06:  // 0 1 2
                STR_SPLIT_1P(pv->v[0], pv->v[2], k[0]);
                in->v[0]        = pv->v[0];
                in->v[1]        = pv->v[1];
                in->v[2]        = sp[0];
                ++(*n_in);
                break;
            case 0x21:  // 2 0 1
                STR_SPLIT_1P(pv->v[1], pv->v[2], k[1]);
                in->v[0]        = pv->v[2];
                in->v[1]        = pv->v[0];
                in->v[2]        = sp[0];
                ++(*n_in);
                break;
            case 0x18:  // 1 2 0
                STR_SPLIT_1P(pv->v[0], pv->v[1], k[0]);
                in->v[0]        = pv->v[1];
                in->v[1]        = pv->v[2];
                in->v[2]        = sp[0];
                ++(*n_in);
                break;

            // 1 intersection, 1 triangle above, 1 triangle below, clockwise
            case 0x24:  // 2 1 0
                STR_SPLIT_1P(pv->v[0], pv->v[2], k[0]);
                in->v[0]        = pv->v[2];
                in->v[1]        = sp[0];
                in->v[2]        = pv->v[1];
                ++(*n_in);
                break;
            case 0x12:  // 1 0 2
                STR_SPLIT_1P(pv->v[0], pv->v[1], k[0]);
                in->v[0]        = pv->v[0];
                in->v[1]        = sp[0];
                in->v[2]        = pv->v[2];
                ++(*n_in);
                break;
            case 0x09:  // 0 2 1
                STR_SPLIT_1P(pv->v[1], pv->v[2], k[1]);
                in->v[0]        = pv->v[1];
                in->v[1]        = sp[0];
                in->v[2]        = pv->v[0];
                ++(*n_in);
                break;

            // 2 intersections, 1 triangle below, 2 triangles above
            case 0x02:  // 0 0 2
                STR_SPLIT_2P(pv->v[0], pv->v[1], pv->v[2], k[0]);
                in->v[0]        = pv->v[0];
                in->v[1]        = sp[0];
                in->v[2]        = sp[1];
                ++(*n_in);
                break;
            case 0x08:  // 0 2 0
                STR_SPLIT_2P(pv->v[1], pv->v[0], pv->v[2], k[1]);
                in->v[0]        = pv->v[1];
                in->v[1]        = sp[1];
                in->v[2]        = sp[0];
                ++(*n_in);
                break;
            case 0x20:  // 2 0 0
                STR_SPLIT_2P(pv->v[2], pv->v[0], pv->v[1], k[2]);
                in->v[0]        = pv->v[2];
                in->v[1]        = sp[0];
                in->v[2]        = sp[1];
                ++(*n_in);
                break;

            // 2 intersections, 1 triangle above, 2 triangles below
            case 0x28:  // 2 2 0
                STR_SPLIT_2P(pv->v[0], pv->v[1], pv->v[2], k[0]);
                in[0].v[0]      = pv->v[1];
                in[0].v[1]      = sp[1];
                in[0].v[2]      = sp[0];
                in[1].v[0]      = pv->v[2];
                in[1].v[1]      = sp[1];
                in[1].v[2]      = pv->v[1];
                (*n_in)        += 2;
                break;

            case 0x22:  // 2 0 2
                STR_SPLIT_2P(pv->v[1], pv->v[0], pv->v[2], k[1]);
                in[0].v[0]      = pv->v[2];
                in[0].v[1]      = sp[0];
                in[0].v[2]      = sp[1];
                in[1].v[0]      = pv->v[0];
                in[1].v[1]      = sp[0];
                in[1].v[2]      = pv->v[2];
                (*n_in)        += 2;
                break;

            case 0x0a:  // 0 2 2
                STR_SPLIT_2P(pv->v[2], pv->v[0], pv->v[1], k[2]);
                in[0].v[0]      = pv->v[0];
                in[0].v[1]      = sp[1];
                in[0].v[2]      = sp[0];
                in[1].v[0]      = pv->v[1];
                in[1].v[1]      = sp[1];
                in[1].v[2]      = pv->v[0];
                (*n_in)        += 2;
                break;

            default:
                break;
        }

        #undef STR_SPLIT_1P
        #undef STR_SPLIT_2P
    }

    size_t colocation_x3_v1p3(const vector3d_t *pl, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2)
    {
        float k[3];

        k[0]    = p0->x * pl->dx + p0->y * pl->dy + p0->z * pl->dz + p0->w * pl->dw;
        k[1]    = p1->x * pl->dx + p1->y * pl->dy + p1->z * pl->dz + p1->w * pl->dw;
        k[2]    = p2->x * pl->dx + p2->y * pl->dy + p2->z * pl->dz + p2->w * pl->dw;

        size_t tag  = (k[0] > DSP_3D_TOLERANCE) ? 0x00 : (k[0] < -DSP_3D_TOLERANCE) ? 0x02 : 0x01;
        tag        |= (k[1] > DSP_3D_TOLERANCE) ? 0x00 : (k[1] < -DSP_3D_TOLERANCE) ? 0x08 : 0x04;
        tag        |= (k[2] > DSP_3D_TOLERANCE) ? 0x00 : (k[2] < -DSP_3D_TOLERANCE) ? 0x20 : 0x10;

        return tag;
    }

    size_t colocation_x3_v1pv(const vector3d_t *pl, const point3d_t *pv)
    {
        float k[3];

        k[0]    = pv[0].x * pl->dx + pv[0].y * pl->dy + pv[0].z * pl->dz + pv[0].w * pl->dw;
        k[1]    = pv[1].x * pl->dx + pv[1].y * pl->dy + pv[1].z * pl->dz + pv[1].w * pl->dw;
        k[2]    = pv[2].x * pl->dx + pv[2].y * pl->dy + pv[2].z * pl->dz + pv[2].w * pl->dw;

        size_t tag  = (k[0] > DSP_3D_TOLERANCE) ? 0x00 : (k[0] < -DSP_3D_TOLERANCE) ? 0x02 : 0x01;
        tag        |= (k[1] > DSP_3D_TOLERANCE) ? 0x00 : (k[1] < -DSP_3D_TOLERANCE) ? 0x08 : 0x04;
        tag        |= (k[2] > DSP_3D_TOLERANCE) ? 0x00 : (k[2] < -DSP_3D_TOLERANCE) ? 0x20 : 0x10;

        return tag;
    }

    size_t colocation_x2_v1p2(const vector3d_t *pl, const point3d_t *p0, const point3d_t *p1)
    {
        float k[2];

        k[0]    = p0->x * pl->dx + p0->y * pl->dy + p0->z * pl->dz + p0->w * pl->dw;
        k[1]    = p1->x * pl->dx + p1->y * pl->dy + p1->z * pl->dz + p1->w * pl->dw;

        size_t tag  = (k[0] > DSP_3D_TOLERANCE) ? 0x00 : (k[0] < -DSP_3D_TOLERANCE) ? 0x02 : 0x01;
        tag        |= (k[1] > DSP_3D_TOLERANCE) ? 0x00 : (k[1] < -DSP_3D_TOLERANCE) ? 0x08 : 0x04;

        return tag;
    }

    size_t colocation_x2_v1pv(const vector3d_t *pl, const point3d_t *pv)
    {
        float k[2];

        k[0]    = pv[0].x * pl->dx + pv[0].y * pl->dy + pv[0].z * pl->dz + pv[0].w * pl->dw;
        k[1]    = pv[1].x * pl->dx + pv[1].y * pl->dy + pv[1].z * pl->dz + pv[1].w * pl->dw;

        size_t tag  = (k[0] > DSP_3D_TOLERANCE) ? 0x00 : (k[0] < -DSP_3D_TOLERANCE) ? 0x02 : 0x01;
        tag        |= (k[1] > DSP_3D_TOLERANCE) ? 0x00 : (k[1] < -DSP_3D_TOLERANCE) ? 0x08 : 0x04;

        return tag;
    }

    size_t colocation_x3_v3p1(const vector3d_t *v0, const vector3d_t *v1, const vector3d_t *v2, const point3d_t *p)
    {
        float k[3];

        k[0]        = p->x*v0->dx + p->y*v0->dy + p->z*v0->dz + v0->dw;
        k[1]        = p->x*v1->dx + p->y*v1->dy + p->z*v1->dz + v1->dw;
        k[2]        = p->x*v2->dx + p->y*v2->dy + p->z*v2->dz + v2->dw;

        size_t tag  = (k[0] > DSP_3D_TOLERANCE) ? 0x00 : (k[0] < -DSP_3D_TOLERANCE) ? 0x02 : 0x01;
        tag        |= (k[1] > DSP_3D_TOLERANCE) ? 0x00 : (k[1] < -DSP_3D_TOLERANCE) ? 0x08 : 0x04;
        tag        |= (k[2] > DSP_3D_TOLERANCE) ? 0x00 : (k[2] < -DSP_3D_TOLERANCE) ? 0x20 : 0x10;

        return tag;
    }

    size_t colocation_x3_vvp1(const vector3d_t *vv, const point3d_t *p)
    {
        float k[3];

        k[0]        = p->x*vv[0].dx + p->y*vv[0].dy + p->z*vv[0].dz + vv[0].dw;
        k[1]        = p->x*vv[1].dx + p->y*vv[1].dy + p->z*vv[1].dz + vv[1].dw;
        k[2]        = p->x*vv[2].dx + p->y*vv[2].dy + p->z*vv[2].dz + vv[2].dw;

        size_t tag  = (k[0] > DSP_3D_TOLERANCE) ? 0x00 : (k[0] < -DSP_3D_TOLERANCE) ? 0x02 : 0x01;
        tag        |= (k[1] > DSP_3D_TOLERANCE) ? 0x00 : (k[1] < -DSP_3D_TOLERANCE) ? 0x08 : 0x04;
        tag        |= (k[2] > DSP_3D_TOLERANCE) ? 0x00 : (k[2] < -DSP_3D_TOLERANCE) ? 0x20 : 0x10;

        return tag;
    }

    void unit_vector_p1p3(vector3d_t *v, const point3d_t *sp, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2)
    {
        point3d_t p;
        p.x     = (p0->x + p1->x + p2->x) / 3.0f;
        p.y     = (p0->y + p1->y + p2->y) / 3.0f;
        p.z     = (p0->z + p1->z + p2->z) / 3.0f;

        v->dx   = p.x - sp->x;
        v->dy   = p.y - sp->y;
        v->dz   = p.z = sp->z;
        v->dw   = 0.0f;

        float   w = sqrtf(v->dx*v->dx + v->dy*v->dy + v->dz*v->dz);
        if (w != 0.0f)
        {
            w           = 1.0f / w;
            v->dx      *= w;
            v->dy      *= w;
            v->dz      *= w;
            v->dw       = 0.0f;
        }
    }

    void unit_vector_p1pv(vector3d_t *v, const point3d_t *sp, const point3d_t *pv)
    {
        point3d_t p;
        p.x     = (pv[0].x + pv[1].x + pv[2].x) / 3.0f;
        p.y     = (pv[0].y + pv[1].y + pv[2].y) / 3.0f;
        p.z     = (pv[0].z + pv[1].z + pv[2].z) / 3.0f;

        v->dx   = p.x - sp->x;
        v->dy   = p.y - sp->y;
        v->dz   = p.z = sp->z;
        v->dw   = 0.0f;

        float   w = sqrtf(v->dx*v->dx + v->dy*v->dy + v->dz*v->dz);
        if (w != 0.0f)
        {
            w           = 1.0f / w;
            v->dx      *= w;
            v->dy      *= w;
            v->dz      *= w;
            v->dw       = 0.0f;
        }
    }
}

#endif /* DSP_ARCH_NATIVE_3DMATH_H_ */
