/*
 * raytrace.cpp
 *
 *  Created on: 14 мая 2019 г.
 *      Author: sadko
 */

#include <core/3d/raytrace.h>

namespace lsp
{
    #define X_SQRT3         1.73205080757

    #define V3(x, y, z)     { x, y, z, 1.0f }

    //-------------------------------------------------------------------------
    // Icosahedron shape
    #define ICO_X       0.525731112119133606f
    #define ICO_N       0.0f
    #define ICO_Z       0.850650808352039932f

    static const point3d_t ico_vertex[] =
    {
        V3( -ICO_X,  ICO_N,  ICO_Z),
        V3(  ICO_X,  ICO_N,  ICO_Z),
        V3( -ICO_X,  ICO_N, -ICO_Z),
        V3(  ICO_X,  ICO_N, -ICO_Z),
        V3(  ICO_N,  ICO_Z,  ICO_X),
        V3(  ICO_N,  ICO_Z, -ICO_X),
        V3(  ICO_N, -ICO_Z,  ICO_X),
        V3(  ICO_N, -ICO_Z, -ICO_X),
        V3(  ICO_Z,  ICO_X,  ICO_N),
        V3( -ICO_Z,  ICO_X,  ICO_N),
        V3(  ICO_Z, -ICO_X,  ICO_N),
        V3( -ICO_Z, -ICO_X,  ICO_N)
    };

    static const uint8_t ico_faces[] =
    {
        0, 1, 4,
        0, 4, 9,
        9, 4, 5,
        4, 8, 5,

        4, 1, 8,
        8, 1, 10,
        8, 10, 3,
        5, 8, 3,

        5, 3, 2,
        2, 3, 7,
        7, 3, 10,
        7, 10, 6,

        7, 6, 11,
        11, 6, 0,
        0, 6, 1,
        6, 10, 1,

        9, 11, 0,
        9, 2, 11,
        9, 5, 2,
        7, 11, 2
    };

    //-------------------------------------------------------------------------
    // Octahedron shape
    static const point3d_t octa_vertex[] =
    {
        V3( 0.0f, 0.0f, 1.0f ),
        V3( 0.0f, 0.0f, -1.0f ),
        V3( 1.0f, 0.0f, 0.0f ),
        V3( 0.0f, 1.0f, 0.0f ),
        V3( -1.0f, 0.0f, 0.0f ),
        V3( 0.0f, -1.0f, 0.0f )
    };

    static const uint8_t octa_faces[] =
    {
        0, 2, 3,
        0, 3, 4,
        0, 4, 5,
        0, 5, 2,
        1, 3, 2,
        1, 4, 3,
        1, 5, 4,
        1, 2, 5
    };

    //-------------------------------------------------------------------------
    // Box shape
    static const point3d_t box_vertex[] =
    {
        V3(  1.0f,  1.0f,  1.0f ),
        V3( -1.0f,  1.0f,  1.0f ),
        V3( -1.0f, -1.0f,  1.0f ),
        V3(  1.0f, -1.0f,  1.0f ),

        V3(  1.0f,  1.0f, -1.0f ),
        V3( -1.0f,  1.0f, -1.0f ),
        V3( -1.0f, -1.0f, -1.0f ),
        V3(  1.0f, -1.0f, -1.0f ),
    };

    static const uint8_t box_faces[] =
    {
        0, 1, 2,
        0, 2, 3,
        0, 4, 5,
        0, 5, 1,
        1, 5, 6,
        1, 6, 2,
        0, 3, 7,
        0, 7, 4,
        3, 2, 6,
        3, 6, 7,
        5, 4, 7,
        5, 7, 6
    };

    //-------------------------------------------------------------------------
    // Tetrahedron shape
    static const point3d_t tetra_vertex[] =
    {
        V3( 0.942809041582f, 0.0f, -0.333333333333f ),
        V3( -0.471404520791, 0.816496580928f, -0.333333333333f ),
        V3( -0.471404520791, -0.816496580928f, -0.333333333333f ),
        V3( 0.0f, 0.0f, 1.0f )
    };

    static const uint8_t tetra_faces[] =
    {
        0, 1, 2,
        0, 2, 3,
        0, 3, 1,
        1, 2, 3
    };

