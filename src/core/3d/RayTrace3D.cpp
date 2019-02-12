/*
 * RayTrace3D.cpp
 *
 *  Created on: 9 апр. 2017 г.
 *      Author: sadko
 */

#include <core/alloc.h>
#include <core/3d/RayTrace3D.h>

namespace lsp
{


    RayTrace3D::RayTrace3D()
    {
        pScene          = NULL;
        pProgress       = NULL;
        pProgressData   = NULL;
        nSampleRate     = DEFAULT_SAMPLE_RATE;
        nSphereId       = -1;
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
        sPrimitives.destroy();

        vMaterials.flush();
        vSources.flush();
        vCaptures.flush();

        sPrimitives.destroy();
        nSphereId       = -1;
    }

    status_t RayTrace3D::add_source(const ray3d_t *position, float size, rt_audio_source_t type, float volume)
    {
        if (position == NULL)
            return STATUS_NO_MEM;

        source_t *src   = vSources.add();
        if (src == NULL)
            return STATUS_NO_MEM;

        src->position       = *position;
        src->size           = size;
        src->type           = type;
        src->volume         = volume;

        return STATUS_OK;
    }

    status_t RayTrace3D::add_capture(const ray3d_t *position, float size, rt_audio_capture_t type, Sample *sample, size_t channel)
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
        cap->size           = size;
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

    status_t RayTrace3D::process()
    {
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

            // TODO: estimate matrix
            matrix3d_t tm; // Transformation matrix
            dsp::init_matrix3d_identity(&tm);

            for (size_t i=0, n=obj->num_triangles(); i<n; ++i)
            {
                obj_triangle_t *t = obj->triangle(i);
                rt_context_t *ctx   = new rt_context_t(NULL);
                if (ctx == NULL)
                    return STATUS_NO_MEM;

                ctx->view.s         = src->position.z;
            }
        }

        return STATUS_OK;
    }

} /* namespace lsp */
