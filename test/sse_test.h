#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include <core/types.h>
#include <dsp/dsp.h>
#include <core/windows.h>
#include <core/envelope.h>

#include "common.h"

namespace native
{
    void init_point_xyz(point3d_t *p, float x, float y, float z);
    void init_point(point3d_t *p, const point3d_t *s);
    void normalize_point(point3d_t *p);

    void init_vector_dxyz(vector3d_t *v, float dx, float dy, float dz);
    void init_vector(vector3d_t *p, const vector3d_t *s);
    void normalize_vector(vector3d_t *v);

    void init_ray_xyz(ray3d_t *l, float x0, float y0, float z0, float x1, float y1, float z1);
    void init_ray_dxyz(ray3d_t *l, float x0, float y0, float z0, float dx, float dy, float dz);
    void init_ray_pdv(ray3d_t *l, const point3d_t *p, const vector3d_t *v);
    void init_ray_p2(ray3d_t *l, const point3d_t *p1, const point3d_t *p2);
    void init_ray_pv(ray3d_t *l, const point3d_t *p);
    void init_ray(ray3d_t *l, const ray3d_t *r);
    void calc_ray_xyz(ray3d_t *l, float x0, float y0, float z0, float x1, float y1, float z1);
    void calc_ray_dxyz(ray3d_t *l, float x0, float y0, float z0, float dx, float dy, float dz);
    void calc_ray_pdv(ray3d_t *l, const point3d_t *p, const vector3d_t *v);
    void calc_ray_p2(ray3d_t *l, const point3d_t *p1, const point3d_t *p2);
    void calc_ray_pv(ray3d_t *l, const point3d_t *p);
    void calc_ray(ray3d_t *l, const ray3d_t *r);
    void init_segment_xyz(segment3d_t *s, float x0, float y0, float z0,float x1, float y1, float z1);
    void init_segment_p2(segment3d_t *s, const point3d_t *p1, const point3d_t *p2);
    void init_segment_pv(segment3d_t *s, const point3d_t *p);

    void init_matrix3d(matrix3d_t *dst, const matrix3d_t *src);
    void init_matrix3d_zero(matrix3d_t *m);
    void init_matrix3d_one(matrix3d_t *m);
    void init_matrix3d_identity(matrix3d_t *m);
    void init_matrix3d_translate(matrix3d_t *m, float dx, float dy, float dz);
    void init_matrix3d_scale(matrix3d_t *m, float sx, float sy, float sz);
    void init_matrix3d_rotate_x(matrix3d_t *m, float angle);
    void init_matrix3d_rotate_y(matrix3d_t *m, float angle);
    void init_matrix3d_rotate_z(matrix3d_t *m, float angle);
    void init_matrix3d_rotate_xyz(matrix3d_t *m, float x, float y, float z, float angle);
    void apply_matrix3d_mv2(vector3d_t *r, const vector3d_t *v, const matrix3d_t *m);
    void apply_matrix3d_mv1(vector3d_t *r, const matrix3d_t *m);
    void apply_matrix3d_mp2(point3d_t *r, const point3d_t *p, const matrix3d_t *m);
    void apply_matrix3d_mp1(point3d_t *r, const matrix3d_t *m);
    void apply_matrix3d_mm2(matrix3d_t *r, const matrix3d_t *s, const matrix3d_t *m);
    void apply_matrix3d_mm1(matrix3d_t *r, const matrix3d_t *m);
    void transpose_matrix3d1(matrix3d_t *r);
    void transpose_matrix3d2(matrix3d_t *r, const matrix3d_t *m);

    float check_point3d_location_tp(const triangle3d_t *t, const point3d_t *p);
    float check_point3d_location_pvp(const point3d_t *t, const point3d_t *p);
    float check_point3d_location_p3p(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3, const point3d_t *p);

    float check_point3d_on_triangle_p3p(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3, const point3d_t *p);
    float check_point3d_on_triangle_pvp(const point3d_t *pv, const point3d_t *p);
    float check_point3d_on_triangle_tp(const triangle3d_t *t, const point3d_t *p);

    size_t longest_edge3d_p3(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3);
    size_t longest_edge3d_pv(const point3d_t *p);

    float find_intersection3d_rt(point3d_t *ip, const ray3d_t *l, const triangle3d_t *t);