    inline float apply_tangent(rt_group_t *g, float tg)
    {
        vector3d_t xn;
        dsp::calc_plane_pv(&xn, g->p);
        float d = g->s.x * xn.dx + g->s.y * xn.dy + g->s.z * xn.dz + xn.dw;
        xn.dw   = 0.0f;
        dsp::add_vector_pvk1(&g->s, &xn, d * (tg - 1.0f));
        return d;
    }

    status_t gen_triangle_source(cstorage<rt_group_t> &out, const rt_source_settings_t *cfg)
    {
        rt_group_t *g = out.append();
        if (g == NULL)
            return STATUS_NO_MEM;

        float tg    = tanf((5.0f + 0.8 * cfg->angle) * M_PI / 180.0f);
        float half  = 0.5f * cfg->size;

        dsp::init_point_xyz(&g->s, -cfg->size, 0.0f, 0.0f);
        dsp::init_point_xyz(&g->p[0], 0.0f, 0.0f, cfg->size);
        dsp::init_point_xyz(&g->p[1], 0.0f, -half * X_SQRT3, -half);
        dsp::init_point_xyz(&g->p[2], 0.0f, half * X_SQRT3, -half);
        apply_tangent(g, tg);

        return STATUS_OK;
    }

    status_t gen_octa_source(cstorage<rt_group_t> &out, const rt_source_settings_t *cfg)
    {
        size_t n            = sizeof(octa_faces) / (sizeof(uint8_t) * 3);
        rt_group_t *g       = out.append_n(n);
        if (g == NULL)
            return STATUS_NO_MEM;

        float tg    = tanf((5.0f + 0.8 * cfg->angle) * M_PI / 180.0f);

        point3d_t sp;
        dsp::init_point_xyz(&sp, 0.0f, 0.0f, 0.0f);

        const uint8_t *vi   = octa_faces;
        for (size_t i=0; i<n; ++i, vi += 3, ++g)
        {
            g->s    = sp;
            for (size_t j=0; j<3; ++j)
            {
                g->p[j]     = octa_vertex[vi[j]];
                g->p[j].x  *= cfg->size;
                g->p[j].y  *= cfg->size;
                g->p[j].z  *= cfg->size;
            }
            apply_tangent(g, tg);
        }

        return STATUS_OK;
    }

    status_t gen_ico_source(cstorage<rt_group_t> &out, const rt_source_settings_t *cfg)
    {
        size_t n            = sizeof(ico_faces) / (sizeof(uint8_t) * 3);
        rt_group_t *g       = out.append_n(n);
        if (g == NULL)
            return STATUS_NO_MEM;

        float tg    = tanf((5.0f + 0.8 * cfg->angle) * M_PI / 180.0f);

        point3d_t sp;
        dsp::init_point_xyz(&sp, 0.0f, 0.0f, 0.0f);

        const uint8_t *vi   = ico_faces;
        for (size_t i=0; i<n; ++i, vi += 3, ++g)
        {
            g->s    = sp;
            for (size_t j=0; j<3; ++j)
            {
                g->p[j]     = ico_vertex[vi[j]];
                g->p[j].x  *= cfg->size;
                g->p[j].y  *= cfg->size;
                g->p[j].z  *= cfg->size;
            }
            apply_tangent(g, tg);
        }

        return STATUS_OK;
    }

    status_t gen_box_source(cstorage<rt_group_t> &out, const rt_source_settings_t *cfg)
    {
        size_t n            = sizeof(box_faces) / (sizeof(uint8_t) * 3);
        rt_group_t *g       = out.append_n(n);
        if (g == NULL)
            return STATUS_NO_MEM;

        float tg    = tanf((5.0f + 0.8 * cfg->angle) * M_PI / 180.0f);

        point3d_t sp;
        dsp::init_point_xyz(&sp, 0.0f, 0.0f, 0.0f);

        const uint8_t *vi   = box_faces;
        for (size_t i=0; i<n; ++i, vi += 3, ++g)
        {
            g->s    = sp;
            for (size_t j=0; j<3; ++j)
            {
                g->p[j]     = box_vertex[vi[j]];
                g->p[j].x  *= cfg->size;
                g->p[j].y  *= cfg->size;
                g->p[j].z  *= cfg->size;
            }
            apply_tangent(g, tg);
        }

        return STATUS_OK;
    }

