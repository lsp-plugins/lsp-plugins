/*
 * edge.cpp
 *
 *  Created on: 31 авг. 2018 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <dsp/dsp.h>

namespace native
{
    size_t longest_edge3d_p3(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3);
    size_t longest_edge3d_pv(const point3d_t *p);
}

IF_ARCH_X86(
    namespace sse
    {
        size_t longest_edge3d_p3(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3);
        size_t longest_edge3d_pv(const point3d_t *p);
    }
)

typedef size_t (* longest_edge3d_p3_t)(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3);
typedef size_t (* longest_edge3d_pv_t)(const point3d_t *p);

UTEST_BEGIN("dsp.3d", edge)

    void call(const char *label,
            longest_edge3d_p3_t longest_edge3d_p3,
            longest_edge3d_pv_t longest_edge3d_pv
            )
    {
        printf("Testing %s...\n", label);

        point3d_t lp[3];
        dsp::init_point_xyz(&lp[0], 1.0f, 1.0f, 1.0f);
        dsp::init_point_xyz(&lp[1], 2.0f, 2.0f, 2.0f);
        dsp::init_point_xyz(&lp[2], 0.1f, 0.1f, 0.1f);

        UTEST_ASSERT_MSG(longest_edge3d_pv(lp) == 1, "longest_edge3d_pv failed");

        UTEST_ASSERT_MSG(longest_edge3d_p3(&lp[0], &lp[1], &lp[2]) == 1, "longest_edge3d_p3 failed");
        UTEST_ASSERT_MSG(longest_edge3d_p3(&lp[1], &lp[2], &lp[0]) == 0, "longest_edge3d_p3 failed");
        UTEST_ASSERT_MSG(longest_edge3d_p3(&lp[2], &lp[0], &lp[1]) == 2, "longest_edge3d_p3 failed");
    }

    UTEST_MAIN
    {
        call("native::longest_edge",
                native::longest_edge3d_p3,
                native::longest_edge3d_pv
            );
        IF_ARCH_X86(
            call("sse::longest_edge",
                    sse::longest_edge3d_p3,
                    sse::longest_edge3d_pv
                )
        );
    }

UTEST_END





