/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 3 апр. 2019 г.
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
#include <test/ptest.h>
#include <core/sugar.h>
#include <test/helpers.h>

#define N_PLANES        256
#define N_LINES         256

namespace native
{
    size_t colocation_x2_v1p2(const vector3d_t *pl, const point3d_t *p0, const point3d_t *p1);
    size_t colocation_x2_v1pv(const vector3d_t *pl, const point3d_t *pv);
}

IF_ARCH_X86(
    namespace sse
    {
        size_t colocation_x2_v1p2(const vector3d_t *pl, const point3d_t *p0, const point3d_t *p1);
        size_t colocation_x2_v1pv(const vector3d_t *pl, const point3d_t *pv);
    }

    namespace sse3
    {
        size_t colocation_x2_v1p2(const vector3d_t *pl, const point3d_t *p0, const point3d_t *p1);
        size_t colocation_x2_v1pv(const vector3d_t *pl, const point3d_t *pv);
    }
)

typedef size_t (* colocation_x2_v1p2_t)(const vector3d_t *pl, const point3d_t *p0, const point3d_t *p1);
typedef size_t (* colocation_x2_v1pv_t)(const vector3d_t *pl, const point3d_t *pv);

//-----------------------------------------------------------------------------
// Performance test
PTEST_BEGIN("dsp.3d", colocation_x2, 5, 1000)

    void call(const char *label, const vector3d_t *pl, const point3d_t *pv, colocation_x2_v1p2_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s", label);
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            const vector3d_t *xpl = pl;
            for (size_t i=0; i<N_PLANES; ++i, ++xpl)
            {
                const point3d_t *xp = pv;
                for (size_t j=0; j<N_LINES; ++j, xp += 2)
                    func(xpl, &xp[0], &xp[1]);
            }
        );
    }

    void call(const char *label, const vector3d_t *pl, const point3d_t *pv, colocation_x2_v1pv_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s", label);
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            const vector3d_t *xpl = pl;
            for (size_t i=0; i<N_PLANES; ++i, ++xpl)
            {
                const point3d_t *xp = pv;
                for (size_t j=0; j<N_LINES; ++j, xp += 2)
                    func(xpl, xp);
            }
        );
    }

    PTEST_MAIN
    {
        size_t buf_size     = N_PLANES * sizeof(vector3d_t) + N_LINES * 2 * sizeof(point3d_t);
        uint8_t *data       = NULL;
        uint8_t *ptr        = alloc_aligned<uint8_t>(data, buf_size, 64);

        vector3d_t *planes  = reinterpret_cast<vector3d_t *>(ptr);
        ptr                += N_PLANES * sizeof(vector3d_t);
        point3d_t *points   = reinterpret_cast<point3d_t *>(ptr);

        // Initialize planes
        for (size_t i=0; i < N_PLANES;)
        {
            vector3d_t *pl  = &planes[i];

            dsp::init_vector_dxyz(pl, randf(-1.0f, 1.0f), randf(-1.0f, 1.0f), randf(-1.0f, 1.0f));
            float w = sqrtf(pl->dx * pl->dx + pl->dy * pl->dy + pl->dz * pl->dz);
            if (w <= DSP_3D_TOLERANCE)
                continue;

            pl->dx     /= w;
            pl->dy     /= w;
            pl->dz     /= w;
            pl->dw      = randf(-1.0f, 1.0f);

            ++i;
        }

        // Initialize points
        for (size_t i=0; i < N_LINES*2; ++i)
            dsp::init_point_xyz(&points[i], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));


        call("native::colocation_x2_v1p2", planes, points, native::colocation_x2_v1p2);
        IF_ARCH_X86(call("sse::colocation_x2_v1p2", planes, points, sse::colocation_x2_v1p2));
        IF_ARCH_X86(call("sse3::colocation_x2_v1p2", planes, points, sse3::colocation_x2_v1p2));
        PTEST_SEPARATOR;

        call("native::colocation_x2_v1pv", planes, points, native::colocation_x2_v1pv);
        IF_ARCH_X86(call("sse::colocation_x2_v1pv", planes, points, sse::colocation_x2_v1pv));
        IF_ARCH_X86(call("sse3::colocation_x2_v1pv", planes, points, sse3::colocation_x2_v1pv));
        PTEST_SEPARATOR;

        free_aligned(data);
    }
PTEST_END