    status_t gen_tetra_source(cstorage<rt_group_t> &out, const rt_source_settings_t *cfg)
    {
        size_t n            = sizeof(tetra_faces) / (sizeof(uint8_t) * 3);
        rt_group_t *g       = out.append_n(n);
        if (g == NULL)
            return STATUS_NO_MEM;

        float tg    = tanf((5.0f + 0.8 * cfg->angle) * M_PI / 180.0f);

        point3d_t sp;
        dsp::init_point_xyz(&sp, 0.0f, 0.0f, 0.0f);

        const uint8_t *vi   = tetra_faces;
        for (size_t i=0; i<n; ++i, vi += 3, ++g)
        {
            g->s    = sp;
            for (size_t j=0; j<3; ++j)
            {
                g->p[j]     = tetra_vertex[vi[j]];
                g->p[j].x  *= cfg->size;
                g->p[j].y  *= cfg->size;
                g->p[j].z  *= cfg->size;
            }
            apply_tangent(g, tg);
        }

        return STATUS_OK;
    }

    status_t gen_cyl_source(cstorage<rt_group_t> &out, const rt_source_settings_t *cfg)
    {
        size_t n            = 16;
        rt_group_t *g       = out.append_n(n*2);
        if (g == NULL)
            return STATUS_NO_MEM;

        float tg    = tanf((5.0f + 0.8 * cfg->angle) * M_PI / 180.0f);

        point3d_t sp;
        dsp::init_point_xyz(&sp, 0.0f, 0.0f, 0.0f);
        float half          = cfg->height;
        float ka            = M_PI / n;
        float kb            = ka + ka;

        point3d_t pt[2], pb[2];
        float at[2], ab[2];

        for (size_t i=0; i<n; ++i, g += 2)
        {
            at[0]       = i * kb;
            at[1]       = at[0] + kb;
            ab[0]       = at[0] + ka;
            ab[1]       = at[1] + ka;

            dsp::init_point_xyz(&pt[0], cosf(at[0]) * cfg->size, sinf(at[0]) * cfg->size, half);
            dsp::init_point_xyz(&pt[1], cosf(at[1]) * cfg->size, sinf(at[1]) * cfg->size, half);
            dsp::init_point_xyz(&pb[0], cosf(ab[0]) * cfg->size, sinf(ab[0]) * cfg->size, -half);
            dsp::init_point_xyz(&pb[1], cosf(ab[1]) * cfg->size, sinf(ab[1]) * cfg->size, -half);

            g[0].s      = sp;
            g[0].p[0]   = pt[0];
            g[0].p[1]   = pb[0];
            g[0].p[2]   = pt[1];

            g[1].s      = sp;
            g[1].p[0]   = pb[0];
            g[1].p[1]   = pb[1];
            g[1].p[2]   = pt[1];

            apply_tangent(&g[0], tg);
            apply_tangent(&g[1], tg);
        }

        return STATUS_OK;
    }

    status_t gen_cone_source(cstorage<rt_group_t> &out, const rt_source_settings_t *cfg)
    {
        size_t n            = 16;
        rt_group_t *g       = out.append_n(n);
        if (g == NULL)
            return STATUS_NO_MEM;

        float tg    = tanf((5.0f + 0.8 * cfg->angle) * M_PI / 180.0f);

        point3d_t sp, tp;
        dsp::init_point_xyz(&sp, 0.0f, 0.0f, 0.0f);
        float ka            = 2.0f * M_PI / n;

        dsp::init_point_xyz(&tp, cfg->height, 0.0f, 0.0f);

        for (size_t i=0; i<n; ++i, ++g)
        {
            float a1    = i * ka;
            float a2    = a1 + ka;

            g[0].s      = sp;
            g->p[0]     = tp;
            dsp::init_point_xyz(&g->p[1], 0.0f, cosf(a2) * cfg->size, sinf(a2) * cfg->size);
            dsp::init_point_xyz(&g->p[2], 0.0f, cosf(a1) * cfg->size, sinf(a1) * cfg->size);

            apply_tangent(g, tg);
        }

        return STATUS_OK;
    }

