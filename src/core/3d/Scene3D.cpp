/*
 * Scene3D.cpp
 *
 *  Created on: 9 апр. 2017 г.
 *      Author: sadko
 */

#include <core/3d/Scene3D.h>

namespace lsp
{
    Scene3D::Scene3D()
    {
    }
    
    Scene3D::~Scene3D()
    {
        destroy();
    }

    void Scene3D::destroy(bool recursive)
    {
        if (recursive)
        {
            size_t items = vObjects.size();
            for (size_t i=0; i<items; ++i)
            {
                Object3D *obj = vObjects.at(i);
                if (obj != NULL)
                {
                    obj->destroy();
                    delete obj;
                }
            }

            items = vCaptures.size();
            for (size_t i=0; i<items; ++i)
            {
                TraceCapture3D *capt = vCaptures.at(i);
                if (capt != NULL)
                    delete capt;
            }

            items = vSources.size();
            for (size_t i=0; i<items; ++i)
            {
                RaySource3D *rs = vSources.at(i);
                if (rs != NULL)
                    delete rs;
            }
        }

        vRays.flush();
        vObjects.flush();
        vSources.flush();
        vCaptures.flush();
        vPoints.flush();
        vSegments.flush();
    }

    bool Scene3D::add_object(Object3D *obj, TraceCapture3D *capt)
    {
        // Add object
        if (!vObjects.add(obj))
            return false;

        // Add capture
        if (!vCaptures.add(capt))
        {
            vObjects.remove(vObjects.size());
            return false;
        }

        // Add bounding box
        bound_box3d_t *bbox = vBoundBoxes.add();
        if (bbox == NULL)
        {
            vObjects.remove(vObjects.size());
            vCaptures.remove(vCaptures.size());
        }

        // Compute bounding box
        point3d_t *p = obj->get_vertexes();
        size_t n = obj->get_vertex_count();
        dsp::calc_bound_box(bbox, p, n);

        return true;
    }

    bool Scene3D::add_ray(const ray3d_t *r)
    {
        return vRays.append(r);
    }

    bool Scene3D::add_source(RaySource3D *obj)
    {
        return vSources.add(obj);
    }

    bool Scene3D::add_point(const point3d_t *p)
    {
        return vPoints.append(p);
    }

    bool Scene3D::add_segment(const segment3d_t *s)
    {
        return vSegments.append(s);
    }

    Object3D *Scene3D::get_object(size_t index)
    {
        return vObjects.get(index);
    }

    bound_box3d_t *Scene3D::get_bound_box(size_t index)
    {
        return vBoundBoxes.get(index);
    }

    TraceCapture3D *Scene3D::get_capture(size_t index)
    {
        return vCaptures.get(index);
    }

    ray3d_t *Scene3D::get_ray(size_t index)
    {
        return vRays.get(index);
    }

    point3d_t *Scene3D::get_point(size_t index)
    {
        return vPoints.get(index);
    }

    segment3d_t *Scene3D::get_segment(size_t index)
    {
        return vSegments.get(index);
    }

    size_t Scene3D::num_triangles()
    {
        size_t result = 0;
        Object3D **items = vObjects.get_array();
        for (size_t i=vObjects.size(); i > 0; --i, ++items)
        {
            if (*items)
                result += (*items)->get_indices_count() / 3;
        }
        return result;
    }

    TraceMap3D *Scene3D::build_trace_map()
    {
        TraceMap3D *tm = new TraceMap3D();
        if (tm == NULL)
            return NULL;

        // Add Objects and captures
        Object3D **items        = vObjects.get_array();
        TraceCapture3D **capts  = vCaptures.get_array();
        for (size_t i=vObjects.size(); i > 0; --i)
        {
            Object3D *obj           = *(items++);
            TraceCapture3D *capt    = *(capts++);
            if ((obj == NULL) || (!obj->is_traceable()))
                continue;

            if (!tm->add_object(obj, capt))
            {
                tm->destroy();
                return NULL;
            }
        }

        // Add Sources
        RaySource3D **rs        = vSources.get_array();
        for (size_t i=vSources.size(); i > 0; --i)
        {
            RaySource3D *r          = *(rs++);
            if ((r == NULL) || (!r->is_enabled()))
                continue;

            if (!tm->add_source(r))
            {
                tm->destroy();
                return NULL;
            }
        }

        return tm;
    }

} /* namespace lsp */
