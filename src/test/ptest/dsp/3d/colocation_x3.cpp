/*
 * colocation_x3.cpp
 *
 *  Created on: 3 апр. 2019 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>
#include <core/sugar.h>
#include <test/helpers.h>

#define N_PLANES        256
#define N_POINTS        256

namespace native
{
    size_t colocation_x3_v1p3(const vector3d_t *pl, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2);
    size_t colocation_x3_v1pv(const vector3d_t *pl, const point3d_t *pv);
    size_t colocation_x3_v3p1(const vector3d_t *v0, const vector3d_t *v1, const vector3d_t *v2, const point3d_t *p);
    size_t colocation_x3_vvp1(const vector3d_t *vv, const point3d_t *p);
}

IF_ARCH_X86(
    namespace sse
    {
        size_t colocation_x3_v1p3(const vector3d_t *pl, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2);
        size_t colocation_x3_v1pv(const vector3d_t *pl, const point3d_t *pv);
        size_t colocation_x3_v3p1(const vector3d_t *v0, const vector3d_t *v1, const vector3d_t *v2, const point3d_t *p);
        size_t colocation_x3_vvp1(const vector3d_t *vv, const point3d_t *p);
    }

    namespace sse3
    {
        size_t colocation_x3_v1p3(const vector3d_t *pl, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2);
        size_t colocation_x3_v1pv(const vector3d_t *pl, const point3d_t *pv);
        size_t colocation_x3_v3p1(const vector3d_t *v0, const vector3d_t *v1, const vector3d_t *v2, const point3d_t *p);
        size_t colocation_x3_vvp1(const vector3d_t *vv, const point3d_t *p);
    }
)

typedef size_t (* colocation_x3_v1p3_t)(const vector3d_t *pl, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2);
typedef size_t (* colocation_x3_v1pv_t)(const vector3d_t *pl, const point3d_t *pv);
typedef size_t (* colocation_x3_v3p1_t)(const vector3d_t *v0, const vector3d_t *v1, const vector3d_t *v2, const point3d_t *p);
typedef size_t (* colocation_x3_vvp1_t)(const vector3d_t *vv, const point3d_t *p);

//-----------------------------------------------------------------------------
// Performance test
PTEST_BEGIN("dsp.3d", colocation_x3, 5, 1000)

    void call_pv(const char *label, const vector3d_t *pl, const point3d_t *pv, colocation_x3_v1p3_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s", label);
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            const vector3d_t *xpl = pl;
            for (size_t i=0; i<N_PLANES*3; ++i, ++xpl)
            {
                const point3d_t *xp = pv;
                for (size_t j=0; j<N_POINTS; ++j, xp += 3)
                    func(xpl, &xp[0], &xp[1], &xp[2]);
            }
        );
    }

    void call_pv(const char *label, const vector3d_t *pl, const point3d_t *pv, colocation_x3_v1pv_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s", label);
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            const vector3d_t *xpl = pl;
            for (size_t i=0; i<N_PLANES*3; ++i, ++xpl)
            {
                const point3d_t *xp = pv;
                for (size_t j=0; j<N_POINTS; ++j, xp += 3)
                    func(xpl, xp);
            }
        );
    }

    void call_vv(const char *label, const vector3d_t *pl, const point3d_t *pv, colocation_x3_v3p1_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s", label);
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            const vector3d_t *xpl = pl;
            for (size_t i=0; i<N_PLANES; ++i, xpl += 3)
            {
                const point3d_t *xp = pv;
                for (size_t j=0; j<N_POINTS*3; ++j, ++xp)
                    func(&xpl[0], &xpl[1], &xpl[2], xp);
            }
        );
    }

    void call_vv(const char *label, const vector3d_t *pl, const point3d_t *pv, colocation_x3_vvp1_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        sprintf(buf, "%s", label);
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            const vector3d_t *xpl = pl;
            for (size_t i=0; i<N_PLANES; ++i, xpl += 3)
            {
                const point3d_t *xp = pv;
                for (size_t j=0; j<N_POINTS*3; ++j, ++xp)
                    func(xpl, xp);
            }
        );
    }

    PTEST_MAIN
    {
        size_t buf_size     = N_PLANES * 3 * sizeof(vector3d_t) + N_POINTS * 3 * sizeof(point3d_t);
        uint8_t *data       = NULL;
        uint8_t *ptr        = alloc_aligned<uint8_t>(data, buf_size, 64);

        vector3d_t *planes  = reinterpret_cast<vector3d_t *>(ptr);
        ptr                += N_PLANES * 3 * sizeof(vector3d_t);
        point3d_t *points   = reinterpret_cast<point3d_t *>(ptr);

        // Initialize planes
        for (size_t i=0; i < N_PLANES*3;)
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
        for (size_t i=0; i < N_POINTS*3; ++i)
            dsp::init_point_xyz(&points[i], randf(-10.0f, 10.0f), randf(-10.0f, 10.0f), randf(-10.0f, 10.0f));


        call_pv("native::colocation_x3_v1p3", planes, points, native::colocation_x3_v1p3);
        IF_ARCH_X86(call_pv("sse::colocation_x3_v1p3", planes, points, sse::colocation_x3_v1p3));
        IF_ARCH_X86(call_pv("sse3::colocation_x3_v1p3", planes, points, sse3::colocation_x3_v1p3));
        PTEST_SEPARATOR;

        call_pv("native::colocation_x3_v1pv", planes, points, native::colocation_x3_v1pv);
        IF_ARCH_X86(call_pv("sse::colocation_x3_v1pv", planes, points, sse::colocation_x3_v1pv));
        IF_ARCH_X86(call_pv("sse3::colocation_x3_v1pv", planes, points, sse3::colocation_x3_v1pv));
        PTEST_SEPARATOR;

        call_vv("native::colocation_x3_v3p1", planes, points, native::colocation_x3_v3p1);
        IF_ARCH_X86(call_vv("sse::colocation_x3_v3p1", planes, points, sse::colocation_x3_v3p1));
        IF_ARCH_X86(call_vv("sse3::colocation_x3_v3p1", planes, points, sse3::colocation_x3_v3p1));
        PTEST_SEPARATOR;

        call_vv("native::colocation_x3_vvp1", planes, points, native::colocation_x3_vvp1);
        IF_ARCH_X86(call_vv("sse::colocation_x3_vvp1", planes, points, sse::colocation_x3_vvp1));
        IF_ARCH_X86(call_vv("sse3::colocation_x3_vvp1", planes, points, sse3::colocation_x3_vvp1));
        PTEST_SEPARATOR;

        free_aligned(data);
    }
PTEST_END