    status_t gen_flat_spot_source(cstorage<rt_group_t> &out, const rt_source_settings_t *cfg)
    {
        size_t n            = 16;
        rt_group_t *g       = out.append_n(n);
        if (g == NULL)
            return STATUS_NO_MEM;

        float tg    = tanf((5.0f + 0.8 * cfg->angle) * M_PI / 180.0f);

        point3d_t sp, tp;
        dsp::init_point_xyz(&sp, -cfg->size, 0.0f, 0.0f);
        float ka            = 2.0f * M_PI / n;

        dsp::init_point_xyz(&tp, 0.0f, 0.0f, 0.0f);

        for (size_t i=0; i<n; ++i, ++g)
        {
            float a1    = i * ka;
            float a2    = a1 + ka;

            g[0].s      = sp;
            g->p[0]     = tp;
            dsp::init_point_xyz(&g->p[1], 0.0f, cosf(a2) * cfg->size, sinf(a2) * cfg->size);
            dsp::init_point_xyz(&g->p[2], 0.0f, cosf(a1) * cfg->size, sinf(a1) * cfg->size);

            apply_tangent(g, tg);
        }

        return STATUS_OK;
    }

    status_t gen_sphere_spot_source(cstorage<rt_group_t> &out, const rt_source_settings_t *cfg)
    {
        size_t n = 16, r = 4;
        rt_group_t *g       = out.append_n(n * r * 2);
        if (g == NULL)
            return STATUS_NO_MEM;

        float tg    = tanf((5.0f + 0.8 * cfg->angle) * M_PI / 180.0f);
        float intr  = cfg->size * (1 + tanf((100.0f - cfg->curvature) * 0.85f * M_PI / 180.0f)); // internal radius
        float sa    = acosf(cfg->size / intr);  // start angle
        float step  = (M_PI_2 - sa) / r;

        point3d_t sp;
        dsp::init_point_xyz(&sp, -intr * sinf(sa), 0.0f, 0.0f);

        float ka            = M_PI / n;
        float kb            = ka + ka;
        float off           = 0.0f;

        point3d_t pt[2], pb[2];
        float at[2], ab[2];

        for (size_t i=0; i<r; ++i)
        {
            float a1    = sa + i * step;
            float a2    = a1 + step;

            float r1    = cosf(a1) * intr;
            float r2    = cosf(a2) * intr;
            float x1    = sp.x + sinf(a1) * intr;
            float x2    = sp.x + sinf(a2) * intr;

            for (size_t j=0; j<n; ++j, g += 2)
            {
                at[0]       = off + j * kb;
                at[1]       = at[0] + kb;
                ab[0]       = at[0] + ka;
                ab[1]       = at[1] + ka;

                dsp::init_point_xyz(&pt[0], x1, cosf(at[0]) * r1, sinf(at[0]) * r1);
                dsp::init_point_xyz(&pt[1], x1, cosf(at[1]) * r1, sinf(at[1]) * r1);
                dsp::init_point_xyz(&pb[0], x2, cosf(ab[0]) * r2, sinf(ab[0]) * r2);
                dsp::init_point_xyz(&pb[1], x2, cosf(ab[1]) * r2, sinf(ab[1]) * r2);

                g[0].s      = sp;
                g[0].p[0]   = pt[0];
                g[0].p[1]   = pb[0];
                g[0].p[2]   = pt[1];

                g[1].s      = sp;
                g[1].p[0]   = pb[0];
                g[1].p[1]   = pb[1];
                g[1].p[2]   = pt[1];

                apply_tangent(&g[0], tg);
                apply_tangent(&g[1], tg);
            }

            off    += ka;
        }

        return STATUS_OK;
    }

