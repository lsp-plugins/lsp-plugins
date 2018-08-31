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

    size_t longest_edge3d_p3(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3);
    size_t longest_edge3d_pv(const point3d_t *p);

    float find_intersection3d_rt(point3d_t *ip, const ray3d_t *l, const triangle3d_t *t);
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

    size_t longest_edge3d_p3(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3);
    size_t longest_edge3d_pv(const point3d_t *p);

    float find_intersection3d_rt(point3d_t *ip, const ray3d_t *l, const triangle3d_t *t);
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

    int test(int argc, const char **argv)
    {
        dsp::context_t ctx;
        dsp::init();
        dsp::start(&ctx);

        int code = 0;
        #define LAUNCH(x, ...) --code; lsp_trace("Launching %s(%s)...", #x, #__VA_ARGS__); if (!x(__VA_ARGS__)) return code;

//        LAUNCH(test_point_location_native);
//        LAUNCH(test_point_location_sse);

        LAUNCH(test_edge_detection_native);
        LAUNCH(test_edge_detection_sse);

        LAUNCH(check_intersection3d_rt);

        lsp_info("All tests have been successfully passed");

        dsp::finish(&ctx);

        return 0;
    }
}

