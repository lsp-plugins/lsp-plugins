/*
 * RayTrace3D.cpp
 *
 *  Created on: 9 апр. 2017 г.
 *      Author: sadko
 */

#include <core/alloc.h>
#include <core/3d/common.h>
#include <core/3d/RayTrace3D.h>
#include <stdlib.h>
#include <sys/time.h>

#define SAMPLE_QUANTITY     512
#define TASK_LO_THRESH      0x2000
#define TASK_HI_THRESH      0x4000

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
        // Cleanup capture state
        for (size_t i=0; i<bindings.size(); ++i)
        {
            rt_binding_t *b     = bindings.at(i);
            if (b == NULL)
                continue;
            for (size_t j=0; j<b->bindings.size(); ++j)
            {
                // Cleanup sample
                sample_t *samp  = b->bindings.at(j);
                if (samp->sample != NULL)
                {
                    samp->sample->destroy();
                    delete samp->sample;
                    samp->sample = NULL;
                }
            }
            delete b;
        }

        destroy_objects(&objects);
        bindings.flush();
    }

    status_t RayTrace3D::TaskThread::run()
    {
        // Initialize DSP context
        dsp::context_t ctx;
        dsp::start(&ctx);

        // Enter the main loop
        status_t res = main_loop();
        destroy_tasks(&tasks);
        destroy_objects(&objects);

        // Finalize DSP context and return result
        dsp::finish(&ctx);
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
            if (res == STATUS_BREAK_POINT)
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
        status_t res;

        for (size_t i=0,n=trace->vSources.size(); i<n; ++i)
        {
            rt_source_settings_t *src = trace->vSources.get(i);
            if (src == NULL)
                return STATUS_CORRUPTED;

            // Generate source mesh
            cstorage<rt_group_t> groups;
            res = rt_gen_source_mesh(groups, src);
            if (res != STATUS_OK)
                return res;

            // Generate sources
            matrix3d_t tm = src->pos;

            for (size_t ti=0, n=groups.size(); ti<n; ++ti)
            {
                rt_group_t *grp = groups.at(ti);
                if (grp == NULL)
                    continue;

                rt_context_t *ctx   = new rt_context_t();
                if (ctx == NULL)
                    return STATUS_NO_MEM;

                RT_TRACE(trace->pDebug, ctx->set_debug_context(trace->pDebug); )

                dsp::apply_matrix3d_mp2(&ctx->view.s, &grp->s, &tm);
                dsp::apply_matrix3d_mp2(&ctx->view.p[0], &grp->p[0], &tm);
                dsp::apply_matrix3d_mp2(&ctx->view.p[1], &grp->p[1], &tm);
                dsp::apply_matrix3d_mp2(&ctx->view.p[2], &grp->p[2], &tm);

                ctx->state          = S_SCAN_OBJECTS;
                ctx->view.location  = 1.0f;
                ctx->view.oid       = -1;
                ctx->view.face      = -1;
                ctx->view.speed     = SOUND_SPEED_M_S;

                ctx->view.amplitude = src->amplitude;
                ctx->view.time[0]   = 0.0f;
                ctx->view.time[1]   = 0.0f;
                ctx->view.time[2]   = 0.0f;

                if (!tasks->add(ctx))
                {
                    delete ctx;
                    return STATUS_NO_MEM;
                }

                RT_TRACE_BREAK(trace->pDebug,
                    lsp_trace("Generated %d raytrace contexts for source %d", int(groups.size()), int(i));
                    for (size_t i=0, n=tasks->size(); i<n; ++i)
                    {
                        rt_context_t *ctx = tasks->at(i);
                        trace->pDebug->trace.add_view_1c(&ctx->view, &C3D_MAGENTA);
                    }
                );
            }
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

            ctx->trace.add_view_1c(&ctx->view, &C3D_MAGENTA);
            v_vertex3d_t v[3];
            for (size_t j=0, m = sizeof(bbox_map)/sizeof(size_t); j < m; )
            {
                v[0].p      = box.p[bbox_map[j++]];
                v[0].c      = C3D_YELLOW;
                v[1].p      = box.p[bbox_map[j++]];
                v[1].c      = C3D_YELLOW;
                v[2].p      = box.p[bbox_map[j++]];
                v[2].c      = C3D_YELLOW;

                dsp::calc_normal3d_p3(&v[0].n, &v[0].p, &v[1].p, &v[2].p);
                v[1].n      = v[0].n;
                v[2].n      = v[0].n;

                ctx->trace.add_triangle(v);
            }
        )

        // Perform simple bounding-box check
        bool res    = check_bound_box(&box, &ctx->view);

#ifdef LSP_RT_TRACE
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
#endif /* LSP_RT_TRACE */

        return (res) ? STATUS_OK : STATUS_SKIP;
    }

    status_t RayTrace3D::TaskThread::generate_root_mesh()
    {
        status_t res;
        size_t obj_id = 0;

        // Clear contents of the root context
        rt_mesh_t root;
        RT_TRACE(trace->pDebug, root.set_debug_context(trace->pDebug, &trace->pDebug->trace); );

        // Add capture objects as fake icosphere objects
        for (size_t i=0, n=trace->vCaptures.size(); i<n; ++i, ++obj_id)
        {
            capture_t *cap      = trace->vCaptures.get(i);
            if (cap == NULL)
                return STATUS_BAD_STATE;
            if ((res = generate_capture_mesh(obj_id, cap)) != STATUS_OK)
                return res;
        }

        // Add scene objects
        for (size_t i=0, oid=obj_id, n=trace->pScene->num_objects(); i<n; ++i, ++oid)
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
            res         = root.add_object(obj, oid, obj->matrix(), m);
            if (res != STATUS_OK)
                return res;
        }

        RT_TRACE_BREAK(trace->pDebug,
            lsp_trace("Prepared scene (%d triangles)", int(root.triangle.size()));
            for (size_t i=0,n=root.triangle.size(); i<n; ++i)
                trace->pDebug->trace.add_triangle_3c(root.triangle.get(i), &C3D_RED, &C3D_GREEN, &C3D_BLUE);
        );

        // Solve conflicts between all objects
        res = root.solve_conflicts();
        if (res != STATUS_OK)
            return res;

        lsp_trace("Overall mesh statistics: %d vertexes, %d edges, %d triangles",
                int(root.vertex.size()), int(root.edge.size()), int(root.triangle.size()));

        // Generate object meshes
        destroy_objects(&objects);
        for (size_t i=0, n=trace->pScene->num_objects(); i<n; ++i, ++obj_id)
        {
            // Get object
            Object3D *obj       = trace->pScene->object(i);
            if (obj == NULL)
                return STATUS_BAD_STATE;
            else if (!obj->is_visible()) // Skip invisible objects
                continue;

            // Create object
            rt_object_t *rt = new rt_object_t();
            if (rt == NULL)
                return STATUS_NO_MEM;
            else if (!objects.add(rt)) {
                delete rt;
                return STATUS_NO_MEM;
            }

            // Compute object's bounding box
            obj->calc_bound_box();
            if ((res = generate_object_mesh(obj_id, rt, &root, obj, obj->matrix())) != STATUS_OK)
                return res;
        }

        RT_TRACE(trace->pDebug,
            if (!trace->pScene->validate())
                return STATUS_CORRUPTED;
        )

        RT_TRACE_BREAK(trace->pDebug,
            lsp_trace("Added capture objects (%d triangles)", int(root.triangle.size()));
            for (size_t i=0,n=root.triangle.size(); i<n; ++i)
                trace->pDebug->trace.add_triangle_3c(root.triangle.get(i), &C3D_RED, &C3D_GREEN, &C3D_BLUE);
        );

        return res;
    }

    status_t RayTrace3D::TaskThread::generate_capture_mesh(size_t id, capture_t *c)
    {
        status_t res;
        cstorage<raw_triangle_t> mesh;

        // Generate capture mesh
        if ((res = rt_gen_capture_mesh(mesh, c)) != STATUS_OK)
            return res;

        // Generate bound box for capture and it's mesh
        bound_box3d_t *b    = &c->bbox;
        float r             = c->radius;
        dsp::init_point_xyz(&b->p[0], -r, r, r);
        dsp::init_point_xyz(&b->p[1], -r, -r, r);
        dsp::init_point_xyz(&b->p[2], r, -r, r);
        dsp::init_point_xyz(&b->p[3], r, r, r);
        dsp::init_point_xyz(&b->p[4], -r, r, -r);
        dsp::init_point_xyz(&b->p[5], -r, -r, -r);
        dsp::init_point_xyz(&b->p[6], r, -r, -r);
        dsp::init_point_xyz(&b->p[7], r, r, -r);

        // Apply changes to bounding box
        for (size_t j=0; j<8; ++j)
            dsp::apply_matrix3d_mp1(&b->p[j], &c->pos);

        // Convert data
        size_t count = mesh.size();
        raw_triangle_t *src = mesh.get_array();
        rt_triangle_t *dst = c->mesh.append_n(count);
        if (dst == NULL)
            return STATUS_NO_MEM;

        for (size_t j=0; j < count; ++j, ++src, ++dst)
        {
            // Generate points and fill additional fields
            dsp::apply_matrix3d_mp2(&dst->v[0], &src->v[0], &c->pos);
            dsp::apply_matrix3d_mp2(&dst->v[1], &src->v[1], &c->pos);
            dsp::apply_matrix3d_mp2(&dst->v[2], &src->v[2], &c->pos);
            dsp::calc_plane_pv(&dst->n, src->v);

            dst->oid        = id;
            dst->face       = j;
            dst->m          = NULL;
        }

        return res;
    }

    status_t RayTrace3D::TaskThread::generate_object_mesh(ssize_t id, rt_object_t *o, rt_mesh_t *src, Object3D *obj, const matrix3d_t *m)
    {
        rtx_edge_t *e;

        // Initialize ptag
        for (size_t i=0, n=src->edge.size(); i<n; ++i)
            src->edge.get(i)->itag  = -1;

        // Build set of triangles and edges
        ssize_t itag = 0;
        for (size_t i=0, n=src->triangle.size(); i<n; ++i)
        {
            // Fetch triangle
            const rtm_triangle_t *t = src->triangle.get(i);
            if (t->oid != id)
                continue;

            // Add triangle
            rtx_triangle_t *rt = o->mesh.add();
            if (rt == NULL)
                return STATUS_NO_MEM;

            rt->v[0]    = *(t->v[0]);
            rt->v[1]    = *(t->v[1]);
            rt->v[2]    = *(t->v[2]);
            rt->n       = t->n;
            rt->oid     = t->oid;
            rt->face    = t->face;
            rt->m       = t->m;

            // Add edges to if they are still not present
            for (size_t j=0; j<3; ++j)
            {
                rtm_edge_t *se  = t->e[j];
                rt->e[j]        = reinterpret_cast<rtx_edge_t *>(se);

                if (se->itag < 0)
                {
                    if ((e = o->plan.add()) == NULL)
                        return STATUS_NO_MEM;
                    e->v[0]         = *(se->v[0]);
                    e->v[1]         = *(se->v[1]);
                    se->itag        = itag++;
                }
            }
        }

        // Patch edge pointers according to stored indexes
        for (size_t i=0, n=o->mesh.size(); i<n; ++i)
        {
            rtx_triangle_t *rt = o->mesh.at(i);
            for (size_t j=0; j<3; ++j)
            {
                rtm_edge_t *se  = reinterpret_cast<rtm_edge_t *>(rt->e[j]);
                rt->e[j]        = o->plan.at(se->itag);
            }
        }

        // Apply changes to bound box
        const obj_boundbox_t *bbox = obj->bound_box();
        for (size_t i=0; i<8; ++i)
            dsp::apply_matrix3d_mp2(&o->bbox.p[i], &bbox->p[i], m);

        return STATUS_OK;
    }

    status_t RayTrace3D::TaskThread::scan_objects(rt_context_t *ctx)
    {
        status_t res = STATUS_OK;

        RT_TRACE_BREAK(trace->pDebug,
            lsp_trace("Scanning objects...");
            ctx->trace.add_view_1c(&ctx->view, &C3D_MAGENTA);
        )

        // Initialize context's view
        ctx->init_view();

        // Initialize scan variables
        size_t max_objs     = trace->pScene->num_objects() + trace->vCaptures.size();
        size_t segs         = (max_objs + (sizeof(size_t) * 8) - 1) / (sizeof(size_t) * 8);
        size_t *objs        = reinterpret_cast<size_t *>(alloca(sizeof(size_t) * segs));
        for (size_t i=0; i<segs; ++i)
            objs[i]             = 0;

        size_t n_objs       = 0;
        size_t obj_id       = 0;

        // Add captures as opaque objects
        obj_id = 0;
        for (size_t i=0, n=trace->vCaptures.size(); i<n; ++i, ++obj_id)
        {
            capture_t *cap      = trace->vCaptures.at(i);
            if (cap == NULL)
                return STATUS_BAD_STATE;

            RT_TRACE_BREAK(trace->pDebug,
                lsp_trace("Testing capture bound box");

                ctx->trace.add_view_1c(&ctx->view, &C3D_MAGENTA);
                v_vertex3d_t v[3];
                for (size_t j=0, m = sizeof(bbox_map)/sizeof(size_t); j < m; )
                {
                    v[0].p      = cap->bbox.p[bbox_map[j++]];
                    v[0].c      = C3D_YELLOW;
                    v[1].p      = cap->bbox.p[bbox_map[j++]];
                    v[1].c      = C3D_YELLOW;
                    v[2].p      = cap->bbox.p[bbox_map[j++]];
                    v[2].c      = C3D_YELLOW;

                    dsp::calc_normal3d_p3(&v[0].n, &v[0].p, &v[1].p, &v[2].p);
                    v[1].n      = v[0].n;
                    v[2].n      = v[0].n;

                    ctx->trace.add_triangle(v);
                }
            )

            if (!check_bound_box(&cap->bbox, &ctx->view))
            {
                RT_TRACE_BREAK(trace->pDebug,
                    ctx->trace.add_view_1c(&ctx->view, &C3D_MAGENTA);
                    for (size_t j=0,m=cap->mesh.size(); j<m; ++j)
                    {
                        rt_triangle_t *st = cap->mesh.at(j);

                        v_triangle3d_t t;
                        t.p[0]  = st->v[0];
                        t.p[1]  = st->v[1];
                        t.p[2]  = st->v[2];
                        t.n[0]  = st->n;
                        t.n[1]  = st->n;
                        t.n[2]  = st->n;

                        ctx->ignored.add(&t);
                    }
                );
                continue;
            }

            // Add capture as opaque object
            if ((res = ctx->add_opaque_object(cap->mesh.get_array(), cap->mesh.size())) != STATUS_OK)
                return res;
        }

        // Iterate all object and add to the context if the object is potentially participating the ray tracing algorithm
        for (size_t i=0, n=objects.size(); i<n; ++i)
        {
            rt_object_t *rt = objects.at(i);
            if (rt == NULL)
                return STATUS_BAD_STATE;

            // Check bound box
            bool check = rt->mesh.size() > 16;
            if (check)
            {
                RT_TRACE_BREAK(trace->pDebug,
                    lsp_trace("Testing bound box");

                    ctx->trace.add_view_1c(&ctx->view, &C3D_MAGENTA);
                    v_vertex3d_t v[3];
                    for (size_t j=0, m = sizeof(bbox_map)/sizeof(size_t); j < m; )
                    {
                        v[0].p      = rt->bbox.p[bbox_map[j++]];
                        v[0].c      = C3D_YELLOW;
                        v[1].p      = rt->bbox.p[bbox_map[j++]];
                        v[1].c      = C3D_YELLOW;
                        v[2].p      = rt->bbox.p[bbox_map[j++]];
                        v[2].c      = C3D_YELLOW;

                        dsp::calc_normal3d_p3(&v[0].n, &v[0].p, &v[1].p, &v[2].p);
                        v[1].n      = v[0].n;
                        v[2].n      = v[0].n;

                        ctx->trace.add_triangle(v);
                    }
                )

                // Perform simple bounding-box check
                check = !check_bound_box(&rt->bbox, &ctx->view);
            }
            if (check)
            {
                RT_TRACE(trace->pDebug,
                    for (size_t j=0,m=rt->mesh.size(); j<m; ++j)
                    {
                        rtx_triangle_t *st = rt->mesh.at(j);

                        v_triangle3d_t t;
                        t.p[0]  = st->v[0];
                        t.p[1]  = st->v[1];
                        t.p[2]  = st->v[2];
                        t.n[0]  = st->n;
                        t.n[1]  = st->n;
                        t.n[2]  = st->n;

                        ctx->ignored.add(&t);
                    }
                );
                continue;
            }

            // Add object to context
            res = ctx->add_object(rt->mesh.get_array(), rt->plan.get_array(), rt->mesh.size(), rt->plan.size());
            if (res != STATUS_OK)
                return res;
            ++n_objs;
        }

        RT_TRACE_BREAK(trace->pDebug,
            lsp_trace("Fetched %d objects", int(n_objs));
            ctx->trace.add_view_1c(&ctx->view, &C3D_MAGENTA);
            for (size_t i=0,n=ctx->triangle.size(); i<n; ++i)
                ctx->trace.add_triangle_3c(ctx->triangle.get(i), &C3D_RED, &C3D_GREEN, &C3D_BLUE);

            for (size_t i=0, n=ctx->plan.items.size(); i<n; ++i)
                ctx->trace.add_segment(ctx->plan.items.get(i), &C3D_YELLOW);
        )

        // Update state
        if (!ctx->plan.is_empty())
            ctx->state  = S_SPLIT;
        else if (ctx->triangle.size() <= 0)
        {
            delete ctx;
            return STATUS_OK;
        }
        else
            ctx->state  = S_REFLECT;

        return submit_task(ctx);
    }

    status_t RayTrace3D::TaskThread::cull_view(rt_context_t *ctx)
    {
        status_t res = ctx->cull_view();
        if (res != STATUS_OK)
            return res;

        if (!ctx->plan.is_empty())
            ctx->state  = S_SPLIT;
        else if (ctx->triangle.size() <= 0)
        {
            delete ctx;
            return STATUS_OK;
        }
        else
            ctx->state  = S_REFLECT;

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
            ctx->state  = (ctx->plan.is_empty()) ? S_REFLECT : S_SPLIT;
            return submit_task(ctx);
        }
        else if (out.triangle.size() > 0)
        {
            ctx->swap(&out);
            ctx->state  = (ctx->plan.is_empty()) ? S_REFLECT : S_SPLIT;

            return submit_task(ctx);
        }

        delete ctx;
        return STATUS_OK;
    }

    status_t RayTrace3D::TaskThread::cullback_view(rt_context_t *ctx)
    {
        RT_TRACE_BREAK(trace->pDebug,
            lsp_trace("Performing depth test...");
            ctx->trace.add_view_1c(&ctx->view, &C3D_MAGENTA);
            for (size_t i=0,n=ctx->triangle.size(); i<n; ++i)
                ctx->trace.add_triangle_1c(ctx->triangle.get(i), &C3D_RED);
        )

        // Perform cullback
        status_t res = ctx->depth_test();
        if (res != STATUS_OK)
            return res;

        RT_TRACE_BREAK(trace->pDebug,
            lsp_trace("After depth test %d triangles", int(ctx->triangle.size()));
        ctx->trace.add_view_1c(&ctx->view, &C3D_MAGENTA);
            for (size_t i=0,n=ctx->triangle.size(); i<n; ++i)
                ctx->trace.add_triangle_1c(ctx->triangle.get(i), &C3D_GREEN);
        );

        if (ctx->triangle.size() <= 0)
        {
            delete ctx;
            return STATUS_OK;
        }

        ctx->state  = S_REFLECT;
        return submit_task(ctx);
    }