    status_t gen_cyl_spot_source(cstorage<rt_group_t> &out, const rt_source_settings_t *cfg)
    {
        size_t n            = 8;
        rt_group_t *g       = out.append_n(n*2 + 1);
        if (g == NULL)
            return STATUS_NO_MEM;

        float tg    = tanf((5.0f + 0.8 * cfg->angle) * M_PI / 180.0f);
        float intr  = cfg->size * (1 + tanf((100.0f - cfg->curvature) * 0.85f * M_PI / 180.0f)); // internal radius

        float sa    = acosf(cfg->size / intr);  // start angle
        float ea    = M_PI - sa;
        float kb    = (ea - sa) / n;
        float ka    = 0.5f * kb;

        point3d_t sp;
        dsp::init_point_xyz(&sp, -intr * sinf(sa), 0.0f, 0.0f);

        point3d_t pt[2], pb[2];
        float at[2], ab[2];

        for (size_t i=0; i<n; ++i, g += 2)
        {
            at[0]       = sa + i*kb;
            at[1]       = at[0] + kb;
            ab[0]       = at[0] - ka;
            ab[1]       = at[1] - ka;

            if (ab[0] < sa)
                ab[0] = sa;

            dsp::init_point_xyz(&pt[0], sp.x + sinf(at[0]) * intr, cosf(at[0]) * intr, cfg->height);
            dsp::init_point_xyz(&pt[1], sp.x + sinf(at[1]) * intr, cosf(at[1]) * intr, cfg->height);
            dsp::init_point_xyz(&pb[0], sp.x + sinf(ab[0]) * intr, cosf(ab[0]) * intr, -cfg->height);
            dsp::init_point_xyz(&pb[1], sp.x + sinf(ab[1]) * intr, cosf(ab[1]) * intr, -cfg->height);

            g[0].s      = sp;
            g[0].p[0]   = pt[0];
            g[0].p[1]   = pb[1];
            g[0].p[2]   = pb[0];

            g[1].s      = sp;
            g[1].p[0]   = pt[0];
            g[1].p[1]   = pt[1];
            g[1].p[2]   = pb[1];

            apply_tangent(&g[0], tg);
            apply_tangent(&g[1], tg);
        }

        // Add last element
        dsp::init_point_xyz(&pb[0], sp.x + sinf(ea) * intr, cosf(ea) * intr, -cfg->height);

        g[0].s      = sp;
        g[0].p[0]   = pt[1];
        g[0].p[1]   = pb[0];
        g[0].p[2]   = pb[1];

        apply_tangent(&g[0], tg);

        return STATUS_OK;
    }

    status_t gen_icosphere_source(cstorage<rt_group_t> &out, const rt_source_settings_t *cfg)
    {
        size_t n            = sizeof(ico_faces) / (sizeof(uint8_t) * 3);
        rt_group_t *g       = out.append_n(n * 4);
        if (g == NULL)
            return STATUS_NO_MEM;

        float tg    = tanf((5.0f + 0.8 * cfg->angle) * M_PI / 180.0f);

        point3d_t sp;
        dsp::init_point_xyz(&sp, 0.0f, 0.0f, 0.0f);

        const uint8_t *vi   = ico_faces;
        point3d_t p[3], xp[3];

        for (size_t i=0; i<n; ++i, vi += 3, g += 4)
        {
            xp[0]   = ico_vertex[vi[0]];
            xp[1]   = ico_vertex[vi[1]];
            xp[2]   = ico_vertex[vi[2]];

            p[0].x  = (xp[0].x + xp[1].x) * 0.5f;
            p[0].y  = (xp[0].y + xp[1].y) * 0.5f;
            p[0].z  = (xp[0].z + xp[1].z) * 0.5f;
            p[0].w  = 1.0f;

            p[1].x  = (xp[1].x + xp[2].x) * 0.5f;
            p[1].y  = (xp[1].y + xp[2].y) * 0.5f;
            p[1].z  = (xp[1].z + xp[2].z) * 0.5f;
            p[1].w  = 1.0f;

            p[2].x  = (xp[2].x + xp[0].x) * 0.5f;
            p[2].y  = (xp[2].y + xp[0].y) * 0.5f;
            p[2].z  = (xp[2].z + xp[0].z) * 0.5f;
            p[2].w  = 1.0f;

            g[0].s  = sp;
            g[1].s  = sp;
            g[2].s  = sp;
            g[3].s  = sp;

            dsp::scale_point2(&g[0].p[0], &xp[0], cfg->size);
            dsp::scale_point2(&g[0].p[1], &p[0], cfg->size);
            dsp::scale_point2(&g[0].p[2], &p[2], cfg->size);

            dsp::scale_point2(&g[1].p[0], &xp[1], cfg->size);
            dsp::scale_point2(&g[1].p[1], &p[1], cfg->size);
            g[1].p[2]   = g[0].p[1];

            dsp::scale_point2(&g[2].p[0], &xp[2], cfg->size);
            g[2].p[1]   = g[0].p[2];
            g[2].p[2]   = g[1].p[1];

            g[3].p[0]   = g[0].p[1];
            g[3].p[1]   = g[1].p[1];
            g[3].p[2]   = g[2].p[1];

            apply_tangent(&g[0], tg);
            apply_tangent(&g[1], tg);
            apply_tangent(&g[2], tg);
            apply_tangent(&g[3], tg);
        }

        return STATUS_OK;
    }

