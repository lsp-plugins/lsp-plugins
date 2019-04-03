/*
 * RayTrace3D.cpp
 *
 *  Created on: 9 апр. 2017 г.
 *      Author: sadko
 */

#include <core/alloc.h>
#include <core/3d/common.h>
#include <core/3d/RayTrace3D.h>

#define SAMPLE_QUANTITY     512
#define TASK_LO_THRESH      1024
#define TASK_HI_THRESH      8192

namespace lsp
{
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


    RayTrace3D::TaskThread::TaskThread(RayTrace3D *trace)
    {
        this->trace     = trace;
        heavy_state     = S_SCAN_OBJECTS;
    }

    RayTrace3D::TaskThread::~TaskThread()
    {
        factory.clear();

        // Cleanup capture state
        for (size_t i=0; i<captures.size(); ++i)
        {
            capture_t *cap = captures.get(i);

            // Cleanup bindings
            for (size_t j=0; j<cap->bindings.size(); ++j)
            {
                // Cleanup sample
                sample_t *samp  = cap->bindings.get(j);

                if (samp->sample != NULL)
                {
                    samp->sample->destroy();
                    delete samp->sample;
                    samp->sample = NULL;
                }
            }

            cap->bindings.first();
            delete cap;
        }

        captures.flush();
    }

    status_t RayTrace3D::TaskThread::run()
    {
        clear_stats(&stats);
        status_t res = main_loop();
        destroy_tasks(&tasks);
        return res;
    }

    status_t RayTrace3D::TaskThread::main_loop()
    {
        rt_context_t *ctx   = NULL;
        bool report         = false;
        status_t res        = STATUS_OK;

        // Perform main loop of raytracing
        while (true)
        {
            // Check cancellation flag
            if ((trace->bCancelled) || (trace->bFailed))
            {
                res = STATUS_CANCELLED;
                break;
            }

            // Try to fetch new task from internal queue
            if (!tasks.pop(&ctx))
            {
                // Are there any tasks left in global space?
                trace->lkTasks.lock();
                if (!trace->vTasks.pop(&ctx))
                {
                    trace->lkTasks.unlock();
                    break;
                }

                // Update statistics
                if (trace->nQueueSize > trace->vTasks.size())
                {
                    report      = true;
                    trace->nQueueSize  = trace->vTasks.size();
                }
                ++stats.root_tasks;
                trace->lkTasks.unlock();
            }
            else
                ++stats.local_tasks;

            if (ctx == NULL)
                break;

            // Process context state
            res     = process_context(ctx);

            // Report status if required
            if ((res == STATUS_OK) && (report))
            {
                report      = false;

                trace->lkTasks.lock();
                float prg   = float(trace->nProgressPoints) / float(trace->nProgressMax);
                lsp_trace("Reporting progress %d/%d = %.2f%%", int(trace->nProgressPoints), int(trace->nProgressMax), prg * 100.0f);
                ++trace->nProgressPoints;
                res         = trace->report_progress(prg);
                trace->lkTasks.unlock();
            }

            if (res != STATUS_OK)
            {
                trace->bFailed  = true; // Report fail status if at least one thread has failed
                break;
            }
        }

        return res;
    }

    status_t RayTrace3D::TaskThread::submit_task(rt_context_t *ctx)
    {
        // 'Heavy' state and pretty high number of pending tasks - submit task to global queue
        if (ctx->state == heavy_state)
        {
            if (trace->vTasks.size() < TASK_LO_THRESH)
            {
                trace->lkTasks.lock();
                status_t res = (trace->vTasks.push(ctx)) ? STATUS_OK : STATUS_NO_MEM;
                trace->lkTasks.unlock();
                return res;
            }
        }

        // Otherwise, submit to local task queue
        return (tasks.push(ctx)) ? STATUS_OK : STATUS_NO_MEM;
    }

    status_t RayTrace3D::TaskThread::process_context(rt_context_t *ctx)
    {
        status_t res;

        switch (ctx->state)
        {
            case S_SCAN_OBJECTS:
                ++stats.calls_scan;
                res     = scan_objects(ctx);
                break;
//            case S_CULL_VIEW:
//                ++stats.calls_cull;
//                res     = cull_view(ctx);
//                break;
            case S_SPLIT:
                ++stats.calls_split;
                res     = split_view(ctx);
                break;
            case S_CULL_BACK:
                ++stats.calls_cullback;
                res     = cullback_view(ctx);
                break;
            case S_REFLECT:
                ++stats.calls_reflect;
                res     = reflect_view(ctx);
                break;
            default:
                res = STATUS_BAD_STATE;
                break;
        }

        // Analyze status
        RT_TRACE(trace->pDebug,
            if (res == STATUS_BREAKPOINT)
            {
                trace->pDebug->ignored.swap(&ctx->ignored);
                trace->pDebug->trace.swap(&ctx->trace);
            }
        )

        // Force context to be deleted
        if (res != STATUS_OK)
            delete ctx;

        return res;
    }

    status_t RayTrace3D::TaskThread::generate_tasks(cvector<rt_context_t> *tasks, float initial)
    {
        for (size_t i=0,n=trace->vSources.size(); i<n; ++i)
        {
            source_t *src = trace->vSources.get(i);
            if (src == NULL)
                return STATUS_CORRUPTED;

            // Generate/fetch object
            Object3D *obj = NULL;
            switch (src->type)
            {
                case RT_AS_SPOT:        // TODO
                case RT_AS_SPEAKER:     // TODO
                case RT_AS_TRIANGLE:
                    obj     = factory.buildTriangle();
                    break;
                case RT_AS_OMNI:
                    obj     = factory.buildIcosphere(0);
                    break;
            }

            if (obj == NULL)
                return STATUS_NO_MEM;

            // Prepare transformation matrix
            matrix3d_t tm;
            dsp::calc_matrix3d_transform_r1(&tm, &src->position);

            // Estimate area of the source surface and apply it as a denominator of initial energy
            point3d_t p[3];
            float area = 0.0f;

            for (size_t ti=0, n=obj->num_triangles(); ti<n; ++ti)
            {
                obj_triangle_t *t   = obj->triangle(ti);
                dsp::apply_matrix3d_mp2(&p[0], t->v[0], &tm);
                dsp::apply_matrix3d_mp2(&p[1], t->v[1], &tm);
                dsp::apply_matrix3d_mp2(&p[2], t->v[2], &tm);
                area               += dsp::calc_area_pv(p);
            }
//            float e_norming     = initial * src->volume / sqrtf(area);

            // Generate sources
            for (size_t ti=0, n=obj->num_triangles(); ti<n; ++ti)
            {
//                if (ti != 6) // TODO: DEBUG
//                    continue;

                obj_triangle_t *t   = obj->triangle(ti);
                rt_context_t *ctx   = new rt_context_t();
                if (ctx == NULL)
                    return STATUS_NO_MEM;

                RT_TRACE(trace->pDebug, ctx->set_debug_context(trace->pDebug); )

                ctx->view.s         = src->position.z;
                dsp::apply_matrix3d_mp2(&ctx->view.p[0], t->v[0], &tm);
                dsp::apply_matrix3d_mp2(&ctx->view.p[1], t->v[2], &tm);
                dsp::apply_matrix3d_mp2(&ctx->view.p[2], t->v[1], &tm);

                ctx->state          = S_SCAN_OBJECTS;
                ctx->view.location  = 1.0f;
                ctx->view.oid       = -1;
                ctx->view.face      = -1;
                ctx->view.speed     = SOUND_SPEED_M_S;

                ctx->view.amplitude = 1.0f; //sqrtf(dsp::calc_area_pv(ctx->view.p)) * e_norming;
//                ctx->view.energy    = dsp::calc_area_pv(ctx->view.p) * e_norming;
                ctx->view.time[0]   = 0.0f;
                ctx->view.time[1]   = 0.0f;
                ctx->view.time[2]   = 0.0f;

                if (!tasks->add(ctx))
                {
                    delete ctx;
                    return STATUS_NO_MEM;
                }
            }

            RT_TRACE_BREAK(trace->pDebug,
                lsp_trace("Generated %d raytrace contexts for source %d", int(obj->num_triangles()), int(i));
                for (size_t i=0, n=tasks->size(); i<n; ++i)
                {
                    rt_context_t *ctx = tasks->at(i);
                    trace->pDebug->trace.add_view_1c(&ctx->view, &C_MAGENTA);
                }
            );
        }

        return STATUS_OK;
    }