    float calc_angle3d_v2(const vector3d_t *v1, const vector3d_t *v2);
    float calc_angle3d_vv(const vector3d_t *v);
}

namespace sse
{
    void init_point_xyz(point3d_t *p, float x, float y, float z);
    void init_point(point3d_t *p, const point3d_t *s);
    void normalize_point(point3d_t *p);

    void init_vector_dxyz(vector3d_t *v, float dx, float dy, float dz);
    void init_vector(vector3d_t *p, const vector3d_t *s);
    void normalize_vector(vector3d_t *v);

    void init_ray_xyz(ray3d_t *l, float x0, float y0, float z0, float x1, float y1, float z1);
    void init_ray_dxyz(ray3d_t *l, float x0, float y0, float z0, float dx, float dy, float dz);
    void init_ray_pdv(ray3d_t *l, const point3d_t *p, const vector3d_t *v);
    void init_ray_p2(ray3d_t *l, const point3d_t *p1, const point3d_t *p2);
    void init_ray_pv(ray3d_t *l, const point3d_t *p);
    void init_ray(ray3d_t *l, const ray3d_t *r);
    void calc_ray_xyz(ray3d_t *l, float x0, float y0, float z0, float x1, float y1, float z1);
    void calc_ray_dxyz(ray3d_t *l, float x0, float y0, float z0, float dx, float dy, float dz);
    void calc_ray_pdv(ray3d_t *l, const point3d_t *p, const vector3d_t *v);
    void calc_ray_p2(ray3d_t *l, const point3d_t *p1, const point3d_t *p2);
    void calc_ray_pv(ray3d_t *l, const point3d_t *p);
    void calc_ray(ray3d_t *l, const ray3d_t *r);
    void init_segment_xyz(segment3d_t *s, float x0, float y0, float z0,float x1, float y1, float z1);
    void init_segment_p2(segment3d_t *s, const point3d_t *p1, const point3d_t *p2);
    void init_segment_pv(segment3d_t *s, const point3d_t *p);

    void init_matrix3d(matrix3d_t *dst, const matrix3d_t *src);
    void init_matrix3d_zero(matrix3d_t *m);
    void init_matrix3d_one(matrix3d_t *m);
    void init_matrix3d_identity(matrix3d_t *m);
    void init_matrix3d_translate(matrix3d_t *m, float dx, float dy, float dz);
    void init_matrix3d_scale(matrix3d_t *m, float sx, float sy, float sz);
    void init_matrix3d_rotate_x(matrix3d_t *m, float angle);
    void init_matrix3d_rotate_y(matrix3d_t *m, float angle);
    void init_matrix3d_rotate_z(matrix3d_t *m, float angle);
    void init_matrix3d_rotate_xyz(matrix3d_t *m, float x, float y, float z, float angle);
    void apply_matrix3d_mv2(vector3d_t *r, const vector3d_t *v, const matrix3d_t *m);
    void apply_matrix3d_mv1(vector3d_t *r, const matrix3d_t *m);
    void apply_matrix3d_mp2(point3d_t *r, const point3d_t *p, const matrix3d_t *m);
    void apply_matrix3d_mp1(point3d_t *r, const matrix3d_t *m);
    void apply_matrix3d_mm2(matrix3d_t *r, const matrix3d_t *s, const matrix3d_t *m);
    void apply_matrix3d_mm1(matrix3d_t *r, const matrix3d_t *m);
    void transpose_matrix3d1(matrix3d_t *r);
    void transpose_matrix3d2(matrix3d_t *r, const matrix3d_t *m);

    float check_point3d_location_tp(const triangle3d_t *t, const point3d_t *p);
    float check_point3d_location_pvp(const point3d_t *t, const point3d_t *p);
    float check_point3d_location_p3p(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3, const point3d_t *p);

    float check_point3d_on_triangle_p3p(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3, const point3d_t *p);
    float check_point3d_on_triangle_pvp(const point3d_t *pv, const point3d_t *p);
    float check_point3d_on_triangle_tp(const triangle3d_t *t, const point3d_t *p);

    size_t longest_edge3d_p3(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3);
    size_t longest_edge3d_pv(const point3d_t *p);

    float find_intersection3d_rt(point3d_t *ip, const ray3d_t *l, const triangle3d_t *t);

