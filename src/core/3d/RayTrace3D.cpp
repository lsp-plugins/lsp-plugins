/*
 * RayTrace3D.cpp
 *
 *  Created on: 9 апр. 2017 г.
 *      Author: sadko
 */

#include <core/alloc.h>
#include <core/3d/common.h>
#include <core/3d/RayTrace3D.h>

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

    RayTrace3D::RayTrace3D()
    {
        pScene          = NULL;
        pProgress       = NULL;
        pProgressData   = NULL;
        nSampleRate     = DEFAULT_SAMPLE_RATE;
        pDebug          = NULL;
    }

    RayTrace3D::~RayTrace3D()
    {
        destroy(true);
    }

    void RayTrace3D::destroy_tasks()
    {
        for (size_t i=0, n=vTasks.size(); i<n; ++i)
        {
            rt_context_t *ctx   = vTasks.get(i);
            if (ctx != NULL)
                delete ctx;
        }

        vTasks.flush();
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
        clear_progress_callback();
        destroy_tasks();
        remove_scene(recursive);

        sFactory.clear();
        sRoot.flush();

        vMaterials.flush();
        vSources.flush();
        vCaptures.flush();
    }

    status_t RayTrace3D::add_source(const ray3d_t *position, rt_audio_source_t type, float volume)
    {
        if (position == NULL)
            return STATUS_NO_MEM;

        source_t *src   = vSources.add();
        if (src == NULL)
            return STATUS_NO_MEM;

        src->position       = *position;
        src->type           = type;
        src->volume         = volume;

        return STATUS_OK;
    }

    status_t RayTrace3D::add_capture(const ray3d_t *position, rt_audio_capture_t type, Sample *sample, size_t channel)
    {
        capture_t *cap      = vCaptures.add();
        if (cap == NULL)
            return STATUS_NO_MEM;

        cap->position       = *position;
        cap->type           = type;
        cap->sample         = sample;
        cap->channel        = channel;

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

    status_t RayTrace3D::process(float initial)
    {
        // Report progress as 0%
        status_t res = report_progress(0.0f);
        if (res != STATUS_OK)
            return res;

        // Generate ray-tracing tasks
        res         = generate_tasks(initial);
        if (res != STATUS_OK)
            return res;

        // Values to report progress
        size_t p_points     = 1;
        size_t p_denom      = vTasks.size() + 3; // Number of tasks + generate_tasks + prepare_root_context + final cleanup
        size_t p_thresh     = vTasks.size();

        // Report progress
        res         = report_progress(float(p_points++) / float(p_denom));
        if (res != STATUS_OK)
            return res;

        // Prepare root context
        res         = prepare_root_context();
        if (res != STATUS_OK)
            return res;

        // Report progress again
        res         = report_progress(float(p_points++) / float(p_denom));
        if (res != STATUS_OK)
            return res;

        // Perform main loop of raytracing
        rt_context_t *ctx = NULL;
        res         = STATUS_OK;

        while (vTasks.size() > 0)
        {
            // Get next context from queue
            if (!vTasks.pop(&ctx))
                return STATUS_CORRUPTED;

            size_t t_size   = vTasks.size(); // Remember size fo task stack

            // Check that we need to perform a scan
            switch (ctx->state)
            {
                case S_SCAN_OBJECTS:
                    res     = scan_objects(ctx);
                    break;
                case S_CULL_VIEW:
                    res     = cull_view(ctx);
                    break;
                case S_SPLIT:
                    res     = split_view(ctx);
                    break;
                case S_CULL_BACK:
                    res     = cullback_view(ctx);
                    break;
                case S_REFLECT:
                    res     = reflect_view(ctx);
                    break;
                case S_IGNORE:
                    RT_TRACE(pDebug,
                        for (size_t i=0,n=ctx->triangle.size(); i<n; ++i)
                            ctx->ignore(ctx->triangle.get(i));
                    )
                    delete ctx;
                    break;
                default:
                    res = STATUS_BAD_STATE;
                    break;
            }

            // Report status if required
            if ((res == STATUS_OK) && (t_size < p_thresh))
            {
                p_thresh    = vTasks.size();
                float prg   = float(p_points) / float(p_denom);
                lsp_trace("Reporting progress %d/%d = %.2f%%", int(p_points), int(p_denom), prg * 100.0f);
                res         = report_progress(prg);

                ++p_points;
            }

            // Analyze status
            RT_TRACE(pDebug,
                if (res == STATUS_BREAKPOINT)
                {
                    pDebug->matched.swap(&ctx->matched);
                    pDebug->ignored.swap(&ctx->ignored);
                    pDebug->trace.swap(&ctx->trace);
                }
            )

            if (res != STATUS_OK)
            {
                delete ctx;
                break;
            }
        }

        // Destroy all tasks
        destroy_tasks();
        sRoot.flush();
        if (res != STATUS_OK)
            return res;

        // Report progress
        return report_progress(float(p_points++) / float(p_denom));
    }

    status_t RayTrace3D::generate_tasks(float initial)
    {
        for (size_t i=0,n=vSources.size(); i<n; ++i)
        {
            source_t *src = vSources.get(i);
            if (src == NULL)
                return STATUS_CORRUPTED;

            // Generate/fetch object
            Object3D *obj = NULL;
            switch (src->type)
            {
                case RT_AS_SPOT:        // TODO
                case RT_AS_SPEAKER:     // TODO
                case RT_AS_OMNI:
                    obj     = sFactory.buildIcosphere(2);
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
            initial /= area;

            // Generate sources
            for (size_t ti=0, n=obj->num_triangles(); ti<n; ++ti)
            {
                obj_triangle_t *t   = obj->triangle(ti);
                rt_context_t *ctx   = new rt_context_t();
                if (ctx == NULL)
                    return STATUS_NO_MEM;

                RT_TRACE(pDebug, ctx->set_debug_context(pDebug); )

                ctx->view.s         = src->position.z;
                dsp::apply_matrix3d_mp2(&ctx->view.p[0], t->v[0], &tm);
                dsp::apply_matrix3d_mp2(&ctx->view.p[1], t->v[2], &tm);
                dsp::apply_matrix3d_mp2(&ctx->view.p[2], t->v[1], &tm);

                ctx->view.oid       = -1;
                ctx->view.face      = -1;
                ctx->view.speed     = SOUND_SPEED_M_S;
                ctx->view.energy    = dsp::calc_area_pv(ctx->view.p) * initial;
                ctx->view.time[0]   = 0.0f;
                ctx->view.time[1]   = 0.0f;
                ctx->view.time[2]   = 0.0f;

                if (!vTasks.add(ctx))
                {
                    delete ctx;
                    return STATUS_NO_MEM;
                }
            }

            RT_TRACE_BREAK(pDebug,
                lsp_trace("Generated %d raytrace contexts for source %d", int(obj->num_triangles()), int(i));
                for (size_t i=0, n=vTasks.size(); i<n; ++i)
                {
                    rt_context_t *ctx = vTasks.at(i);
                    pDebug->trace.add_view_1c(&ctx->view, &C_MAGENTA);
                }
            );
        }

        return STATUS_OK;
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

            if ((*n_in) > 0)
                break;
        }

        return (*n_in) > 0;
    }

    status_t RayTrace3D::check_object(rt_context_t *ctx, Object3D *obj, const matrix3d_t *m)
    {
        // Ensure that we need to perform additional checks
        if (obj->num_triangles() < 16)
            return STATUS_OK;

        // Prepare bounding-box check
        bound_box3d_t box = *(obj->bound_box());
        for (size_t j=0; j<8; ++j)
            dsp::apply_matrix3d_mp1(&box.p[j], m);

        RT_TRACE_BREAK(pDebug,
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
            RT_TRACE(pDebug,
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

    status_t RayTrace3D::prepare_root_context()
    {
        status_t res;
        size_t obj_id = 0;

        // Clear contents of the root context
        sRoot.clear();
        RT_TRACE(pDebug, sRoot.set_debug_context(pDebug); )

        // Add scene objects
        for (size_t i=0, n=pScene->num_objects(); i<n; ++i, ++obj_id)
        {
            // Get object
            Object3D *obj       = pScene->object(i);
            if (obj == NULL)
                return STATUS_BAD_STATE;
            else if (!obj->is_visible()) // Skip invisible objects
                continue;

            // Get material
            rt_material_t *m    = vMaterials.get(i);
            if (m == NULL)
                return STATUS_BAD_STATE;

            // Add object to context
            res         = sRoot.add_object(obj, obj_id, obj->matrix(), m);
            if (res != STATUS_OK)
                return res;
        }

        RT_TRACE_BREAK(pDebug,
            lsp_trace("Prepared scene (%d triangles)", int(sRoot.triangle.size()));
            for (size_t i=0,n=sRoot.triangle.size(); i<n; ++i)
                pDebug->trace.add_triangle_3c(sRoot.triangle.get(i), &C_RED, &C_GREEN, &C_BLUE);
        );

        // Add capture objects as fake icosphere objects
        for (size_t i=0, n=vCaptures.size(); i<n; ++i, ++obj_id)
        {
            capture_t *cap      = vCaptures.get(i);
            if (cap == NULL)
                return STATUS_BAD_STATE;

            Object3D *obj       = sFactory.buildIcosphere(1);
            if (obj == NULL)
                return STATUS_NO_MEM;

            // Add capture object to context
            res     = sRoot.add_object(obj, obj_id, &cap->matrix, &cap->material);
            if (res != STATUS_OK)
                return res;
        }

        RT_TRACE_BREAK(pDebug,
            lsp_trace("Added capture objects (%d triangles)", int(sRoot.triangle.size()));
            for (size_t i=0,n=sRoot.triangle.size(); i<n; ++i)
                pDebug->trace.add_triangle_3c(sRoot.triangle.get(i), &C_RED, &C_GREEN, &C_BLUE);
        );

        // Solve conflicts between all objects
        res = sRoot.solve_conflicts();
        if (res != STATUS_OK)
            return res;

        RT_TRACE_BREAK(pDebug,
            lsp_trace("Solved conflicts (%d triangles)", int(sRoot.triangle.size()));
            for (size_t i=0,n=sRoot.triangle.size(); i<n; ++i)
                pDebug->trace.add_triangle_3c(sRoot.triangle.get(i), &C_RED, &C_GREEN, &C_BLUE);
        );

        return res;
    }

    status_t RayTrace3D::scan_objects(rt_context_t *ctx)
    {
        status_t res = STATUS_OK;

        RT_TRACE_BREAK(pDebug,
            lsp_trace("Scanning objects...");
            ctx->trace.add_view_1c(&ctx->view, &C_MAGENTA);
        )

        size_t max_objs     = pScene->num_objects() + vCaptures.size();
        size_t *objs        = reinterpret_cast<size_t *>(alloca(sizeof(size_t) * max_objs));
        size_t n_objs       = 0;
        size_t obj_id       = 0;

        // Iterate all object and add to the context if the object is potentially participating the ray tracing algorithm
        for (size_t i=0, n=pScene->num_objects(); i<n; ++i, ++obj_id)
        {
            // Get object
            Object3D *obj       = pScene->object(i);
            if (obj == NULL)
                return STATUS_BAD_STATE;
            else if (!obj->is_visible()) // Skip invisible objects
                continue;

            // Get material
            rt_material_t *m    = vMaterials.get(i);
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

        // Add all captures
        for (size_t i=0, n=vCaptures.size(); i<n; ++i)
        {
            capture_t *cap      = vCaptures.get(i);
            if (cap == NULL)
                return STATUS_BAD_STATE;

            Object3D *obj       = sFactory.buildIcosphere(1);
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

        RT_TRACE(pDebug,
            if (!pScene->validate())
                return STATUS_CORRUPTED;
        )

        // Fetch visible objects from root context into current context
        lsp_trace("Fetch %d objects", int(n_objs));
        res     = ctx->fetch_objects(&sRoot, n_objs, objs);
        if (res != STATUS_OK) // Some error occurred
            return res;
        else if (ctx->triangle.size() <= 0) // Empty context
        {
            delete ctx;
            return STATUS_OK;
        }

        RT_TRACE_BREAK(pDebug,
            lsp_trace("Fetched %d objects", int(n_objs));
            ctx->trace.add_view_1c(&ctx->view, &C_MAGENTA);
            for (size_t i=0,n=ctx->triangle.size(); i<n; ++i)
                ctx->trace.add_triangle_3c(ctx->triangle.get(i), &C_RED, &C_GREEN, &C_BLUE);
        )

        // Update state
        ctx->state      = S_CULL_VIEW;
        return (vTasks.push(ctx)) ? STATUS_OK : STATUS_NO_MEM;
    }

    status_t RayTrace3D::cull_view(rt_context_t *ctx)
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

        return (vTasks.push(ctx)) ? STATUS_OK : STATUS_NO_MEM;
    }

    status_t RayTrace3D::split_view(rt_context_t *ctx)
    {
        rt_context_t out;
        RT_TRACE(pDebug, out.set_debug_context(pDebug); );

        // Perform binary split
        status_t res = ctx->edge_split(&out);
        if (res == STATUS_NOT_FOUND)
        {
            ctx->state      = S_CULL_BACK;
            return (vTasks.push(ctx)) ? STATUS_OK : STATUS_NO_MEM;
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
                else if (!vTasks.push(nctx))
                {
                    delete nctx;
                    return STATUS_NO_MEM;
                }

                RT_TRACE(pDebug,
                    nctx->set_debug_context(pDebug);

                    nctx->ignored.add_all(&ctx->ignored);
                    nctx->matched.add_all(&ctx->matched);
                    nctx->trace.add_all(&ctx->trace);

                    for (size_t i=0,n=ctx->triangle.size(); i<n; ++i)
                        nctx->ignore(ctx->triangle.get(i));
                    for (size_t i=0,n=out.triangle.size(); i<n; ++i)
                        ctx->ignore(out.triangle.get(i));
                );

                nctx->swap(&out);
            }

            return (vTasks.push(ctx)) ? STATUS_OK : STATUS_NO_MEM;
        }
        else if (out.triangle.size() > 0)
        {
            ctx->swap(&out);
            ctx->state  = (ctx->triangle.size() > 1) ? S_SPLIT : S_REFLECT;

            return (vTasks.push(ctx)) ? STATUS_OK : STATUS_NO_MEM;
        }

        delete ctx;
        return STATUS_OK;
    }

    status_t RayTrace3D::cullback_view(rt_context_t *ctx)
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

        return (vTasks.push(ctx)) ? STATUS_OK : STATUS_NO_MEM;
    }

    status_t RayTrace3D::reflect_view(rt_context_t *ctx)
    {
        rt_context_t *rc;
        rt_view_t sv, v, cv, rv, tv;    // source view, view, captured view, reflected view, transparent trace
        vector3d_t vpl;                 // trace plane, split plane
        point3d_t p[3];                 // Projection points
        float d[3], t[3];               // distance
        float a[3], A, kd;              // particular area, area, dispersion coefficient

        sv      = ctx->view;

        dsp::calc_plane_pv(&vpl, ctx->view.p);
        A       = dsp::calc_area_pv(sv.p);

        status_t res    = STATUS_OK;

        for (size_t i=0,n=ctx->triangle.size(); i<n; ++i)
        {
            rt_triangle_t *ct = ctx->triangle.get(i);
            ctx->match(ctx->triangle.get(i));

            // get material
            rt_material_t *m    = ct->m;
            if (m == NULL)
                continue;

            RT_TRACE_BREAK(pDebug,
                lsp_trace("Reflecting triangle");
                ctx->trace.add_triangle_1c(ct, &C_YELLOW);
                ctx->trace.add_triangle_pvnc1(sv.p, &vpl, &C_MAGENTA);
                ctx->trace.add_plane_3pn1c(ct->v[0], ct->v[1], ct->v[2], &ct->n, &C_MAGENTA);
                ctx->trace.add_plane_3pn1c(&sv.p[0], &sv.p[1], &sv.p[2], &ct->n, &C_YELLOW);
                ctx->trace.add_view_1c(&sv, &C_MAGENTA);
            );

            // Estimate the start time for each trace point using barycentric coordinates
            for (size_t j=0; j<3; ++j)
            {
                dsp::calc_split_point_p2v1(&p[j], &sv.s, ct->v[j], &vpl);     // Project triangle point to trace point
                d[j]        = dsp::calc_distance_p2(&p[j], ct->v[j]);         // Compute distance between projected point and triangle point

                a[0]        = dsp::calc_area_p3(&p[j], &sv.p[1], &sv.p[2]);   // Compute area 0
                a[1]        = dsp::calc_area_p3(&p[j], &sv.p[0], &sv.p[2]);   // Compute area 1
                a[2]        = dsp::calc_area_p3(&p[j], &sv.p[0], &sv.p[1]);   // Compute area 2
                t[j]        = (sv.time[0] * a[0] + sv.time[1] * a[1] + sv.time[2] * a[2]) / A; // Compute projected point's time
                v.time[j]   = t[j] + (d[j] / sv.speed);
            }

            // Determine the direction from which came the wave front
            v.oid       = ct->oid;
            v.face      = ct->face;
            v.s         = sv.s;
            v.energy    = (sv.energy * dsp::calc_area_pv(p)) / A;
            v.p[0]      = *(ct->v[0]);
            v.p[1]      = *(ct->v[1]);
            v.p[2]      = *(ct->v[2]);

            float distance  = sv.s.x * ct->n.dx + sv.s.y * ct->n.dy + sv.s.z * ct->n.dz + ct->n.dw;

            RT_TRACE_BREAK(pDebug,
                lsp_trace("Projection points distance: {%f, %f, %f}", d[0], d[1], d[2]);
                lsp_trace("View points time: {%f, %f, %f}", sv.time[0], sv.time[1], sv.time[2]);
                lsp_trace("Projection points time: {%f, %f, %f}", t[0], t[1], t[2]);
                lsp_trace("Target points time: {%f, %f, %f}", v.time[0], v.time[1], v.time[2]);
                lsp_trace("Energy: %f -> %f", sv.energy, v.energy);
                lsp_trace("Distance between source point and triangle: %f", distance);

                ctx->trace.add_triangle_1c(ct, &C_YELLOW);
                ctx->trace.add_plane_3pn1c(ct->v[0], ct->v[1], ct->v[2], &ct->n, &C_GREEN);
                ctx->trace.add_view_1c(&sv, &C_MAGENTA);
                ctx->trace.add_segment(&p[0], ct->v[0], &C_RED);
                ctx->trace.add_segment(&p[1], ct->v[1], &C_GREEN);
                ctx->trace.add_segment(&p[2], ct->v[2], &C_BLUE);
            )

            cv          = v;
            rv          = v;
            tv          = v;

            if (distance > 0.0f)
            {
                cv.energy       = v.energy * m->absorption[0];
                v.energy       *= (1.0f - m->absorption[0]);

                kd              = (1.0f + 1.0f / m->dispersion[0]) * distance;
                rv.energy       = v.energy * (m->transparency[0] - 1.0f); // Sign negated
                rv.s.x         -= kd * ct->n.dx;
                rv.s.y         -= kd * ct->n.dy;
                rv.s.z         -= kd * ct->n.dz;

                kd              = (m->permeability/m->dissipation[0] - 1.0f) * distance;
                tv.energy       = v.energy * m->transparency[0];
                tv.speed       *= m->permeability;
                tv.s.x         += kd * ct->n.dx;
                tv.s.y         += kd * ct->n.dy;
                tv.s.z         += kd * ct->n.dz;

                RT_TRACE_BREAK(pDebug,
                    lsp_trace("Outside->inside reflect_view");
                    lsp_trace("Energy: captured=%f, reflected=%f, refracted=%f", cv.energy, rv.energy, tv.energy);
                    lsp_trace("Material: absorption=%f, transparency=%f, permeability=%f, dispersion=%f, dissipation=%f",
                            m->absorption[0], m->transparency[0], m->permeability, m->dispersion[0], m->dissipation[0]);

                    ctx->trace.add_view_1c(&sv, &C_RED);
                    ctx->trace.add_view_1c(&rv, &C_GREEN);
                    ctx->trace.add_view_1c(&tv, &C_CYAN);
                )
            }
            else
            {
                cv.energy       = v.energy * m->absorption[1];
                v.energy       *= (1.0f - m->absorption[1]);

                kd              = (1.0f + 1.0f / m->dispersion[1]) * distance;
                rv.energy       = v.energy * (m->transparency[1] - 1.0f); // Sign negated
                rv.s.x         -= kd * ct->n.dx;
                rv.s.y         -= kd * ct->n.dy;
                rv.s.z         -= kd * ct->n.dz;

                kd              = (1.0f/(m->dissipation[1]*m->permeability) - 1.0f) * distance;
                tv.energy       = v.energy * m->transparency[1];
                tv.speed       /= m->permeability;
                tv.s.x         += kd * ct->n.dx;
                tv.s.y         += kd * ct->n.dy;
                tv.s.z         += kd * ct->n.dz;

                RT_TRACE_BREAK(pDebug,
                    lsp_trace("Inside->outside reflect_view");
                    lsp_trace("Energy: captured=%f, reflected=%f, refracted=%f", cv.energy, rv.energy, tv.energy);
                    lsp_trace("Material: absorption=%f, transparency=%f, permeability=%f, dispersion=%f, dissipation=%f",
                            m->absorption[1], m->transparency[1], m->permeability, m->dispersion[1], m->dissipation[1]);

                    ctx->trace.add_view_1c(&sv, &C_RED);
                    ctx->trace.add_view_1c(&rv, &C_GREEN);
                    ctx->trace.add_view_1c(&tv, &C_CYAN);
                )
            }

            // Perform capture
            ssize_t capture_id = ct->oid - pScene->num_objects();
            if (capture_id >= 0)
            {
                capture_t *cap  = vCaptures.get(capture_id);
                res     = (cap != NULL) ? capture(cap, &cv) : STATUS_CORRUPTED;
                if (res != STATUS_OK)
                    break;
            }

            // Create reflection context
            if ((rv.energy <= -DSP_3D_TOLERANCE) || (rv.energy >= DSP_3D_TOLERANCE))
            {
                rc      = new rt_context_t(&rv, S_SCAN_OBJECTS);
                if (rc == NULL)
                    return STATUS_NO_MEM;
                else if (!vTasks.add(rc))
                {
                    delete rc;
                    res = STATUS_NO_MEM;
                    break;
                }

                RT_TRACE(pDebug,
                    rc->set_debug_context(pDebug);
                );
            }

            // Create refraction context
            if ((tv.energy <= -DSP_3D_TOLERANCE) || (tv.energy >= DSP_3D_TOLERANCE))
            {
                rc      = new rt_context_t(&tv, S_SCAN_OBJECTS);
                if (rc == NULL)
                    return STATUS_NO_MEM;
                else if (!vTasks.add(rc))
                {
                    delete rc;
                    res = STATUS_NO_MEM;
                    break;
                }

                RT_TRACE(pDebug,
                    rc->set_debug_context(pDebug);
                );
            }
        }

        delete ctx;
        return res;
    }

    status_t RayTrace3D::capture(capture_t *capture, const rt_view_t *v)
    {
        // TODO
        return STATUS_OK;
    }

} /* namespace lsp */
