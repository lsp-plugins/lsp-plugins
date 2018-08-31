/*
 * intersection.cpp
 *
 *  Created on: 31 авг. 2018 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <dsp/dsp.h>

namespace native
{
    float find_intersection3d_rt(point3d_t *ip, const ray3d_t *l, const triangle3d_t *t);
}

IF_ARCH_X86(
    namespace sse
    {
        float find_intersection3d_rt(point3d_t *ip, const ray3d_t *l, const triangle3d_t *t);
    }
)

typedef float (* find_intersection3d_rt_t)(point3d_t *ip, const ray3d_t *l, const triangle3d_t *t);

UTEST_BEGIN("dsp.3d", intersection)

    void call(const char *label,
            find_intersection3d_rt_t find_intersection3d_rt
            )
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
            ck = find_intersection3d_rt(&p, &r, &vt[i]);
            printf("find_intersection3d_rt: ck=%.3f, p=(%.3f, %.3f, %.3f)\n", ck, p.x, p.y, p.z);
            UTEST_ASSERT_MSG(ck >= 0, "failed intersection check");
        }
    }

    UTEST_MAIN
    {
        call("native_check_intersection",
                native::find_intersection3d_rt
            );
        IF_ARCH_X86(
            call("sse_check_intersection",
                    sse::find_intersection3d_rt
                )
        );
    }

UTEST_END


