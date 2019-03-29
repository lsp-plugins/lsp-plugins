/*
 * split_point.cpp
 *
 *  Created on: 29 мар. 2019 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <test/utest/dsp/3d/helpers.h>

namespace native
{
    void calc_split_point_p2v1(point3d_t *sp, const point3d_t *l0, const point3d_t *l1, const vector3d_t *pl);
    void calc_split_point_pvv1(point3d_t *sp, const point3d_t *lv, const vector3d_t *pl);
}

IF_ARCH_X86(
    namespace sse
    {
        void calc_split_point_p2v1(point3d_t *sp, const point3d_t *l0, const point3d_t *l1, const vector3d_t *pl);
        void calc_split_point_pvv1(point3d_t *sp, const point3d_t *lv, const vector3d_t *pl);
    }
)

typedef void (* calc_split_point_p2v1_t)(point3d_t *sp, const point3d_t *l0, const point3d_t *l1, const vector3d_t *pl);
typedef void (* calc_split_point_pvv1_t)(point3d_t *sp, const point3d_t *lv, const vector3d_t *pl);

UTEST_BEGIN("dsp.3d", split_point)

    void call(const char *label, calc_split_point_p2v1_t fn)
    {
        if (!UTEST_SUPPORTED(fn))
            return;

        printf("Testing %s...\n", label);

        point3d_t pv[2], sp[2];
        vector3d_t pl;

        for (size_t i=0; i<0x200; )
        {
            // Intialize split plane
            if ((i&0x3f) == 0)
            {
                dsp::init_vector_dxyz(&pl, randf(-1.0f, 1.0f), randf(-1.0f, 1.0f), randf(-1.0f, 1.0f));
                float w = sqrtf(pl.dx * pl.dx + pl.dy * pl.dy + pl.dz * pl.dz);
                if (w <= DSP_3D_TOLERANCE)
                    continue;
                pl.dx      /= w;
                pl.dy      /= w;
                pl.dz      /= w;
                pl.dw       = randf(-1.0f, 1.0f);

                ++i;
                continue;
            }

//            pl = { -2.905356e-01, -7.746597e-01, 5.616862e-01, 9.420407e-02 };
//            pv[0] = { -7.260440e+00, 2.809329e+00, -8.186333e+00, 1.000000e+00 };
//            pv[1] = { 8.526232e+00, -8.515192e+00, -8.606664e+00, 1.000000e+00 };

            // Initialize points
            dsp::init_point_xyz(&pv[0], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));
            dsp::init_point_xyz(&pv[1], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));
            float k1 = pv[0].x*pl.dx + pv[0].y*pl.dy + pv[0].z*pl.dz + pl.dw;
            float k2 = pv[0].x*pl.dx + pv[0].y*pl.dy + pv[0].z*pl.dz + pl.dw;
            if ((k1 * k2) <= DSP_3D_TOLERANCE) // Points should lay on different sides of a plane
                continue;

            // Compute split point
            ++i;
            native::calc_split_point_p2v1(&sp[0], &pv[0], &pv[1], &pl);
            dsp::calc_split_point_p2v1(&sp[1], &pv[0], &pv[1], &pl);

            // Compare
            if (!point3d_ack(&sp[0], &sp[1], 1e-4))
            {
                dump_vector("pl", &pl);
                dump_point("p[0]", &pv[0]);
                dump_point("p[1]", &pv[1]);
                dump_point("sp[0]", &sp[0]);
                dump_point("sp[1]", &sp[1]);
                UTEST_FAIL_MSG("Failed %s, function result differs", label);
            }
        }
    }

    void call(const char *label, calc_split_point_pvv1_t fn)
    {
        if (!UTEST_SUPPORTED(fn))
            return;

        printf("Testing %s...\n", label);

        point3d_t pv[2], sp[2];
        vector3d_t pl;

        for (size_t i=0; i<0x200; )
        {
            // Intialize split plane
            if ((i&0x3f) == 0)
            {
                dsp::init_vector_dxyz(&pl, randf(-1.0f, 1.0f), randf(-1.0f, 1.0f), randf(-1.0f, 1.0f));
                float w = sqrtf(pl.dx * pl.dx + pl.dy * pl.dy + pl.dz * pl.dz);
                if (w <= DSP_3D_TOLERANCE)
                    continue;
                pl.dx      /= w;
                pl.dy      /= w;
                pl.dz      /= w;
                pl.dw       = randf(-1.0f, 1.0f);

                ++i;
                continue;
            }

            // Initialize points
            dsp::init_point_xyz(&pv[0], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));
            dsp::init_point_xyz(&pv[1], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));
            float k1 = pv[0].x*pl.dx + pv[0].y*pl.dy + pv[0].z*pl.dz + pl.dw;
            float k2 = pv[0].x*pl.dx + pv[0].y*pl.dy + pv[0].z*pl.dz + pl.dw;
            if ((k1 * k2) <= DSP_3D_TOLERANCE) // Points should lay on different sides of a plane
                continue;

            // Compute split point
            ++i;
            native::calc_split_point_pvv1(&sp[0], pv, &pl);
            dsp::calc_split_point_pvv1(&sp[1], pv, &pl);

            // Compare
            if (!point3d_ack(&sp[0], &sp[1], 1e-4))
            {
                dump_vector("pl", &pl);
                dump_point("p[0]", &pv[0]);
                dump_point("p[1]", &pv[1]);
                dump_point("sp[0]", &sp[0]);
                dump_point("sp[1]", &sp[1]);
                UTEST_FAIL_MSG("Failed %s, function result differs", label);
            }
        }
    }

    UTEST_MAIN
    {
        IF_ARCH_X86(call("sse::calc_split_point_p2v1", sse::calc_split_point_p2v1));
        IF_ARCH_X86(call("sse::calc_split_point_pvv1", sse::calc_split_point_pvv1));
    }
UTEST_END;


