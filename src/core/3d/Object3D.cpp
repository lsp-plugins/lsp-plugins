/*
 * Object3D.cpp
 *
 *  Created on: 9 апр. 2017 г.
 *      Author: sadko
 */

#include <core/alloc.h>
#include <core/status.h>
#include <core/3d/Scene3D.h>
#include <core/3d/Object3D.h>

namespace lsp
{
    Object3D::Object3D(Scene3D *scene, const LSPString *name)
    {
        pScene                  = scene;
        bVisible                = true;

        sName.set(name);
        dsp::init_matrix3d_identity(&sMatrix);
    }

    Object3D::~Object3D()
    {
        destroy();
    }
    
    void Object3D::destroy()
    {
        vTriangles.flush();
    }

    status_t Object3D::add_triangle(
            ssize_t v1, ssize_t v2, ssize_t v3,
            ssize_t vn1, ssize_t vn2, ssize_t vn3
        )
    {
        // Check vertex index
        ssize_t v_limit  = pScene->vVertexes.size();
        if ((v1 >= v_limit) || (v2 >= v_limit) || (v3 >= v_limit))
            return -STATUS_INVALID_VALUE;
        if ((v1 < 0) || (v2 < 0) || (v3 < 0))
            return -STATUS_INVALID_VALUE;

        // Check normal index
        ssize_t n_limit  = pScene->vNormals.size();
        if ((vn1 >= n_limit) || (vn2 >= n_limit) || (vn3 >= n_limit))
            return -STATUS_INVALID_VALUE;

        // Allocate triangle
        obj_triangle_t *t = pScene->vTriangles.alloc();
        if (t == NULL)
            return -STATUS_NO_MEM;

        // Store vertexes
        t->v[0]     = pScene->vertex(v1);
        t->v[1]     = pScene->vertex(v2);
        t->v[2]     = pScene->vertex(v3);

        // Store normals
        obj_normal_t *xvn       = NULL;
        if ((vn1 < 0) || (vn2 < 0) || (vn3 < 0))
        {
            // Add normal
            obj_normal_t *xvn       = pScene->vXNormals.alloc();
            if (xvn == NULL)
                return -STATUS_NO_MEM;

            // Get points
            dsp::calc_normal3d_p3(xvn, t->v[0], t->v[1], t->v[2]);
        }

        t->n[0]     = (vn1 >= 0) ? pScene->normal(vn1) : xvn;
        t->n[1]     = (vn2 >= 0) ? pScene->normal(vn2) : xvn;
        t->n[2]     = (vn3 >= 0) ? pScene->normal(vn3) : xvn;

        // Store edges
        obj_vertex_t *v[2];
        for (size_t i=0; i<3; ++i)
        {
            v[0]    = t->v[i];
            v[1]    = t->v[(i+1)%3];

            // Lookup for already existing edge
            obj_edge_t *e = v[0]->ve;
            while (e != NULL)
            {
                if (e->v[0] == v[0])
                {
                    if (e->v[1] == v[1])
                        break;
                    e = e->vlnk[0];
                }
                else // e->v[1] == v[0]
                {
                    if (e->v[0] == v[1])
                        break;
                    e = e->vlnk[1];
                }
            }

            // Need to create new edge and link?
            if (e == NULL)
            {
                e = pScene->vEdges.alloc();
                if (e == NULL)
                    return -STATUS_NO_MEM;

                e->v[0]     = v[0];
                e->v[1]     = v[1];
                e->vlnk[0]  = v[0]->ve;
                e->vlnk[1]  = v[1]->ve;
                v[0]->ve    = e;
                v[1]->ve    = e;
            }

            // Add edge to triangle
            t->e[i]     = e;
        }

        return (vTriangles.add(t)) ? STATUS_OK : STATUS_NO_MEM;
    }

    status_t Object3D::add_triangle(ssize_t *vv, ssize_t *vn)
    {
        return add_triangle(vv[0], vv[1], vv[2], vn[0], vn[1], vn[2]);
    }

    status_t Object3D::add_triangle(ssize_t *vv)
    {
        return add_triangle(vv[0], vv[1], vv[2], -1, -1, -1);
    }

} /* namespace lsp */
