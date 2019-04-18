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
    Scene3D::Scene3D(size_t blk_size):
        vVertexes(blk_size),
        vNormals(blk_size),
        vXNormals(blk_size),
        vEdges(blk_size),
        vTriangles(blk_size)
    {
    }
    
    Scene3D::~Scene3D()
    {
        destroy();
    }

    void Scene3D::swap(Scene3D *scene)
    {
        vObjects.swap_data(&scene->vObjects);
        vVertexes.swap(&scene->vVertexes);
        vNormals.swap(&scene->vNormals);
        vXNormals.swap(&scene->vXNormals);
        vEdges.swap(&scene->vEdges);
        vTriangles.swap(&scene->vTriangles);
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

    Object3D *Scene3D::add_object(const char *utf8_name)
    {
        LSPString name;
        if (!name.set_utf8(utf8_name))
            return NULL;

        Object3D *obj = new Object3D(this, &name);
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
            v->x        = p->x;
            v->y        = p->y;
            v->z        = p->z;
            v->w        = p->w;
            v->id       = res;
            v->ve       = NULL;
            v->ptag     = NULL;
            v->itag     = -1;
        }
        return res;
    }

    ssize_t Scene3D::add_normal(const vector3d_t *n)
    {
        obj_normal_t *an;
        ssize_t res = vNormals.ialloc(&an);
        if (res >= 0)
        {
            an->dx      = n->dx;
            an->dy      = n->dy;
            an->dz      = n->dz;
            an->dw      = n->dw;
            an->id      = res;
            an->ptag    = NULL;
            an->itag    = -1;
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
        vXNormals.destroy();
        vEdges.destroy();
        vTriangles.destroy();
    }

    status_t Scene3D::do_clone(Scene3D *s)
    {
        //----------------------------------------------------------
        // Clone vertexes
        for (size_t i=0, n=s->vVertexes.size(); i<n; ++i)
        {
            obj_vertex_t *sv        = s->vVertexes.get(i);
            obj_vertex_t *dv        = vVertexes.alloc(sv);
            if (dv == NULL)
                return STATUS_NO_MEM;
        }

        // Clone normals
        ssize_t norms = s->vNormals.size();
        for (size_t i=0, n=s->vNormals.size(); i<n; ++i)
        {
            obj_normal_t *sn        = s->vNormals.get(i);
            obj_normal_t *dn        = vNormals.alloc(sn);
            if (dn == NULL)
                return STATUS_NO_MEM;
        }

        for (size_t i=0, n=s->vXNormals.size(); i<n; ++i)
        {
            obj_normal_t *sn        = s->vXNormals.get(i);
            obj_normal_t *dn        = vXNormals.alloc(sn);
            if (dn == NULL)
                return STATUS_NO_MEM;
        }

        // Clone edges
        for (size_t i=0, n=s->vEdges.size(); i<n; ++i)
        {
            obj_edge_t *se          = s->vEdges.get(i);
            obj_edge_t *de          = vEdges.alloc(se);
            if (de == NULL)
                return STATUS_NO_MEM;
        }

        // Clone Triangles
        for (size_t i=0, n=s->vTriangles.size(); i<n; ++i)
        {
            obj_triangle_t *st      = s->vTriangles.get(i);
            obj_triangle_t *dt      = vTriangles.alloc(st);
            if (dt == NULL)
                return STATUS_NO_MEM;
        }

        // Clone Objects
        for (size_t i=0, n=s->vObjects.size(); i<n; ++i)
        {
            LSPString name;
            Object3D *sobj          = s->vObjects.get(i);
            if (!sobj->get_name(&name))
                return STATUS_NO_MEM;

            Object3D *dobj          = new Object3D(this, &name);
            if (!vObjects.add(dobj))
            {
                delete dobj;
                return STATUS_NO_MEM;
            }
        }

        //----------------------------------------------------------
        // Patch vertex pointers
        for (size_t i=0, n=vVertexes.size(); i<n; ++i)
        {
            // Patch edge pointer
            obj_vertex_t *dv        = vVertexes.get(i);
            if (dv->ve != NULL)
            {
                obj_edge_t *pe          = vEdges.get(dv->ve->id);
                if ((pe == NULL) || (pe->id != dv->ve->id))
                    return STATUS_BAD_STATE;
                dv->ve          = pe;
            }
        }

        // Patch edge pointers again
        for (size_t i=0, n=vEdges.size(); i<n; ++i)
        {
            // Patch edge pointer
            obj_edge_t *de          = vEdges.get(i);

            for (size_t j=0; j<2; ++j)
            {
                // Patch vertex pointer
                if (de->v[j] != NULL)
                {
                    obj_vertex_t *pv        = vVertexes.get(de->v[j]->id);
                    if ((pv == NULL) || (pv->id != de->v[j]->id))
                        return STATUS_NO_MEM;

                    de->v[j]                = pv;
                }

                // Patch link pointer
                if (de->vlnk[j] != NULL)
                {
                    obj_edge_t *pe          = vEdges.get(de->vlnk[j]->id);
                    if ((pe == NULL) || (pe->id != de->vlnk[j]->id))
                        return STATUS_BAD_STATE;
                    de->vlnk[j]             = pe;
                }
            }
        }

        // Patch triangle pointers
        for (size_t i=0, n=vTriangles.size(); i<n; ++i)
        {
            obj_triangle_t *dt      = vTriangles.get(i);

            for (size_t j=0; j<3; ++j)
            {
                // Patch vertex pointer
                if (dt->v[j] != NULL)
                {
                    obj_vertex_t *pv        = vVertexes.get(dt->v[j]->id);
                    if ((pv == NULL) || (pv->id != dt->v[j]->id))
                        return STATUS_BAD_STATE;
                    dt->v[j]        = pv;
                }

                // Patch normal pointer
                if (dt->n[j] != NULL)
                {
                    obj_normal_t *pn        = (dt->n[j]->id < norms) ?
                                                vNormals.get(dt->n[j]->id) : vXNormals.get(dt->n[j]->id - norms);
                    if ((pn == NULL) || (pn->id != dt->n[j]->id))
                        return STATUS_BAD_STATE;
                    dt->n[j]        = pn;
                }

                // Patch edge pointer
                if (dt->e[j] != NULL)
                {
                    obj_edge_t *pe          = vEdges.get(dt->e[j]->id);
                    if ((pe == NULL) || (pe->id != dt->e[j]->id))
                        return STATUS_BAD_STATE;
                    dt->e[j]        = pe;
                }
            }
        }

        //----------------------------------------------------------
        // Commit triangles to Objects
        for (size_t i=0, n=s->vObjects.size(); i<n; ++i)
        {
            Object3D *d             = s->vObjects.get(i);
            Object3D *o             = vObjects.get(i);

            // Add triangles
            for (size_t j=0, m=d->vTriangles.size(); j<m; ++j)
            {
                obj_triangle_t *t       = d->vTriangles.get(j);
                obj_triangle_t *x       = vTriangles.get(t->id);
                if (x == NULL)
                    return STATUS_BAD_STATE;

                if (!o->vTriangles.add(x))
                    return STATUS_NO_MEM;
            }
        }

        return STATUS_OK;
    }

    status_t Scene3D::clone_from(const Scene3D *src)
    {
        Scene3D *s = const_cast<Scene3D *>(src);
        if (s == NULL)
            return STATUS_BAD_ARGUMENTS;

        destroy();

        status_t res = do_clone(s);
        if (res != STATUS_OK)
            destroy();

        return res;
    }

    /**
     * Initialize all tags (prepare for data manipulations)
     * @param ptag pointer tag
     * @param itag integer tag
     */
    void Scene3D::init_tags(void *ptag, ssize_t itag)
    {
        for (size_t i=0, n=vVertexes.size(); i<n; ++i)
        {
            obj_vertex_t *dv = vVertexes.get(i);
            dv->ptag        = ptag;
            dv->itag        = itag;
        }

        for (size_t i=0, n=vNormals.size(); i<n; ++i)
        {
            obj_normal_t *dn = vNormals.get(i);
            dn->ptag        = ptag;
            dn->itag        = itag;
        }

        for (size_t i=0, n=vXNormals.size(); i<n; ++i)
        {
            obj_normal_t *dn = vXNormals.get(i);
            dn->ptag        = ptag;
            dn->itag        = itag;
        }

        for (size_t i=0, n=vEdges.size(); i<n; ++i)
        {
            obj_edge_t *de  = vEdges.get(i);
            de->ptag        = ptag;
            de->itag        = itag;
        }

        for (size_t i=0, n=vTriangles.size(); i<n; ++i)
        {
            obj_triangle_t *dt = vTriangles.get(i);
            dt->ptag        = ptag;
            dt->itag        = itag;
        }
    }

    /**
     * Validate scene consistence
     * @return true if scene is self-consistent
     */
    bool Scene3D::validate()
    {
        for (size_t i=0, n=vVertexes.size(); i<n; ++i)
        {
            obj_vertex_t *dv = vVertexes.get(i);
            if (dv == NULL)
                return false;

            if (!vEdges.validate(dv->ve))
                return false;
        }

        for (size_t i=0, n=vEdges.size(); i<n; ++i)
        {
            obj_edge_t *de  = vEdges.get(i);

            for (size_t j=0; j<2; ++j)
            {
                if (!vVertexes.validate(de->v[j]))
                    return false;
                if (!vEdges.validate(de->vlnk[j]))
                    return false;
            }
        }

        for (size_t i=0, n=vTriangles.size(); i<n; ++i)
        {
            obj_triangle_t *dt = vTriangles.get(i);

            for (size_t j=0; j<3; ++j)
            {
                if (!vVertexes.validate(dt->v[j]))
                    return false;
                if (!vEdges.validate(dt->e[j]))
                    return false;
                if (!vNormals.validate(dt->n[j]))
                {
                    if (!vXNormals.validate(dt->n[j]))
                        return false;
                }
            }
        }
        return true;
    }

    void Scene3D::postprocess_after_loading()
    {
        size_t norms = vNormals.size();
        if (norms <= 0)
            return;

        // Patch identifier for generated normals
        for (size_t i=0, n=vXNormals.size(); i<n; ++i)
        {
            obj_normal_t *xn = vXNormals.get(i);
            xn->id          += norms;
        }
    }

} /* namespace lsp */