#ifdef LSP_TRACE
    void invalid_state_hook()
    {
        lsp_error("Invalid state");
    }
#endif

    status_t RayTrace3D::TaskThread::reflect_view(rt_context_t *ctx)
    {
        rt_context_t *rc;
        rt_view_t sv, v, rv, tv;        // source view, view, captured view, reflected view, transparent trace
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
                if ((dA <= -trace->fTolerance) || (dA >= trace->fTolerance))
                {
                    valid = false;
                    break;
                }

                t[j]        = (sv.time[0] * a[0] + sv.time[1] * a[1] + sv.time[2] * a[2]) * revA; // Compute projected point's time
                v.time[j]   = t[j] + (d[j] / sv.speed);
                #ifdef LSP_TRACE
                    if (v.time[j] > 30.0f)
                        invalid_state_hook();
                #endif
            }

            if (!valid)
                continue;

            // Compute area of projected triangle
            float area  = dsp::calc_area_pv(p);
            if (area <= trace->fDetalization)
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

                ctx->trace.add_triangle_1c(ct, &C3D_YELLOW);
                ctx->trace.add_plane_3pn1c(&ct->v[0], &ct->v[1], &ct->v[2], &ct->n, &C3D_GREEN);
                ctx->trace.add_view_1c(&sv, &C3D_MAGENTA);
                ctx->trace.add_segment(&p[0], &ct->v[0], &C3D_RED);
                ctx->trace.add_segment(&p[1], &ct->v[1], &C3D_GREEN);
                ctx->trace.add_segment(&p[2], &ct->v[2], &C3D_BLUE);
            )

            // Perform capture/reflect
            capture_t *cap  = trace->vCaptures.get(ct->oid);
            if (cap != NULL)
            {
                rt_binding_t *b = bindings.get(ct->oid);
                if (b != NULL)
                {
                    // Perform synchronized capturing
                    ++stats.calls_capture;
                    #ifdef LSP_RT_TRACE
                        res = capture(cap, &b->bindings, &v, &ctx->trace);
                    #else
                        res = capture(cap, &b->bindings, &v);
                    #endif /* LSP_RT_TRACE */
                }
                else
                    res = STATUS_CORRUPTED;
            }
            else
            {
                // Get material
                rt_material_t *m    = ct->m;

                // Compute reflected and refracted views
                rv          = v;
                tv          = v;

                if (distance > 0.0f)
                {
                    v.amplitude    *= (1.0f - m->absorption[0]);

                    kd              = (1.0f + 1.0f / m->diffusion[0]) * distance;
                    rv.amplitude    = v.amplitude * (m->transparency[0] - 1.0f); // Sign negated
                    rv.s.x         -= kd * ct->n.dx;
                    rv.s.y         -= kd * ct->n.dy;
                    rv.s.z         -= kd * ct->n.dz;
                    rv.rnum         = v.rnum + 1;       // Increment reflection number

                    kd              = (m->permeability/m->dispersion[0] - 1.0f) * distance;
                    tv.amplitude    = v.amplitude * m->transparency[0];
                    tv.speed       *= m->permeability;
                    tv.s.x         += kd * ct->n.dx;
                    tv.s.y         += kd * ct->n.dy;
                    tv.s.z         += kd * ct->n.dz;
                    tv.location     = - v.location;     // Invert location of transparent trace

                    #ifdef LSP_TRACE
                        if (tv.speed > 10000.0f)
                            invalid_state_hook();
                        else if (tv.speed < 10.0f)
                            invalid_state_hook();
                    #endif

                    RT_TRACE_BREAK(trace->pDebug,
                        lsp_trace("Outside->inside reflect_view");
                        lsp_trace("Amplitude: reflected=%e, refracted=%e", rv.amplitude, tv.amplitude);
                        lsp_trace("Material: absorption=%f, transparency=%f, permeability=%f, diffusion=%f, dispersion=%f",
                                m->absorption[0], m->transparency[0], m->permeability, m->diffusion[0], m->dispersion[0]);

                        ctx->trace.add_view_1c(&sv, &C3D_RED);
                        ctx->trace.add_view_1c(&rv, &C3D_GREEN);
                        ctx->trace.add_view_1c(&tv, &C3D_CYAN);
                    )
                }
                else
                {
                    v.amplitude    *= (1.0f - m->absorption[1]);

                    kd              = (1.0f + 1.0f / m->diffusion[1]) * distance;
                    rv.amplitude    = v.amplitude * (m->transparency[1] - 1.0f); // Sign negated
                    rv.s.x         -= kd * ct->n.dx;
                    rv.s.y         -= kd * ct->n.dy;
                    rv.s.z         -= kd * ct->n.dz;
                    rv.rnum         = v.rnum + 1;       // Increment reflection number

                    kd              = (1.0f/(m->dispersion[1]*m->permeability) - 1.0f) * distance;
                    tv.amplitude    = v.amplitude * m->transparency[1];
                    tv.speed       /= m->permeability;
                    tv.s.x         += kd * ct->n.dx;
                    tv.s.y         += kd * ct->n.dy;
                    tv.s.z         += kd * ct->n.dz;
                    tv.location     = - v.location;     // Invert location of transparent trace

                    #ifdef LSP_TRACE
                        if (tv.speed > 10000.0f)
                            invalid_state_hook();
                        else if (tv.speed < 10.0f)
                            invalid_state_hook();
                    #endif

                    RT_TRACE_BREAK(trace->pDebug,
                        lsp_trace("Inside->outside reflect_view");
                        lsp_trace("Amplitude: reflected=%e, refracted=%e", rv.amplitude, tv.amplitude);
                        lsp_trace("Material: absorption=%f, transparency=%f, permeability=%f, diffusion=%f, dispersion=%f",
                                m->absorption[1], m->transparency[1], m->permeability, m->diffusion[1], m->dispersion[1]);

                        ctx->trace.add_view_1c(&sv, &C3D_RED);
                        ctx->trace.add_view_1c(&rv, &C3D_GREEN);
                        ctx->trace.add_view_1c(&tv, &C3D_CYAN);
                    )
                }

                // Create reflection context
                if ((rv.amplitude <= -trace->fEnergyThresh) || (rv.amplitude >= trace->fEnergyThresh))
                {
                    // Revert the order of triangle because direction has changed to opposite
                    rv.p[1]     = v.p[2];
                    rv.p[2]     = v.p[1];

                    if ((rc = new rt_context_t(&rv, S_SCAN_OBJECTS)) != NULL)
                    {
                        RT_TRACE(trace->pDebug,
                            rc->set_debug_context(trace->pDebug);
                        );

                        if ((res = submit_task(rc)) != STATUS_OK)
                            delete rc;
                    }
                    else
                        res = STATUS_NO_MEM;
                }

                // Create refraction context
                if ((tv.amplitude <= -trace->fEnergyThresh) || (tv.amplitude >= trace->fEnergyThresh))
                {
                    if ((rc = new rt_context_t(&tv, S_SCAN_OBJECTS)) != NULL)
                    {
                        RT_TRACE(trace->pDebug,
                            rc->set_debug_context(trace->pDebug);
                        );

                        if ((res = submit_task(rc)) != STATUS_OK)
                            delete rc;
                    }
                    else
                        res = STATUS_NO_MEM;
                }
            }

            // Analyze result
            if (res != STATUS_OK)
                break;
        }

        // Drop current context on OK status
        if (res == STATUS_OK)
            delete ctx;
        return res;
    }

