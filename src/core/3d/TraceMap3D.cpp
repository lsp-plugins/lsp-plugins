/*
 * TraceMap.cpp
 *
 *  Created on: 20 апр. 2017 г.
 *      Author: sadko
 */

#include <core/debug.h>
#include <core/3d/TraceMap3D.h>
#include <core/3d/Scene3D.h>

namespace lsp
{
    TraceMap3D::TraceMap3D()
    {
        pRoot       = NULL;
    }

    TraceMap3D::~TraceMap3D()
    {
        destroy();
    }

    bool TraceMap3D::add_object(Object3D *obj, TraceCapture3D *capt)
    {
        if ((obj == NULL) || (!obj->is_traceable()))
            return true;

        size_t n_triangles  = obj->get_triangles_count();
        size_t to_alloc     = sizeof(triangle3d_t) * n_triangles + sizeof(segment_t);
        uint8_t *ptr        = lsp_tmalloc(uint8_t, to_alloc + DEFAULT_ALIGN);
        if (ptr == NULL)
            return false;

        segment_t *seg      = reinterpret_cast<segment_t *>(ptr);
        ptr                 = ALIGN_PTR(ptr + sizeof(segment_t), DEFAULT_ALIGN);

        seg->nItems         = n_triangles;
        seg->vTriangles     = reinterpret_cast<triangle3d_t *>(ptr);
        seg->pObject        = obj;
        seg->pCapture       = capt;
        seg->pMaterial      = obj->get_material();
        seg->pNext          = pRoot;

        if ((capt != NULL) && (!vCaptures.add_unique(capt)))
        {
            lsp_free(seg);
            return false;
        }

        // Get object properties
        matrix3d_t *m       = obj->get_matrix();
        vertex_index_t *vi  = obj->get_vertex_indexes();
        size_t nv           = obj->get_vertex_count();

        // Initialize capture
        if (capt != NULL)
            dsp::apply_matrix3d_mp2(&seg->sCapture, obj->get_center(), m);
        else
            dsp::init_point_xyz(&seg->sCapture, 0.0f, 0.0f, 0.0f);

        // Copy vertexes
        point3d_t *vv       = lsp_tmalloc(point3d_t, nv);
        if (vv == NULL)
        {
            lsp_free(seg);
            return false;
        }

        // Apply matrix to points
        const point3d_t *xv = obj->get_vertexes();
        for (size_t i=0; i<nv; ++i)
            dsp::apply_matrix3d_mp2(&vv[i], &xv[i], m);

        // Initialize octant
        dsp::init_octant3d_v(&seg->sOctant, vv, nv);

        // Initialize triangles
        triangle3d_t *dt    = seg->vTriangles;

        for (size_t i=0; i<n_triangles; ++i)
        {
            dsp::calc_triangle3d_p3(dt++, &vv[vi[0]], &vv[vi[1]], &vv[vi[2]]);
            vi         += 3;
        }

        lsp_free(vv);
        pRoot               = seg;

        return true;
    }

    bool TraceMap3D::add_source(RaySource3D *rs)
    {
        if (rs == NULL)
            return false;
        vSources.add(rs);
        return true;
    }

