/*
 * plane.cpp
 *
 *  Created on: 29 мар. 2019 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <testing/utest/dsp/3d/helpers.h>

namespace native
{
    float calc_plane_p3(vector3d_t *v, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2);
    float calc_plane_pv(vector3d_t *v, const point3d_t *pv);
    float calc_plane_v1p2(vector3d_t *v, const vector3d_t *v0, const point3d_t *p0, const point3d_t *p1);
}

IF_ARCH_X86(
    namespace sse
    {
        float calc_plane_p3(vector3d_t *v, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2);
        float calc_plane_pv(vector3d_t *v, const point3d_t *pv);
        float calc_plane_v1p2(vector3d_t *v, const vector3d_t *v0, const point3d_t *p0, const point3d_t *p1);
    }
)

typedef float (* calc_plane_p3_t)(vector3d_t *v, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2);
typedef float (* calc_plane_pv_t)(vector3d_t *v, const point3d_t *pv);
typedef float (* calc_plane_v1p2_t)(vector3d_t *v, const vector3d_t *v0, const point3d_t *p0, const point3d_t *p1);

UTEST_BEGIN("dsp.3d", plane)

    void call(const char *label, calc_plane_p3_t f)
    {
        if (!UTEST_SUPPORTED(f))
            return;

        printf("Testing %s...\n", label);

        point3d_t pv[3];
        vector3d_t v1, v2;

        for (size_t i=0; i<0x200; ++i)
        {
            // Intialize points
            for (size_t j=0; j<3; ++j)
                dsp::init_point_xyz(&pv[j], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));

//            pv[0] = { 6.411041e+00, 5.610571e+00, 3.340876e+00, 1.000000e+00 };
//            pv[1] = { 2.980657e+00, 5.403591e+00, -8.846043e+00, 1.000000e+00 };
//            pv[2] = { 8.912350e+00, 4.731712e+00, 2.927614e+00, 1.000000e+00 };

            // Compute the value
            float w1  = native::calc_plane_p3(&v1, &pv[0], &pv[1], &pv[2]);
            float w2  = f(&v2, &pv[0], &pv[1], &pv[2]);

            if ((!float_equals_adaptive(w1, w2, DSP_3D_TOLERANCE)) ||
                (!vector3d_ack(&v1, &v2)))
            {
                dump_point("pv[0]", &pv[0]);
                dump_point("pv[1]", &pv[1]);
                dump_point("pv[2]", &pv[2]);
                dump_vector("v[0]", &v1);
                dump_vector("v[1]", &v2);
                printf("w[0] = %e, w[1] = %e\n", w1, w2);
                UTEST_FAIL_MSG("result of functions differ");
            }
        }
    }

    void call(const char *label, calc_plane_pv_t f)
    {
        if (!UTEST_SUPPORTED(f))
            return;

        printf("Testing %s...\n", label);

        point3d_t pv[3];
        vector3d_t v1, v2;

        for (size_t i=0; i<0x200; ++i)
        {
            // Intialize points
            for (size_t j=0; j<3; ++j)
                dsp::init_point_xyz(&pv[j], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));

    //            pv[0] = { 6.411041e+00, 5.610571e+00, 3.340876e+00, 1.000000e+00 };
    //            pv[1] = { 2.980657e+00, 5.403591e+00, -8.846043e+00, 1.000000e+00 };
    //            pv[2] = { 8.912350e+00, 4.731712e+00, 2.927614e+00, 1.000000e+00 };

            // Compute the value
            float w1  = native::calc_plane_pv(&v1, pv);
            float w2  = f(&v2, pv);

            if ((!float_equals_adaptive(w1, w2, DSP_3D_TOLERANCE)) ||
                (!vector3d_ack(&v1, &v2)))
            {
                dump_point("pv[0]", &pv[0]);
                dump_point("pv[1]", &pv[1]);
                dump_point("pv[2]", &pv[2]);
                dump_vector("v[0]", &v1);
                dump_vector("v[1]", &v2);
                printf("w[0] = %e, w[1] = %e\n", w1, w2);
                UTEST_FAIL_MSG("result of functions differ");
            }
        }
    }

    void call(const char *label, calc_plane_v1p2_t f)
    {
        if (!UTEST_SUPPORTED(f))
            return;

        printf("Testing %s...\n", label);

        point3d_t pv[2];
        vector3d_t sv;
        vector3d_t v1, v2;

        for (size_t i=0; i<0x200; ++i)
        {
            // Intialize points
            dsp::init_vector_dxyz(&sv, randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));
            dsp::init_point_xyz(&pv[0], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));
            dsp::init_point_xyz(&pv[1], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));

            // Compute the value
            float w1  = native::calc_plane_v1p2(&v1, &sv, &pv[0], &pv[1]);
            float w2  = f(&v2, &sv, &pv[0], &pv[1]);

            if ((!float_equals_adaptive(w1, w2, DSP_3D_TOLERANCE)) ||
                (!vector3d_ack(&v1, &v2)))
            {
                dump_point("pv[0]", &pv[0]);
                dump_point("pv[1]", &pv[1]);
                dump_point("pv[2]", &pv[2]);
                dump_vector("v[0]", &v1);
                dump_vector("v[1]", &v2);
                printf("w[0] = %e, w[1] = %e\n", w1, w2);
                UTEST_FAIL_MSG("result of functions differ");
            }
        }
    }

    UTEST_MAIN
    {
        IF_ARCH_X86(call("sse::calc_plane_p3", sse::calc_plane_p3));
        IF_ARCH_X86(call("sse::calc_plane_pv", sse::calc_plane_pv));
        IF_ARCH_X86(call("sse::calc_plane_v1p2", sse::calc_plane_v1p2));
    }
UTEST_END;