    float calc_angle3d_v2(const vector3d_t *v1, const vector3d_t *v2);
    float calc_angle3d_vv(const vector3d_t *v);
}

namespace sse_test
{
    using namespace test;
    using namespace lsp;

    typedef void (* unary_math_t) (float *dst, size_t count);
    typedef void (* binary_math_t) (float *dst, const float *src, size_t count);
    typedef float (* hfunc_t) (const float *src, size_t count);

    #define TEST_FOREACH(var, ...)    \
        const size_t ___sizes[] = { __VA_ARGS__ }; \
        for (size_t ___i=0, var=___sizes[0]; ___i<(sizeof(___sizes)/sizeof(size_t)); ++___i, var=___sizes[___i])

    inline bool point3d_ck(const point3d_t *p1, const point3d_t *p2)
    {
        return
            (float_ck(p1->x, p2->x)) &&
            (float_ck(p1->y, p2->y)) &&
            (float_ck(p1->z, p2->z));
    }

    inline bool point3d_sck(const point3d_t *p1, const point3d_t *p2)
    {
        return
            (float_ck(p1->x, p2->x)) &&
            (float_ck(p1->y, p2->y)) &&
            (float_ck(p1->z, p2->z)) &&
            (float_ck(p1->w, p2->w));
    }

    inline bool vector3d_sck(const vector3d_t *v1, const vector3d_t *v2)
    {
        return
            (float_ck(v1->dx, v2->dx)) &&
            (float_ck(v1->dy, v2->dy)) &&
            (float_ck(v1->dz, v2->dz)) &&
            (float_ck(v1->dw, v2->dw));
    }