    status_t gen_octasphere_source(cstorage<rt_group_t> &out, const rt_source_settings_t *cfg)
    {
        size_t n            = sizeof(octa_faces) / (sizeof(uint8_t) * 3);
        rt_group_t *g       = out.append_n(n * 4);
        if (g == NULL)
            return STATUS_NO_MEM;

        float tg    = tanf((5.0f + 0.8 * cfg->angle) * M_PI / 180.0f);

        point3d_t sp;
        dsp::init_point_xyz(&sp, 0.0f, 0.0f, 0.0f);

        const uint8_t *vi   = octa_faces;
        point3d_t p[3], xp[3];

        for (size_t i=0; i<n; ++i, vi += 3, g += 4)
        {
            xp[0]   = octa_vertex[vi[0]];
            xp[1]   = octa_vertex[vi[1]];
            xp[2]   = octa_vertex[vi[2]];

            p[0].x  = (xp[0].x + xp[1].x) * 0.5f;
            p[0].y  = (xp[0].y + xp[1].y) * 0.5f;
            p[0].z  = (xp[0].z + xp[1].z) * 0.5f;
            p[0].w  = 1.0f;

            p[1].x  = (xp[1].x + xp[2].x) * 0.5f;
            p[1].y  = (xp[1].y + xp[2].y) * 0.5f;
            p[1].z  = (xp[1].z + xp[2].z) * 0.5f;
            p[1].w  = 1.0f;

            p[2].x  = (xp[2].x + xp[0].x) * 0.5f;
            p[2].y  = (xp[2].y + xp[0].y) * 0.5f;
            p[2].z  = (xp[2].z + xp[0].z) * 0.5f;
            p[2].w  = 1.0f;

            g[0].s  = sp;
            g[1].s  = sp;
            g[2].s  = sp;
            g[3].s  = sp;

            dsp::scale_point2(&g[0].p[0], &xp[0], cfg->size);
            dsp::scale_point2(&g[0].p[1], &p[0], cfg->size);
            dsp::scale_point2(&g[0].p[2], &p[2], cfg->size);

            dsp::scale_point2(&g[1].p[0], &xp[1], cfg->size);
            dsp::scale_point2(&g[1].p[1], &p[1], cfg->size);
            g[1].p[2]   = g[0].p[1];

            dsp::scale_point2(&g[2].p[0], &xp[2], cfg->size);
            g[2].p[1]   = g[0].p[2];
            g[2].p[2]   = g[1].p[1];

            g[3].p[0]   = g[0].p[1];
            g[3].p[1]   = g[1].p[1];
            g[3].p[2]   = g[2].p[1];

            apply_tangent(&g[0], tg);
            apply_tangent(&g[1], tg);
            apply_tangent(&g[2], tg);
            apply_tangent(&g[3], tg);
        }

        return STATUS_OK;
    }

    status_t rt_gen_source_mesh(cstorage<rt_group_t> &out, const rt_source_settings_t *cfg)
    {
        out.clear();
        switch (cfg->type)
        {
            case RT_AS_ICOSPHERE:   return gen_icosphere_source(out, cfg);
            case RT_AS_OCTASPHERE:  return gen_octasphere_source(out, cfg);
            case RT_AS_ICO:         return gen_ico_source(out, cfg);
            case RT_AS_OCTA:        return gen_octa_source(out, cfg);
            case RT_AS_TETRA:       return gen_tetra_source(out, cfg);
            case RT_AS_BOX:         return gen_box_source(out, cfg);
            case RT_AS_CYLINDER:    return gen_cyl_source(out, cfg);
            case RT_AS_CONE:        return gen_cone_source(out, cfg);
            case RT_AS_FSPOT:       return gen_flat_spot_source(out, cfg);
            case RT_AS_SSPOT:       return gen_sphere_spot_source(out, cfg);
            case RT_AS_CSPOT:       return gen_cyl_spot_source(out, cfg);
            case RT_AS_TRIANGLE:    return gen_triangle_source(out, cfg);
        }
        return STATUS_BAD_ARGUMENTS;
    }

