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
                Material *m         = vMaterials.get(size++);
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

                m->capture          = NULL;
                m->capture_data     = NULL;
            }
        }

        return STATUS_OK;
    }

    void RayTrace3D::destroy(bool recursive)
    {
        clear_progress_callback();
        destroy_tasks();
        remove_scene(recursive);
        sFactory.clear();

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

        Material m; // "Black hole"
        m.absorption[0]     = 1.0f;
        m.dispersion[0]     = 1.0f;
        m.dissipation[0]    = 1.0f;
        m.transparency[0]   = 0.0f;

        m.absorption[0]     = 1.0f;
        m.dispersion[0]     = 1.0f;
        m.dissipation[0]    = 1.0f;
        m.transparency[0]   = 0.0f;

        m.permeability      = 1.0f;

        cap->position       = *position;
        cap->material       = m;
        cap->type           = type;
        cap->sample         = sample;
        cap->channel        = channel;

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

    status_t RayTrace3D::set_progress_callback(rt_progress_t *callback, void *data)
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

    status_t RayTrace3D::process()
    {
        size_t p_denom = 1, p_points = 0, p_thresh = 0; // Values to report progress
        status_t res = report_progress(0.0f);
        if (res != STATUS_OK)
            return res;

        // Generate sources
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

            for (size_t ti=0, n=obj->num_triangles(); ti<n; ++ti)
            {
                obj_triangle_t *t   = obj->triangle(ti);
                rt_context_t *ctx   = new rt_context_t(NULL);
                if (ctx == NULL)
                    return STATUS_NO_MEM;

                ctx->view.s         = src->position.z;
                dsp::apply_matrix3d_mp2(&ctx->view.p[0], t->v[0], &tm);
                dsp::apply_matrix3d_mp2(&ctx->view.p[1], t->v[1], &tm);
                dsp::apply_matrix3d_mp2(&ctx->view.p[2], t->v[2], &tm);

                ctx->shared         = pDebug;
                ctx->view.face      = -1;
                ctx->view.speed     = SOUND_SPEED_M_S;
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
                );
        }

        p_points    = 1;
        p_thresh    = vTasks.size();
        p_denom    += vTasks.size() + 1;

        // Perform raytracing
        rt_context_t *ctx = NULL;
        status_t res = STATUS_OK;

        while (vTasks.size() > 0)
        {
            // Get next context from queue
            if (!vTasks.pop(&ctx))
                return STATUS_CORRUPTED;

            // Report status if required
            if (vTasks.size() < p_thresh)
            {
                p_thresh    = vTasks.size();
                res         = report_progress(float(++p_points) / float(p_denom));

                if (res != STATUS_OK)
                {
                    delete ctx;
                    break;
                }
            }

            // Check that we need to perform a scan
            switch (ctx->state)
            {
                case S_SCAN_OBJECTS:
                    res = scan_objects(ctx);
                    break;
                case S_CULL_VIEW:
                    res = cull_view(ctx);
                    break;
                case S_SPLIT:
                    res = split_view(ctx);
                    break;
                case S_CULL_BACK:
                    res = cullback_view(ctx);
                    break;
                case S_REFLECT:
                    res = reflect_view(ctx);
                    break;
                case S_IGNORE:
                    RT_TRACE(ctx->shared,
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

        destroy_tasks();
        if (res != STATUS_OK)
            return res;

        return report_progress(1.0f);
    }

    status_t RayTrace3D::scan_objects(rt_context_t *ctx)
    {
        return STATUS_NOT_IMPLEMENTED;
    }

    status_t RayTrace3D::cull_view(rt_context_t *ctx)
    {
        return STATUS_NOT_IMPLEMENTED;
    }

    status_t RayTrace3D::split_view(rt_context_t *ctx)
    {
        return STATUS_NOT_IMPLEMENTED;
    }

    status_t RayTrace3D::cullback_view(rt_context_t *ctx)
    {
        return STATUS_NOT_IMPLEMENTED;
    }

    status_t RayTrace3D::reflect_view(rt_context_t *ctx)
    {
        return STATUS_NOT_IMPLEMENTED;
    }

} /* namespace lsp */
