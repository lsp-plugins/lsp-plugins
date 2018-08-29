#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stddef.h>

#include <core/types.h>
#include <dsp/dsp.h>
#include <core/windows.h>
#include <core/envelope.h>

#include "common.h"

namespace native
{
    void fill(float *dst, float value, size_t count);

    void abs1(float *src, size_t count);
    void abs2(float *dst, const float *src, size_t count);

    void reverse1(float *dst, size_t count);
    void reverse2(float *dst, const float *src, size_t count);

    float min(const float *src, size_t count);
    float max(const float *src, size_t count);
    void minmax(const float *src, size_t count, float *min, float *max);

    float abs_min(const float *src, size_t count);
    float abs_max(const float *src, size_t count);
    void abs_minmax(const float *src, size_t count, float *min, float *max);

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

    float check_triplet3d_p3n(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3, const vector3d_t *n);
    float check_triplet3d_pvn(const point3d_t *pv, const vector3d_t *n);
    float check_triplet3d_v2n(const vector3d_t *v1, const vector3d_t *v2, const vector3d_t *n);
    float check_triplet3d_vvn(const vector3d_t *v, const vector3d_t *n);
    float check_triplet3d_vv(const vector3d_t *v);
    float check_triplet3d_t(const triangle3d_t *t);
    float check_triplet3d_tn(const triangle3d_t *t, const vector3d_t *n);

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

    void packed_complex_mod(float *dst_mod, const float *src, size_t count);
}

namespace sse
{
    void move(float *dst, const float *src, size_t count);
    void fill(float *dst, float value, size_t count);

    void abs1(float *src, size_t count);
    void abs2(float *dst, const float *src, size_t count);

    void reverse1(float *dst, size_t count);
    void reverse2(float *dst, const float *src, size_t count);

    float min(const float *src, size_t count);
    float max(const float *src, size_t count);
    void minmax(const float *src, size_t count, float *min, float *max);

    float abs_min(const float *src, size_t count);
    float abs_max(const float *src, size_t count);
    void abs_minmax(const float *src, size_t count, float *min, float *max);

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

    float check_triplet3d_p3n(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3, const vector3d_t *n);
    float check_triplet3d_pvn(const point3d_t *pv, const vector3d_t *n);
    float check_triplet3d_v2n(const vector3d_t *v1, const vector3d_t *v2, const vector3d_t *n);
    float check_triplet3d_vvn(const vector3d_t *v, const vector3d_t *n);
    float check_triplet3d_vv(const vector3d_t *v);
    float check_triplet3d_t(const triangle3d_t *t);
    float check_triplet3d_tn(const triangle3d_t *t, const vector3d_t *n);

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

    void packed_complex_mod(float *dst_mod, const float *src, size_t count);
}

namespace sse_test
{
    using namespace test;
    using namespace lsp;

    typedef void (* unary_math_t) (float *dst, size_t count);
    typedef void (* binary_math_t) (float *dst, const float *src, size_t count);
    typedef float (* hfunc_t) (const float *src, size_t count);
    typedef void (* minmax_t) (const float *src, size_t count, float *a, float *b);

    #define TEST_FOREACH(var, ...)    \
        const size_t ___sizes[] = { __VA_ARGS__ }; \
        for (size_t ___i=0, var=___sizes[0]; ___i<(sizeof(___sizes)/sizeof(size_t)); ++___i, var=___sizes[___i])

    bool test_fill()
    {
        TEST_FOREACH(sz, 0, 1, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0x80, 0x100, 0x1ff, 0xfff)
        {
            for (size_t mask=0; mask <= 0x01; ++mask)
            {
                FBuffer dst1(sz, mask & 0x01);
                FBuffer dst2(sz, mask & 0x01);

                native::fill(dst1, 3.14f, sz);
                sse::fill(dst2, 3.14f, sz);

                if (!dst1.compare(dst2))
                {
                    lsp_error("  Failed test size = %d, mask = 0x%x, overflow=%s",
                        int(sz), int(mask), (dst2.validate()) ? "false" : "true");
                    return false;
                }
            }
        }

        return true;
    }