    status_t rt_gen_capture_mesh(cstorage<raw_triangle_t> &out, const rt_capture_settings_t *cfg)
    {
        out.clear();

        size_t n            = sizeof(ico_faces) / (sizeof(uint8_t) * 3);
        raw_triangle_t *g   = out.append_n(n * 4);
        if (g == NULL)
            return STATUS_NO_MEM;

        point3d_t sp;
        dsp::init_point_xyz(&sp, 0.0f, 0.0f, 0.0f);

        const uint8_t *vi   = ico_faces;
        point3d_t p[3], xp[3];

        for (size_t i=0; i<n; ++i, vi += 3, g += 4)
        {
            xp[0]   = ico_vertex[vi[0]];
            xp[1]   = ico_vertex[vi[1]];
            xp[2]   = ico_vertex[vi[2]];

            p[0].x  = (xp[0].x + xp[1].x) * 0.5f;
            p[0].y  = (xp[0].y + xp[1].y) * 0.5f;
            p[0].z  = (xp[0].z + xp[1].z) * 0.5f;
            p[0].w  = 1.0f;

            p[1].x  = (xp[1].x + xp[2].x) * 0.5f;
            p[1].y  = (xp[1].y + xp[2].y) * 0.5f;
            p[1].z  = (xp[1].z + xp[2].z) * 0.5f;
            p[1].w  = 1.0f;

            p[2].x  = (xp[2].x + xp[0].x) * 0.5f;
            p[2].y  = (xp[2].y + xp[0].y) * 0.5f;
            p[2].z  = (xp[2].z + xp[0].z) * 0.5f;
            p[2].w  = 1.0f;

            dsp::scale_point2(&g[0].v[0], &xp[0], cfg->radius);
            dsp::scale_point2(&g[0].v[1], &p[0], cfg->radius);
            dsp::scale_point2(&g[0].v[2], &p[2], cfg->radius);

            dsp::scale_point2(&g[1].v[0], &xp[1], cfg->radius);
            dsp::scale_point2(&g[1].v[1], &p[1], cfg->radius);
            g[1].v[2]   = g[0].v[1];

            dsp::scale_point2(&g[2].v[0], &xp[2], cfg->radius);
            g[2].v[1]   = g[0].v[2];
            g[2].v[2]   = g[1].v[1];

            g[3].v[0]   = g[0].v[1];
            g[3].v[1]   = g[1].v[1];
            g[3].v[2]   = g[2].v[1];
        }

        return STATUS_OK;
    }

    status_t rt_configure_source(rt_source_settings_t *out, const room_source_config_t *in)
    {
        matrix3d_t delta, m;

        // Compute position
        dsp::init_matrix3d_translate_p1(&delta, &in->sPos);

        dsp::init_matrix3d_rotate_z(&m, in->fYaw * M_PI / 180.0f);
        dsp::apply_matrix3d_mm1(&delta, &m);

        dsp::init_matrix3d_rotate_y(&m, in->fPitch * M_PI / 180.0f);
        dsp::apply_matrix3d_mm1(&delta, &m);

        dsp::init_matrix3d_rotate_x(&m, in->fRoll * M_PI / 180.0f);
        dsp::apply_matrix3d_mm1(&delta, &m);

        // Store parameters
        out->pos        = delta;
        out->type       = in->enType;
        out->size       = in->fSize;
        out->height     = in->fHeight;
        out->angle      = in->fAngle;
        out->curvature  = in->fCurvature;
        out->amplitude  = in->fAmplitude;

        return STATUS_OK;
    }