    status_t TraceMap3D::raytrace(size_t rays, Scene3D *s)
    {
        // Check state
        if (vSources.size() <= 0)
            return STATUS_OK;
        if ((vCaptures.size() <= 0) && (s == NULL))
            return STATUS_OK;

        // Initialize captures
        for (size_t i=0; i< vCaptures.size(); ++i)
        {
            TraceCapture3D *capt = vCaptures.at(i);
            if (capt != NULL)
                capt->reset();
        }

        // Initialize raytracing
        RayTrace3D rt;

        for (size_t i=0; i< vSources.size(); ++i)
        {
            // Get ray source
            RaySource3D *rs = vSources.at(i);
            if (rs == NULL)
                continue;

            // Initialize source and generate rays
            rs->reset();
            rs->generate(&rt, rays);
        }

        // Do raytracing for each ray in the stack
        raytrace3d_t    rtx, rfx;   // Reflected and Refracted rays
        capture3d_t     ix;         // Intersection capture point
        vector3d_t      cv;         // Capture vector
        size_t ray_id = 0;
        size_t rays_left = rt.size();

//        s = NULL;

        // Process each ray independently
        while (rt.pop(&rtx))
        {
            if (rt.size() < rays_left)
            {
                lsp_trace("Rays left: %d", int(rays_left));
                rays_left   = rt.size();
            }

            if ((ray_id % 10000) == 0)
                lsp_trace("Tracing ray #%d", int(ray_id));
//            lsp_trace(" ray=(%.3f, %.3f, %.3f - %.3f, %.3f, %.3f)",
//                    rtx.r.z.x, rtx.r.z.y, rtx.r.z.z, rtx.r.v.dx, rtx.r.v.dy, rtx.r.v.dz);
            ray_id++;

            if (s != NULL) // DEBUG
                s->add_point(&rtx.r.z);

            // Process ray until it's energy falls down
            do
            {
                // Do raytracing: find intersection of ray with triangles
                dsp::init_intersection3d(&ix);

//                if (ray_id == 7066155)
//                    printf("debug");

                raytrace_step(&ix, &rtx);

                // Consider that there was no intersection found
                if (ix.n <= 0)
                {
                    if (s != NULL) // DEBUG
                        s->add_ray(&rtx.r);
                    break;
                }

                if (s != NULL) // DEBUG
                {
                    segment3d_t seg;
                    dsp::init_segment_p2(&seg, &rtx.r.z, &ix.p);
                    s->add_segment(&seg);
                    s->add_point(&ix.p);
                }

                // Reflect and refract the ray with triangles
                dsp::reflect_ray(&rtx, &rfx, &ix);

                // Trigger captures
                bool has_capture = false;
                for (size_t i=0; i<ix.n; ++i)
                {
                    segment_t *c = ix.pSeg[i];
                    if ((c == NULL) || (c->pCapture == NULL))
                        continue;

                    has_capture = true;
                    dsp::init_vector_p2(&cv, &ix.p, &c->sCapture);

                    // Calculate cosine between vectors
                    float angle = dsp::calc_angle3d_v2(&rtx.r.v, &cv);
                    c->pCapture->capture(&cv, rtx.amplitude * angle, rtx.delay / SOUND_SPEED_M_S);
                }

                if (!has_capture)
                {
                    if (!rt.push(&rfx)) // Queue refracted ray for future processing
                    {
                        rt.destroy();
                        return STATUS_NO_MEM;
                    }
                }
                else if (s != NULL)
                    s->add_point(&ix.p);

    //            lsp_trace(" refl=(%.3f, %.3f, %.3f - %.3f, %.3f, %.3f), ampl=%f, time=%.3f",
    //                        rt.r.z.x, rt.r.z.y, rt.r.z.z, rt.r.v.dx, rt.r.v.dy, rt.r.v.dz,
    //                        rt.amplitude, rt.delay);
    //            lsp_trace(" refr=(%.3f, %.3f, %.3f - %.3f, %.3f, %.3f), ampl=%f, time=%.3f",
    //                        rf.r.z.x, rf.r.z.y, rf.r.z.z, rf.r.v.dx, rf.r.v.dy, rf.r.v.dz,
    //                        rf.amplitude, rf.delay);
            }
            while (fabs(rtx.amplitude) >= CMP_TOLERANCE);
        }

        // Destroy raytrace stack
        rt.destroy();

        return STATUS_OK;
    }

    bool TraceMap3D::has_triangle(const intersection3d_t *is, const triangle3d_t *t)
    {
        for (size_t i=0; i<is->n; ++i)
            if (is->t[i] == t)
                return true;
        return false;
    }

    void TraceMap3D::raytrace_step(capture3d_t *ix, const raytrace3d_t *rt)
    {
        point3d_t i;
        const triangle3d_t *t;

        for (segment_t *seg = pRoot; seg != NULL; seg = seg->pNext)
        {
            // Check that has elements
            if (seg->nItems <= 0)
                continue;

            // Check octant
            if (!dsp::check_octant3d_rv(&seg->sOctant, &rt->r))
                continue;

            t = seg->vTriangles;

            for (size_t count=seg->nItems; count > 0; count--, t++)
            {
                if (has_triangle(&rt->x, t))
                    continue;

                float dist      = dsp::find_intersection3d_rt(&i, &rt->r, t);
                if ((dist < 0.0f) || (dist > ix->p.w))
                    continue;

//                printf("intersection at (%.3f, %.3f, %.3f) with triangle (%.3f, %.3f, %.3f; %.3f, %.3f, %.3f; %.3f, %.3f, %.3f), projection=%.3f\n",
//                    i.x, i.y, i.z,
//                    t->p[0].x, t->p[0].y, t->p[0].z,
//                    t->p[1].x, t->p[1].y, t->p[1].z,
//                    t->p[2].x, t->p[2].y, t->p[2].z,
//                    proj
//                );

                // This point is the right intersection, store it
                if (fabs(dist - ix->p.w) > DSP_3D_TOLERANCE)
                    ix->n = 0;

                ix->p       = i;
                ix->p.w     = dist;

                // Store triangle with limit checking
                if (ix->n < DSP_3D_MAXISECT)
                {
                    size_t idx      = ix->n++;
                    ix->t[idx]      = t;
                    ix->m[idx]      = seg->pMaterial;
                    ix->pSeg[idx]   = seg;
                }
            }
        }
    }

    void TraceMap3D::destroy()
    {
        for (segment_t *seg = pRoot; seg != NULL; )
        {
            segment_t *next = seg->pNext;
            lsp_free(seg);
            seg = next;
        }
        pRoot = NULL;
        vSources.clear();
        vCaptures.clear();
    }

} /* namespace lsp */
