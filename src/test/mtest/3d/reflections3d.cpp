/*
 * reflections3d.cpp
 *
 *  Created on: 24 дек. 2018 г.
 *      Author: sadko
 */

#include <test/mtest.h>
#include <test/mtest/3d/common/X11Renderer.h>
#include <core/files/Model3DFile.h>
#include <core/3d/rt_context.h>

#include <core/types.h>
#include <core/debug.h>
#include <core/sugar.h>
#include <core/status.h>
#include <stdlib.h>
#include <errno.h>
#include <data/cstorage.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysymdef.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include <sys/poll.h>

//#define TEST_DEBUG

#ifndef TEST_DEBUG
    #define BREAKPOINT_STEP     0

    #define INIT_FRONT(front) \
        dsp::init_point_xyz(&front.p[0], 0.0f, 1.0f, 0.0f); \
        dsp::init_point_xyz(&front.p[1], -1.0f, -0.5f, 0.0f); \
        dsp::init_point_xyz(&front.p[2], 1.0f, -0.5f, 0.0f); \
        dsp::init_point_xyz(&front.s, 0.0f, 0.0f, 1.0f);

/*
        dsp::init_point_xyz(&front.p[0], -0.980776, -0.195088, 0.000000); \
        dsp::init_point_xyz(&front.p[1], 0.685477, -0.883233, 0.000000); \
        dsp::init_point_xyz(&front.p[2], 0.295300, 1.078322, 0.000000); \
        dsp::init_point_xyz(&front.s, 0.000000, 0.000000, 1.000000);
*/

#else /* DEBUG */
    #define BREAKPOINT_STEP     0

    #define INIT_FRONT(front) \
        dsp::init_point_xyz(&front.s, 0.0f, 0.0f, -0.75f); \
        dsp::init_point_xyz(&front.p[0], 0.378978, -0.513494, -1.580061); \
        dsp::init_point_xyz(&front.p[1], 0.453656, -0.486792, -1.562277); \
        dsp::init_point_xyz(&front.p[2], 0.352607, -0.494265, -1.620808);
#endif /* DEBUG */

namespace mtest
{
    using namespace lsp;

    static const size_t bbox_map[] =
    {
        0, 1, 2,
        0, 2, 3,
        6, 5, 4,
        6, 4, 7,
        1, 0, 4,
        1, 4, 5,
        3, 2, 6,
        3, 6, 7,
        1, 5, 2,
        2, 5, 6,
        0, 3, 4,
        3, 7, 4
    };

#if 0
    static inline void add_to_view(context_t *ctx, const rt_triangle3d_t *rt, const color3d_t *c)
    {
        ctx->global->view->add_triangle_pvnc1(rt->p, &rt->n, c);
    }

    static inline void add_to_view_3c(context_t *ctx, const rt_triangle3d_t *rt, const color3d_t *c0, const color3d_t *c1, const color3d_t *c2)
    {
        ctx->global->view->add_triangle_pvnc3(rt->p, &rt->n, c0, c1, c2);
    }

    static void destroy_tasks(cvector<context_t> &tasks)
    {
        for (size_t i=0, n=tasks.size(); i<n; ++i)
        {
            context_t *ctx = tasks.get(i);
            if (ctx == NULL)
                continue;

            ctx->global     = NULL;
            ctx->source.flush();

            delete ctx;
        }

        tasks.flush();
    }

//    static void init_triangle_pv(v_triangle3d_t *t, const point3d_t *pv)
//    {
//        t->p[0] = pv[0];
//        t->p[1] = pv[1];
//        t->p[2] = pv[2];
//        dsp::calc_normal3d_pv(&t->n[0], pv);
//        t->n[1] = t->n[0];
//        t->n[2] = t->n[0];
//    }


    static void calc_plane_vector_pv(vector3d_t *v, const point3d_t *p)
    {
        // Calculate edge parameters
        vector3d_t d[2];
        d[0].dx     = p[1].x - p[0].x;
        d[0].dy     = p[1].y - p[0].y;
        d[0].dz     = p[1].z - p[0].z;
        d[0].dw     = p[1].w - p[0].w;

        d[1].dx     = p[2].x - p[1].x;
        d[1].dy     = p[2].y - p[1].y;
        d[1].dz     = p[2].z - p[1].z;
        d[1].dw     = p[2].w - p[1].w;

        // Do vector multiplication to calculate the normal vector
        v->dx       = d[0].dy*d[1].dz - d[0].dz*d[1].dy;
        v->dy       = d[0].dz*d[1].dx - d[0].dx*d[1].dz;
        v->dz       = d[0].dx*d[1].dy - d[0].dy*d[1].dx;
        v->dw       = 0.0f;

        dsp::normalize_vector(v);

        v->dw       = - ( v->dx * p[0].x + v->dy * p[0].y + v->dz * p[0].z); // Parameter for the plane equation
    }

