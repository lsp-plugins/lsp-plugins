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
//    #define BREAKPOINT_STEP     -1
    #define BREAKPOINT_STEP     0
//    #define BREAKPOINT_STEP     231

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
                    dsp::split_triangle_raw(out, &n_out, in, n_in, &pl[k], &q[l]);
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
            res = ctx->add_object(obj, NULL);
            if (res != STATUS_OK)
                break;

            RT_TRACE(
                if (!ctx->validate())
                    return STATUS_BAD_STATE;
            )
        }

        RT_TRACE(
            if (!ctx->shared->scene->validate())
                return STATUS_CORRUPTED;
        )

        res = ctx->solve_conflicts();
        if (res != STATUS_OK)
            return res;

        // Update state
        ctx->state      = S_CULL_VIEW;
        return (tasks.push(ctx)) ? STATUS_OK : STATUS_NO_MEM;
    }

    status_t cull_view(cvector<rt_context_t> &tasks, rt_context_t *ctx)
    {
        status_t res = ctx->cull_view();
        if (res != STATUS_OK)
            return res;

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
        {
            res     = ctx->sort_edges();
            if (res != STATUS_OK)
                return res;
            ctx->state      = S_SPLIT;
        }

        return (tasks.push(ctx)) ? STATUS_OK : STATUS_NO_MEM;
    }

    status_t split_view(cvector<rt_context_t> &tasks, rt_context_t *ctx)
    {
        rt_context_t out(ctx->shared);

        // Perform binary split
        status_t res = ctx->edge_split(&out);
        if (res == STATUS_NOT_FOUND)
        {
            ctx->state      = S_CULL_BACK;
            return (tasks.push(ctx)) ? STATUS_OK : STATUS_NO_MEM;
        }
        else if (res != STATUS_OK)
            return res;

        // Analyze state of current and out context
        if (ctx->triangle.size() > 0)
        {
            // Analyze state of 'out' context
            if (out.triangle.size() > 0)
            {
                // Allocate additional context and add to task list
                rt_context_t *nctx = new rt_context_t(ctx->shared);
                if (nctx == NULL)
                    return STATUS_NO_MEM;
                else if (!tasks.push(nctx))
                {
                    delete nctx;
                    return STATUS_NO_MEM;
                }

                nctx->swap(&out);
                nctx->view  = ctx->view;
                nctx->state = (nctx->triangle.size() > 1) ? S_SPLIT : S_REFLECT;
            }

            return (tasks.push(ctx)) ? STATUS_OK : STATUS_NO_MEM;
        }
        else if (out.triangle.size() > 0)
        {
            ctx->swap(&out);
            ctx->state  = (ctx->triangle.size() > 1) ? S_SPLIT : S_REFLECT;

            return (tasks.push(ctx)) ? STATUS_OK : STATUS_NO_MEM;
        }

        delete ctx;
        return STATUS_OK;
    }

    status_t cullback_view(cvector<rt_context_t> &tasks, rt_context_t *ctx)
    {
        // Perform cullback
        status_t res = ctx->depth_cullback();
        if (res != STATUS_OK)
            return res;
        if (ctx->triangle.size() <= 0)
        {
            delete ctx;
            return STATUS_OK;
        }
        ctx->state  = S_REFLECT;

        return (tasks.push(ctx)) ? STATUS_OK : STATUS_NO_MEM;
    }

    status_t dump_view(cvector<rt_context_t> &tasks, rt_context_t *ctx)
    {
        // DEBUG
        for (size_t i=0,n=ctx->triangle.size(); i<n; ++i)
            ctx->match(ctx->triangle.get(i));
//        for (size_t i=0,n=ctx->edge.size(); i<n; ++i)
//        {
//            rt_edge_t *e = ctx->edge.get(i);
//            if (e->itag & RT_EF_PLANE)
//                ctx->shared->view->add_segment(e, &C_YELLOW);
//        }

        delete ctx;
        return STATUS_OK;
    }