    bool test_unary_abs(unary_math_t native, unary_math_t sse)
    {
        TEST_FOREACH(sz, 0, 1, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0xfff)
        {
            for (size_t mask=0; mask <= 0x01; ++mask)
            {
                FBuffer dst1(sz, mask & 0x01);
                FBuffer dst2(dst1);

                native(dst1, sz);
                sse(dst2, sz);

                if (!dst1.compare(dst2))
                {
                    lsp_error("  Failed test size = %d, mask = 0x%x, overflow=%s",
                        int(sz), int(mask), (dst2.validate()) ? "false" : "true");
                    return false;
                }
            }
        }

        return true;
    }

    bool test_binary_abs(binary_math_t native, binary_math_t sse)
    {
        TEST_FOREACH(sz, 0, 1, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0xfff)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                FBuffer src(sz, mask & 0x01);
                src.randomize_negative();
                FBuffer dst1(sz, mask & 0x02);
                FBuffer dst2(dst1);

                native(dst1, src, sz);
                sse(dst2, src, sz);

                if (!dst1.compare(dst2))
                {
                    lsp_error("  Failed test size = %d, mask = 0x%x, overflow=%s",
                        int(sz), int(mask), (dst2.validate()) ? "false" : "true");
                    return false;
                }
            }
        }

        return true;
    }

    bool test_move()
    {
        TEST_FOREACH(sz, 0x00, 0x01, 0x03, 0x0f, 0x1f, 0x3f, 0x7f, 0xff, 0xfff, 0x1000)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                FBuffer dst1(sz, mask & 0x01);
                FBuffer dst2(sz, mask & 0x02);

                // Test both forward and backward algorithms
                sse::move(dst1, dst2, sz);
                if (!dst1.compare(dst2))
                {
                    lsp_error("  Failed move dst2 -> dst1 size = %d, mask = 0x%x, overflow=%s",
                        int(sz), int(mask), (dst1.validate()) ? "false" : "true");
                    return false;
                }

                dst2.randomize();
                sse::move(dst2, dst1, sz);

                if (!dst2.compare(dst1))
                {
                    lsp_error("  Failed move dst1 -> dst2 size = %d, mask = 0x%x, overflow=%s",
                        int(sz), int(mask), (dst2.validate()) ? "false" : "true");
                    return false;
                }
            }
        }

        return true;
    }

    bool test_minmax(hfunc_t native, hfunc_t sse)
    {
        TEST_FOREACH(sz, 0x00, 0x01, 0x03, 0x08, 0x09, 0x0f, 0x1f, 0x3f, 0x7f, 0xff, 0xfff, 0x1000)
        {
            for (size_t mask=0; mask <= 0x01; ++mask)
            {
                FBuffer src(sz, mask & 0x01);
                src.randomize_sign();

                float s1 = native(src, sz);
                float s2 = sse(src, sz);

                if (fabs(1.0f - s1/s2) >= 1e-5)
                {
                    lsp_error("  Failed hsum: size=%d, mask=0x%x, s1=%f, s2=%f",
                        int(sz), int(mask), s1, s2);
                    return false;
                }
            }
        }

        return true;
    }

    bool test_minmax(minmax_t native, minmax_t sse)
    {
        TEST_FOREACH(sz, 0x00, 0x01, 0x03, 0x08, 0x09, 0x0f, 0x1f, 0x3f, 0x7f, 0xff, 0xfff, 0x1000)
        {
            for (size_t mask=0; mask <= 0x01; ++mask)
            {
                FBuffer src(sz, mask & 0x01);
                src.randomize_sign();

                float min1, min2, max1, max2;
                native(src, sz, &min1, &max1);
                sse(src, sz, &min2, &max2);

                if ((max1 < min1) || (max2 < min2))
                {
                    lsp_error("  Failed minmax: size=%d, mask=0x%x, min1=%f, min2=%f, max1=%f, max2=%f",
                        int(sz), int(mask), min1, min2, max1, max2);
                    return false;
                }
                else if (fabs(1.0f - min1/min2) >= 1e-5)
                {
                    lsp_error("  Failed minmax: size=%d, mask=0x%x, min1=%f, min2=%f",
                        int(sz), int(mask), min1, min2);
                    return false;
                }
                else if (fabs(1.0f - max1/max2) >= 1e-5)
                {
                    lsp_error("  Failed minmax: size=%d, mask=0x%x, max1=%f, max2=%f",
                        int(sz), int(mask), max1, max2);
                    return false;
                }
            }
        }

        return true;
    }

    bool test_reverse1()
    {
        TEST_FOREACH(sz, 0x00, 0x01, 0x03, 0x08, 0x09, 0x0f, 0x1f, 0x3f, 0x7f, 0xff, 0xfff, 0x1000)
        {
            for (size_t mask=0; mask <= 0x01; ++mask)
            {
                FBuffer dst(sz, mask & 0x01);
                FBuffer dst1(dst);
                FBuffer dst2(dst);

                native::reverse1(dst1, sz);
                sse::reverse1(dst2, sz);

                if (!dst2.compare(dst1))
                {
                    lsp_error("  Failed test_reverse1: size=%d, mask=0x%x, overflow=%s",
                        int(sz), int(mask), (dst2.validate()) ? "false" : "true");
                    return false;
                }

                native::reverse1(dst1, sz);
                sse::reverse1(dst2, sz);

                if (!dst2.compare(dst1))
                {
                    lsp_error("  Failed test_reverse1: size=%d, mask=0x%x, overflow=%s",
                        int(sz), int(mask), (dst2.validate()) ? "false" : "true");
                    return false;
                }
                else if (!dst2.compare(dst))
                {
                    lsp_error("  Failed test_reverse1: size=%d, mask=0x%x, overflow=%s",
                        int(sz), int(mask), (dst2.validate()) ? "false" : "true");
                    return false;
                }
            }
        }

        return true;
    }

    bool test_reverse2()
    {
        TEST_FOREACH(sz, 0x00, 0x01, 0x03, 0x08, 0x09, 0x0f, 0x1f, 0x3f, 0x7f, 0xff, 0xfff, 0x1000)
        {
            for (size_t mask=0; mask <= 0x07; ++mask)
            {
                FBuffer src(sz, mask & 0x01);
                FBuffer dst1(sz, mask & 0x02);
                FBuffer dst2(sz, mask & 0x02);
                FBuffer dst3(sz, mask & 0x04);
                FBuffer dst4(sz, mask & 0x04);

                native::reverse2(dst1, src, sz);
                sse::reverse2(dst2, src, sz);

                if (!dst2.compare(dst1))
                {
                    lsp_error("  Failed test_reverse1: size=%d, mask=0x%x, overflow=%s",
                        int(sz), int(mask), (dst2.validate()) ? "false" : "true");
                    return false;
                }

                native::reverse2(dst3, dst1, sz);
                sse::reverse2(dst4, dst2, sz);

                if (!dst4.compare(dst3))
                {
                    lsp_error("  Failed test_reverse1: size=%d, mask=0x%x, overflow=%s",
                        int(sz), int(mask), (dst4.validate()) ? "false" : "true");
                    return false;
                }
                else if (!dst4.compare(src))
                {
                    lsp_error("  Failed test_reverse1: size=%d, mask=0x%x, overflow=%s",
                        int(sz), int(mask), (dst4.validate()) ? "false" : "true");
                    return false;
                }
            }
        }

        return true;
    }

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

    bool test_geometry_basic()
    {
        point3d_t   p1, p2, p3;
        vector3d_t  v1, v2, v3;

        native::init_point_xyz(&p1, 1.0f, 2.0f, 3.0f);
        sse::init_point_xyz(&p2, 1.0f, 2.0f, 3.0f);
        if (!point3d_sck(&p1, &p2))
        {
            lsp_error("  Failed init_point_xyz");
            return false;
        }

        native::init_point(&p2, &p1);
        sse::init_point(&p3, &p1);
        if (!point3d_sck(&p1, &p2))
        {
            lsp_error("  Failed native init_point");
            return false;
        }
        else if (!point3d_sck(&p1, &p3))
        {
            lsp_error("  Failed sse init_point");
            return false;
        }

        native::normalize_point(&p2);
        sse::normalize_point(&p3);
        if (!point3d_sck(&p2, &p3))
        {
            lsp_error("  Failed sse normalize_point");
            return false;
        }

        native::init_vector_dxyz(&v1, 1.0f, 2.0f, 3.0f);
        sse::init_vector_dxyz(&v2, 1.0f, 2.0f, 3.0f);
        if (!vector3d_sck(&v1, &v2))
        {
            lsp_error("  Failed init_vector_dxyz");
            return false;
        }

        native::init_vector(&v2, &v1);
        sse::init_vector(&v3, &v1);
        if (!vector3d_sck(&v1, &v2))
        {
            lsp_error("  Failed native init_vector");
            return false;
        }
        else if (!vector3d_sck(&v1, &v3))
        {
            lsp_error("  Failed sse init_vector");
            return false;
        }

        native::normalize_vector(&v2);
        sse::normalize_vector(&v3);
        if (!vector3d_sck(&v2, &v3))
        {
            lsp_error("  Failed sse normalize_vector");
            return false;
        }

//        void init_ray_xyz(ray3d_t *l, float x0, float y0, float z0, float x1, float y1, float z1);
//        void init_ray_dxyz(ray3d_t *l, float x0, float y0, float z0, float dx, float dy, float dz);
//        void init_ray_pdv(ray3d_t *l, const point3d_t *p, const vector3d_t *v);
//        void init_ray_p2(ray3d_t *l, const point3d_t *p1, const point3d_t *p2);
//        void init_ray_pv(ray3d_t *l, const point3d_t *p);
//        void init_ray(ray3d_t *l, const ray3d_t *r);
//        void calc_ray_xyz(ray3d_t *l, float x0, float y0, float z0, float x1, float y1, float z1);
//        void calc_ray_dxyz(ray3d_t *l, float x0, float y0, float z0, float dx, float dy, float dz);
//        void calc_ray_pdv(ray3d_t *l, const point3d_t *p, const vector3d_t *v);
//        void calc_ray_p2(ray3d_t *l, const point3d_t *p1, const point3d_t *p2);
//        void calc_ray_pv(ray3d_t *l, const point3d_t *p);
//        void calc_ray(ray3d_t *l, const ray3d_t *r);
//        void init_segment_xyz(segment3d_t *s, float x0, float y0, float z0,float x1, float y1, float z1);
//        void init_segment_p2(segment3d_t *s, const point3d_t *p1, const point3d_t *p2);
//        void init_segment_pv(segment3d_t *s, const point3d_t *p);

        return true;
    }

    bool test_matrix_native()
    {
        matrix3d_t m;
        point3d_t p1, p2, pc;

        native::init_point_xyz(&p1, 1.0f, 2.0f, 3.0f);
        native::init_point_xyz(&pc, 1.0f, 2.0f, 3.0f);

        // Apply identity matrix
        native::init_matrix3d_identity(&m);
        native::apply_matrix3d_mp2(&p2, &p1, &m);
        native::apply_matrix3d_mp1(&p1, &m);
        if (!point3d_ck(&p1, &pc))
        {
            lsp_error("  Failed test_matrix::apply_matrix3d_mp1 on identity matrix");
            return false;
        }
        else if (!point3d_ck(&p1, &p2))
        {
            lsp_error("  Failed test_matrix::apply_matrix3d_mp2 on identity matrix");
            return false;
        }

        // Apply translation matrix
        native::init_point_xyz(&p1, 1.0f, 1.0f, 1.0f);
        native::init_point_xyz(&pc, 2.0f, 3.0f, 4.0f);
        native::init_matrix3d_translate(&m, 1.0f, 2.0f, 3.0f);
        native::apply_matrix3d_mp2(&p2, &p1, &m);
        native::apply_matrix3d_mp1(&p1, &m);
        if (!point3d_ck(&p1, &pc))
        {
            lsp_error("  Failed test_matrix::apply_matrix3d_mp1 on translation matrix");
            return false;
        }
        else if (!point3d_ck(&p1, &p2))
        {
            lsp_error("  Failed test_matrix::apply_matrix3d_mp2 on translation matrix");
            return false;
        }

        // Apply rotation Z matrix
        native::init_point_xyz(&p1, M_SQRT1_2, M_SQRT1_2, 0.0f);
        native::init_point_xyz(&pc, 0.0f, 1.0f, 0.0f);
        native::init_matrix3d_rotate_z(&m, M_PI * 0.25f);
        native::apply_matrix3d_mp2(&p2, &p1, &m);
        native::apply_matrix3d_mp1(&p1, &m);
        if (!point3d_ck(&p1, &pc))
        {
            lsp_error("  Failed test_matrix::apply_matrix3d_mp1 on rotate_z matrix");
            return false;
        }
        else if (!point3d_ck(&p1, &p2))
        {
            lsp_error("  Failed test_matrix::apply_matrix3d_mp2 on rotate_z matrix");
            return false;
        }

        // Apply rotation X matrix
        native::init_point_xyz(&p1, 0.0f, M_SQRT1_2, M_SQRT1_2);
        native::init_point_xyz(&pc, 0.0f, 0.0f, 1.0f);
        native::init_matrix3d_rotate_x(&m, M_PI * 0.25f);
        native::apply_matrix3d_mp2(&p2, &p1, &m);
        native::apply_matrix3d_mp1(&p1, &m);
        if (!point3d_ck(&p1, &pc))
        {
            lsp_error("  Failed test_matrix::apply_matrix3d_mp1 on rotate_x matrix");
            return false;
        }
        else if (!point3d_ck(&p1, &p2))
        {
            lsp_error("  Failed test_matrix::apply_matrix3d_mp2 on rotate_x matrix");
            return false;
        }

        // Apply rotation Y matrix
        native::init_point_xyz(&p1, M_SQRT1_2, 0.0f, M_SQRT1_2);
        native::init_point_xyz(&pc, 1.0f, 0.0f, 0.0f);
        native::init_matrix3d_rotate_y(&m, M_PI * 0.25f);
        native::apply_matrix3d_mp2(&p2, &p1, &m);
        native::apply_matrix3d_mp1(&p1, &m);
        if (!point3d_ck(&p1, &pc))
        {
            lsp_error("  Failed test_matrix::apply_matrix3d_mp1 on rotate_y matrix");
            return false;
        }
        else if (!point3d_ck(&p1, &p2))
        {
            lsp_error("  Failed test_matrix::apply_matrix3d_mp2 on rotate_y matrix");
            return false;
        }

        // Apply scale matrix
        native::init_point_xyz(&p1, 1.0f, 1.0f, 1.0f);
        native::init_point_xyz(&pc, 2.0f, 4.0f, 8.0f);
        native::init_matrix3d_scale(&m, 2.0f, 4.0f, 8.0f);
        native::apply_matrix3d_mp2(&p2, &p1, &m);
        native::apply_matrix3d_mp1(&p1, &m);
        if (!point3d_ck(&p1, &pc))
        {
            lsp_error("  Failed test_matrix::apply_matrix3d_mp1 on scale matrix");
            return false;
        }
        else if (!point3d_ck(&p1, &p2))
        {
            lsp_error("  Failed test_matrix::apply_matrix3d_mp2 on scale matrix");
            return false;
        }

        // Transpose matrix
        matrix3d_t mt1, mt2;
        for (int i=0; i<16; ++i)
            m.m[i]      = i + 1;
        native::init_matrix3d(&mt2, &m);
        native::transpose_matrix3d2(&mt1, &m);
        native::transpose_matrix3d1(&mt2);

        if (!matrix3d_ck(&mt1, &mt2))
        {
            lsp_error("  Failed test_matrix::transpose3d matrix");
            return false;
        }

        native::transpose_matrix3d1(&mt1);
        if (!matrix3d_ck(&mt1, &m))
        {
            lsp_error("  Failed test_matrix::transpose3d1 matrix");
            return false;
        }

        return true;
    }

    bool test_matrix_sse()
    {
        matrix3d_t m;
        point3d_t p1, p2, pc;

        sse::init_point_xyz(&p1, 1.0f, 2.0f, 3.0f);
        sse::init_point_xyz(&pc, 1.0f, 2.0f, 3.0f);

        // Apply identity matrix
        sse::init_matrix3d_identity(&m);
        sse::apply_matrix3d_mp2(&p2, &p1, &m);
        sse::apply_matrix3d_mp1(&p1, &m);
        if (!point3d_ck(&p1, &pc))
        {
            lsp_error("  Failed test_matrix::apply_matrix3d_mp1 on identity matrix");
            return false;
        }
        else if (!point3d_ck(&p1, &p2))
        {
            lsp_error("  Failed test_matrix::apply_matrix3d_mp2 on identity matrix");
            return false;
        }

        // Apply translation matrix
        sse::init_point_xyz(&p1, 1.0f, 1.0f, 1.0f);
        sse::init_point_xyz(&pc, 2.0f, 3.0f, 4.0f);
        sse::init_matrix3d_translate(&m, 1.0f, 2.0f, 3.0f);
        sse::apply_matrix3d_mp2(&p2, &p1, &m);
        sse::apply_matrix3d_mp1(&p1, &m);
        if (!point3d_ck(&p1, &pc))
        {
            lsp_error("  Failed test_matrix::apply_matrix3d_mp1 on translation matrix");
            return false;
        }
        else if (!point3d_ck(&p1, &p2))
        {
            lsp_error("  Failed test_matrix::apply_matrix3d_mp2 on translation matrix");
            return false;
        }

        // Apply rotation Z matrix
        sse::init_point_xyz(&p1, M_SQRT1_2, M_SQRT1_2, 0.0f);
        sse::init_point_xyz(&pc, 0.0f, 1.0f, 0.0f);
        sse::init_matrix3d_rotate_z(&m, M_PI * 0.25f);
        sse::apply_matrix3d_mp2(&p2, &p1, &m);
        sse::apply_matrix3d_mp1(&p1, &m);
        if (!point3d_ck(&p1, &pc))
        {
            lsp_error("  Failed test_matrix::apply_matrix3d_mp1 on rotate_z matrix");
            return false;
        }
        else if (!point3d_ck(&p1, &p2))
        {
            lsp_error("  Failed test_matrix::apply_matrix3d_mp2 on rotate_z matrix");
            return false;
        }

        // Apply rotation X matrix
        sse::init_point_xyz(&p1, 0.0f, M_SQRT1_2, M_SQRT1_2);
        sse::init_point_xyz(&pc, 0.0f, 0.0f, 1.0f);
        sse::init_matrix3d_rotate_x(&m, M_PI * 0.25f);
        sse::apply_matrix3d_mp2(&p2, &p1, &m);
        sse::apply_matrix3d_mp1(&p1, &m);
        if (!point3d_ck(&p1, &pc))
        {
            lsp_error("  Failed test_matrix::apply_matrix3d_mp1 on rotate_x matrix");
            return false;
        }
        else if (!point3d_ck(&p1, &p2))
        {
            lsp_error("  Failed test_matrix::apply_matrix3d_mp2 on rotate_x matrix");
            return false;
        }

        // Apply rotation Y matrix
        sse::init_point_xyz(&p1, M_SQRT1_2, 0.0f, M_SQRT1_2);
        sse::init_point_xyz(&pc, 1.0f, 0.0f, 0.0f);
        sse::init_matrix3d_rotate_y(&m, M_PI * 0.25f);
        sse::apply_matrix3d_mp2(&p2, &p1, &m);
        sse::apply_matrix3d_mp1(&p1, &m);
        if (!point3d_ck(&p1, &pc))
        {
            lsp_error("  Failed test_matrix::apply_matrix3d_mp1 on rotate_y matrix");
            return false;
        }
        else if (!point3d_ck(&p1, &p2))
        {
            lsp_error("  Failed test_matrix::apply_matrix3d_mp2 on rotate_y matrix");
            return false;
        }

        // Apply scale matrix
        sse::init_point_xyz(&p1, 1.0f, 1.0f, 1.0f);
        sse::init_point_xyz(&pc, 2.0f, 4.0f, 8.0f);
        sse::init_matrix3d_scale(&m, 2.0f, 4.0f, 8.0f);
        sse::apply_matrix3d_mp2(&p2, &p1, &m);
        sse::apply_matrix3d_mp1(&p1, &m);
        if (!point3d_ck(&p1, &pc))
        {
            lsp_error("  Failed test_matrix::apply_matrix3d_mp1 on scale matrix");
            return false;
        }
        else if (!point3d_ck(&p1, &p2))
        {
            lsp_error("  Failed test_matrix::apply_matrix3d_mp2 on scale matrix");
            return false;
        }

        // Transpose matrix
        matrix3d_t mt1, mt2;
        for (int i=0; i<16; ++i)
            m.m[i]      = i + 1;
        sse::init_matrix3d(&mt2, &m);
        sse::transpose_matrix3d2(&mt1, &m);
        sse::transpose_matrix3d1(&mt2);

        if (!matrix3d_ck(&mt1, &mt2))
        {
            lsp_error("  Failed test_matrix::transpose3d matrix");
            return false;
        }

        sse::transpose_matrix3d1(&mt1);
        if (!matrix3d_ck(&mt1, &m))
        {
            lsp_error("  Failed test_matrix::transpose3d1 matrix");
            return false;
        }

        return true;
    }

    bool test_triplet_native()
    {
        triangle3d_t t[3];
        dsp::calc_triangle3d_xyz(&t[0], 1.0f, 1.0f, 1.0f, 2.0f, 2.0f, 1.0f, -3.0f, 3.0f, 1.0f);
        dsp::calc_triangle3d_xyz(&t[1], 1.0f, 2.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 3.0f, -3.0f);
        dsp::calc_triangle3d_xyz(&t[2], 1.0f, 1.0f, 2.0f, 2.0f, 1.0f, 2.0f, 3.0f, 1.0f, -3.0f);

        vector3d_t n1, n2;
        dsp::init_vector_dxyz(&n1, 1.0f, 1.0f, 1.0f);
        dsp::init_vector_dxyz(&n2, -1.0f, -1.0f, -1.0f);

        for (size_t i=0; i<3; ++i)
        {
            if (native::check_triplet3d_t(&t[i]) < 0.0f)
            {
                lsp_error("  native::check_triplet3d_t");
                return false;
            }

            if (native::check_triplet3d_tn(&t[i], &n1) < 0.0f)
            {
                lsp_error("  native::check_triplet3d_t for 1st normal");
                return false;
            }
            else if (native::check_triplet3d_tn(&t[i], &n2) > 0.0f)
            {
                lsp_error("  native::check_triplet3d_tn for 2nd normal");
                return false;
            }

            if (native::check_triplet3d_p3n(&t[i].p[0], &t[i].p[1], &t[i].p[2], &n1) < 0.0f)
            {
                lsp_error("  native::check_triplet3d_p3n for 1st normal");
                return false;
            }
            else if (native::check_triplet3d_p3n(&t[i].p[0], &t[i].p[1], &t[i].p[2], &n2) > 0.0f)
            {
                lsp_error("  native::check_triplet3d_p3n for 2nd normal");
                return false;
            }

            if (native::check_triplet3d_pvn(&t[i].p[0], &n1) < 0.0f)
            {
                lsp_error("  native::check_triplet3d_pvn for 1st normal");
                return false;
            }
            else if (native::check_triplet3d_pvn(&t[i].p[0], &n2) > 0.0f)
            {
                lsp_error("  native::check_triplet3d_pvn for 2nd normal");
                return false;
            }
        }

        return true;
    }

    bool test_triplet_sse()
    {
        triangle3d_t t[3];
        dsp::calc_triangle3d_xyz(&t[0], 1.0f, 1.0f, 1.0f, 2.0f, 2.0f, 1.0f, -3.0f, 3.0f, 1.0f);
        dsp::calc_triangle3d_xyz(&t[1], 1.0f, 2.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 3.0f, -3.0f);
        dsp::calc_triangle3d_xyz(&t[2], 1.0f, 1.0f, 2.0f, 2.0f, 1.0f, 2.0f, 3.0f, 1.0f, -3.0f);

        vector3d_t n1, n2;
        dsp::init_vector_dxyz(&n1, 1.0f, 1.0f, 1.0f);
        dsp::init_vector_dxyz(&n2, -1.0f, -1.0f, -1.0f);

        float v1, v2;

        for (size_t i=0; i<3; ++i)
        {
            v1 = native::check_triplet3d_t(&t[i]);
            v2 = sse::check_triplet3d_t(&t[i]);

            if (!float_ck(v1, v2))
            {
                lsp_error("  sse::check_triplet3d_t");
                return false;
            }

            v1 = native::check_triplet3d_tn(&t[i], &n1);
            v2 = sse::check_triplet3d_tn(&t[i], &n1);

            if (!float_ck(v1, v2))
            {
                lsp_error("  sse::check_triplet3d_t for 1st normal");
                return false;
            }

            v1 = native::check_triplet3d_tn(&t[i], &n2);
            v2 = sse::check_triplet3d_tn(&t[i], &n2);

            if (!float_ck(v1, v2))
            {
                lsp_error("  sse::check_triplet3d_tn for 2nd normal");
                return false;
            }

            v1 = native::check_triplet3d_p3n(&t[i].p[0], &t[i].p[1], &t[i].p[2], &n1);
            v2 = sse::check_triplet3d_p3n(&t[i].p[0], &t[i].p[1], &t[i].p[2], &n1);

            if (!float_ck(v1, v2))
            {
                lsp_error("  sse::check_triplet3d_p3n for 1st normal");
                return false;
            }

            v1 = native::check_triplet3d_p3n(&t[i].p[0], &t[i].p[1], &t[i].p[2], &n2);
            v2 = sse::check_triplet3d_p3n(&t[i].p[0], &t[i].p[1], &t[i].p[2], &n2);

            if (!float_ck(v1, v2))
            {
                lsp_error("  sse::check_triplet3d_p3n for 2nd normal");
                return false;
            }

            v1 = native::check_triplet3d_pvn(t[i].p, &n1);
            v2 = sse::check_triplet3d_pvn(t[i].p, &n1);

            if (!float_ck(v1, v2))
            {
                lsp_error("  sse::check_triplet3d_pvn for 1st normal");
                return false;
            }

            v1 = native::check_triplet3d_pvn(t[i].p, &n2);
            v2 = sse::check_triplet3d_pvn(t[i].p, &n2);

            if (!float_ck(v1, v2))
            {
                lsp_error("  sse::check_triplet3d_pvn for 2nd normal");
                return false;
            }
        }

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

    bool test_complex_mod()
    {
        TEST_FOREACH(sz, 12, 13, 16, 17, 23, 0x1000)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                FBuffer src(sz*2, mask & 0x01);
                FBuffer dst1(sz, mask & 0x02);
                FBuffer dst2(sz, mask & 0x02);

                float *data = src.data();

                for (size_t i=0; i<sz; ++i)
                {
                    *(data++) = i + 1;
                    *(data++) = float(i + 1) / float(sz + 1);
                }

                native::packed_complex_mod(dst1, src, sz);
                sse::packed_complex_mod(dst2, src, sz);

                if (!dst1.compare(dst2))
                {
                    lsp_error("  Failed test size = %d, mask = 0x%x, overflow=%s",
                        int(sz), int(mask), (dst2.validate()) ? "false" : "true");
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

        LAUNCH(test_complex_mod)

        LAUNCH(test_unary_abs, native::abs1, sse::abs1)
        LAUNCH(test_binary_abs, native::abs2, sse::abs2)

        LAUNCH(test_move)
        LAUNCH(test_fill)
        LAUNCH(test_reverse1)
        LAUNCH(test_reverse2)

        LAUNCH(test_minmax, native::min, sse::min);
        LAUNCH(test_minmax, native::max, sse::max);
        LAUNCH(test_minmax, native::abs_min, sse::abs_min);
        LAUNCH(test_minmax, native::abs_max, sse::abs_max);
        LAUNCH(test_minmax, native::minmax, sse::minmax);
        LAUNCH(test_minmax, native::abs_minmax, sse::abs_minmax);

        LAUNCH(test_geometry_basic);
        LAUNCH(test_matrix_native);
        LAUNCH(test_matrix_sse);

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