    static bool check_triangle(const rt_triangle3d_t *t)
    {
        vector3d_t d[3];
        d[0].dx     = t->p[1].x - t->p[0].x;
        d[0].dy     = t->p[1].y - t->p[0].y;
        d[0].dz     = t->p[1].z - t->p[0].z;

        d[1].dx     = t->p[2].x - t->p[0].x;
        d[1].dy     = t->p[2].y - t->p[0].y;
        d[1].dz     = t->p[2].z - t->p[0].z;

        d[2].dx     = d[0].dy*d[1].dz - d[0].dz*d[1].dy;
        d[2].dy     = d[0].dz*d[1].dx - d[0].dx*d[1].dz;
        d[2].dz     = d[0].dx*d[1].dy - d[0].dy*d[1].dx;

        float w     = d[2].dx*d[2].dx + d[2].dy*d[2].dy + d[2].dz*d[2].dz;

        return w > DSP_3D_TOLERANCE;
    }

//    static float check_triangle_left_order_pv(const vector3d_t *pov, const point3d_t *p)
//    {
//        vector3d_t d[3];
//        d[0].dx     = p[1].x - p[0].x;
//        d[0].dy     = p[1].y - p[0].y;
//        d[0].dz     = p[1].z - p[0].z;
//
//        d[1].dx     = p[2].x - p[0].x;
//        d[1].dy     = p[2].y - p[0].y;
//        d[1].dz     = p[2].z - p[0].z;
//
//        d[2].dx     = d[0].dy*d[1].dz - d[0].dz*d[1].dy;
//        d[2].dy     = d[0].dz*d[1].dx - d[0].dx*d[1].dz;
//        d[2].dz     = d[0].dx*d[1].dy - d[0].dy*d[1].dx;
//
//        return pov->dx * d[2].dx + pov->dy * d[2].dy + pov->dz * d[2].dz;
//    }
    /**
     * Project triangle to the plane
     * @param pv array of 3 points to store projected points
     * @param fp focus point, the point where all projective lines do intersect
     * @param pl plane equation vector
     * @param tv triangle points
     */
    static void project_triangle(
        point3d_t *pv,
        const point3d_t *fp,
        const vector3d_t *pl,
        const point3d_t *tv
    )
    {
        vector3d_t d[3];
        float k, t[3];

        d[0].dx     = fp->x - tv[0].x;
        d[0].dy     = fp->y - tv[0].y;
        d[0].dz     = fp->z - tv[0].z;
        d[0].dw     = 0.0f;

        d[1].dx     = fp->x - tv[1].x;
        d[1].dy     = fp->y - tv[1].y;
        d[1].dz     = fp->z - tv[1].z;
        d[1].dw     = 0.0f;

        d[2].dx     = fp->x - tv[2].x;
        d[2].dy     = fp->y - tv[2].y;
        d[2].dz     = fp->z - tv[2].z;
        d[2].dw     = 0.0f;

        k           = - (pl->dx*fp->x + pl->dy*fp->y + pl->dz*fp->z + pl->dw);
        t[0]        = k / (pl->dx*d[0].dx + pl->dy*d[0].dy + pl->dz*d[0].dz);
        t[1]        = k / (pl->dx*d[1].dx + pl->dy*d[1].dy + pl->dz*d[1].dz);
        t[2]        = k / (pl->dx*d[2].dx + pl->dy*d[2].dy + pl->dz*d[2].dz);

        pv[0].x     = fp->x + t[0] * d[0].dx;
        pv[0].y     = fp->y + t[0] * d[0].dy;
        pv[0].z     = fp->z + t[0] * d[0].dz;
        pv[0].w     = 1.0f;

        pv[1].x     = fp->x + t[1] * d[1].dx;
        pv[1].y     = fp->y + t[1] * d[1].dy;
        pv[1].z     = fp->z + t[1] * d[1].dz;
        pv[1].w     = 1.0f;

        pv[2].x     = fp->x + t[2] * d[2].dx;
        pv[2].y     = fp->y + t[2] * d[2].dy;
        pv[2].z     = fp->z + t[2] * d[2].dz;
        pv[2].w     = 1.0f;
    }
#endif

    //    static void parallel_flip_plane(vector3d_t *v, const vector3d_t *pl, const point3d_t *p)
//    {
//        // Calculate edge parameters
//        v->dx       = - pl->dx;
//        v->dy       = - pl->dy;
//        v->dz       = - pl->dz;
//        v->dw       = pl->dx * p->x + pl->dy * p->y + pl->dz * p->z; // Parameter for the plane equation
//    }

    static void init_triangle_p3(v_triangle3d_t *t, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2, const vector3d_t *n)
    {
        dsp::calc_normal3d_p3(&t->n[0], p0, p1, p2);
        float a = t->n[0].dx * n->dx + t->n[0].dy * n->dy + t->n[0].dz * n->dz;
        if (a < 0.0f)
        {
            t->p[0] = *p0;
            t->p[1] = *p2;
            t->p[2] = *p1;

            t->n[0].dx  = -t->n[0].dx;
            t->n[0].dy  = -t->n[0].dy;
            t->n[0].dz  = -t->n[0].dz;
        }
        else
        {
            t->p[0] = *p0;
            t->p[1] = *p1;
            t->p[2] = *p2;
        }

        t->n[1] = t->n[0];
        t->n[2] = t->n[0];

        // Shift triangle in space to match the plane
        float dw = n->dw + (t->n[0].dx * t->p[0].x + t->n[0].dy * t->p[0].y + t->n[0].dz * t->p[0].z);
        for (size_t i=0; i<3; ++i)
        {
            t->p[i].x  -= dw * t->n[i].dx;
            t->p[i].y  -= dw * t->n[i].dy;
            t->p[i].z  -= dw * t->n[i].dz;
        }
    }

