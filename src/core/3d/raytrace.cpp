/*
 * raytrace.cpp
 *
 *  Created on: 14 мая 2019 г.
 *      Author: sadko
 */

#include <core/3d/raytrace.h>

namespace lsp
{
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
        V3( 0.57735026919f, 0.942809041582f, 0.0f ),
        V3( -0.333333333333f, -0.471404520791f, 0.816496580928f ),
        V3( -0.333333333333f, -0.471404520791f, -0.816496580928f ),
        V3( 1.0f, 0.0f, 0.0f )
    };

    static const uint8_t tetra_faces[] =
    {
        // TODO
    };

    status_t gen_triangle_source(cstorage<rt_group_t> &out, const room_source_settings_t *cfg)
    {
        rt_group_t *g = out.append();
        if (g == NULL)
            return STATUS_NO_MEM;

        float a = 90 - 0.5f * cfg->angle;

        dsp::init_point_xyz(&g->s, -tanf(a * M_PI / 180.0f), 0.0f, 0.0f);
        dsp::init_point_xyz(&g->p[0], 0.0f, 0.0f, 1.0f);
        dsp::init_point_xyz(&g->p[1], 0.0f, -0.5f * M_SQRT2, -0.5f);
        dsp::init_point_xyz(&g->p[2], 0.0f, 0.5f * M_SQRT2, -0.5f);

        return STATUS_OK;
    }

    status_t gen_octa_source(cstorage<rt_group_t> &out, const room_source_settings_t *cfg)
    {
        size_t n            = sizeof(octa_faces) / (sizeof(uint8_t) * 3);
        rt_group_t *g       = out.append_n(n);
        if (g == NULL)
            return STATUS_NO_MEM;

        point3d_t sp;
        dsp::init_point_xyz(&sp, 0.0f, 0.0f, 0.0f);

        const uint8_t *vi   = ico_faces;
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
        }

        return STATUS_OK;
    }

    status_t gen_ico_source(cstorage<rt_group_t> &out, const room_source_settings_t *cfg)
    {
        size_t n            = sizeof(ico_faces) / (sizeof(uint8_t) * 3);
        rt_group_t *g       = out.append_n(n);
        if (g == NULL)
            return STATUS_NO_MEM;

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
        }

        return STATUS_OK;
    }

    status_t gen_box_source(cstorage<rt_group_t> &out, const room_source_settings_t *cfg)
    {
        size_t n            = sizeof(box_faces) / (sizeof(uint8_t) * 3);
        rt_group_t *g       = out.append_n(n);
        if (g == NULL)
            return STATUS_NO_MEM;

        point3d_t sp;
        dsp::init_point_xyz(&sp, 0.0f, 0.0f, 0.0f);

        const uint8_t *vi   = ico_faces;
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
        }

        return STATUS_OK;
    }

    status_t gen_tetra_source(cstorage<rt_group_t> &out, const room_source_settings_t *cfg)
    {
        size_t n            = sizeof(tetra_faces) / (sizeof(uint8_t) * 3);
        rt_group_t *g       = out.append_n(n);
        if (g == NULL)
            return STATUS_NO_MEM;

        point3d_t sp;
        dsp::init_point_xyz(&sp, 0.0f, 0.0f, 0.0f);

        const uint8_t *vi   = ico_faces;
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
        }

        return STATUS_OK;
    }

    status_t gen_cyl_source(cstorage<rt_group_t> &out, const room_source_settings_t *cfg)
    {
        return STATUS_OK;
    }

    status_t gen_cone_source(cstorage<rt_group_t> &out, const room_source_settings_t *cfg)
    {
        return STATUS_OK;
    }

    status_t gen_flat_spot_source(cstorage<rt_group_t> &out, const room_source_settings_t *cfg)
    {
        return STATUS_OK;
    }

    status_t gen_sphere_spot_source(cstorage<rt_group_t> &out, const room_source_settings_t *cfg)
    {
        return STATUS_OK;
    }

    status_t gen_cyl_spot_source(cstorage<rt_group_t> &out, const room_source_settings_t *cfg)
    {
        return STATUS_OK;
    }

    status_t gen_omni_source(cstorage<rt_group_t> &out, const room_source_settings_t *cfg)
    {
        return STATUS_OK;
    }

    status_t gen_omni2_source(cstorage<rt_group_t> &out, const room_source_settings_t *cfg)
    {
        return STATUS_OK;
    }

    status_t gen_source_mesh(cstorage<rt_group_t> &out, const room_source_settings_t *cfg)
    {
        out.clear();
        switch (cfg->type)
        {
            case RT_AS_ICOSPHERE:        return gen_omni_source(out, cfg);
            case RT_AS_OCTASPHERE:       return gen_omni2_source(out, cfg);
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
}


