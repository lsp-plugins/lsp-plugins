/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 30 авг. 2018 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#include <dsp/dsp.h>
#include <test/utest.h>
#include <testing/utest/dsp/3d/helpers.h>

namespace native
{
    void init_point_xyz(point3d_t *p, float x, float y, float z);
    void init_point(point3d_t *p, const point3d_t *s);
    void normalize_point(point3d_t *p);
}

IF_ARCH_X86(
    namespace sse
    {
        void init_point_xyz(point3d_t *p, float x, float y, float z);
        void init_point(point3d_t *p, const point3d_t *s);
        void normalize_point(point3d_t *p);
    }
)

typedef void (* init_point_xyz_t)(point3d_t *p, float x, float y, float z);
typedef void (* init_point_t)(point3d_t *p, const point3d_t *s);
typedef void (* normalize_point_t)(point3d_t *p);

UTEST_BEGIN("dsp.3d", point)

    void call(
            const char *label,
            init_point_xyz_t init_xyz,
            init_point_t init,
            normalize_point_t norm
        )
    {
        if ((!UTEST_SUPPORTED(init_xyz)) || (!UTEST_SUPPORTED(init)) || (!UTEST_SUPPORTED(norm)))
            return;

        printf("Testing %s\n", label);

        point3d_t   p1, p2, p3;

        native::init_point_xyz(&p1, 1.0f, 2.0f, 3.0f);
        init_xyz(&p2, 1.0f, 2.0f, 3.0f);
        UTEST_ASSERT_MSG(point3d_sck(&p1, &p2), "Failed init_point_xyz");

        native::init_point(&p2, &p1);
        init(&p3, &p1);
        UTEST_ASSERT_MSG(point3d_sck(&p1, &p2), "Failed native init_point");
        UTEST_ASSERT_MSG(point3d_sck(&p1, &p3), "Failed optimized init_point");

        native::normalize_point(&p2);
        norm(&p3);

        UTEST_ASSERT_MSG(point3d_sck(&p2, &p3), "Failed normalize point");
    }

    UTEST_MAIN
    {
        IF_ARCH_X86(call("sse_point", sse::init_point_xyz, sse::init_point, sse::normalize_point));
    }
UTEST_END;