    /**
     * Split raw triangle with plane, generates output set of triangles into out (triangles above split plane)
     * and in (triangles below split plane). For every triangle, points 1 and 2 are the points that
     * lay on the split plane, the first triangle ALWAYS has 2 common points with plane (1 and 2)
     *
     * @param out array of vertexes above plane
     * @param n_out counter of vertexes above plane (multiple of 3), should be initialized
     * @param in array of vertexes below plane
     * @param n_in counter of vertexes below plane (multiple of 3), should be initialized
     * @param pl plane equation
     * @param pv triangle to perform the split
     */
    static void split_triangle_raw(
            raw_triangle_t *out,
            size_t *n_out,
            raw_triangle_t *in,
            size_t *n_in,
            const vector3d_t *pl,
            const raw_triangle_t *pv
        )
    {
        point3d_t sp[2];    // Split point
        vector3d_t d[2];    // Delta vector
        point3d_t p[3];     // Triangle sources
        float k[3];         // Co-location of points
        float t[2];

        in     += *n_in;
        out    += *n_out;

        p[0]    = pv->p[0];
        p[1]    = pv->p[1];
        p[2]    = pv->p[2];

        k[0]    = pl->dx*p[0].x + pl->dy*p[0].y + pl->dz*p[0].z + pl->dw;
        k[1]    = pl->dx*p[1].x + pl->dy*p[1].y + pl->dz*p[1].z + pl->dw;
        k[2]    = pl->dx*p[2].x + pl->dy*p[2].y + pl->dz*p[2].z + pl->dw;

        // Check that the whole triangle lies above the plane or below the plane
        if (k[0] < 0.0f)
        {
            if ((k[1] <= 0.0f) && (k[2] <= 0.0f))
            {
                in->p[0]        = p[0];
                in->p[1]        = p[1];
                in->p[2]        = p[2];
                ++*n_in;
                return;
            }
        }
        else if (k[0] > 0.0f)
        {
            if ((k[1] >= 0.0f) && (k[2] >= 0.0f))
            {
                out->p[0]       = p[0];
                out->p[1]       = p[1];
                out->p[2]       = p[2];
                ++*n_out;
                return;
            }
        }
        else // (k[0] == 0)
        {
            if ((k[1] >= 0.0f) && (k[2] >= 0.0f))
            {
                out->p[0]       = p[0];
                out->p[1]       = p[1];
                out->p[2]       = p[2];
                ++*n_out;
                return;
            }
            else if ((k[1] <= 0.0f) && (k[2] <= 0.0f))
            {
                in->p[0]        = p[0];
                in->p[1]        = p[1];
                in->p[2]        = p[2];
                ++*n_in;
                return;
            }
        }

        // There is an intersection with plane, we need to analyze it
        // Rotate triangle until vertex 0 is above the split plane
        while (k[0] <= 0.0f)
        {
            t[0]    = k[0];
            sp[0]   = p[0];

            k[0]    = k[1];
            p[0]    = p[1];
            k[1]    = k[2];
            p[1]    = p[2];
            k[2]    = t[0];
            p[2]    = sp[0];
        }

        // Now we have p[0] guaranteed to be above plane, analyze p[1] and p[2]
        if (k[1] < 0.0f) // k[1] < 0
        {
            d[0].dx = p[0].x - p[1].x;
            d[0].dy = p[0].y - p[1].y;
            d[0].dz = p[0].z - p[1].z;

            t[0]    = -k[0] / (pl->dx*d[0].dx + pl->dy*d[0].dy + pl->dz*d[0].dz);

            sp[0].x = p[0].x + d[0].dx * t[0];
            sp[0].y = p[0].y + d[0].dy * t[0];
            sp[0].z = p[0].z + d[0].dz * t[0];
            sp[0].w = 1.0f;

            if (k[2] < 0.0f) // (k[1] < 0) && (k[2] < 0)
            {
                d[1].dx = p[0].x - p[2].x;
                d[1].dy = p[0].y - p[2].y;
                d[1].dz = p[0].z - p[2].z;

                t[1]    = -k[0] / (pl->dx*d[1].dx + pl->dy*d[1].dy + pl->dz*d[1].dz);

                sp[1].x = p[0].x + d[1].dx * t[1];
                sp[1].y = p[0].y + d[1].dy * t[1];
                sp[1].z = p[0].z + d[1].dz * t[1];
                sp[1].w = 1.0f;

                // 1 triangle above plane, 2 below
                out->p[0]       = p[0];
                out->p[1]       = sp[0];
                out->p[2]       = sp[1];
                ++*n_out;
                ++out;

                in->p[0]        = p[1];
                in->p[1]        = sp[1];
                in->p[2]        = sp[0];
                ++*n_in;
                ++in;

                in->p[0]        = p[2];
                in->p[1]        = sp[1];
                in->p[2]        = p[1];
                ++*n_in;
            }
            else if (k[2] > 0.0f) // (k[1] < 0) && (k[2] > 0)
            {
                d[1].dx = p[2].x - p[1].x;
                d[1].dy = p[2].y - p[1].y;
                d[1].dz = p[2].z - p[1].z;

                t[1]    = -k[2] / (pl->dx*d[1].dx + pl->dy*d[1].dy + pl->dz*d[1].dz);

                sp[1].x = p[2].x + d[1].dx * t[1];
                sp[1].y = p[2].y + d[1].dy * t[1];
                sp[1].z = p[2].z + d[1].dz * t[1];
                sp[1].w = 1.0f;

                // 2 triangles above plane, 1 below
                out->p[0]       = p[2];
                out->p[1]       = sp[0];
                out->p[2]       = sp[1];
                ++*n_out;
                ++out;

                out->p[0]       = p[0];
                out->p[1]       = sp[0];
                out->p[2]       = p[2];
                ++*n_out;

                in->p[0]        = p[1];
                in->p[1]        = sp[1];
                in->p[2]        = sp[0];
                ++*n_in;
            }
            else // (k[1] < 0) && (k[2] == 0)
            {
                // 1 triangle above plane, 1 below
                out->p[0]       = p[0];
                out->p[1]       = sp[0];
                out->p[2]       = p[2];
                ++*n_out;

                in->p[0]        = p[1];
                in->p[1]        = p[2];
                in->p[2]        = sp[0];
                ++*n_in;
            }
        }
        else // (k[1] >= 0) && (k[2] < 0)
        {
            d[0].dx = p[0].x - p[2].x;
            d[0].dy = p[0].y - p[2].y;
            d[0].dz = p[0].z - p[2].z;

            t[0]    = -k[0] / (pl->dx*d[0].dx + pl->dy*d[0].dy + pl->dz*d[0].dz);

            sp[0].x = p[0].x + d[0].dx * t[0];
            sp[0].y = p[0].y + d[0].dy * t[0];
            sp[0].z = p[0].z + d[0].dz * t[0];
            sp[0].w = 1.0f;

            if (k[1] > 0.0f) // (k[1] > 0) && (k[2] < 0)
            {
                d[1].dx = p[1].x - p[2].x;
                d[1].dy = p[1].y - p[2].y;
                d[1].dz = p[1].z - p[2].z;

                t[1]    = -k[1] / (pl->dx*d[1].dx + pl->dy*d[1].dy + pl->dz*d[1].dz);

                sp[1].x = p[1].x + d[1].dx * t[1];
                sp[1].y = p[1].y + d[1].dy * t[1];
                sp[1].z = p[1].z + d[1].dz * t[1];
                sp[1].w = 1.0f;

                // 2 triangles above plane, 1 below
                out->p[0]       = p[0];
                out->p[1]       = sp[1];
                out->p[2]       = sp[0];
                ++*n_out;
                ++out;

                out->p[0]       = p[1];
                out->p[1]       = sp[1];
                out->p[2]       = p[0];
                ++*n_out;

                in->p[0]        = p[2];
                in->p[1]        = sp[0];
                in->p[2]        = sp[1];
                ++*n_in;
            }
            else // (k[1] == 0) && (k[2] < 0)
            {
                // 1 triangle above plane, 1 triangle below plane
                out->p[0]       = p[0];
                out->p[1]       = p[1];
                out->p[2]       = sp[0];
                ++*n_out;

                in->p[0]        = p[2];
                in->p[1]        = sp[0];
                in->p[2]        = p[1];
                ++*n_in;
            }
        }
    }