#ifdef LSP_RT_TRACE
    status_t RayTrace3D::TaskThread::capture(capture_t *capture, cstorage<sample_t> *bindings, const rt_view_t *v, View3D *view)
#else
    status_t RayTrace3D::TaskThread::capture(capture_t *capture, cstorage<sample_t> *bindings, const rt_view_t *v)
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
        if (v_area <= trace->fDetalization) // Prevent from becoming NaNs
            return STATUS_OK;

        vector3d_t cv, pv;
        float afactor       = v->amplitude / sqrtf(v_area); // The norming energy factor
        dsp::unit_vector_p1pv(&cv, &v->s, v->p);
        pv                  = capture->direction;
        float kcos          = cv.dx*pv.dx + cv.dy*pv.dy + cv.dz * pv.dz; // cos(a)

        // Analyze capture type
        switch (capture->type)
        {
            case RT_AC_CARDIO:
                afactor    *= 0.5f * (1.0f - kcos); // 0.5 * (1 + cos(a))
                break;

            case RT_AC_SCARDIO:
                afactor    *= 2*fabs(0.5 - kcos)/3.0f; // 2*(0.5 + cos(a)) / 3
                break;

            case RT_AC_HCARDIO:
                afactor    *= 0.8*fabs(0.25 - kcos); // 4*(0.25 + cos(a)) / 5
                break;

            case RT_AC_BIDIR:
                afactor    *= kcos;    // factor = factor * cos(a)
                break;

            case RT_AC_EIGHT:
                afactor    *= kcos*kcos;  // factor = factor * cos(a)^2
                break;

            case RT_AC_OMNI: // factor = factor * 1
            default:
                break;
        }

        // Estimate distance and time parameters for source point
        vector3d_t ds[3];
        raw_triangle_t src;
        float ts[3], tsn[3];

        for (size_t i=0; i<3; ++i)
        {
            src.v[i]    = v->p[i];
            dsp::init_vector_p2(&ds[i], &v->s, &src.v[i]);  // Delta vector
            float dist  = dsp::calc_distance_v1(&ds[i]);
            ts[i]       = v->time[i] - dist / v->speed;     // Time at the source point
            tsn[i]      = v->time[i] * trace->nSampleRate;  // Sample number
        }

        // Estimate the culling sample number
        ssize_t csn;
        if ((tsn[0] < tsn[1]) && (tsn[0] < tsn[2]))
            csn     = tsn[0];
        else
            csn     = (tsn[1] < tsn[2]) ? tsn[1] : tsn[2];
