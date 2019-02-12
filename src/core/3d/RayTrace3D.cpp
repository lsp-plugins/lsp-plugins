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
#define ICO_X       0.525731112119133606f
#define ICO_N       0.0f
#define ICO_Z       0.850650808352039932f

    // Icosahedron vertex coordinates
    static const point3d_t ico_vertex[] =
    {
        { -ICO_X,  ICO_N,  ICO_Z,  1.0f },
        {  ICO_X,  ICO_N,  ICO_Z,  1.0f },
        { -ICO_X,  ICO_N, -ICO_Z,  1.0f },
        {  ICO_X,  ICO_N, -ICO_Z,  1.0f },
        {  ICO_N,  ICO_Z,  ICO_X,  1.0f },
        {  ICO_N,  ICO_Z, -ICO_X,  1.0f },
        {  ICO_N, -ICO_Z,  ICO_X,  1.0f },
        {  ICO_N, -ICO_Z, -ICO_X,  1.0f },
        {  ICO_Z,  ICO_X,  ICO_N,  1.0f },
        { -ICO_Z,  ICO_X,  ICO_N,  1.0f },
        {  ICO_Z, -ICO_X,  ICO_N,  1.0f },
        { -ICO_Z, -ICO_X,  ICO_N,  1.0f }
    };

    // Icosahedron triangle vertex indexes
    static const uint8_t ico_triangle[] =
    {
        0, 4, 1,
        0, 9, 4,
        9, 5, 4,
        4, 5, 8,

        4, 8, 1,
        8, 10, 1,
        8, 3, 10,
        5, 3, 8,

        5, 2, 3,
        2, 7, 3,
        7, 10, 3,
        7, 6, 10,

        7, 11, 6,
        11, 0, 6,
        0, 1, 6,
        6, 1, 10,

        9, 0, 11,
        9, 11, 2,
        9, 2, 5,
        7, 2, 11
    };