    status_t rt_configure_capture(size_t *n, rt_capture_settings_t *out, const room_capture_config_t *cfg)
    {
        matrix3d_t m, delta;
        float a[2];
        vector3d_t dp[2];

//        // Compute parameters of capture in point(0, 0, 0)
//        for (size_t i=0; i<2; ++i)
//        {
//            dsp::init_point_xyz(&out->pos[i].z, 0.0f, 0.0f, 0.0f);
//            dsp::init_vector_dxyz(&out->pos[i].v, 1.0f, 0.0f, 0.0f);
//        }
        dsp::init_vector_dxyz(&dp[0], 0.0f, 0.0f, 0.0f);
        dsp::init_vector_dxyz(&dp[1], 0.0f, 0.0f, 0.0f);

//        out->r[0]       = in->fCapsule * 0.01f; // cm -> m
//        out->r[1]       = in->fCapsule * 0.01f; // cm -> m
//        out->type[0]    = in->enDirection;
//        out->type[1]    = in->enDirection;
        float r         = cfg->fCapsule * 0.01f;

        switch (cfg->sConfig)
        {
            case RT_CC_MONO:
                *n                  = 1;
                a[0]                = 0.0f;
                a[1]                = 0.0f;
                out[0].type         = cfg->enDirection;
                out[1].type         = cfg->enDirection;
                break;
            case RT_CC_XY:
                *n                  = 2;
                dp[0].dy           -= r;
                dp[1].dy           += r;
                a[0]                = 45.0f + (cfg->fAngle - 90.0f) * 0.5f;
                a[1]                = -45.0f - (cfg->fAngle - 90.0f) * 0.5f;

                out[0].type         = cfg->enDirection;
                out[1].type         = cfg->enDirection;
                break;
            case RT_CC_AB:
                *n                  = 2;
                dp[0].dy           += cfg->fDistance * 0.5f;
                dp[1].dy           -= cfg->fDistance * 0.5f;
                a[0]                = 0.0f;
                a[1]                = 0.0f;

                out[0].type         = cfg->enDirection;
                out[1].type         = cfg->enDirection;
                break;
            case RT_CC_ORTF:
                *n                  = 2;
                dp[0].dy           += 0.075f;  // Half of human's head width
                dp[1].dy           -= 0.075f;  // Half of human's head width
                a[0]                =  45.0f + (cfg->fAngle - 90.0f) * 0.5f;  // -45 + (a - 90) * 45 / 90
                a[1]                = -45.0f - (cfg->fAngle - 90.0f) * 0.5f; // -45 - (a - 90) * 45 / 90

                out[0].type         = cfg->enDirection;
                out[1].type         = cfg->enDirection;
                break;
            case RT_CC_MS:
                *n                  = 2;
                dp[0].dz           += r;
                dp[1].dz           -= r;
                a[0]                = 0.0f;
                a[1]                = 90.0f;

                out[0].type         = cfg->enDirection;
                out[1].type         = cfg->enSide;
                break;

            default:
                return STATUS_BAD_ARGUMENTS;
        }

        out[0].radius       = r;
        out[1].radius       = r;

        // Compute rotation matrix
        dsp::init_matrix3d_translate_p1(&delta, &cfg->sPos);

        dsp::init_matrix3d_rotate_z(&m, cfg->fYaw * M_PI / 180.0f);
        dsp::apply_matrix3d_mm1(&delta, &m);

        dsp::init_matrix3d_rotate_y(&m, cfg->fPitch * M_PI / 180.0f);
        dsp::apply_matrix3d_mm1(&delta, &m);

        dsp::init_matrix3d_rotate_x(&m, cfg->fRoll * M_PI / 180.0f);
        dsp::apply_matrix3d_mm1(&delta, &m);

        // Compute initial matrices
        for (size_t i=0; i<2; ++i)
        {
            out[i].pos  = delta;

            dsp::init_matrix3d_translate(&m, dp[i].dx, dp[i].dy, dp[i].dz);
            dsp::apply_matrix3d_mm1(&out[i].pos, &m);

            dsp::init_matrix3d_rotate_z(&m, a[i] * M_PI / 180.0f);
            dsp::apply_matrix3d_mm1(&out[i].pos, &m);
        }

        return STATUS_OK;
    }
}


