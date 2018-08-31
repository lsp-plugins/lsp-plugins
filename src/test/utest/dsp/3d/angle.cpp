/*
 * angle.cpp
 *
 *  Created on: 31 авг. 2018 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <test/helpers.h>
#include <dsp/dsp.h>

namespace native
{
    float calc_angle3d_v2(const vector3d_t *v1, const vector3d_t *v2);
    float calc_angle3d_vv(const vector3d_t *v);
}

IF_ARCH_X86(
    namespace sse
    {
        float calc_angle3d_v2(const vector3d_t *v1, const vector3d_t *v2);
        float calc_angle3d_vv(const vector3d_t *v);
    }
)

typedef float (* calc_angle3d_v2_t)(const vector3d_t *v1, const vector3d_t *v2);
typedef float (* calc_angle3d_vv_t)(const vector3d_t *v);

UTEST_BEGIN("dsp.3d", angle)

    void call(const char *label,
            calc_angle3d_v2_t calc_angle3d_v2,
            calc_angle3d_vv_t calc_angle3d_vv
            )
    {
        for (float phi=0.0f; phi<M_PI*2; phi += M_PI*0.125f)
        {
            for (float rho=0.0f; rho<M_PI; rho += M_PI*0.125f)
            {
                vector3d_t v[2];
                matrix3d_t m;

                dsp::init_vector_dxyz(&v[0], 1.0f, 0.0f, 0.0f);
                dsp::init_vector(&v[1], &v[0]);
                dsp::init_matrix3d_rotate_z(&m, rho);
                dsp::apply_matrix3d_mv1(&v[1], &m);

                dsp::init_matrix3d_rotate_y(&m, phi);
                dsp::apply_matrix3d_mv1(&v[0], &m);
                dsp::apply_matrix3d_mv1(&v[1], &m);

                float angle = calc_angle3d_v2(&v[0], &v[1]);
                UTEST_ASSERT_MSG(float_equals_relative(angle, cosf(rho)),
                            "calc_angle3d_v2 failed phi=%.3f, rho=%.3f, result=%.3f", phi, rho, angle);

                angle = calc_angle3d_vv(v);
                UTEST_ASSERT_MSG(float_equals_relative(angle, cosf(rho)),
                                        "calc_angle3d_vv failed phi=%.3f, rho=%.3f, result=%.3f", phi, rho, angle);
            }
        }
    }

    UTEST_MAIN
    {
        call("native_calc_angle",
                native::calc_angle3d_v2,
                native::calc_angle3d_vv
            );
        IF_ARCH_X86(
            call("sse_calc_angle",
                    sse::calc_angle3d_v2,
                    sse::calc_angle3d_vv
                )
        );
    }

UTEST_END
