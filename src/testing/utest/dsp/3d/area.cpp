/*
 * area.cpp
 *
 *  Created on: 29 мар. 2019 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <testing/utest/dsp/3d/helpers.h>

namespace native
{
    float calc_area_p3(const point3d_t *p0, const point3d_t *p1, const point3d_t *p2);
    float calc_area_pv(const point3d_t *pv);
}

IF_ARCH_X86(
    namespace sse
    {
        float calc_area_p3(const point3d_t *p0, const point3d_t *p1, const point3d_t *p2);
        float calc_area_pv(const point3d_t *pv);
    }
)

typedef float (* calc_area_p3_t)(const point3d_t *p0, const point3d_t *p1, const point3d_t *p2);
typedef float (* calc_area_pv_t)(const point3d_t *pv);

UTEST_BEGIN("dsp.3d", area)

    void call(const char *label, calc_area_p3_t ca_p3, calc_area_pv_t ca_pv)
    {
        point3d_t pv[3];

        if ((!UTEST_SUPPORTED(ca_p3)) || (!UTEST_SUPPORTED(ca_pv)))
            return;

        printf("Testing %s...\n", label);

        for (size_t i=0; i<0x200; ++i)
        {
            // Intialize points
            for (size_t j=0; j<3; ++j)
                dsp::init_point_xyz(&pv[j], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));

            // Compute the value
            float np3 = native::calc_area_p3(&pv[0], &pv[1], &pv[2]);
            float npv = native::calc_area_pv(pv);

            if (!float_equals_adaptive(np3, npv, DSP_3D_TOLERANCE))
            {
                dump_point("pv[0]", &pv[0]);
                dump_point("pv[1]", &pv[1]);
                dump_point("pv[2]", &pv[2]);
                UTEST_FAIL_MSG("result of native::calc_area_p3 (%e) differs from result of native::calc_area_pv(%e)", np3, npv);
            }

            float cp3 = ca_p3(&pv[0], &pv[1], &pv[2]);
            if (!float_equals_adaptive(np3, cp3, DSP_3D_TOLERANCE))
            {
                dump_point("pv[0]", &pv[0]);
                dump_point("pv[1]", &pv[1]);
                dump_point("pv[2]", &pv[2]);
                UTEST_FAIL_MSG("result of native::calc_area_p3 (%e) differs from result of %s_p3(%e)", np3, label, cp3);
            }

            float cpv = ca_pv(pv);
            if (!float_equals_adaptive(npv, cpv, DSP_3D_TOLERANCE))
            {
                dump_point("pv[0]", &pv[0]);
                dump_point("pv[1]", &pv[1]);
                dump_point("pv[2]", &pv[2]);
                UTEST_FAIL_MSG("result of native::calc_area_pv (%e) differs from result of %s_pv(%e)", npv, label, cpv);
            }
        }
    }

    UTEST_MAIN
    {
        IF_ARCH_X86(call("sse::calc_area", sse::calc_area_p3, sse::calc_area_pv));
    }
UTEST_END;