#undef ICO_X
#undef ICO_N
#undef ICO_Z

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

    status_t RayTrace3D::build_sphere()
    {
        typedef struct sp_triangle_t
        {
            size_t      vi[3];  // Vertex indexes
        } sp_triangle_t;

        // Create copy of current scene
        Scene3D scene;
        status_t res = scene.clone_from(&sPrimitives);
        if (res != STATUS_OK)
            return res;

        // Add initial vertexes (icosahedron has 12 vertexes)
        size_t ico_index[12];
        for (size_t i=0; i<12; ++i)
        {
            ssize_t idx     = scene.add_vertex(&ico_vertex[i]);
            if (idx < 0)
                return -idx;
            ico_index[i]    = idx;
        }

        // Create initial triangles (icosahedron has 20 triangles)
        sp_triangle_t *vt   = new sp_triangle_t[320];
        if (vt == NULL)
            return STATUS_NO_MEM;

        const uint8_t *src_idx = ico_triangle;
        for (size_t i=0; i<20; ++i, src_idx += 3)
        {
            sp_triangle_t *t    = &vt[i];
            t->vi[0]        = ico_index[size_t(src_idx[0])];
            t->vi[1]        = ico_index[size_t(src_idx[1])];
            t->vi[2]        = ico_index[size_t(src_idx[2])];
        }

        // Generate additional triangles
        point3d_t *p[3];
        point3d_t sp[3];

        for (size_t step=1; step<=2; ++step)
        {
            size_t nt           = 20 << (1 << (step << 1)); // 20 * 2^(2*step) == 20 * 4^step
            sp_triangle_t *st   = vt;       // source triangles
            sp_triangle_t *dt   = &dt[nt];  // newly-allocated triangles

            // Perform split of each triangle into 4 sub-triangles
            for (size_t i=0; i<nt; ++i)
            {
                p[0]        = scene.vertex(st->vi[0]);
                p[1]        = scene.vertex(st->vi[1]);
                p[2]        = scene.vertex(st->vi[2]);

                sp[0].x     = (p[0]->x + p[1]->x) * 0.5f;
                sp[0].y     = (p[0]->y + p[1]->y) * 0.5f;
                sp[0].z     = (p[0]->z + p[1]->z) * 0.5f;
                sp[0].w     = 1.0f;

                sp[1].x     = (p[1]->x + p[2]->x) * 0.5f;
                sp[1].y     = (p[1]->y + p[2]->y) * 0.5f;
                sp[1].z     = (p[1]->z + p[2]->z) * 0.5f;
                sp[1].w     = 1.0f;

                sp[2].x     = (p[2]->x + p[0]->x) * 0.5f;
                sp[2].y     = (p[2]->y + p[0]->y) * 0.5f;
                sp[2].z     = (p[2]->z + p[0]->z) * 0.5f;
                sp[2].w     = 1.0f;

                dsp::normalize_point(&sp[0]);
                dsp::normalize_point(&sp[1]);
                dsp::normalize_point(&sp[2]);

                dt[0].vi[0] = scene.add_vertex(&sp[0]);
                if (dt[0].vi[0] < 0)
                {
                    delete [] vt;
                    return STATUS_NO_MEM;
                }

                dt[0].vi[1] = scene.add_vertex(&sp[1]);
                if (dt[0].vi[1] < 0)
                {
                    delete [] vt;
                    return STATUS_NO_MEM;
                }

                dt[0].vi[2] = scene.add_vertex(&sp[2]);
                if (dt[0].vi[2] < 0)
                {
                    delete [] vt;
                    return STATUS_NO_MEM;
                }

                dt[1].vi[0] = dt[0].vi[0];
                dt[1].vi[1] = st->vi[1];
                dt[1].vi[2] = dt[0].vi[1];

                dt[2].vi[0] = dt[0].vi[1];
                dt[2].vi[1] = st->vi[2];
                dt[2].vi[2] = dt[0].vi[2];

                st->vi[1]   = dt[0].vi[0];
                st->vi[2]   = dt[0].vi[2];
            }
        }

        // Allocate icospheric object
        Object3D *obj = scene.add_object("Icosphere");
        if (obj == NULL)
        {
            delete [] vt;
            return STATUS_NO_MEM;
        }

        // Add each triangle to the scene
        vector3d_t n;
        for (size_t i=0; i<320; ++i)
        {
            // Compute normal value
            p[0]    = scene.vertex(vt[i].vi[0]);
            p[1]    = scene.vertex(vt[i].vi[1]);
            p[2]    = scene.vertex(vt[i].vi[2]);

            if ((p[0] == NULL) || (p[1] == NULL) || (p[2] == NULL))
            {
                delete [] vt;
                return STATUS_CORRUPTED;
            }

            dsp::calc_normal3d_p3(&n, p[0], p[1], p[2]);

            size_t nid  = scene.add_normal(&n);
            if (nid < 0)
            {
                delete [] vt;
                return -nid;
            }

            // Add triangle
            res = obj->add_triangle(i, vt[i].vi[0], vt[i].vi[1], vt[i].vi[2], nid, nid, nid);
            if (res != STATUS_OK)
            {
                delete [] vt;
                return res;
            }
        }

        // Now all is OK, we can swap scene state and save spere identifier
        sPrimitives.swap(&scene);
        nSphereId       = sPrimitives.index_of(obj);

        return STATUS_OK;
    }

    ssize_t RayTrace3D::get_icosphere()
    {
        if (nSphereId >= 0)
            return nSphereId;

        status_t res = build_sphere();
        return (res == STATUS_OK) ? nSphereId : -res;
    }

    status_t RayTrace3D::process()
    {
        // Generate sources
        for (size_t i=0,n=vSources.size(); i<n; ++i)
        {
            source_t *src = vSources.get(i);
            if (src == NULL)
                return STATUS_CORRUPTED;
            ssize_t object_id   = -1;

            switch (src->type)
            {
                case RT_AS_SPOT:
                case RT_AS_SPEAKER:
                case RT_AS_OMNI:
                    object_id   = get_icosphere();
                    if (object_id < 0)
                        return -object_id;
                    break;
            }
        }

        return STATUS_OK;
    }

} /* namespace lsp */
