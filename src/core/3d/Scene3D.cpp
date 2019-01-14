/*
 * Scene3D.cpp
 *
 *  Created on: 9 апр. 2017 г.
 *      Author: sadko
 */

#include <core/3d/common.h>
#include <core/3d/Scene3D.h>
#include <core/3d/Object3D.h>

namespace lsp
{
    Scene3D::Scene3D():
        vVertexes(1024),
        vNormals(1024),
        vXNormals(1024),
        vEdges(1024),
        vTriangles(1024)
    {
    }
    
    Scene3D::~Scene3D()
    {
        destroy();
    }

    Object3D *Scene3D::add_object(const LSPString *name)
    {
        Object3D *obj = new Object3D(this, name);
        if (obj == NULL)
            return NULL;

        if (!vObjects.add(obj))
        {
            delete obj;
            return NULL;
        }

        return obj;
    }

    ssize_t Scene3D::add_vertex(const point3d_t *p)
    {
        obj_vertex_t *v;
        ssize_t res = vVertexes.ialloc(&v);
        if (res >= 0)
        {
            v->x    = p->x;
            v->y    = p->y;
            v->z    = p->z;
            v->w    = p->w;
            v->ve   = NULL;
        }
        return res;
    }

    ssize_t Scene3D::add_normal(const vector3d_t *n)
    {
        obj_normal_t *an;
        ssize_t res = vNormals.ialloc(&an);
        if (res >= 0)
        {
            an->dx   = n->dx;
            an->dy   = n->dy;
            an->dz   = n->dz;
            an->dw   = n->dw;
        }

        return res;
    }

    void Scene3D::destroy()
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

        vObjects.flush();
        vVertexes.destroy();
        vNormals.destroy();
        vEdges.destroy();
        vTriangles.destroy();
    }

} /* namespace lsp */