    status_t RayTrace3D::TaskThread::check_object(rt_context_t *ctx, Object3D *obj, const matrix3d_t *m)
    {
        // Ensure that we need to perform additional checks
        if (obj->num_triangles() < 16)
            return STATUS_OK;

        // Prepare bounding-box check
        bound_box3d_t box = *(obj->bound_box());
        for (size_t j=0; j<8; ++j)
            dsp::apply_matrix3d_mp1(&box.p[j], m);

        RT_TRACE_BREAK(trace->pDebug,
            lsp_trace("Testing bound box");

            ctx->trace.add_view_1c(&ctx->view, &C_MAGENTA);
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

                ctx->trace.add_triangle(v);
            }
        )

        // Perform simple bounding-box check
        bool res    = check_bound_box(&box, &ctx->view);

        if (!res)
        {
            RT_TRACE(trace->pDebug,
                for (size_t j=0,n=obj->num_triangles(); j<n; ++j)
                {
                    obj_triangle_t *st = obj->triangle(j);

                    v_triangle3d_t t;
                    dsp::apply_matrix3d_mp2(&t.p[0], st->v[0], m);
                    dsp::apply_matrix3d_mp2(&t.p[1], st->v[1], m);
                    dsp::apply_matrix3d_mp2(&t.p[2], st->v[2], m);

                    dsp::apply_matrix3d_mv2(&t.n[0], st->n[0], m);
                    dsp::apply_matrix3d_mv2(&t.n[1], st->n[1], m);
                    dsp::apply_matrix3d_mv2(&t.n[2], st->n[2], m);

                    ctx->ignored.add(&t);
                }
            );
        }