    static void destroy_tasks(cvector<rt_context_t> &tasks)
    {
        for (size_t i=0, n=tasks.size(); i<n; ++i)
        {
            rt_context_t *ctx = tasks.get(i);
            if (ctx != NULL)
                delete ctx;
        }

        tasks.flush();
    }

    bool check_bound_box(const bound_box3d_t *bbox, const rt_view_t *view)
    {
        vector3d_t pl[4];

        dsp::calc_plane_p3(&pl[0], &view->s, &view->p[0], &view->p[1]);
        dsp::calc_plane_p3(&pl[1], &view->s, &view->p[1], &view->p[2]);
        dsp::calc_plane_p3(&pl[2], &view->s, &view->p[2], &view->p[0]);
        dsp::calc_plane_p3(&pl[3], &view->p[0], &view->p[1], &view->p[2]);

        raw_triangle_t out[16], buf1[16], buf2[16], *q, *in, *tmp;
        size_t n_out, n_buf1, n_buf2, *n_q, *n_in, *n_tmp;

        // Cull each triangle of bounding box with four scissor planes
        for (size_t j=0, m = sizeof(bbox_map)/sizeof(size_t); j < m; )
        {
            // Initialize input and queue buffer
            q = buf1, in = buf2;
            n_q = &n_buf1, n_in = &n_buf2;

            // Put to queue with updated matrix
            *n_q        = 1;
            n_out       = 0;
            q->p[0]     = bbox->p[bbox_map[j++]];
            q->p[1]     = bbox->p[bbox_map[j++]];
            q->p[2]     = bbox->p[bbox_map[j++]];

            // Cull triangle with planes
            for (size_t k=0; ; )
            {
                // Reset counters
                *n_in   = 0;

                // Split all triangles:
                // Put all triangles above the plane to out
                // Put all triangles below the plane to in
                for (size_t l=0; l < *n_q; ++l)
                {
                    split_triangle_raw(out, &n_out, in, n_in, &pl[k], &q[l]);
                    if ((n_out > 16) || ((*n_in) > 16))
                        lsp_trace("split overflow: n_out=%d, n_in=%d", int(n_out), int(*n_in));
                }

                // Interrupt cycle if there is no data to process
                if ((*n_in <= 0) || ((++k) >= 4))
                   break;

                // Swap buffers buf0 <-> buf1
                n_tmp = n_in, tmp = in;
                n_in = n_q, in = q;
                n_q = n_tmp, q = tmp;
            }

            if (*n_in > 0) // Is there intersection with bounding box?
                break;
        }

        return (*n_in) > 0;
    }

