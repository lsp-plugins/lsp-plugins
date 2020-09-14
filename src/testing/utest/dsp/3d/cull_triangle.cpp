/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 5 апр. 2019 г.
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

#include <core/alloc.h>
#include <test/utest.h>

#include <core/3d/common.h>

using namespace lsp;

namespace native
{
    void cull_triangle_raw(raw_triangle_t *in, size_t *n_in, const vector3d_t *pl, const raw_triangle_t *pv);
}

IF_ARCH_X86(
    namespace sse
    {
        void cull_triangle_raw(raw_triangle_t *in, size_t *n_in, const vector3d_t *pl, const raw_triangle_t *pv);
    }

    namespace sse3
    {
        void cull_triangle_raw(raw_triangle_t *in, size_t *n_in, const vector3d_t *pl, const raw_triangle_t *pv);
    }
)

typedef void (* cull_triangle_raw_t)(raw_triangle_t *in, size_t *n_in, const vector3d_t *pl, const raw_triangle_t *pv);

UTEST_BEGIN("dsp.3d", cull_triangle)

    bool do_test(cull_triangle_raw_t fn,
            const vector3d_t &pl, const point3d_t &p0, const point3d_t &p1, const point3d_t &p2,
            size_t ein)
    {
        raw_triangle_t rt, in[2];
        size_t nin;
        rt.v[0] = p0;
        rt.v[1] = p1;
        rt.v[2] = p2;

        nin  = 0;

        fn(in, &nin, &pl, &rt);
        UTEST_ASSERT(nin <= 2);

        return (nin == ein);
    }

    void test_func(const char *label, cull_triangle_raw_t fn)
    {
        if (!UTEST_SUPPORTED(fn))
            return;

        printf("Testing %s...\n", label);

        // Create culling plane
        vector3d_t pl;
        point3d_t p[3];

        // Culling plane
        dsp::init_vector_dxyz(&pl, 0.0f, 1.0f, 0.0f);

        // Test 1 intersection
        dsp::init_point_xyz(&p[0], -1.0f, -1.0f, 0.0f);
        dsp::init_point_xyz(&p[1], 1.0f, 0.0f, 0.0f);
        dsp::init_point_xyz(&p[2], -1.0f, 1.0f, 0.0f);

        UTEST_ASSERT(do_test(fn, pl, p[0], p[1], p[2], 1));
        UTEST_ASSERT(do_test(fn, pl, p[1], p[2], p[0], 1));
        UTEST_ASSERT(do_test(fn, pl, p[2], p[0], p[1], 1));
        UTEST_ASSERT(do_test(fn, pl, p[0], p[2], p[1], 1));
        UTEST_ASSERT(do_test(fn, pl, p[2], p[1], p[0], 1));
        UTEST_ASSERT(do_test(fn, pl, p[1], p[0], p[2], 1));

        // Test 2 intersections
        dsp::init_point_xyz(&p[0], 0.0f, 1.0f, 0.0f);
        dsp::init_point_xyz(&p[1], -1.0f, -1.0f, 0.0f);
        dsp::init_point_xyz(&p[2], 1.0f, -1.0f, 0.0f);

        UTEST_ASSERT(do_test(fn, pl, p[0], p[1], p[2], 2));
        UTEST_ASSERT(do_test(fn, pl, p[1], p[2], p[0], 2));
        UTEST_ASSERT(do_test(fn, pl, p[2], p[0], p[1], 2));
        UTEST_ASSERT(do_test(fn, pl, p[0], p[2], p[1], 2));
        UTEST_ASSERT(do_test(fn, pl, p[2], p[1], p[0], 2));
        UTEST_ASSERT(do_test(fn, pl, p[1], p[0], p[2], 2));

        dsp::init_point_xyz(&p[0], 0.0f, -1.0f, 0.0f);
        dsp::init_point_xyz(&p[1], -1.0f, 1.0f, 0.0f);
        dsp::init_point_xyz(&p[2], 1.0f, 1.0f, 0.0f);

        UTEST_ASSERT(do_test(fn, pl, p[0], p[1], p[2], 1));
        UTEST_ASSERT(do_test(fn, pl, p[1], p[2], p[0], 1));
        UTEST_ASSERT(do_test(fn, pl, p[2], p[0], p[1], 1));
        UTEST_ASSERT(do_test(fn, pl, p[0], p[2], p[1], 1));
        UTEST_ASSERT(do_test(fn, pl, p[2], p[1], p[0], 1));
        UTEST_ASSERT(do_test(fn, pl, p[1], p[0], p[2], 1));

        // Test 1 touch
        dsp::init_point_xyz(&p[0], 0.0f, 0.0f, 0.0f);
        dsp::init_point_xyz(&p[1], -1.0f, -1.0f, 0.0f);
        dsp::init_point_xyz(&p[2], 1.0f, -1.0f, 0.0f);

        UTEST_ASSERT(do_test(fn, pl, p[0], p[1], p[2], 1));
        UTEST_ASSERT(do_test(fn, pl, p[1], p[2], p[0], 1));
        UTEST_ASSERT(do_test(fn, pl, p[2], p[0], p[1], 1));
        UTEST_ASSERT(do_test(fn, pl, p[0], p[2], p[1], 1));
        UTEST_ASSERT(do_test(fn, pl, p[2], p[1], p[0], 1));
        UTEST_ASSERT(do_test(fn, pl, p[1], p[0], p[2], 1));

        dsp::init_point_xyz(&p[0], 0.0f, 0.0f, 0.0f);
        dsp::init_point_xyz(&p[1], -1.0f, 1.0f, 0.0f);
        dsp::init_point_xyz(&p[2], 1.0f, 1.0f, 0.0f);
        UTEST_ASSERT(do_test(fn, pl, p[0], p[1], p[2], 0));
        UTEST_ASSERT(do_test(fn, pl, p[1], p[2], p[0], 0));
        UTEST_ASSERT(do_test(fn, pl, p[2], p[0], p[1], 0));
        UTEST_ASSERT(do_test(fn, pl, p[0], p[2], p[1], 0));
        UTEST_ASSERT(do_test(fn, pl, p[2], p[1], p[0], 0));
        UTEST_ASSERT(do_test(fn, pl, p[1], p[0], p[2], 0));

        // Test 2 touches
        dsp::init_point_xyz(&p[0], 0.0f, 1.0f, 0.0f);
        dsp::init_point_xyz(&p[1], -1.0f, 0.0f, 0.0f);
        dsp::init_point_xyz(&p[2], 1.0f, 0.0f, 0.0f);

        UTEST_ASSERT(do_test(fn, pl, p[0], p[1], p[2], 0));
        UTEST_ASSERT(do_test(fn, pl, p[1], p[2], p[0], 0));
        UTEST_ASSERT(do_test(fn, pl, p[2], p[0], p[1], 0));
        UTEST_ASSERT(do_test(fn, pl, p[0], p[2], p[1], 0));
        UTEST_ASSERT(do_test(fn, pl, p[2], p[1], p[0], 0));
        UTEST_ASSERT(do_test(fn, pl, p[1], p[0], p[2], 0));

        dsp::init_point_xyz(&p[0], 0.0f, -1.0f, 0.0f);
        dsp::init_point_xyz(&p[1], 1.0f, 0.0f, 0.0f);
        dsp::init_point_xyz(&p[2], -1.0f, 0.0f, 0.0f);

        UTEST_ASSERT(do_test(fn, pl, p[0], p[1], p[2], 1));
        UTEST_ASSERT(do_test(fn, pl, p[1], p[2], p[0], 1));
        UTEST_ASSERT(do_test(fn, pl, p[2], p[0], p[1], 1));
        UTEST_ASSERT(do_test(fn, pl, p[0], p[2], p[1], 1));
        UTEST_ASSERT(do_test(fn, pl, p[2], p[1], p[0], 1));
        UTEST_ASSERT(do_test(fn, pl, p[1], p[0], p[2], 1));

        // Test 3 touches
        dsp::init_point_xyz(&p[0], 0.0f, 0.0f, 1.0f);
        dsp::init_point_xyz(&p[1], -1.0f, 0.0f, -1.0f);
        dsp::init_point_xyz(&p[2], 1.0f, 0.0f, -1.0f);

        UTEST_ASSERT(do_test(fn, pl, p[0], p[1], p[2], 0));
        UTEST_ASSERT(do_test(fn, pl, p[1], p[2], p[0], 0));
        UTEST_ASSERT(do_test(fn, pl, p[2], p[0], p[1], 0));
        UTEST_ASSERT(do_test(fn, pl, p[0], p[2], p[1], 0));
        UTEST_ASSERT(do_test(fn, pl, p[2], p[1], p[0], 0));
        UTEST_ASSERT(do_test(fn, pl, p[1], p[0], p[2], 0));
    }

    UTEST_MAIN
    {
        test_func("native::cull_triangle_raw", native::cull_triangle_raw);
        IF_ARCH_X86(test_func("sse::cull_triangle_raw", sse::cull_triangle_raw));
        IF_ARCH_X86(test_func("sse3::cull_triangle_raw", sse3::cull_triangle_raw));
    }
UTEST_END;