        return (res) ? STATUS_OK : STATUS_SKIP;
    }

    status_t RayTrace3D::TaskThread::generate_root_mesh()
    {
        status_t res;
        size_t obj_id = 0;

        // Clear contents of the root context
        root.clear();
        RT_TRACE(trace->pDebug, root.set_debug_context(trace->pDebug, &trace->pDebug->trace); );
/*
        // Add capture objects as fake icosphere objects
        for (size_t i=0, n=trace->vCaptures.size(); i<n; ++i, ++obj_id)
        {
            capture_t *cap      = trace->vCaptures.get(i);
            if (cap == NULL)
                return STATUS_BAD_STATE;

            Object3D *obj       = factory.buildIcosphere(1);
            if (obj == NULL)
                return STATUS_NO_MEM;

            // Estimate the area of the source
            float area = 0.0f;

            point3d_t p[3];
            for (size_t i=0, n=obj->num_triangles(); i<n; ++i)
            {
                obj_triangle_t *t = obj->triangle(i);
                dsp::apply_matrix3d_mp2(&p[0], t->v[0], &cap->matrix);
                dsp::apply_matrix3d_mp2(&p[1], t->v[1], &cap->matrix);
                dsp::apply_matrix3d_mp2(&p[2], t->v[2], &cap->matrix);

                area += dsp::calc_area_pv(p);
            }
            cap->volume     = 1.0f;
//            cap->volume     = cap->gain / sqrtf(area);
//            if (cap->volume > max_volume)
//                max_volume      = cap->volume;

            // Add capture object to context
            res     = root.add_object_exclusive(obj, obj_id, &cap->matrix, &cap->material);
            if (res != STATUS_OK)
                return res;
        }
*/
        // Add scene objects
        for (size_t i=0, n=trace->pScene->num_objects(); i<n; ++i, ++obj_id)
        {
            // Get object
            Object3D *obj       = trace->pScene->object(i);
            if (obj == NULL)
                return STATUS_BAD_STATE;
            else if (!obj->is_visible()) // Skip invisible objects
                continue;

            // Get material
            rt_material_t *m    = trace->vMaterials.get(i);
            if (m == NULL)
                return STATUS_BAD_STATE;

            // Add object to context
            res         = root.add_object(obj, obj_id, obj->matrix(), m);
            if (res != STATUS_OK)
                return res;
        }

        RT_TRACE_BREAK(trace->pDebug,
            lsp_trace("Prepared scene (%d triangles)", int(root.triangle.size()));
            for (size_t i=0,n=root.triangle.size(); i<n; ++i)
                trace->pDebug->trace.add_triangle_3c(root.triangle.get(i), &C_RED, &C_GREEN, &C_BLUE);
        );

        // Normalize capture volume
//        for (size_t i=0, n=vCaptures.size(); i<n; ++i, ++obj_id)
//        {
//            capture_t *cap      = vCaptures.get(i);
//            cap->volume        /= max_volume;
//        }

        // Update source energy (add extra volume)
//        for (size_t i=0,n=vTasks.size();i<n; ++i)
//        {
//            rt_context_t *ct    = vTasks.get(i);
//            ct->view.energy    *= max_volume;
//        }

        // Solve conflicts between all objects
        res = root.solve_conflicts();
        if (res != STATUS_OK)
            return res;

        RT_TRACE(trace->pDebug,
            if (!trace->pScene->validate())
                return STATUS_CORRUPTED;
        )

        RT_TRACE_BREAK(trace->pDebug,
            lsp_trace("Added capture objects (%d triangles)", int(root.triangle.size()));
            for (size_t i=0,n=root.triangle.size(); i<n; ++i)
                trace->pDebug->trace.add_triangle_3c(root.triangle.get(i), &C_RED, &C_GREEN, &C_BLUE);
        );

        return res;
    }


    status_t RayTrace3D::TaskThread::scan_objects(rt_context_t *ctx)
    {
        status_t res = STATUS_OK;

        RT_TRACE_BREAK(trace->pDebug,
            lsp_trace("Scanning objects...");
            ctx->trace.add_view_1c(&ctx->view, &C_MAGENTA);
        )

        size_t max_objs     = trace->pScene->num_objects() + trace->vCaptures.size();
        size_t *objs        = reinterpret_cast<size_t *>(alloca(sizeof(size_t) * max_objs));
        size_t n_objs       = 0;
        size_t obj_id       = 0;

        // Add all captures
        for (size_t i=0, n=trace->vCaptures.size(); i<n; ++i, ++obj_id)
        {
            capture_t *cap      = trace->vCaptures.get(i);
            if (cap == NULL)
                return STATUS_BAD_STATE;

            Object3D *obj       = factory.buildIcosphere(1);
            if (obj == NULL)
                return STATUS_NO_MEM;

            // Add capture identifier to list of visible objects
            res     =  check_object(ctx, obj, &cap->matrix);
            if (res == STATUS_OK)
                objs[n_objs++]      = obj_id;
            else if (res == STATUS_SKIP)
                res                 = STATUS_OK;
            else
                return res;
        }

        // Iterate all object and add to the context if the object is potentially participating the ray tracing algorithm
        for (size_t i=0, n=trace->pScene->num_objects(); i<n; ++i, ++obj_id)
        {
            // Get object
            Object3D *obj       = trace->pScene->object(i);
            if (obj == NULL)
                return STATUS_BAD_STATE;
            else if (!obj->is_visible()) // Skip invisible objects
                continue;

            // Get material
            rt_material_t *m    = trace->vMaterials.get(i);
            if (m == NULL)
                return STATUS_BAD_STATE;

            // Add object identifier to list of visible objects
            res     =  check_object(ctx, obj, obj->matrix());
            if (res == STATUS_OK)
                objs[n_objs++]      = obj_id;
            else if (res == STATUS_SKIP)
                res                 = STATUS_OK;
            else
                return res;
        }

        // Fetch visible objects from root context into current context
//        lsp_trace("Fetch %d objects", int(n_objs));
        res     = ctx->fetch_objects(&root, n_objs, objs);
        if (res != STATUS_OK) // Some error occurred
            return res;

        RT_TRACE_BREAK(trace->pDebug,
            lsp_trace("Fetched %d objects", int(n_objs));
            ctx->trace.add_view_1c(&ctx->view, &C_MAGENTA);
            for (size_t i=0,n=ctx->triangle.size(); i<n; ++i)
                ctx->trace.add_triangle_3c(ctx->triangle.get(i), &C_RED, &C_GREEN, &C_BLUE);

            for (size_t i=0, n=ctx->plan.items.size(); i<n; ++i)
                ctx->trace.add_segment(ctx->plan.items.get(i), &C_YELLOW);
        )

        // Update state
        //ctx->state      = S_CULL_VIEW;
        n_objs = ctx->triangle.size();
        if (n_objs <= 1)
        {
            if (n_objs <= 0)
            {
                delete ctx;
                return STATUS_OK;
            }
            ctx->state  = S_REFLECT;
        }
        else
            ctx->state  = S_SPLIT;

        return submit_task(ctx);
    }

    status_t RayTrace3D::TaskThread::cull_view(rt_context_t *ctx)
    {
        status_t res = ctx->cull_view();
        if (res != STATUS_OK)
            return res;

        size_t n = ctx->triangle.size();
        if (n <= 1)
        {
            if (n <= 0)
            {
                delete ctx;
                return STATUS_OK;
            }
            ctx->state  = S_REFLECT;
        }
        else
            ctx->state  = S_SPLIT;

        return submit_task(ctx);
    }

    status_t RayTrace3D::TaskThread::split_view(rt_context_t *ctx)
    {
        rt_context_t out;
        RT_TRACE(trace->pDebug, out.set_debug_context(trace->pDebug); );

        // Perform binary split
        status_t res = ctx->edge_split(&out);
        if (res == STATUS_NOT_FOUND)
        {
            ctx->state      = S_CULL_BACK;
            return submit_task(ctx);
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
                rt_context_t *nctx = new rt_context_t(&ctx->view, (out.triangle.size() > 1) ? S_SPLIT : S_REFLECT);
                if (nctx == NULL)
                    return STATUS_NO_MEM;

                RT_TRACE(trace->pDebug,
                    nctx->set_debug_context(trace->pDebug);

                    nctx->ignored.add_all(&ctx->ignored);
                    nctx->trace.add_all(&ctx->trace);

                    for (size_t i=0,n=ctx->triangle.size(); i<n; ++i)
                        nctx->ignore(ctx->triangle.get(i));
                    for (size_t i=0,n=out.triangle.size(); i<n; ++i)
                        ctx->ignore(out.triangle.get(i));
                );

                nctx->swap(&out);

                // Submit task
                res = submit_task(nctx);
                if (res != STATUS_OK)
                {
                    delete nctx;
                    return res;
                }
            }

            // Update context state
            ctx->state  = (ctx->triangle.size() > 1) ? S_SPLIT : S_REFLECT;
            return submit_task(ctx);
        }
        else if (out.triangle.size() > 0)
        {
            ctx->swap(&out);
            ctx->state  = (ctx->triangle.size() > 1) ? S_SPLIT : S_REFLECT;

            return submit_task(ctx);
        }

        delete ctx;
        return STATUS_OK;
    }

    status_t RayTrace3D::TaskThread::cullback_view(rt_context_t *ctx)
    {
        RT_TRACE_BREAK(trace->pDebug,
            lsp_trace("Performing depth test...");
            ctx->trace.add_view_1c(&ctx->view, &C_MAGENTA);
            for (size_t i=0,n=ctx->triangle.size(); i<n; ++i)
                ctx->trace.add_triangle_1c(ctx->triangle.get(i), &C_RED);
        )

        // Perform cullback
        status_t res = ctx->depth_test();
        if (res != STATUS_OK)
            return res;

        RT_TRACE_BREAK(trace->pDebug,
            lsp_trace("After depth test %d triangles", int(ctx->triangle.size()));
        ctx->trace.add_view_1c(&ctx->view, &C_MAGENTA);
            for (size_t i=0,n=ctx->triangle.size(); i<n; ++i)
                ctx->trace.add_triangle_1c(ctx->triangle.get(i), &C_GREEN);
        );

        if (ctx->triangle.size() <= 0)
        {
            delete ctx;
            return STATUS_OK;
        }
        ctx->state  = S_REFLECT;

        return submit_task(ctx);
    }

    void invalid_state_hook()
    {
        lsp_error("Invalid state");
    }

    status_t RayTrace3D::TaskThread::reflect_view(rt_context_t *ctx)
    {
        rt_context_t *rc;
        rt_view_t sv, v, cv, rv, tv;    // source view, view, captured view, reflected view, transparent trace
        vector3d_t vpl;                 // trace plane, split plane
        point3d_t p[3];                 // Projection points
        float d[3], t[3];               // distance, time
        float a[3], A, kd;              // particular area, area, dispersion coefficient

        sv      = ctx->view;
        A       = dsp::calc_area_pv(sv.p);

        if (A <= trace->fTolerance)
        {
            delete ctx;
            return STATUS_OK;
        }
        float revA = 1.0f / A;

        dsp::calc_plane_pv(&vpl, ctx->view.p);
        status_t res    = STATUS_OK;

        for (size_t i=0,n=ctx->triangle.size(); i<n; ++i)
        {
            // Get current triangle to perform reflection
            rt_triangle_t *ct = ctx->triangle.get(i);

            // get material
            rt_material_t *m    = ct->m;
            if (m == NULL)
                continue;

            // Estimate co-location of source point and reflecting triangle
            float distance  = sv.s.x * ct->n.dx + sv.s.y * ct->n.dy + sv.s.z * ct->n.dz + ct->n.dw;

            // We need to skip the interaction with triangle due two reasons:
            //   1. The ray group reaches the face with non-matching normal
            //   2. The ray group reaches the face with matching normal but invalid object identifier
            if (distance > 0.0f) // we're entering from outside the object inside the object
            {
                // The source point should be above the triangle
                if (sv.location <= 0.0f)
                    continue;
            }
            else if (distance < 0.0f)
            {
                // The source point should be below triangle and match the object identifier
                if ((sv.location >= 0.0f) || (sv.oid != ct->oid))
                    continue;
            }
            else
                continue;

            // Estimate the start time for each trace point using barycentric coordinates
            // Additionally filter invalid triangles
            bool valid = true;
            for (size_t j=0; j<3; ++j)
            {
                dsp::calc_split_point_p2v1(&p[j], &sv.s, &ct->v[j], &vpl);    // Project triangle point to trace point
                d[j]        = dsp::calc_distance_p2(&p[j], &ct->v[j]);        // Compute distance between projected point and triangle point

                a[0]        = dsp::calc_area_p3(&p[j], &sv.p[1], &sv.p[2]);   // Compute area 0
                a[1]        = dsp::calc_area_p3(&p[j], &sv.p[0], &sv.p[2]);   // Compute area 1
                a[2]        = dsp::calc_area_p3(&p[j], &sv.p[0], &sv.p[1]);   // Compute area 2

                float dA    = A - (a[0] + a[1] + a[2]); // Point should lay within the view
                if ((dA <= -DSP_3D_TOLERANCE) || (dA >= DSP_3D_TOLERANCE))
                {
                    valid = false;
                    break;
                }

                t[j]        = (sv.time[0] * a[0] + sv.time[1] * a[1] + sv.time[2] * a[2]) * revA; // Compute projected point's time
                v.time[j]   = t[j] + (d[j] / sv.speed);
                if (v.time[j] > 1.0f)
                    invalid_state_hook();
            }

            if (!valid)
                continue;

            // Compute area of projected triangle
            float area  = dsp::calc_area_pv(p);
            if (area <= 1e-10f)
                continue;

            // Determine the direction from which came the wave front
            v.oid       = ct->oid;
            v.face      = ct->face;
            v.s         = sv.s;
            v.amplitude = sv.amplitude * sqrtf(area * revA);
            v.location  = sv.location;
            v.speed     = sv.speed;
            v.rnum      = sv.rnum;
            v.p[0]      = ct->v[0];
            v.p[1]      = ct->v[1];
            v.p[2]      = ct->v[2];

            RT_TRACE_BREAK(trace->pDebug,
                lsp_trace("Projection points distance: {%f, %f, %f}", d[0], d[1], d[2]);
                lsp_trace("View points time: {%f, %f, %f}", sv.time[0], sv.time[1], sv.time[2]);
                lsp_trace("Projection points time: {%f, %f, %f}", t[0], t[1], t[2]);
                lsp_trace("Target points time: {%f, %f, %f}", v.time[0], v.time[1], v.time[2]);
                lsp_trace("Amplitude: %e -> %e", sv.amplitude, v.amplitude);
                lsp_trace("Distance between source point and triangle: %f", distance);
                lsp_trace("oid = %d, caprues.size=%d", int(ct->oid), int(trace->vCaptures.size()));

                ctx->trace.add_triangle_1c(ct, &C_YELLOW);
                ctx->trace.add_plane_3pn1c(&ct->v[0], &ct->v[1], &ct->v[2], &ct->n, &C_GREEN);
                ctx->trace.add_view_1c(&sv, &C_MAGENTA);
                ctx->trace.add_segment(&p[0], &ct->v[0], &C_RED);
                ctx->trace.add_segment(&p[1], &ct->v[1], &C_GREEN);
                ctx->trace.add_segment(&p[2], &ct->v[2], &C_BLUE);
            )

            cv          = v;
            rv          = v;
            tv          = v;

            if (distance > 0.0f)
            {
                cv.amplitude    = v.amplitude * m->absorption[0];
                v.amplitude    *= (1.0f - m->absorption[0]);

                kd              = (1.0f + 1.0f / m->dispersion[0]) * distance;
                rv.amplitude    = v.amplitude * (m->transparency[0] - 1.0f); // Sign negated
                rv.s.x         -= kd * ct->n.dx;
                rv.s.y         -= kd * ct->n.dy;
                rv.s.z         -= kd * ct->n.dz;
                rv.rnum         = v.rnum + 1;       // Increment reflection number

                kd              = (m->permeability/m->dissipation[0] - 1.0f) * distance;
                tv.amplitude    = v.amplitude * m->transparency[0];
                tv.speed       *= m->permeability;
                tv.s.x         += kd * ct->n.dx;
                tv.s.y         += kd * ct->n.dy;
                tv.s.z         += kd * ct->n.dz;
                tv.location     = - v.location;     // Invert location of transparent trace

                if (tv.speed > 5000.0f)
                    invalid_state_hook();
                else if (tv.speed < 300.0f)
                    invalid_state_hook();

                RT_TRACE_BREAK(trace->pDebug,
                    lsp_trace("Outside->inside reflect_view");
                    lsp_trace("Amplitude: captured=%e, reflected=%e, refracted=%e", cv.amplitude, rv.amplitude, tv.amplitude);
                    lsp_trace("Material: absorption=%f, transparency=%f, permeability=%f, dispersion=%f, dissipation=%f",
                            m->absorption[0], m->transparency[0], m->permeability, m->dispersion[0], m->dissipation[0]);

                    ctx->trace.add_view_1c(&sv, &C_RED);
                    ctx->trace.add_view_1c(&rv, &C_GREEN);
                    ctx->trace.add_view_1c(&tv, &C_CYAN);
                )
            }
            else
            {
                cv.amplitude    = v.amplitude * m->absorption[1];
                v.amplitude    *= (1.0f - m->absorption[1]);

                kd              = (1.0f + 1.0f / m->dispersion[1]) * distance;
                rv.amplitude    = v.amplitude * (m->transparency[1] - 1.0f); // Sign negated
                rv.s.x         -= kd * ct->n.dx;
                rv.s.y         -= kd * ct->n.dy;
                rv.s.z         -= kd * ct->n.dz;
                rv.rnum         = v.rnum + 1;       // Increment reflection number

                kd              = (1.0f/(m->dissipation[1]*m->permeability) - 1.0f) * distance;
                tv.amplitude    = v.amplitude * m->transparency[1];
                tv.speed       /= m->permeability;
                tv.s.x         += kd * ct->n.dx;
                tv.s.y         += kd * ct->n.dy;
                tv.s.z         += kd * ct->n.dz;
                tv.location     = - v.location;     // Invert location of transparent trace

                if (tv.speed > 5000.0f)
                    invalid_state_hook();
                else if (tv.speed < 300.0f)
                    invalid_state_hook();

                RT_TRACE_BREAK(trace->pDebug,
                    lsp_trace("Inside->outside reflect_view");
                    lsp_trace("Amplitude: captured=%e, reflected=%e, refracted=%e", cv.amplitude, rv.amplitude, tv.amplitude);
                    lsp_trace("Material: absorption=%f, transparency=%f, permeability=%f, dispersion=%f, dissipation=%f",
                            m->absorption[1], m->transparency[1], m->permeability, m->dispersion[1], m->dissipation[1]);

                    ctx->trace.add_view_1c(&sv, &C_RED);
                    ctx->trace.add_view_1c(&rv, &C_GREEN);
                    ctx->trace.add_view_1c(&tv, &C_CYAN);
                )
            }

            // Perform capture
            if (size_t(ct->oid) < captures.size())
            {
                capture_t *cap  = captures.get(ct->oid);
                if (cap->bindings.size() > 0)
                {
                    // Perform synchronized capturing
                    ++stats.calls_capture;
                    #ifdef LSP_RT_TRACE
                        res = capture(cap, &cv, &ctx->trace);
                    #else
                        res = capture(cap, &cv);
                    #endif /* LSP_RT_TRACE */
                    if (res != STATUS_OK)
                        break;
                }
            }

            // Create reflection context
            if ((rv.amplitude <= -trace->fEnergyThresh) || (rv.amplitude >= trace->fEnergyThresh))
            {
                // Revert the order of triangle because direction has changed to opposite
                rv.p[1]     = v.p[2];
                rv.p[2]     = v.p[1];

                rc      = new rt_context_t(&rv, S_SCAN_OBJECTS);
                if (rc == NULL)
                {
                    res = STATUS_NO_MEM;
                    break;
                }

                RT_TRACE(trace->pDebug,
                    rc->set_debug_context(trace->pDebug);
                );

                res = submit_task(rc);
                if (res != STATUS_OK)
                {
                    delete rc;
                    break;
                }
            }

            // Create refraction context
            if ((tv.amplitude <= -trace->fEnergyThresh) || (tv.amplitude >= trace->fEnergyThresh))
            {
                rc      = new rt_context_t(&tv, S_SCAN_OBJECTS);
                if (rc == NULL)
                {
                    res = STATUS_NO_MEM;
                    break;
                }

                RT_TRACE(trace->pDebug,
                    rc->set_debug_context(trace->pDebug);
                );

                res = submit_task(rc);
                if (res != STATUS_OK)
                {
                    delete rc;
                    break;
                }
            }
        }

        if (res == STATUS_OK)
            delete ctx;
        return res;
    }