    /**
     * Scan scene for triangles laying inside the viewing area of wave front
     * @param ctx wave front context
     * @return status of operation
     */
    status_t scan_objects(cvector<rt_context_t> &tasks, rt_context_t *ctx)
    {
        status_t res = STATUS_OK;

        RT_TRACE_BREAK(ctx,
            lsp_trace("Scanning objects...");

            for (size_t i=0, n=ctx->shared->scene->num_objects(); i<n; ++i)
            {
                Object3D *obj = ctx->shared->scene->object(i);
                if ((obj == NULL) || (!obj->is_visible()))
                    continue;
                for (size_t j=0,m=obj->num_triangles(); j<m; ++j)
                    ctx->shared->view->add_triangle_3c(obj->triangle(j), &C_RED, &C_GREEN, &C_BLUE);
            }
        )

        // Check for crossing with all bounding boxes
        for (size_t i=0, n=ctx->shared->scene->num_objects(); i<n; ++i)
        {
            Object3D *obj = ctx->shared->scene->object(i);
            if (obj == NULL)
                return STATUS_BAD_STATE;
            else if (!obj->is_visible()) // Skip invisible objects
                continue;

            // Ensure that we need to add the object to queue
            if (obj->num_triangles() >= 16)
            {
                matrix3d_t *m = obj->matrix();
                bound_box3d_t box = *(obj->bound_box());
                for (size_t j=0; j<8; ++j)
                    dsp::apply_matrix3d_mp1(&box.p[i], m);

                // Skip object if view is not crossing bounding-box
                RT_TRACE_BREAK(ctx,
                    lsp_trace("Testing bound box");

                    v_vertex3d_t v[3];
                    for (size_t j=0, m = sizeof(bbox_map)/sizeof(size_t); j < m; )
                    {
                        v[0].p      = box.p[bbox_map[j++]];
                        v[0].c      = C_YELLOW;
                        v[1].p      = box.p[bbox_map[j++]];
                        v[1].c      = C_YELLOW;
                        v[2].p      = box.p[bbox_map[j++]];
                        v[2].c      = C_YELLOW;

                        dsp::calc_normal3d_p3(&v[0].n, &v[0].p, &v[1].p, &v[2].p);
                        v[1].n      = v[0].n;
                        v[2].n      = v[0].n;

                        ctx->shared->view->add_triangle(v);
                    }
                )

                if (!check_bound_box(obj->bound_box(), &ctx->view))
                {
                    RT_TRACE(
                        matrix3d_t *mx = obj->matrix();

                        for (size_t j=0,m=obj->num_triangles(); j<m; ++j)
                        {
                            obj_triangle_t *st = obj->triangle(j);

                            v_triangle3d_t t;
                            dsp::apply_matrix3d_mp2(&t.p[0], st->v[0], mx);
                            dsp::apply_matrix3d_mp2(&t.p[1], st->v[1], mx);
                            dsp::apply_matrix3d_mp2(&t.p[2], st->v[2], mx);

                            dsp::apply_matrix3d_mv2(&t.n[0], st->n[0], mx);
                            dsp::apply_matrix3d_mv2(&t.n[1], st->n[1], mx);
                            dsp::apply_matrix3d_mv2(&t.n[2], st->n[2], mx);

                            ctx->shared->ignored.add(&t);
                        }
                    );

                    continue;
                }
            }

            // Add object to context
            res = ctx->add_object(obj);
            if (res != STATUS_OK)
                break;
#ifdef LSP_DEBUG
            if (!ctx->validate())
                return STATUS_BAD_STATE;
#endif /* LSP_DEBUG */
        }

#ifdef LSP_DEBUG
        if (!ctx->shared->scene->validate())
            return STATUS_CORRUPTED;
#endif /* LSP_DEBUG */

        // Update state
        ctx->state      = S_FILTER_VIEW;
        return (tasks.push(ctx)) ? STATUS_OK : STATUS_NO_MEM;
    }

    status_t filter_view(cvector<rt_context_t> &tasks, rt_context_t *ctx)
    {
        status_t res;
        vector3d_t pl[4]; // Split plane
        RT_TRACE(v_triangle3d_t npt[4]); // Split plane presentation

        RT_TRACE(
            // Split edges
            if (!ctx->validate())
                return STATUS_BAD_STATE;
            if (!ctx->shared->scene->validate())
                return STATUS_CORRUPTED;
        )

        dsp::calc_plane_p3(&pl[0], &ctx->view.p[0], &ctx->view.p[1], &ctx->view.p[2]);
        dsp::calc_plane_p3(&pl[1], &ctx->view.s, &ctx->view.p[0], &ctx->view.p[1]);
        dsp::calc_plane_p3(&pl[2], &ctx->view.s, &ctx->view.p[1], &ctx->view.p[2]);
        dsp::calc_plane_p3(&pl[3], &ctx->view.s, &ctx->view.p[2], &ctx->view.p[0]);

        RT_TRACE(
            init_triangle_p3(&npt[0], &ctx->view.p[0], &ctx->view.p[1], &ctx->view.p[2], &pl[0]);
            init_triangle_p3(&npt[1], &ctx->view.s, &ctx->view.p[0], &ctx->view.p[1], &pl[1]);
            init_triangle_p3(&npt[2], &ctx->view.s, &ctx->view.p[1], &ctx->view.p[2], &pl[2]);
            init_triangle_p3(&npt[3], &ctx->view.s, &ctx->view.p[2], &ctx->view.p[0], &pl[3]);
        );

        rt_context_t in(ctx->shared);
        RT_TRACE(
            rt_context_t out(ctx->shared);
        )

        for (size_t pi=0; pi<4; ++pi)
        {
            RT_TRACE_BREAK(ctx,
                lsp_trace("Filtering space with view plane #%d", int(pi));

                for (size_t j=0, n=ctx->triangle.size(); j<n; ++j)
                   ctx->shared->view->add_triangle_1c(ctx->triangle.get(j), &C_DARKGREEN);

                ctx->shared->view->add_triangle_pv1c(ctx->view.p, &C_MAGENTA);
                ctx->shared->view->add_plane_pv1c(npt[pi].p, &C_YELLOW);
            )

#ifdef LSP_DEBUG
            res = ctx->filter(&out, &in, &pl[pi]);
#else
            res = ctx->filter(NULL, &in, &pl[pi]);
#endif /* LSP_DEBUG */
            if (res != STATUS_OK)
                return res;

            RT_TRACE_BREAK(ctx,
                lsp_trace("Data after filtering (%d triangles)", int(in.triangle.size()));
                for (size_t j=0,n=in.triangle.size(); j<n; ++j)
                    ctx->shared->view->add_triangle_3c(in.triangle.get(j), &C_CYAN, &C_MAGENTA, &C_YELLOW);
            );

            RT_TRACE(
                // Add set of triangles to ignored
                for (size_t i=0,n=out.triangle.size(); i<n; ++i)
                    ctx->ignore(out.triangle.get(i));
            );

            // Swap content
            ctx->swap(&in);
            if (ctx->triangle.size() <= 0)
            {
                delete ctx;
                return STATUS_OK;
            }
        }

        // Change state and submit to queue
        if (ctx->triangle.size() <= 1)
        {
            if (ctx->triangle.size() == 0)
            {
                delete ctx;
                return STATUS_OK;
            }
            ctx->state      = S_REFLECT;
        }
        else
            ctx->state      = S_CULL_VIEW;

        return (tasks.push(ctx)) ? STATUS_OK : STATUS_NO_MEM;
    }