//    static void project_triangle(
//        point3d_t *pv,
//        const point3d_t *fp,
//        const vector3d_t *pl,
//        const point3d_t *tv
//    )
//    {
//
//    }

    void rearrange_view(rt_view_t *v)
    {
        float d[3], tt;
        point3d_t tp;
        d[0]    = dsp::calc_sqr_distance_pv(&v->p[0]);
        d[1]    = dsp::calc_sqr_distance_pv(&v->p[1]);
        d[2]    = dsp::calc_sqr_distance_p2(&v->p[0], &v->p[2]);

        if ((d[0] >= d[1]) && (d[0] >= d[2])) // All is OK
            return;

        if (d[1] >= d[2]) // Rotate clockwise
        {
            tp          = v->p[0];
            v->p[0]     = v->p[1];
            v->p[1]     = v->p[2];
            v->p[2]     = tp;

            tt          = v->time[0];
            v->time[0]  = v->time[1];
            v->time[1]  = v->time[2];
            v->time[2]  = tt;
        }
        else // Rotate counter-clockwise
        {
            tp          = v->p[2];
            v->p[2]     = v->p[1];
            v->p[1]     = v->p[0];
            v->p[0]     = tp;

            tt          = v->time[2];
            v->time[2]  = v->time[1];
            v->time[1]  = v->time[0];
            v->time[0]  = tt;
        }
    }

    status_t reflect_view(cvector<rt_context_t> &tasks, rt_context_t *ctx)
    {
#if 1
        rt_view_t v, sv;
        point3d_t sp[2];                // focus point, split points
        vector3d_t vpl, spl;            // view plane, split plane
        float ksa, kt[2], t[2];         // area multiplier coefficient, projection coefficient, time coefficients
        float d[3];                     // distance

        sv      = ctx->view;
        rearrange_view(&sv);            // Make edge 0 of the view the longest possible

        dsp::calc_plane_pv(&vpl, ctx->view.p);
        ksa     = 1.0f / dsp::calc_area_pv(sv.p);

        status_t res    = STATUS_OK;

        for (size_t i=0,n=ctx->triangle.size(); i<n; ++i)
        {
            rt_triangle_t *ct = ctx->triangle.get(i);
            ctx->match(ctx->triangle.get(i));

            // get material
            rt_material_t *m    = ct->m;
            if (m == NULL)
                continue;

            v.face          = ct->face;

            // Project all triangle's points to view points
            dsp::calc_split_point_p2v1(&v.p[0], &sv.s, ct->v[0], &vpl);
            dsp::calc_split_point_p2v1(&v.p[1], &sv.s, ct->v[1], &vpl);
            dsp::calc_split_point_p2v1(&v.p[2], &sv.s, ct->v[2], &vpl);
            d[0]    = dsp::calc_distance_p2(&v.p[0], ct->v[0]);
            d[1]    = dsp::calc_distance_p2(&v.p[0], ct->v[0]);
            d[2]    = dsp::calc_distance_p2(&v.p[0], ct->v[0]);

            // Estimate the start time for each view point
            for (size_t j=0; j<3; ++j)
            {
                dsp::calc_parallel_plane_p2p2(&spl, &sv.s, &v.p[j], &ctx->view.p[0], &ctx->view.p[1]);
                dsp::calc_split_point_p2v1(&sp[0], &sv.p[0], &sv.p[2], &spl);
                dsp::calc_split_point_p2v1(&sp[1], &sv.p[1], &sv.p[2], &spl);

                kt[0]       = dsp::projection_length_p2(&sv.p[0], &sv.p[2], &sp[0]);
                kt[1]       = dsp::projection_length_p2(&sv.p[1], &sv.p[2], &sp[1]);
                t[0]        = sv.time[0] + (sv.time[2] - sv.time[0]) * kt[0];
                t[1]        = sv.time[1] + (sv.time[2] - sv.time[1]) * kt[1];

                kt[0]       = dsp::projection_length_p2(&sp[0], &sp[1], &v.p[j]);
                v.time[j]   = t[0] + (t[1]-t[0])*kt[0] + d[j]*sv.speed;
            }

            // Determine the direction from which came the wave front
            float energy    = ksa * dsp::calc_area_pv(sv.p);
            float distance  = sv.s.x * ct->n.dx + sv.s.y * ct->n.dy + sv.s.z * ct->n.dz + ct->n.dw;

            if (distance > 0.0f)
            {
                // Call capturing routine if present
                if ((m->capture != NULL) && ((v.energy <= -DSP_3D_TOLERANCE) || (v.energy >= DSP_3D_TOLERANCE)))
                {
                    v.s             = sv.s;
                    v.energy        = energy * m->absorption[0];
                    res             = m->capture(&v, m->capture_data);
                    if (res != STATUS_OK)
                        break;
                }
                energy         *= (1.0f - m->absorption[0]);

                // Perform refraction
                v.energy        = energy * m->transparency[0];
                if ((v.energy <= -DSP_3D_TOLERANCE) || (v.energy >= DSP_3D_TOLERANCE))
                {
                    v.speed         = m->permeability / sv.speed;   // (Vair / Vmet) / (Vair / Vair) = Vair / Vmet
                    float kd        = m->dissipation[0] * (m->permeability - 1.0f) * distance;
                    v.s.x           = sv.s.x + kd * ct->n.dx;
                    v.s.y           = sv.s.y + kd * ct->n.dy;
                    v.s.z           = sv.s.z + kd * ct->n.dz;
                    v.s.w           = 1.0f;

                    rt_context_t *rc = new rt_context_t(&v, ctx->shared);
                    if ((rc == NULL) || (!tasks.add(rc)))
                        res     = STATUS_NO_MEM;
                    if (res != STATUS_OK)
                    {
                        delete rc;
                        break;
                    }
                }

                // Perform reflection
                v.energy        = energy * (m->transparency[0] - 1.0f); // Energy will negate sign
                if ((v.energy <= -DSP_3D_TOLERANCE) || (v.energy >= DSP_3D_TOLERANCE))
                {
                    v.speed         = sv.speed;
                    float kd        = (1.0f + 1.0f / m->dispersion[0]) * distance; // m->dispersion[0] * distance;
                    v.s.x           = sv.s.x - kd * ct->n.dx;
                    v.s.y           = sv.s.y - kd * ct->n.dy;
                    v.s.z           = sv.s.z - kd * ct->n.dz;
                    v.s.w           = 1.0f;

                    rt_context_t *rc = new rt_context_t(&v, ctx->shared);
                    if ((rc == NULL) || (!tasks.add(rc)))
                        res     = STATUS_NO_MEM;
                    if (res != STATUS_OK)
                    {
                        delete rc;
                        break;
                    }
                }
            }
            else
            {
                // Call capturing routine if present
                if ((m->capture != NULL) && ((v.energy <= -DSP_3D_TOLERANCE) || (v.energy >= DSP_3D_TOLERANCE)))
                {
                    v.s             = sv.s;
                    v.energy        = energy * m->absorption[0];
                    res             = m->capture(&v, m->capture_data);
                    if (res != STATUS_OK)
                        break;
                }
                energy         *= (1.0f - m->absorption[0]);

                // Perform refraction
                v.energy        = energy * m->transparency[1];
                if ((v.energy <= -DSP_3D_TOLERANCE) || (v.energy >= DSP_3D_TOLERANCE))
                {
                    v.speed         = sv.speed / m->permeability;   // (Vair / Vmet) / (Vair / Vmet) = Vair / Vair

                    float kd        = m->dissipation[1] * (m->permeability - 1.0f) * distance;
                    v.s.x           = sv.s.x - kd * ct->n.dx;
                    v.s.y           = sv.s.y - kd * ct->n.dy;
                    v.s.z           = sv.s.z - kd * ct->n.dz;
                    v.s.w           = 1.0f;

                    rt_context_t *rc = new rt_context_t(&v, ctx->shared);
                    if ((rc == NULL) || (!tasks.add(rc)))
                        res     = STATUS_NO_MEM;
                    if (res != STATUS_OK)
                    {
                        delete rc;
                        break;
                    }
                }

                // Perform reflection
                v.energy        = energy * (m->transparency[1] - 1.0f); // Energy will negate sign
                if ((v.energy <= -DSP_3D_TOLERANCE) || (v.energy >= DSP_3D_TOLERANCE))
                {
                    v.speed         = sv.speed;
                    float kd        = (1.0f + 1.0f / m->dispersion[1]) * distance; // m->dispersion[0] * distance;
                    v.s.x           = sv.s.x + kd * ct->n.dx;
                    v.s.y           = sv.s.y + kd * ct->n.dy;
                    v.s.z           = sv.s.z + kd * ct->n.dz;
                    v.s.w           = 1.0f;

                    rt_context_t *rc = new rt_context_t(&v, ctx->shared);
                    if ((rc == NULL) || (!tasks.add(rc)))
                        res     = STATUS_NO_MEM;
                    if (res != STATUS_OK)
                    {
                        delete rc;
                        break;
                    }
                }
            }
        }

        delete ctx;
        return res;
#else
        return dump_view(tasks, ctx);
#endif
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
                case S_CULL_VIEW:
                    res = cull_view(tasks, ctx);
                    break;
                case S_SPLIT:
                    res = split_view(tasks, ctx);
                    break;
                case S_CULL_BACK:
                    res = cullback_view(tasks, ctx);
                    break;
                case S_REFLECT:
                    res = reflect_view(tasks, ctx);
                    break;
                case S_IGNORE:
                    RT_TRACE(
                        for (size_t i=0,n=ctx->triangle.size(); i<n; ++i)
                            ctx->ignore(ctx->triangle.get(i));
                    )
                    delete ctx;
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
            bool            bDrawMatched;
            bool            bDrawIgnored;

        public:
            explicit Renderer(Scene3D *scene, View3D *view): X11Renderer(view)
            {
                pScene = scene;
                bBoundBoxes = false;
                bDrawFront  = true;
                bDrawMatched = true;
                bDrawIgnored = true;
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

                    case 'm':
                    {
                        bDrawMatched = ! bDrawMatched;
                        update_view();
                        break;
                    }

                    case 'i':
                    {
                        bDrawIgnored = ! bDrawIgnored;
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
                ctx->init_view(&sFront.s, sFront.p);

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
                if (bDrawIgnored)
                {
                    for (size_t i=0, m=global.ignored.size(); i < m; ++i)
                        pView->add_triangle_1c(global.ignored.at(i), &C_GRAY);
                }

                if (bDrawMatched)
                {
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