#ifdef LSP_RT_TRACE
    status_t RayTrace3D::TaskThread::capture(capture_t *capture, const rt_view_t *v, View3D *view)
#else
    status_t RayTrace3D::TaskThread::capture(capture_t *capture, const rt_view_t *v)
#endif /* LSP_RT_TRACE */
    {
//        lsp_trace("Capture:\n"
//                "  coord  = {{%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f}}\n"
//                "  time   = {%f, %f, %f}\n"
//                "  energy = %f",
//                v->p[0].x, v->p[0].y, v->p[0].z,
//                v->p[1].x, v->p[1].y, v->p[1].z,
//                v->p[2].x, v->p[2].y, v->p[2].z,
//                v->time[0], v->time[1], v->time[2],
//                v->energy
//            );

        // Compute the area of triangle
        float v_area = dsp::calc_area_pv(v->p);
        if (v_area <= 1e-10f) // Prevent from becoming NaNs
            return STATUS_OK;

        // Estimate distance and time parameters for source point
        vector3d_t ds[3];
        raw_triangle_t src;
        float ts[3], tsn[3];

        for (size_t i=0; i<3; ++i)
        {
            src.p[i]    = v->p[i];
            dsp::init_vector_p2(&ds[i], &v->s, &src.p[i]);  // Delta vector
            float dist  = dsp::calc_distance_v1(&ds[i]);
            ts[i]       = v->time[i] - dist / v->speed;     // Time at the source point
            tsn[i]      = v->time[i] * trace->nSampleRate;  // Sample number
        }

        // Estimate the culling sample number
        ssize_t csn, ssn;
        if ((tsn[0] < tsn[1]) && (tsn[0] < tsn[2]))
            csn     = tsn[0];
        else
            csn     = (tsn[1] < tsn[2]) ? tsn[1] : tsn[2];
        ssn = csn++;                                        // Culling sample number

        // Perform integration
        vector3d_t spl;
        raw_triangle_t in[2], out[2];
        size_t n_in, n_out;
        float prev_area     = 0.0f;                             // The area of polygon at previous step
        float afactor       = v->amplitude / sqrtf(v_area);     // The norming energy factor
        point3d_t p[3];

        do {
            // Estimate the culling plane points
            float ctime     = float(csn) / trace->nSampleRate;
            for (size_t i=0; i<3; ++i)
            {
                float factor    = (ctime  - ts[i]) / (v->time[i] - ts[i]);
                p[i].x  = v->s.x + ds[i].dx * factor;
                p[i].y  = v->s.y + ds[i].dy * factor;
                p[i].z  = v->s.z + ds[i].dz * factor;
                p[i].w  = 1.0f;
            }

            // Compute culling plane
            dsp::calc_oriented_plane_pv(&spl, &v->s, p);

            RT_TRACE_BREAK(trace->pDebug,
                lsp_trace("Integrating triangle...");
                view->add_view_1c(v, &C_MAGENTA);
                view->add_triangle_pv1c(src.p, &C_ORANGE);
                view->add_plane_pvn1c(p, &spl, &C_YELLOW);
            )

            // Perform split
            n_out = 0, n_in = 0;
            dsp::split_triangle_raw(out, &n_out, in, &n_in, &spl, &src);

            // Compute the area of triangle and ensure that it is greater than previous value
            float in_area       = 0.0f;
            for (size_t i=0; i<n_in; ++i)
                in_area          += dsp::calc_area_pv(in[i].p);
            if (in_area > prev_area)
            {
                // Compute the amplitude
                float amplitude = sqrtf(in_area - prev_area) * afactor;
                prev_area       = in_area;

                RT_TRACE_BREAK(trace->pDebug,
                    lsp_trace("After split in=%d (GREEN), out=%d (RED) sample=%d, amplitude=%e",
                            int(n_in), int(n_out),
                            int(csn-1), amplitude
                        );
                    view->add_view_1c(v, &C_MAGENTA);
                    view->add_plane_pvn1c(p, &spl, &C_YELLOW);
                    for (size_t i=0; i<n_in; ++i)
                        view->add_triangle_pv1c(in[i].p, &C_GREEN);
                    for (size_t i=0; i<n_out; ++i)
                        view->add_triangle_pv1c(out[i].p, &C_RED);
                )

                // Deploy energy value to the sample
                if (csn > 0)
                {
                    // Lock capture data
//                    trace->lkCapture.lock();

                    // Append sample to each matching capture
                    for (size_t ci=0, cn=capture->bindings.size(); ci<cn; ++ci)
                    {
                        sample_t *s = capture->bindings.at(ci);

                        // Skip reflection not in range
                        if ((s->r_min >= 0) && (v->rnum < s->r_min))
                            continue;
                        else if ((s->r_max >= 0) && (v->rnum > s->r_max))
                            continue;

                        // Ensure that we need to resize sample
                        size_t len = s->sample->max_length();
                        if (len <= size_t(csn))
                        {
                            len     = (csn + 1 + SAMPLE_QUANTITY) / SAMPLE_QUANTITY;
                            len    *= SAMPLE_QUANTITY;

                            lsp_trace("v->time = {%e, %e, %e}", v->time[0], v->time[1], v->time[2]);
                            lsp_trace("ctime = %e, tsn = {%e, %e, %e}", ctime, tsn[0], tsn[1], tsn[2]);
                            lsp_trace("spl = {%e, %e, %e, %e}",
                                spl.dx, spl.dy, spl.dz, spl.dw);
                            lsp_trace("Requesting sample resize: csn=0x%llx, len=0x%llx, channels=%d",
                                (long long)csn, (long long)len, int(s->sample->channels())
                                );
                            if (len > 0x100000) // TODO: This is currently impossible, added for debugging, remove in future
                                invalid_state_hook();
                            if (!s->sample->resize(s->sample->channels(), len, len))
                            {
                                invalid_state_hook();
                                return STATUS_NO_MEM;
                            }
                        }

                        // Deploy sample to curent channel
                        float *buf  = s->sample->getBuffer(s->channel);
                        buf[csn-1] += amplitude;
                    }

                    // Unlock capture data
//                    trace->lkCapture.unlock();
                }
            }

            ++csn; // Increment culling sample number for next iteration
        } while (n_out > 0);

        lsp_trace("Samples %d-%d -> area=%e amplitude=%e rnum=%d",
                int(ssn), int(csn-1), v_area, v->amplitude, int(v->rnum));

        return STATUS_OK;
    }

    status_t RayTrace3D::TaskThread::prepare_main_loop(float initial)
    {
        // Report progress as 0%
        status_t res    = trace->report_progress(0.0f);
        if (res != STATUS_OK)
            return res;
        else if (trace->bCancelled)
            return STATUS_CANCELLED;

        RT_TRACE_BREAK(trace->pDebug,
            for (size_t i=0,n=trace->pScene->num_objects(); i<n; ++i)
            {
                Object3D *obj = trace->pScene->get_object(i);
                if (!obj->is_visible())
                    continue;
                for (size_t j=0,m=obj->num_triangles(); j<m; ++j)
                    trace->pDebug->trace.add_triangle(obj->triangle(j), &C_RED, &C_GREEN, &C_BLUE);
            }
        );

        // Prepare root context
        res         = generate_root_mesh();
        if (res != STATUS_OK)
            return res;
        else if (trace->bCancelled)
            return STATUS_CANCELLED;

        // Generate ray-tracing tasks
        rt_context_t *ctx = NULL;
        cvector<rt_context_t> estimate;
        res         = generate_tasks(&estimate, initial);
        if (res != STATUS_OK)
        {
            destroy_tasks(&estimate);
            return res;
        }
        else if (trace->bCancelled)
        {
            destroy_tasks(&estimate);
            return STATUS_CANCELLED;
        }

        // Estimate the progress by doing set of steps
        heavy_state = -1; // This guarantees that all tasks will be submitted to local task queue
        do
        {
            while (estimate.size() > 0)
            {
                // Check cancellation flag
                if (trace->bCancelled)
                {
                    destroy_tasks(&tasks);
                    destroy_tasks(&estimate);
                    return STATUS_CANCELLED;
                }

                // Get new task
                if (!estimate.pop(&ctx))
                {
                    destroy_tasks(&tasks);
                    destroy_tasks(&estimate);
                    return STATUS_CORRUPTED;
                }

                // Process new task but store into
                res     = process_context(ctx);
                if (res != STATUS_OK)
                {
                    destroy_tasks(&tasks);
                    destroy_tasks(&estimate);
                    return res;
                }
            }

            // Perform swap: empty local tasks, fill 'estimate' with them
            estimate.swap_data(&tasks);
        } while ((estimate.size() > 0) && (estimate.size() < TASK_LO_THRESH));

        heavy_state         = S_SCAN_OBJECTS; // Enable global task queue for this thread
        trace->vTasks.swap_data(&estimate); // Now all generated tasks are global

        // Values to report progress
        trace->nProgressPoints  = 1;
        trace->nQueueSize       = trace->vTasks.size();
        trace->nProgressMax     = trace->nQueueSize + 2;

        // Report progress
        res         = trace->report_progress(float(trace->nProgressPoints++) / float(trace->nProgressMax));
        if (res != STATUS_OK)
        {
            destroy_tasks(&trace->vTasks);
            return res;
        }
        else if (trace->bCancelled)
        {
            destroy_tasks(&trace->vTasks);
            return STATUS_CANCELLED;
        }

        return STATUS_OK;
    }

    status_t RayTrace3D::TaskThread::prepare_supplementary_loop(TaskThread *t)
    {
        // Cleanup statistics
        clear_stats(&stats);

        // Copy capture state
        for (size_t i=0; i<trace->vCaptures.size(); ++i)
        {
            // Allocate capture
            capture_t *scap = trace->vCaptures.get(i);
            capture_t *dcap = new capture_t();
            if (dcap == NULL)
                return STATUS_NO_MEM;
            else if (!captures.add(dcap))
            {
                delete dcap;
                return STATUS_NO_MEM;
            }

            // Copy bindings
            for (size_t j=0; j<scap->bindings.size(); ++j)
            {
                // Allocate binding
                sample_t *ssamp = scap->bindings.get(j);
                sample_t *dsamp = dcap->bindings.add();
                if (dsamp == NULL)
                    return STATUS_NO_MEM;

                dsamp->sample   = NULL;
                dsamp->channel  = ssamp->channel;
                dsamp->r_min    = ssamp->r_min;
                dsamp->r_max    = ssamp->r_max;

                // Allocate sample and link
                Sample *xsamp   = ssamp->sample;
                Sample *tsamp   = new Sample();
                if (tsamp == NULL)
                    return STATUS_NO_MEM;
                else if (!tsamp->init(xsamp->channels(), xsamp->max_length(), xsamp->length()))
                {
                    tsamp->destroy();
                    delete tsamp;
                    return STATUS_NO_MEM;
                }
                dsamp->sample   = tsamp;
            }
        }

        return root.copy(&t->root);
    }

    RayTrace3D::RayTrace3D()
    {
        pScene          = NULL;
        pProgress       = NULL;
        pProgressData   = NULL;
        nSampleRate     = DEFAULT_SAMPLE_RATE;
        pDebug          = NULL;
        fEnergyThresh   = 1e-6f;
        fTolerance      = 1e-5f;
        bNormalize      = true;
        bCancelled      = false;
        nQueueSize      = 0;
        nProgressPoints = 0;
        nProgressMax    = 0;
    }

    RayTrace3D::~RayTrace3D()
    {
        destroy(true);
    }

    void RayTrace3D::clear_stats(stats_t *stats)
    {
        stats->root_tasks       = 0;
        stats->local_tasks      = 0;
        stats->calls_scan       = 0;
        stats->calls_cull       = 0;
        stats->calls_split      = 0;
        stats->calls_cullback   = 0;
        stats->calls_reflect    = 0;
        stats->calls_capture    = 0;
    }

    void RayTrace3D::dump_stats(const char *label, const stats_t *stats)
    {
        lsp_trace("%s:\n"
                "  root tasks processed     : %lld\n"
                "  local tasks processed    : %lld\n"
                "  scan_objects             : %lld\n"
                "  cull_view                : %lld\n"
                "  split_view               : %lld\n"
                "  cullback_view            : %lld\n"
                "  reflect_view             : %lld\n"
                "  capture                  : %lld\n",
            label,
            (long long)stats->root_tasks,
            (long long)stats->local_tasks,
            (long long)stats->calls_scan,
            (long long)stats->calls_cull,
            (long long)stats->calls_split,
            (long long)stats->calls_cullback,
            (long long)stats->calls_reflect,
            (long long)stats->calls_capture
        );
    }

    void RayTrace3D::merge_stats(stats_t *dst, const stats_t *src)
    {
        dst->root_tasks        += src->root_tasks;
        dst->local_tasks       += src->local_tasks;
        dst->calls_scan        += src->calls_scan;
        dst->calls_cull        += src->calls_cull;
        dst->calls_split       += src->calls_split;
        dst->calls_cullback    += src->calls_cullback;
        dst->calls_reflect     += src->calls_reflect;
        dst->calls_capture     += src->calls_capture;
    }

    void RayTrace3D::destroy_tasks(cvector<rt_context_t> *tasks)
    {
        for (size_t i=0, n=tasks->size(); i<n; ++i)
        {
            rt_context_t *ctx   = tasks->get(i);
            if (ctx != NULL)
                delete ctx;
        }

        tasks->flush();
    }

    void RayTrace3D::remove_scene(bool destroy)
    {
        if (pScene != NULL)
        {
            if (destroy)
            {
                pScene->destroy();
                delete pScene;
            }
            pScene = NULL;
        }
    }

    status_t RayTrace3D::resize_materials(size_t objects)
    {
        size_t size = vMaterials.size();

        if (objects < size)
        {
            if (!vMaterials.remove_n(objects, size - objects))
                return STATUS_UNKNOWN_ERR;
        }
        else if (objects > size)
        {
            if (!vMaterials.append_n(objects - size))
                return STATUS_NO_MEM;

            while (size < objects)
            {
                rt_material_t *m    = vMaterials.get(size++);
                if (m == NULL)
                    return STATUS_UNKNOWN_ERR;

                // By default, we set the material to 'Concrete'
                m->absorption[0]    = 0.02f;
                m->dispersion[0]    = 1.0f;
                m->dissipation[0]   = 1.0f;
                m->transparency[0]  = 0.48f;

                m->absorption[1]    = 0.0f;
                m->dispersion[1]    = 1.0f;
                m->dissipation[1]   = 1.0f;
                m->transparency[1]  = 0.52f;

                m->permeability     = 12.88f;
            }
        }

        return STATUS_OK;
    }

    status_t RayTrace3D::init()
    {
        return STATUS_OK;
    }

    void RayTrace3D::destroy(bool recursive)
    {
        destroy_tasks(&vTasks);
        clear_progress_callback();
        remove_scene(recursive);

        for (size_t i=0, n=vCaptures.size(); i<n; ++i)
        {
            capture_t *cap = vCaptures.get(i);
            if (cap != NULL)
            {
                cap->bindings.flush();
                delete cap;
            }
        }
        vCaptures.flush();

        vMaterials.flush();
        vSources.flush();
        vCaptures.flush();
    }

    status_t RayTrace3D::add_source(const ray3d_t *position, rt_audio_source_t type)
    {
        if (position == NULL)
            return STATUS_NO_MEM;

        source_t *src       = vSources.add();
        if (src == NULL)
            return STATUS_NO_MEM;

        src->position       = *position;
        src->type           = type;

        return STATUS_OK;
    }

    ssize_t RayTrace3D::add_capture(const ray3d_t *position, rt_audio_capture_t type)
    {
        capture_t *cap      = new capture_t();
        if (cap == NULL)
            return -STATUS_NO_MEM;

        size_t idx          = vCaptures.size();
        if (!vCaptures.add(cap))
        {
            delete cap;
            return -STATUS_NO_MEM;
        }

        cap->position       = *position;
        cap->type           = type;

        dsp::calc_matrix3d_transform_r1(&cap->matrix, position);

        // "Black hole"
        rt_material_t *m    = &cap->material;
        m->absorption[0]    = 1.0f;
        m->dispersion[0]    = 1.0f;
        m->dissipation[0]   = 1.0f;
        m->transparency[0]  = 0.0f;

        m->absorption[0]    = 1.0f;
        m->dispersion[0]    = 1.0f;
        m->dissipation[0]   = 1.0f;
        m->transparency[0]  = 0.0f;

        m->permeability     = 1.0f;

        return idx;
    }

    status_t RayTrace3D::bind_capture(size_t id, Sample *sample, size_t channel, ssize_t r_min, ssize_t r_max)
    {
        capture_t *cap = vCaptures.get(id);
        if (cap == NULL)
            return STATUS_INVALID_VALUE;

        sample_t *s     = cap->bindings.add();
        if (s == NULL)
            return STATUS_NO_MEM;

        s->sample       = sample;
        s->channel      = channel;
        s->r_min        = r_min;
        s->r_max        = r_max;

        return STATUS_OK;
    }

    status_t RayTrace3D::set_scene(Scene3D *scene, bool destroy)
    {
        status_t res = resize_materials(scene->num_objects());
        if (res != STATUS_OK)
            return res;

        // Destroy scene
        remove_scene(destroy);
        pScene      = scene;
        return STATUS_OK;
    }

    status_t RayTrace3D::set_progress_callback(rt_progress_t callback, void *data)
    {
        if (callback == NULL)
            return clear_progress_callback();

        pProgress       = callback;
        pProgressData   = data;
        return STATUS_OK;
    }

    status_t RayTrace3D::clear_progress_callback()
    {
        pProgress       = NULL;
        pProgressData   = NULL;
        return STATUS_OK;
    }

    status_t RayTrace3D::report_progress(float progress)
    {
        if (pProgress == NULL)
            return STATUS_OK;
        return pProgress(progress, pProgressData);
    }

    status_t RayTrace3D::process(size_t threads, float initial)
    {
        status_t res = STATUS_OK;
        bCancelled   = false;
        bFailed      = false;

        // Create main thread
        TaskThread *root = new TaskThread(this);
        if (root == NULL)
            return STATUS_NO_MEM;

        // Launch prepare_main_loop in root thread's context
        res    = root->prepare_main_loop(initial);
        if (res != STATUS_OK)
        {
            delete root;
            return res;
        }

        // Launch supplementary threads
        cvector<TaskThread> workers;
        for (size_t i=1; i<threads; ++i)
        {
            // Create thread object
            TaskThread *t   = new TaskThread(this);
            if ((t == NULL) || (!workers.add(t)))
            {
                if (t != NULL)
                    delete t;
                res = STATUS_NO_MEM;
                break;
            }

            // Sync thread data
            res = t->prepare_supplementary_loop(root);
            if (res != STATUS_OK)
                break;

            // Launch thread
            res = t->start();
            if (res != STATUS_OK)
                break;
        }

        // If successful status, perform main loop
        if (res == STATUS_OK)
            res     = root->run();
        else
            bFailed = true;

        // Wait for supplementary threads
        for (size_t i=0,n=workers.size(); i<n; ++i)
        {
            // Wait for thread completion
            TaskThread *t = workers.get(i);
            t->join();
            if (res == STATUS_OK)
                res     = t->get_result(); // Update execution status
        }

        // Get root thread statistics
        stats_t overall;
        clear_stats(&overall);
        merge_stats(&overall, root->get_stats());
        if (res != STATUS_BREAKPOINT)
            dump_stats("Main thread statistics", root->get_stats());

        // Output thread stats and destroy threads
        for (size_t i=0,n=workers.size(); i<n; ++i)
        {
            // Wait for thread completion
            TaskThread *t = workers.get(i);

            // Merge and output statistics
            LSPString s;
            s.fmt_utf8("Supplementary thread %d statistics", int(i));
            merge_stats(&overall, t->get_stats());
            if (res != STATUS_BREAKPOINT)
                dump_stats(s.get_utf8(), t->get_stats());

            // Detroy thread object
            delete t;
        }
        delete root;
        workers.flush();

        // Dump overall statistics
        if (res != STATUS_BREAKPOINT)
            dump_stats("Overall statistics", &overall);

        // Destroy all tasks
        destroy_tasks(&vTasks);
        if (res != STATUS_OK)
            return res;

        // Normalize output
        if (bNormalize)
            normalize_output();

        float prg   = float(nProgressPoints) / float(nProgressMax);
        lsp_trace("Reporting progress %d/%d = %.2f%%", int(nProgressPoints), int(nProgressMax), prg * 100.0f);
        ++nProgressPoints;

        return report_progress(prg);
    }

    bool RayTrace3D::is_already_passed(const sample_t *bind)
    {
        for (size_t i=0; i<vCaptures.size(); ++i)
        {
            capture_t *cap = vCaptures.at(i);
            for (size_t j=0; j<cap->bindings.size(); ++j)
            {
                sample_t *s = cap->bindings.at(j);
                // We reached current position?
                if (s == bind)
                    return false;
                // Check that the same sample and channel have been processed earlier than 'bind'
                if ((s->sample == bind->sample) && (s->channel == bind->channel))
                    return true;
            }
        }
        return false;
    }

    void RayTrace3D::normalize_output()
    {
        float max_gain = 0.0f;
        cstorage<sample_t> plan;

        // Estimate the maximum output gain for each capture's binding
        for (size_t i=0; i<vCaptures.size(); ++i)
        {
            capture_t *cap = vCaptures.at(i);
            for (size_t j=0; j<cap->bindings.size(); ++j)
            {
                sample_t *s = cap->bindings.at(j);
                if (is_already_passed(s)) // Protect from measuring gain more than once
                    continue;

                // Estimate the maximum gain
                float c_gain = dsp::abs_max(s->sample->getBuffer(s->channel), s->sample->length());
                if (max_gain < c_gain)
                    max_gain = c_gain;
            }
        }

        // Now we know the maximum gain
        if (max_gain == 0.0f)
            return;
        max_gain = 1.0f / max_gain; // Now it's a norming factor

        // Perform the gain adjustment
        for (size_t i=0; i<vCaptures.size(); ++i)
        {
            capture_t *cap = vCaptures.at(i);
            for (size_t j=0; j<cap->bindings.size(); ++j)
            {
                sample_t *s = cap->bindings.at(j);
                if (is_already_passed(s)) // Protect from adjusting gain more than once
                    continue;

                // Apply the norming factor
                dsp::scale2(s->sample->getBuffer(s->channel), max_gain, s->sample->length());
            }
        }
    }

    bool RayTrace3D::check_bound_box(const bound_box3d_t *bbox, const rt_view_t *view)
    {
        vector3d_t pl[4];

        dsp::calc_plane_p3(&pl[0], &view->s, &view->p[0], &view->p[1]);
        dsp::calc_plane_p3(&pl[1], &view->s, &view->p[1], &view->p[2]);
        dsp::calc_plane_p3(&pl[2], &view->s, &view->p[2], &view->p[0]);
        dsp::calc_plane_p3(&pl[3], &view->p[0], &view->p[1], &view->p[2]);

        raw_triangle_t out[16], buf1[16], buf2[16], *q, *in;
        size_t n_out, n_buf1, n_buf2, *n_q, *n_in;

        // Cull each triangle of bounding box with four scissor planes
        for (size_t j=0, m = sizeof(bbox_map)/sizeof(size_t); j < m; )
        {
            // Initialize input and queue buffer
            q = buf1, in = buf2;
            n_q = &n_buf1, n_in = &n_buf2;

            // Put to queue with updated matrix
            n_buf1      = 1;    // One triangle in buffer 1
            n_buf2      = 0;    // No triangles in buffer 2
            q->p[0]     = bbox->p[bbox_map[j++]];
            q->p[1]     = bbox->p[bbox_map[j++]];
            q->p[2]     = bbox->p[bbox_map[j++]];

            // Cull triangle with planes
            for (size_t k=0; ; )
            {
                // Reset counters
                *n_in   = 0;
                n_out   = 0;

                // Split all triangles:
                // Put all triangles above the plane to out
                // Put all triangles below the plane to in
                for (size_t l=0; l < *n_q; ++l)
                {
                    dsp::split_triangle_raw(out, &n_out, in, n_in, &pl[k], &q[l]);
                    if ((n_out > 16) || ((*n_in) > 16))
                    {
                        lsp_trace("split overflow: n_out=%d, n_in=%d", int(n_out), int(*n_in));
//                        check_bound_box(bbox, view); // DEBUG: replay
                    }
                }

                // Interrupt cycle if there is no data to process
                if ((*n_in <= 0) || ((++k) >= 4))
                   break;

                // Swap buffers buf0 <-> buf1
                ::swap(n_q, n_in);
                ::swap(in, q);
            }

            if ((*n_in) > 0)
                break;
        }

        return (*n_in) > 0;
    }


} /* namespace lsp */