    status_t cull_view(cvector<rt_context_t> &tasks, rt_context_t *ctx)
    {
        status_t res;
        vector3d_t pl[4]; // Split plane
        RT_TRACE(v_triangle3d_t npt[4]); // Split plane presentation

        RT_TRACE(
            // Split edges
            if (!ctx->validate())
                return STATUS_BAD_STATE;
            if (!ctx->shared->scene->validate())
                return STATUS_CORRUPTED;
        )

        dsp::calc_plane_p3(&pl[0], &ctx->view.p[0], &ctx->view.p[1], &ctx->view.p[2]);
        dsp::calc_plane_p3(&pl[1], &ctx->view.s, &ctx->view.p[0], &ctx->view.p[1]);
        dsp::calc_plane_p3(&pl[2], &ctx->view.s, &ctx->view.p[1], &ctx->view.p[2]);
        dsp::calc_plane_p3(&pl[3], &ctx->view.s, &ctx->view.p[2], &ctx->view.p[0]);

        RT_TRACE(
            init_triangle_p3(&npt[0], &ctx->view.p[0], &ctx->view.p[1], &ctx->view.p[2], &pl[0]);
            init_triangle_p3(&npt[1], &ctx->view.s, &ctx->view.p[0], &ctx->view.p[1], &pl[1]);
            init_triangle_p3(&npt[2], &ctx->view.s, &ctx->view.p[1], &ctx->view.p[2], &pl[2]);
            init_triangle_p3(&npt[3], &ctx->view.s, &ctx->view.p[2], &ctx->view.p[0], &pl[3]);
        );

        rt_context_t in(ctx->shared);
        RT_TRACE(
            rt_context_t out(ctx->shared);
        )

        for (size_t pi=0; pi<4; ++pi)
        {
            RT_TRACE_BREAK(ctx,
                lsp_trace("Culling space with view plane #%d", int(pi));

                for (size_t j=0, n=ctx->triangle.size(); j<n; ++j)
                   ctx->shared->view->add_triangle_1c(ctx->triangle.get(j), &C_DARKGREEN);

                ctx->shared->view->add_triangle_pv1c(ctx->view.p, &C_MAGENTA);
                ctx->shared->view->add_plane_pv1c(npt[pi].p, &C_YELLOW);
            )

#ifdef LSP_DEBUG
            res = ctx->split(&out, &in, &pl[pi]);
#else
            res = ctx->split(NULL, &in, &pl[pi]);
#endif /* LSP_DEBUG */
            if (res != STATUS_OK)
                return res;

            RT_TRACE(
                if (!ctx->shared->scene->validate())
                    return STATUS_CORRUPTED;
                if (!ctx->validate())
                    return STATUS_BAD_STATE;
                if (!out.validate())
                    return STATUS_BAD_STATE;
                if (!in.validate())
                    return STATUS_BAD_STATE;

                // Add set of triangles to ignored
                for (size_t j=0,n=out.triangle.size(); j<n; ++j)
                    ctx->ignore(out.triangle.get(j));
            );

            RT_TRACE_BREAK(ctx,
                lsp_trace("Data after culling (%d triangles)", int(in.triangle.size()));
                for (size_t j=0,n=in.triangle.size(); j<n; ++j)
                    ctx->shared->view->add_triangle_3c(in.triangle.get(j), &C_CYAN, &C_MAGENTA, &C_YELLOW);
            );

            // Check that there is data for processing and take it for next iteration
            ctx->swap(&in);
            if (ctx->triangle.size() <= 0)
                break;
        }

        // Change state and submit to queue
        if (ctx->triangle.size() <= 1)
        {
            if (ctx->triangle.size() == 0)
            {
                delete ctx;
                return STATUS_OK;
            }
            ctx->state      = S_REFLECT;
        }
        else
            ctx->state      = S_PARTITION;

        return (tasks.push(ctx)) ? STATUS_OK : STATUS_NO_MEM;
    }

    status_t partition_view(cvector<rt_context_t> &tasks, rt_context_t *ctx)
    {
        if (ctx->triangle.size() > 0)
        {
            rt_context_t *nctx[3];
            rt_context_t ign(ctx->shared), in(ctx->shared);

            // Create contexts
            for (size_t i=0; i<3; ++i)
            {
                nctx[i]     = new rt_context_t(ctx->shared);
                if (nctx == NULL)
                    return STATUS_NO_MEM;
                else if (!tasks.add(nctx[i]))
                {
                    delete nctx[i];
                    return STATUS_NO_MEM;
                }

                nctx[i]->view   = ctx->view;
                nctx[i]->state  = ctx->state;
            }

            status_t res = ctx->partition(nctx, &ign, &in);
            if (res != STATUS_OK)
                return res;

            // Debug
            RT_TRACE(
                for (size_t i=0,n=ign.triangle.size(); i<n; ++i)
                    ctx->ignore(ign.triangle.get(i));
            )

            // Change state and submit to queue
            ctx->swap(&in);
        }

        // Change state of current context if needed
        if (ctx->triangle.size() <= 1)
        {
            if (ctx->triangle.size() == 0)
            {
                delete ctx;
                return STATUS_OK;
            }
            ctx->state      = S_REFLECT;
        }

        return (tasks.push(ctx)) ? STATUS_OK : STATUS_NO_MEM;
    }