#ifdef LSP_TRACE
//        ssize_t ssn = csn;                                      // Start culling sample number
#endif
        ++csn;

        // Perform integration
        vector3d_t spl;
        raw_triangle_t in[2], out[2];
        size_t n_in, n_out;
        float prev_area     = 0.0f;                             // The area of polygon at previous step
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
                view->add_view_1c(v, &C3D_MAGENTA);
                view->add_triangle_pv1c(src.v, &C3D_ORANGE);
                view->add_plane_pvn1c(p, &spl, &C3D_YELLOW);
            )

            // Perform split
            n_out = 0, n_in = 0;
            dsp::split_triangle_raw(out, &n_out, in, &n_in, &spl, &src);

            // Compute the area of triangle and ensure that it is greater than previous value
            float in_area       = 0.0f;
            for (size_t i=0; i<n_in; ++i)
                in_area          += dsp::calc_area_pv(in[i].v);
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
                    view->add_view_1c(v, &C3D_MAGENTA);
                    view->add_plane_pvn1c(p, &spl, &C3D_YELLOW);
                    for (size_t i=0; i<n_in; ++i)
                        view->add_triangle_pv1c(in[i].v, &C3D_GREEN);
                    for (size_t i=0; i<n_out; ++i)
                        view->add_triangle_pv1c(out[i].v, &C3D_RED);
                )

                // Deploy energy value to the sample
                if (csn > 0)
                {
                    // Lock capture data
//                    trace->lkCapture.lock();

                    // Append sample to each matching capture
                    for (size_t ci=0, cn=bindings->size(); ci<cn; ++ci)
                    {
                        sample_t *s = bindings->at(ci);

                        // Skip reflection not in range
                        if ((s->r_min >= 0) && (v->rnum < s->r_min))
                            continue;
                        else if ((s->r_max >= 0) && (v->rnum > s->r_max))
                            continue;

                        // Ensure that we need to resize sample
                        size_t len = s->sample->length();
                        if (len <= size_t(csn))
                        {
                            // Need to resize sample?
                            if (s->sample->max_length() <= size_t(csn))
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
                                #ifdef LSP_TRACE
                                    if (len > 0x100000) // TODO: This is currently impossible, added for debugging, remove in future
                                        invalid_state_hook();
                                #endif
                                if (!s->sample->resize(s->sample->channels(), len, len))
                                {
                                    #ifdef LSP_TRACE
                                        invalid_state_hook();
                                    #endif
                                    return STATUS_NO_MEM;
                                }
                            }

                            // Update sample length
                            s->sample->setLength(csn + 1);
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

//        lsp_trace("Samples %d-%d -> area=%e amplitude=%e kcos=%f rnum=%d",
//                int(ssn), int(csn-1), v_area, v->amplitude, kcos, int(v->rnum));

        return STATUS_OK;
    }

    status_t RayTrace3D::TaskThread::prepare_main_loop(float initial)
    {
        // Cleanup stats
        clear_stats(&stats);

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
                    trace->pDebug->trace.add_triangle(obj->triangle(j), &C3D_RED, &C3D_GREEN, &C3D_BLUE);
            }
        );

        // Prepare root context and captures
        res         = generate_root_mesh();
        if (res == STATUS_OK)
            res         = prepare_captures();

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
                ++stats.root_tasks;
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

    status_t RayTrace3D::TaskThread::prepare_captures()
    {
        // Copy capture state
        for (size_t i=0; i<trace->vCaptures.size(); ++i)
        {
            // Allocate capture
            capture_t *scap = trace->vCaptures.get(i);
            rt_binding_t *b = new rt_binding_t();
            if (b == NULL)
                return STATUS_NO_MEM;
            else if (!bindings.add(b))
            {
                delete b;
                return STATUS_NO_MEM;
            }

            // Copy bindings
            for (size_t j=0; j<scap->bindings.size(); ++j)
            {
                // Allocate binding
                sample_t *ssamp = scap->bindings.get(j);
                sample_t *dsamp = b->bindings.add();
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

        return STATUS_OK;
    }

    status_t RayTrace3D::TaskThread::prepare_supplementary_loop(TaskThread *t)
    {
        // Cleanup statistics
        clear_stats(&stats);

        // Prepare captures and data context
        status_t res = prepare_captures();
        if (res == STATUS_OK)
            res = copy_objects(&t->objects);

        return res;
    }

    status_t RayTrace3D::TaskThread::copy_objects(cvector<rt_object_t> *src)
    {
        rtx_edge_t *se, *de;
        rtx_triangle_t *dt;

        for (size_t i=0, n=src->size(); i<n; ++i)
        {
            // Get source object
            rt_object_t *s = src->at(i);
            if (s == NULL)
                return STATUS_CORRUPTED;

            // Create destination object
            rt_object_t *d = new rt_object_t;
            if (d == NULL)
                return STATUS_NO_MEM;
            else if (!objects.add(d))
            {
                delete d;
                return STATUS_NO_MEM;
            }

            // Copy edges and triangles
            if (!d->plan.add_all(&s->plan))
                return STATUS_NO_MEM;
            if (!d->mesh.add_all(&s->mesh))
                return STATUS_NO_MEM;

            // Patch pointers
            se = s->plan.get_array();
            de = d->plan.get_array();
            dt = d->mesh.get_array();
            for (size_t j=0, m=d->mesh.size(); j<m; ++j, ++dt)
            {
                dt->e[0] = &de[dt->e[0] - se];
                dt->e[1] = &de[dt->e[1] - se];
                dt->e[2] = &de[dt->e[2] - se];
            }

            // Copy bound box
            d->bbox     = s->bbox;
        }

        return STATUS_OK;
    }

    status_t RayTrace3D::TaskThread::merge_result()
    {
        cvector<capture_t> &dst = trace->vCaptures;
        if (dst.size() != bindings.size())
            return STATUS_CORRUPTED;

        for (size_t i=0; i<dst.size(); ++i)
        {
            rt_binding_t   *csrc    = bindings.at(i);
            capture_t      *cdst    = dst.at(i);

            if (csrc->bindings.size() != cdst->bindings.size())
                return STATUS_CORRUPTED;

            for (size_t j=0; j<csrc->bindings.size(); ++j)
            {
                sample_t *ssrc  = csrc->bindings.at(j);
                sample_t *sdst  = cdst->bindings.at(j);

                if ((ssrc->sample == NULL) || (sdst->sample == NULL))
                    return STATUS_CORRUPTED;

                // Compute new sample size
                size_t nc = ssrc->sample->channels();
                if (nc != sdst->sample->channels())
                    return STATUS_CORRUPTED;

                bool resize = false;
                size_t maxlen = sdst->sample->max_length();
                if (maxlen < ssrc->sample->max_length())
                {
                    maxlen  = ssrc->sample->max_length();
                    resize  = true;
                }

                size_t len = sdst->sample->length();
                if (len < ssrc->sample->length())
                {
                    len     = ssrc->sample->length();
                    resize  = true;
                }

                if (maxlen < len)
                    maxlen  = len;

                // Check that we need resize
                if (resize)
                {
                    if (!sdst->sample->resize(nc, maxlen, len))
                        return STATUS_NO_MEM;
                }

                // Apply changes to the target sample
                for (size_t k=0; k<nc; ++k)
                {
//                    lsp_trace("Merge %p (%lld samples) -> %p (%lld samples), channel=%d/%d",
//                            ssrc->sample->getBuffer(k),
//                            (long long)(ssrc->sample->length()),
//                            sdst->sample->getBuffer(k),
//                            (long long)(sdst->sample->length()),
//                            int(k), int(nc)
//                            );
                    dsp::add2(
                            sdst->sample->getBuffer(k),
                            ssrc->sample->getBuffer(k),
                            ssrc->sample->length()
                        );
                }
            }
        }

        return STATUS_OK;
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
        fDetalization   = 1e-10f;
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

    void RayTrace3D::destroy_objects(cvector<rt_object_t> *objects)
    {
        for (size_t i=0, n=objects->size(); i<n; ++i)
        {
            rt_object_t *obj = objects->get(i);
            if (obj != NULL)
            {
                obj->mesh.flush();
                obj->plan.flush();
                delete obj;
            }
        }

        objects->flush();
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
                m->diffusion[0]     = 1.0f;
                m->dispersion[0]    = 1.0f;
                m->transparency[0]  = 0.48f;

                m->absorption[1]    = 0.0f;
                m->diffusion[1]     = 1.0f;
                m->dispersion[1]    = 1.0f;
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

    status_t RayTrace3D::add_source(const rt_source_settings_t *settings)
    {
        if (settings == NULL)
            return STATUS_BAD_ARGUMENTS;

        rt_source_settings_t *src = vSources.add();
        if (src == NULL)
            return STATUS_NO_MEM;

        *src        = *settings;

        return STATUS_OK;
    }

    ssize_t RayTrace3D::add_capture(const rt_capture_settings_t *settings)
    {
        if (settings == NULL)
            return STATUS_BAD_ARGUMENTS;

        capture_t *cap      = new capture_t();
        if (cap == NULL)
            return -STATUS_NO_MEM;

        size_t idx          = vCaptures.size();
        if (!vCaptures.add(cap))
        {
            delete cap;
            return -STATUS_NO_MEM;
        }

        cap->pos            = settings->pos;
        dsp::init_vector_dxyz(&cap->direction, 1.0f, 0.0f, 0.0f);
        cap->radius         = settings->radius;
        cap->type           = settings->type;

        dsp::apply_matrix3d_mv1(&cap->direction, &cap->pos);
        dsp::normalize_vector(&cap->direction);

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

    status_t RayTrace3D::set_material(size_t idx, const rt_material_t *material)
    {
        rt_material_t *m = vMaterials.get(idx);
        if (m == NULL)
            return STATUS_INVALID_VALUE;
        *m = *material;
        return STATUS_OK;
    }

    status_t RayTrace3D::get_material(rt_material_t *material, size_t idx)
    {
        if (material == NULL)
            return STATUS_BAD_ARGUMENTS;
        rt_material_t *m = vMaterials.get(idx);
        if (m == NULL)
            return STATUS_INVALID_VALUE;

        *material = *m;
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

    status_t RayTrace3D::do_process(size_t threads, float initial)
    {
        status_t res = STATUS_OK;
        bCancelled   = false;
        bFailed      = false;

        // Get time of execution start
#ifdef LSP_TRACE
        struct timespec tstart;
        clock_gettime(CLOCK_REALTIME, &tstart);
#endif

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
        if (vTasks.size() > 0)
        {
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
        root->merge_result();
        if (res != STATUS_BREAK_POINT)
            dump_stats("Main thread statistics", root->get_stats());

        // Output thread stats and destroy threads
        for (size_t i=0,n=workers.size(); i<n; ++i)
        {
            // Post-process each thread
            TaskThread *t = workers.get(i);
            t->merge_result();

            // Merge and output statistics
            LSPString s;
            s.fmt_utf8("Supplementary thread %d statistics", int(i));
            merge_stats(&overall, t->get_stats());
            if (res != STATUS_BREAK_POINT)
                dump_stats(s.get_utf8(), t->get_stats());

            // Detroy thread object
            delete t;
        }
        delete root;
        workers.flush();

        // Dump overall statistics
        if (res != STATUS_BREAK_POINT)
        {
            // Get time of execution end
#ifdef LSP_TRACE
            struct timespec tend;
            clock_gettime(CLOCK_REALTIME, &tend);
            double etime = double(tend.tv_sec - tstart.tv_sec) + double(tend.tv_nsec - tend.tv_nsec) * 1e-6;
#endif

            dump_stats("Overall statistics", &overall);
            lsp_trace("Overall execution time:      %f s", etime);
        }

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

    status_t RayTrace3D::process(size_t threads, float initial)
    {
        // We need to initialize DSP context for processing
        dsp::context_t ctx;
        dsp::start(&ctx);

        // Call processing routine
        status_t res = do_process(threads, initial);

        // Finalize DSP context and exit
        dsp::finish(&ctx);
        return res;
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
                dsp::mul_k2(s->sample->getBuffer(s->channel), max_gain, s->sample->length());
            }
        }
    }

    bool RayTrace3D::check_bound_box(const bound_box3d_t *bbox, const rt_view_t *view)
    {
        const vector3d_t *pl;
        raw_triangle_t buf1[16], buf2[16], *in, *out;
        size_t nin, nout;

        // Cull each triangle of bounding box with four scissor planes
        for (size_t i=0, m = sizeof(bbox_map)/sizeof(size_t); i < m; )
        {
            // Initialize input
            in          = buf1;
            out         = buf2;
            nin         = 1;

            in->v[0]    = bbox->p[bbox_map[i++]];
            in->v[1]    = bbox->p[bbox_map[i++]];
            in->v[2]    = bbox->p[bbox_map[i++]];
            pl          = view->pl;

            // Cull triangle with planes
            for (size_t j=0; j<4; ++j, ++pl)
            {
                // Reset counters
                nout    = 0;
                for (size_t k=0; k < nin; ++k, ++in)
                    dsp::cull_triangle_raw(out, &nout, pl, in);

                // Interrupt cycle if there is no data to process
                if (!nout)
                    break;

                // Update state
                nin     = nout;
                if (j & 1)
                    in = buf1, out = buf2;
                else
                    in = buf2, out = buf1;
            }

            if (nout)
                break;
        }

        return nout;
    }


} /* namespace lsp */