    inline bool matrix3d_ck(const matrix3d_t *m1, const matrix3d_t *m2)
    {
        for (size_t i=0; i<16; ++i)
            if (!float_ck(m1->m[i], m2->m[i]))
                return false;
        return true;
    }

/*
    bool test_point_location_native()
    {
        triangle3d_t t[3];
        dsp::calc_triangle3d_xyz(&t[0], 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, -1.0f, 0.0f);
        dsp::calc_triangle3d_xyz(&t[1], -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, -1.0f);
        dsp::calc_triangle3d_xyz(&t[2], 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, -1.0f);

        point3d_t p[3];
        dsp::init_point_xyz(&p[0], 0.0f, 0.0f, 0.0f);
        dsp::init_point_xyz(&p[1], 2.0f, 2.0f, 2.0f);
        dsp::init_point_xyz(&p[2], 0.1f, 0.1f, 0.1f);

        float k[3] = { 1.0f, -1.0f, 1.0f };
        float v;

        for (size_t i=0; i<3; ++i)
        {
            for (size_t j=0; j<3; ++j)
            {
                v = native::check_point3d_location_tp(&t[i], &p[j]);
                if ((k[j] * v) < 0.0f)
                {
                    lsp_error("  native::check_point3d_location_tp(point) failed i=%d, j=%d", int(i), int(j));
                    return false;
                }

                v = native::check_point3d_location_tp(&t[i], &t[i].p[j]);
                if ((k[j] * v) < 0.0f)
                {
                    lsp_error("  native::check_point3d_location_tp(self) failed i=%d, j=%d", int(i), int(j));
                    return false;
                }

                v = native::check_point3d_location_pvp(t[i].p, &p[j]);
                if ((k[j] * v) < 0.0f)
                {
                    lsp_error("  native::check_point3d_location_pvp(point) failed i=%d, j=%d", int(i), int(j));
                    return false;
                }

                v = native::check_point3d_location_pvp(t[i].p, &t[i].p[j]);
                if ((k[j] * v) < 0.0f)
                {
                    lsp_error("  native::check_point3d_location_pvp(self) failed i=%d, j=%d", int(i), int(j));
                    return false;
                }

                v = native::check_point3d_location_p3p(&t[i].p[0], &t[i].p[1], &t[i].p[2],&p[j]);
                if ((k[j] * v) < 0.0f)
                {
                    lsp_error("  native::check_point3d_location_p3p(point) failed i=%d, j=%d", int(i), int(j));
                    return false;
                }

                v = native::check_point3d_location_p3p(&t[i].p[0], &t[i].p[1], &t[i].p[2],&t[i].p[j]);
                if ((k[j] * v) < 0.0f)
                {
                    lsp_error("  native::check_point3d_location_p3p(self) failed i=%d, j=%d", int(i), int(j));
                    return false;
                }
            }
        }

        return true;
    }

    bool test_point_location_sse()
    {
        triangle3d_t t[3];
        dsp::calc_triangle3d_xyz(&t[0], 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, -1.0f, 0.0f);
        dsp::calc_triangle3d_xyz(&t[1], -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, -1.0f);
        dsp::calc_triangle3d_xyz(&t[2], 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, -1.0f);

        point3d_t p[3];
        dsp::init_point_xyz(&p[0], 0.0f, 0.0f, 0.0f);
        dsp::init_point_xyz(&p[1], 2.0f, 2.0f, 2.0f);
        dsp::init_point_xyz(&p[2], 0.1f, 0.1f, 0.1f);

        float v1, v2;

        for (size_t i=0; i<3; ++i)
        {
            for (size_t j=0; j<3; ++j)
            {
                v1 = sse::check_point3d_location_tp(&t[i], &p[j]);
                v2 = sse::check_point3d_location_tp(&t[i], &p[j]);
                if (!float_ck(v1, v2))
                {
                    lsp_error("  sse::check_point3d_location_tp(point) failed i=%d, j=%d", int(i), int(j));
                    return false;
                }

                v1 = sse::check_point3d_location_tp(&t[i], &t[i].p[j]);
                v2 = sse::check_point3d_location_tp(&t[i], &t[i].p[j]);
                if (!float_ck(v1, v2))
                {
                    lsp_error("  sse::check_point3d_location_tp(self) failed i=%d, j=%d", int(i), int(j));
                    return false;
                }

                v1 = sse::check_point3d_location_pvp(t[i].p, &p[j]);
                v2 = sse::check_point3d_location_pvp(t[i].p, &p[j]);
                if (!float_ck(v1, v2))
                {
                    lsp_error("  sse::check_point3d_location_pvp(point) failed i=%d, j=%d", int(i), int(j));
                    return false;
                }

                v1 = sse::check_point3d_location_pvp(t[i].p, &t[i].p[j]);
                v2 = sse::check_point3d_location_pvp(t[i].p, &t[i].p[j]);
                if (!float_ck(v1, v2))
                {
                    lsp_error("  sse::check_point3d_location_pvp(self) failed i=%d, j=%d", int(i), int(j));
                    return false;
                }

                v1 = sse::check_point3d_location_p3p(&t[i].p[0], &t[i].p[1], &t[i].p[2],&p[j]);
                v2 = sse::check_point3d_location_p3p(&t[i].p[0], &t[i].p[1], &t[i].p[2],&p[j]);
                if (!float_ck(v1, v2))
                {
                    lsp_error("  sse::check_point3d_location_p3p(point) failed i=%d, j=%d", int(i), int(j));
                    return false;
                }

                v1 = sse::check_point3d_location_p3p(&t[i].p[0], &t[i].p[1], &t[i].p[2],&t[i].p[j]);
                v2 = sse::check_point3d_location_p3p(&t[i].p[0], &t[i].p[1], &t[i].p[2],&t[i].p[j]);
                if (!float_ck(v1, v2))
                {
                    lsp_error("  sse::check_point3d_location_p3p(self) failed i=%d, j=%d", int(i), int(j));
                    return false;
                }
            }
        }

        return true;
    }
*/

    bool test_edge_detection_native()
    {
        point3d_t lp[3];
        dsp::init_point_xyz(&lp[0], 1.0f, 1.0f, 1.0f);
        dsp::init_point_xyz(&lp[1], 2.0f, 2.0f, 2.0f);
        dsp::init_point_xyz(&lp[2], 0.1f, 0.1f, 0.1f);

        if (native::longest_edge3d_pv(lp) != 1)
        {
            lsp_error("  native::longest_edge3d_pv failed");
            return false;
        }
        if (native::longest_edge3d_p3(&lp[0], &lp[1], &lp[2]) != 1)
        {
            lsp_error("  native::longest_edge3d_pv(1) failed");
            return false;
        }
        if (native::longest_edge3d_p3(&lp[1], &lp[2], &lp[0]) != 0)
        {
            lsp_error("  native::longest_edge3d_pv(0) failed");
            return false;
        }
        if (native::longest_edge3d_p3(&lp[2], &lp[0], &lp[1]) != 2)
        {
            lsp_error("  native::longest_edge3d_pv(2) failed");
            return false;
        }

        return true;
    }