    static status_t dump_view(cvector<rt_context_t> &tasks, rt_context_t *ctx)
    {
        // DEBUG
        for (size_t i=0,n=ctx->triangle.size(); i<n; ++i)
            ctx->match(ctx->triangle.get(i));
        for (size_t i=0,n=ctx->edge.size(); i<n; ++i)
        {
            rt_edge_t *e = ctx->edge.get(i);
            if (e->itag & RT_EF_PLANE)
                ctx->shared->view->add_segment(e, &C_YELLOW);
        }

        delete ctx;
        return STATUS_OK;
    }

    status_t reflect_view(cvector<rt_context_t> &tasks, rt_context_t *ctx)
    {
        // DEBUG
        for (size_t i=0,n=ctx->triangle.size(); i<n; ++i)
            ctx->match(ctx->triangle.get(i));
        for (size_t i=0,n=ctx->edge.size(); i<n; ++i)
        {
            rt_edge_t *e = ctx->edge.get(i);
            if (e->itag & RT_EF_PLANE)
                ctx->shared->view->add_segment(e, &C_YELLOW);
        }

        delete ctx;
        return STATUS_OK;
    }

    status_t perform_raytrace(cvector<rt_context_t> &tasks)
    {
        rt_context_t *ctx = NULL;
        status_t res = STATUS_OK;

        while (tasks.size() > 0)
        {
            // Get next context from queue
            if (!tasks.pop(&ctx))
                return STATUS_CORRUPTED;

            // Check that we need to perform a scan
            switch (ctx->state)
            {
                case S_SCAN_OBJECTS:
                    res = scan_objects(tasks, ctx);
                    break;
                case S_FILTER_VIEW:
                    res = filter_view(tasks, ctx);
                    break;
                case S_CULL_VIEW:
                    res = cull_view(tasks, ctx);
                    break;
                case S_PARTITION:
                    res = partition_view(tasks, ctx);
//                    res = dump_view(tasks, ctx);
                    break;
                case S_CUTOFF:
//                    res = cutoff_view(tasks, ctx);
                    res = dump_view(tasks, ctx);
                    break;
                case S_REFLECT:
//                    res = reflect_view(tasks, ctx);
                    res = dump_view(tasks, ctx);
                    break;
                default:
                    res = STATUS_BAD_STATE;
                    break;
            }

            // Analyze status
            if (res != STATUS_OK)
            {
                delete ctx;
                break;
            }
        }

        destroy_tasks(tasks);
        return res;
    }
} // Namespace mtest