    bool test_edge_detection_sse()
    {
        point3d_t lp[3];
        dsp::init_point_xyz(&lp[0], 1.0f, 1.0f, 1.0f);
        dsp::init_point_xyz(&lp[1], 2.0f, 2.0f, 2.0f);
        dsp::init_point_xyz(&lp[2], 0.1f, 0.1f, 0.1f);

        if (sse::longest_edge3d_pv(lp) != 1)
        {
            lsp_error("  sse::longest_edge3d_pv failed");
            return false;
        }
        if (sse::longest_edge3d_p3(&lp[0], &lp[1], &lp[2]) != 1)
        {
            lsp_error("  sse::longest_edge3d_pv(1) failed");
            return false;
        }
        if (sse::longest_edge3d_p3(&lp[1], &lp[2], &lp[0]) != 0)
        {
            lsp_error("  sse::longest_edge3d_pv(0) failed");
            return false;
        }
        if (sse::longest_edge3d_p3(&lp[2], &lp[0], &lp[1]) != 2)
        {
            lsp_error("  sse::longest_edge3d_pv(2) failed");
            return false;
        }

        return true;
    }

    bool check_triangle_3d_native()
    {
        triangle3d_t t;
        point3d_t cp[12];
        point3d_t ip[10];
        float ck;

        // Special check
        dsp::init_triangle3d_xyz(&t, 2.0f, -1.0f, 0.0f, 0.0f, 2.0f, 0.0f, -2.0f, 4.0f, 0.0f);
        dsp::init_point_xyz(&cp[0], -0.5f, 0.5f, 0.0f);
        ck = native::check_point3d_on_triangle_tp(&t, &cp[0]);
        lsp_trace("ck=%f", ck);

        dsp::init_triangle3d_xyz(&t, -8.0f, -2.0f, 0.0f, -2.0f, -4.0f, 0.0f, 0.0f, -2.0f, 0.0f);
        dsp::init_point_xyz(&cp[0], 6.0f, 4.0f, 0.0f);
        ck = native::check_point3d_on_triangle_tp(&t, &cp[0]);
        lsp_trace("ck=%f", ck);

        // Main check
        dsp::init_triangle3d_xyz(&t, -2.0f, -1.0f, 0.0f, 2.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

        dsp::init_point_xyz(&cp[0], -0.5f, 0.5f, 0.0f);
        dsp::init_point_xyz(&cp[1], 0.5f, 0.5f, 0.0f);
        dsp::init_point_xyz(&cp[2], 1.5f, -0.5f, 0.0f);
        dsp::init_point_xyz(&cp[3], -1.5f, -0.5f, 0.0f);
        dsp::init_point_xyz(&cp[4], 0.75f, -1.0f, 0.0f);
        dsp::init_point_xyz(&cp[5], -0.75f, -1.0f, 0.0f);
        dsp::init_point_xyz(&cp[6], 0.0f, 0.0f, 0.0f);
        dsp::init_point_xyz(&cp[7], -0.5f, -0.5f, 0.0f);
        dsp::init_point_xyz(&cp[8], 0.5f, -0.5f, 0.0f);
        dsp::init_point(&cp[9], &t.p[0]);
        dsp::init_point(&cp[10], &t.p[1]);
        dsp::init_point(&cp[11], &t.p[2]);

        for (size_t i=0; i<12; ++i)
        {
            ck = native::check_point3d_on_triangle_tp(&t, &cp[i]);
            if (ck < 0.0f)
            {
                lsp_error("  native::check_point3d_on_triangle_tp(%d) failed", int(i));
                return false;
            }
        }

        dsp::init_point_xyz(&ip[0], 0.0f, 1.5f, 0.0f);
        dsp::init_point_xyz(&ip[1], 0.0f, -1.5f, 0.0f);
        dsp::init_point_xyz(&ip[2], -1.0f, 1.0f, 0.0f);
        dsp::init_point_xyz(&ip[3], 1.0f, 1.0f, 0.0f);
        dsp::init_point_xyz(&ip[4], 2.5f, -1.5f, 0.0f);
        dsp::init_point_xyz(&ip[5], -2.5f, -1.5f, 0.0f);
        dsp::init_point_xyz(&ip[6], -0.5f, 1.5f, 0.0f);
        dsp::init_point_xyz(&ip[7], 0.5f, 1.5f, 0.0f);
        dsp::init_point_xyz(&ip[8], 2.5f, -1.0f, 0.0f);
        dsp::init_point_xyz(&ip[9], -2.5f, -1.0f, 0.0f);

        for (size_t i=0; i<10; ++i)
        {
            ck = native::check_point3d_on_triangle_tp(&t, &ip[i]);

            if (ck >= 0.0f)
            {
                lsp_error("  !native::check_point3d_on_triangle_tp(%d) failed", int(i));
                return false;
            }
        }

        return true;
    }

    bool check_triangle_3d_sse()
    {
        triangle3d_t t;
        point3d_t cp[12];
        point3d_t ip[10];
        float ck;

        // Special check
        dsp::init_triangle3d_xyz(&t, 2.0f, -1.0f, 0.0f, 0.0f, 2.0f, 0.0f, -2.0f, 4.0f, 0.0f);
        dsp::init_point_xyz(&cp[0], -0.5f, 0.5f, 0.0f);
        ck = sse::check_point3d_on_triangle_tp(&t, &cp[0]);
        lsp_trace("ck=%f", ck);

        dsp::init_triangle3d_xyz(&t, -8.0f, -2.0f, 0.0f, -2.0f, -4.0f, 0.0f, 0.0f, -2.0f, 0.0f);
        dsp::init_point_xyz(&cp[0], 6.0f, 4.0f, 0.0f);
        ck = sse::check_point3d_on_triangle_tp(&t, &cp[0]);
        lsp_trace("ck=%f", ck);

        // Main check
        dsp::init_triangle3d_xyz(&t, -2.0f, -1.0f, 0.0f, 2.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

        dsp::init_point_xyz(&cp[0], -0.5f, 0.5f, 0.0f);
        dsp::init_point_xyz(&cp[1], 0.5f, 0.5f, 0.0f);
        dsp::init_point_xyz(&cp[2], 1.5f, -0.5f, 0.0f);
        dsp::init_point_xyz(&cp[3], -1.5f, -0.5f, 0.0f);
        dsp::init_point_xyz(&cp[4], 0.75f, -1.0f, 0.0f);
        dsp::init_point_xyz(&cp[5], -0.75f, -1.0f, 0.0f);
        dsp::init_point_xyz(&cp[6], 0.0f, 0.0f, 0.0f);
        dsp::init_point_xyz(&cp[7], -0.5f, -0.5f, 0.0f);
        dsp::init_point_xyz(&cp[8], 0.5f, -0.5f, 0.0f);
        dsp::init_point(&cp[9], &t.p[0]);
        dsp::init_point(&cp[10], &t.p[1]);
        dsp::init_point(&cp[11], &t.p[2]);

        for (size_t i=0; i<12; ++i)
        {
            ck = sse::check_point3d_on_triangle_tp(&t, &cp[i]);
            if (ck < 0.0f)
            {
                lsp_error("  sse::check_point3d_on_triangle_tp(%d) failed", int(i));
                return false;
            }
        }

        dsp::init_point_xyz(&ip[0], 0.0f, 1.5f, 0.0f);
        dsp::init_point_xyz(&ip[1], 0.0f, -1.5f, 0.0f);
        dsp::init_point_xyz(&ip[2], -1.0f, 1.0f, 0.0f);
        dsp::init_point_xyz(&ip[3], 1.0f, 1.0f, 0.0f);
        dsp::init_point_xyz(&ip[4], 2.5f, -1.5f, 0.0f);
        dsp::init_point_xyz(&ip[5], -2.5f, -1.5f, 0.0f);
        dsp::init_point_xyz(&ip[6], -0.5f, 1.5f, 0.0f);
        dsp::init_point_xyz(&ip[7], 0.5f, 1.5f, 0.0f);
        dsp::init_point_xyz(&ip[8], 2.5f, -1.0f, 0.0f);
        dsp::init_point_xyz(&ip[9], -2.5f, -1.0f, 0.0f);

        for (size_t i=0; i<10; ++i)
        {
            ck = sse::check_point3d_on_triangle_tp(&t, &ip[i]);

            if (ck >= 0.0f)
            {
                lsp_error("  !sse::check_point3d_on_triangle_tp(%d) failed", int(i));
                return false;
            }
        }

        return true;
    }

    bool check_intersection3d_rt()
    {
        triangle3d_t vt[4];
        ray3d_t r;
        point3d_t p;
        float ck;

        dsp::init_ray_dxyz(&r, 3.0f, 5.0f, 7.0f, -1.0f, -2.0f, -3.0f);
        dsp::calc_triangle3d_xyz(&vt[0], 0.0f, -5.0f, -5.0f, 0.0f, 5.0f, -5.0f, 0.0f, 0.0f, 5.0f);
        dsp::calc_triangle3d_xyz(&vt[1], 3.0f, 3.0f, -1.0f, -4.0f, 2.0f, 2.0f, 3.0f, 0.0f, 1.0f);
        dsp::calc_triangle3d_xyz(&vt[2], -5.0f, -5.0f, 0.0f, 5.0f, -5.0f, 0.0f, 0.0f, 5.0f, 0.0f);
        dsp::calc_triangle3d_xyz(&vt[3], -5.0f, 0.0f, -5.0f, -5.0f, 0.0f, 5.0f, 5.0f, 0.0f, 0.0f);

        for (size_t i=0; i<4; ++i)
        {
            dsp::init_point_xyz(&p, 0.0f, 0.0f, 0.0f);
            ck = native::find_intersection3d_rt(&p, &r, &vt[i]);
            lsp_trace("native::find_intersection3d_rt: ck=%.3f, p=(%.3f, %.3f, %.3f)", ck, p.x, p.y, p.z);
            dsp::init_point_xyz(&p, 0.0f, 0.0f, 0.0f);
            ck = sse::find_intersection3d_rt(&p, &r, &vt[i]);
            lsp_trace("sse::find_intersection3d_rt: ck=%.3f, p=(%.3f, %.3f, %.3f)", ck, p.x, p.y, p.z);
        }

        return true;
    }

    bool test_angle()
    {
        for (float phi=0.0f; phi<M_PI*2; phi += M_PI*0.25f)
        {
            for (float rho=0.0f; rho<M_PI; rho += M_PI*0.25f)
            {
                vector3d_t v1;
                vector3d_t v2;
                matrix3d_t m;

                dsp::init_vector_dxyz(&v1, 1.0f, 0.0f, 0.0f);
                v2 = v1;
                dsp::init_matrix3d_rotate_z(&m, rho);
                dsp::apply_matrix3d_mv1(&v2, &m);

                dsp::init_matrix3d_rotate_y(&m, phi);
                dsp::apply_matrix3d_mv1(&v1, &m);
                dsp::apply_matrix3d_mv1(&v2, &m);

                float angle = native::calc_angle3d_v2(&v1, &v2);
                if (!float_ck(angle, cosf(rho)))
                {
                    lsp_error("  native::calc_angle3d_v2 failed phi=%.3f, rho=%.3f, result=%.3f", phi, rho, angle);
                    return false;
                }

                angle = sse::calc_angle3d_v2(&v1, &v2);
                if (!float_ck(angle, cosf(rho)))
                {
                    lsp_error("  sse::calc_angle3d_v2 failed phi=%.3f, rho=%.3f, result=%.3f", phi, rho, angle);
                    return false;
                }
            }
        }

        return true;
    }

    int test(int argc, const char **argv)
    {
        dsp::context_t ctx;
        dsp::init();
        dsp::start(&ctx);

        int code = 0;
        #define LAUNCH(x, ...) --code; lsp_trace("Launching %s(%s)...", #x, #__VA_ARGS__); if (!x(__VA_ARGS__)) return code;

        LAUNCH(test_triplet_native);
        LAUNCH(test_triplet_sse);

//        LAUNCH(test_point_location_native);
//        LAUNCH(test_point_location_sse);

        LAUNCH(test_edge_detection_native);
        LAUNCH(test_edge_detection_sse);
        LAUNCH(check_triangle_3d_native);
        LAUNCH(check_triangle_3d_sse);

        LAUNCH(check_intersection3d_rt);
        LAUNCH(test_angle);


        lsp_info("All tests have been successfully passed");

        dsp::finish(&ctx);

        return 0;
    }
}