MTEST_BEGIN("3d", reflections)

    class Renderer: public X11Renderer
    {
        private:
            Scene3D        *pScene;
            rt_view_t       sFront;
            ssize_t         nTrace;
            bool            bBoundBoxes;
            bool            bDrawFront;

        public:
            explicit Renderer(Scene3D *scene, View3D *view): X11Renderer(view)
            {
                pScene = scene;
                bBoundBoxes = false;
                bDrawFront  = true;
                nTrace = BREAKPOINT_STEP;

                INIT_FRONT(sFront);

                update_view();
            }

            virtual ~Renderer()
            {
            }

        public:
            virtual void on_key_press(const XKeyEvent &ev, KeySym key)
            {
                switch (key)
                {
                    case XK_F1:
                    {
                        float incr = (ev.state & ShiftMask) ? 0.25f : -0.25f;
                        sFront.p[0].x += incr;
                        sFront.p[1].x += incr;
                        sFront.p[2].x += incr;
                        sFront.s.x += incr;
                        update_view();
                        break;
                    }

                    case XK_F2:
                    {
                        float incr = (ev.state & ShiftMask) ? 0.25f : -0.25f;
                        sFront.p[0].y += incr;
                        sFront.p[1].y += incr;
                        sFront.p[2].y += incr;
                        sFront.s.y += incr;
                        update_view();
                        break;
                    }

                    case XK_F3:
                    {
                        float incr = (ev.state & ShiftMask) ? 0.25f : -0.25f;
                        sFront.p[0].z += incr;
                        sFront.p[1].z += incr;
                        sFront.p[2].z += incr;
                        sFront.s.z += incr;
                        update_view();
                        break;
                    }

                    case XK_F4:
                    case XK_F5:
                    case XK_F6:
                    {
                        matrix3d_t m;
                        float incr = (ev.state & ShiftMask) ? M_PI/16.0f : -M_PI/16.0f;

                        for (size_t i=0; i<3; ++i)
                        {
                            sFront.p[i].x -= sFront.s.x;
                            sFront.p[i].y -= sFront.s.y;
                            sFront.p[i].z -= sFront.s.z;
                        }
                        if (key == XK_F4)
                            dsp::init_matrix3d_rotate_x(&m, incr);
                        else if (key == XK_F5)
                            dsp::init_matrix3d_rotate_y(&m, incr);
                        else
                            dsp::init_matrix3d_rotate_z(&m, incr);
                        for (size_t i=0; i<3; ++i)
                            dsp::apply_matrix3d_mp1(&sFront.p[i], &m);
                        for (size_t i=0; i<3; ++i)
                        {
                            sFront.p[i].x += sFront.s.x;
                            sFront.p[i].y += sFront.s.y;
                            sFront.p[i].z += sFront.s.z;
                        }
                        update_view();
                        break;
                    }

                    case XK_Up:
                        nTrace++;
                        lsp_trace("Set trace breakpoint to %d", int(nTrace));
                        update_view();
                        break;
                    case XK_Down:
                        if (nTrace >= 0)
                        {
                            nTrace--;
                            lsp_trace("Set trace breakpoint to %d", int(nTrace));
                            update_view();
                        }
                        break;

                    case 'f':
                        bDrawFront = ! bDrawFront;
                        update_view();
                        break;

                    case 'b':
                    {
                        bBoundBoxes = ! bBoundBoxes;
                        update_view();
                        break;
                    }

                    case '0': case '1': case '2': case '3': case '4':
                    case '5': case '6': case '7': case '8': case '9':
                    {
                        Object3D *obj = pScene->get_object(key - '0');
                        if (obj != NULL)
                        {
                            obj->set_visible(!obj->is_visible());
                            update_view();
                        }
                        break;
                    }
                    default:
                        X11Renderer::on_key_press(ev, key);
                        break;
                }
            }

        protected:
            status_t    update_view()
            {
                v_segment3d_t s;
                v_vertex3d_t v[3];
                status_t res = STATUS_OK;

                if (!pScene->validate())
                    return STATUS_BAD_STATE;

                // Clear view state
                pView->clear_all();

                // List of ignored and matched triangles
                rt_shared_t global;
                global.breakpoint   = nTrace;
                global.step         = 0;
                global.scene        = pScene;
                global.view         = pView;

                cvector<rt_context_t> tasks;

                // Create initial context
                rt_context_t *ctx = new rt_context_t(&global);
                if (ctx == NULL)
                    return STATUS_NO_MEM;

                ctx->state          = S_SCAN_OBJECTS;
                ctx->view           = sFront;

                // Add context to tasks
                if (!tasks.add(ctx))
                {
                    delete ctx;
                    return STATUS_NO_MEM;
                }

                // Render bounding boxes of the scene
                if (bBoundBoxes)
                {
                    s.c[0] = C_ORANGE;
                    s.c[1] = C_ORANGE;
                    for (size_t i=0, n=global.scene->num_objects(); i<n; ++i)
                    {
                        Object3D *o = global.scene->object(i);
                        matrix3d_t *m = o->matrix();
                        bound_box3d_t *pmbox = o->bound_box();
                        bound_box3d_t bbox;

                        for (size_t i=0; i<8; ++i)
                            dsp::apply_matrix3d_mp2(&bbox.p[i], &pmbox->p[i], m);

                        for (size_t i=0; i<4; ++i)
                        {
                            s.p[0] = bbox.p[i];
                            s.p[1] = bbox.p[(i+1)%4];
                            pView->add_segment(&s);
                            s.p[0] = bbox.p[i];
                            s.p[1] = bbox.p[i+4];
                            pView->add_segment(&s);
                            s.p[0] = bbox.p[i+4];
                            s.p[1] = bbox.p[(i+1)%4 + 4];
                            pView->add_segment(&s);
                        }
                    }
                }

                // Clear allocated resources, tasks and ctx should be already deleted
                res = perform_raytrace(tasks);
                if (res == STATUS_BREAKPOINT) // This status is used for immediately returning from traced code
                    res = STATUS_OK;

                if (!pScene->validate())
                    return STATUS_BAD_STATE;

                // Build final scene from matched and ignored items
                for (size_t i=0, m=global.ignored.size(); i < m; ++i)
                    pView->add_triangle_1c(global.ignored.at(i), &C_GRAY);

                for (size_t i=0, m=global.matched.size(); i < m; ++i)
                {
                    v_triangle3d_t *t = global.matched.at(i);
                    v[0].p     = t->p[0];
                    v[0].n     = t->n[0];
                    v[0].c     = C_RED;

                    v[1].p     = t->p[1];
                    v[1].n     = t->n[1];
                    v[1].c     = C_GREEN;

                    v[2].p     = t->p[2];
                    v[2].n     = t->n[2];
                    v[2].c     = C_BLUE;

                    pView->add_triangle(v);
                }

                global.ignored.flush();
                global.matched.flush();

                // Calc scissor planes' normals
                vector3d_t pl[4];
                dsp::calc_plane_p3(&pl[0], &sFront.s, &sFront.p[0], &sFront.p[1]);
                dsp::calc_plane_p3(&pl[1], &sFront.s, &sFront.p[1], &sFront.p[2]);
                dsp::calc_plane_p3(&pl[2], &sFront.s, &sFront.p[2], &sFront.p[0]);
                dsp::calc_plane_p3(&pl[3], &sFront.p[0], &sFront.p[1], &sFront.p[2]);

                // Draw front
                if (bDrawFront)
                {
                    v_ray3d_t r;
                    s.c[0] = C_MAGENTA;
                    s.c[1] = C_MAGENTA;

                    for (size_t i=0; i<3; ++i)
                    {
                        // State
                        r.p = sFront.p[i];
                        dsp::init_vector_p2(&r.v, &sFront.s, &r.p);
                        r.c = C_MAGENTA;
                        pView->add_ray(&r);

                        s.p[0] = sFront.s;
                        s.p[1] = sFront.p[i];
                        pView->add_segment(&s);

                        s.p[0] = sFront.p[(i+1)%3];
                        pView->add_segment(&s);
                    }
                }

                return res;
            }
    };

    MTEST_MAIN
    {
        const char *scene_file = (argc < 1) ? "res/test/3d/cross.obj" : argv[0];

        // Load scene
        Scene3D s;
        View3D v;
        status_t res = Model3DFile::load(&s, scene_file, true);
        MTEST_ASSERT_MSG(res == STATUS_OK, "Error loading scene from file %s", scene_file);

        // Initialize renderer
        Renderer r(&s, &v);
        MTEST_ASSERT_MSG(r.init() == STATUS_OK, "Error initializing renderer");
        r.run();
        r.destroy();

        // Destroy scene
        s.destroy();
    }

MTEST_END

