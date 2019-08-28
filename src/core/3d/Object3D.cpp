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

        dsp::init_point_xyz(&sBoundBox.p[0], 0.0f, 0.0f, 0.0f);
        dsp::init_point_xyz(&sBoundBox.p[1], 0.0f, 0.0f, 0.0f);
        dsp::init_point_xyz(&sBoundBox.p[2], 0.0f, 0.0f, 0.0f);
        dsp::init_point_xyz(&sBoundBox.p[3], 0.0f, 0.0f, 0.0f);
        dsp::init_point_xyz(&sBoundBox.p[4], 0.0f, 0.0f, 0.0f);
        dsp::init_point_xyz(&sBoundBox.p[5], 0.0f, 0.0f, 0.0f);
        dsp::init_point_xyz(&sBoundBox.p[6], 0.0f, 0.0f, 0.0f);
        dsp::init_point_xyz(&sBoundBox.p[7], 0.0f, 0.0f, 0.0f);

        dsp::init_point_xyz(&sCenter, 0.0f, 0.0f, 0.0f);
    }

    Object3D::~Object3D()
    {
        destroy();
    }
    
    void Object3D::destroy()
    {
        vTriangles.flush();
    }

    void Object3D::post_load()
    {
        dsp::init_point_xyz(&sCenter, 0.0f, 0.0f, 0.0f);
        for (size_t i=0; i<8; ++i)
        {
            sCenter.x      += sBoundBox.p[i].x;
            sCenter.y      += sBoundBox.p[i].y;
            sCenter.z      += sBoundBox.p[i].z;
        }
        sCenter.x      *= 0.125f; // 1/8
        sCenter.y      *= 0.125f; // 1/8
        sCenter.z      *= 0.125f; // 1/8
    }

    status_t Object3D::add_triangle(
            ssize_t face_id,
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
        ssize_t tid         = pScene->vTriangles.size();
        obj_triangle_t *t   = pScene->vTriangles.alloc();
        if (t == NULL)
            return -STATUS_NO_MEM;

        // Store vertexes
        t->id       = tid;
        t->face     = face_id;
        t->ptag     = NULL;
        t->itag     = -1;
        t->v[0]     = pScene->vertex(v1);
        t->v[1]     = pScene->vertex(v2);
        t->v[2]     = pScene->vertex(v3);

        // Store normals
        obj_normal_t *xvn       = NULL;
        if ((vn1 < 0) || (vn2 < 0) || (vn3 < 0))
        {
            // Add normal
            xvn         = pScene->vXNormals.alloc();
            if (xvn == NULL)
                return -STATUS_NO_MEM;

            // Get points
            dsp::calc_normal3d_p3(xvn, t->v[0], t->v[1], t->v[2]);
        }

        t->n[0]     = (vn1 >= 0) ? pScene->normal(vn1) : xvn;
        t->n[1]     = (vn2 >= 0) ? pScene->normal(vn2) : xvn;
        t->n[2]     = (vn3 >= 0) ? pScene->normal(vn3) : xvn;

        // Store edges
        for (size_t i=0; i<3; ++i)
        {
            // Lookup for already existing edge
            obj_edge_t *e = register_edge(t->v[i], t->v[(i+1)%3]);
            if (e == NULL)
                return STATUS_NO_MEM;

            // Add edge to triangle
            t->e[i]     = e;
        }

        bool first = vTriangles.size() <= 0;
        if (!vTriangles.add(t))
            return STATUS_NO_MEM;

        // Commit triangle edges
        if (first)
        {
            for (size_t i=0; i<8; ++i)
                sBoundBox.p[i] = *(t->v[0]);
        }
        else
            calc_bound_box(t->v[0]);

        calc_bound_box(t->v[1]);
        calc_bound_box(t->v[2]);

        return STATUS_OK;
    }

    void Object3D::calc_bound_box()
    {
        obj_triangle_t **vt = vTriangles.get_array();
        for (size_t i=0, n=vTriangles.size(); i<n; ++i)
        {
            obj_triangle_t *t = *(vt++);
            if (i == 0)
            {
                for (size_t i=0; i<8; ++i)
                    sBoundBox.p[i] = *(t->v[0]);
            }
            else
                calc_bound_box(t->v[0]);

            calc_bound_box(t->v[1]);
            calc_bound_box(t->v[2]);
        }
    }

    obj_edge_t *Object3D::register_edge(obj_vertex_t *v0, obj_vertex_t *v1)
    {
        // Lookup for already existing edge
        obj_edge_t *e = v0->ve;
        while (e != NULL)
        {
            if (e->v[0] == v0)
            {
                if (e->v[1] == v1)
                    break;
                e = e->vlnk[0];
            }
            else // e->v[1] == v0
            {
                if (e->v[0] == v1)
                    break;
                e = e->vlnk[1];
            }
        }

        // Need to create new edge and link?
        if (e == NULL)
        {
            ssize_t res = pScene->vEdges.ialloc(&e);
            if (res < 0)
                return NULL;

            e->id       = res;
            e->v[0]     = v0;
            e->v[1]     = v1;
            e->vlnk[0]  = v0->ve;
            e->vlnk[1]  = v1->ve;
            e->ptag     = NULL;
            e->itag     = -1;

            v0->ve      = e;
            v1->ve      = e;
        }

        return e;
    }

    void Object3D::calc_bound_box(const obj_vertex_t *p)
    {
        obj_boundbox_t *b = &sBoundBox;

        // Left plane
        if (b->p[0].x > p->x)
            b->p[0].x = p->x;
        if (b->p[1].x > p->x)
            b->p[1].x = p->x;
        if (b->p[4].x > p->x)
            b->p[4].x = p->x;
        if (b->p[5].x > p->x)
            b->p[5].x = p->x;

        // Right plane
        if (b->p[2].x < p->x)
            b->p[2].x = p->x;
        if (b->p[3].x < p->x)
            b->p[3].x = p->x;
        if (b->p[6].x < p->x)
            b->p[6].x = p->x;
        if (b->p[7].x < p->x)
            b->p[7].x = p->x;

        // Near plane
        if (b->p[1].y > p->y)
            b->p[1].y = p->y;
        if (b->p[2].y > p->y)
            b->p[2].y = p->y;
        if (b->p[5].y > p->y)
            b->p[5].y = p->y;
        if (b->p[6].y > p->y)
            b->p[6].y = p->y;

        // Far plane
        if (b->p[0].y < p->y)
            b->p[0].y = p->y;
        if (b->p[3].y < p->y)
            b->p[3].y = p->y;
        if (b->p[4].y < p->y)
            b->p[4].y = p->y;
        if (b->p[7].y < p->y)
            b->p[7].y = p->y;

        // Top plane
        if (b->p[0].z < p->z)
            b->p[0].z = p->z;
        if (b->p[1].z < p->z)
            b->p[1].z = p->z;
        if (b->p[2].z < p->z)
            b->p[2].z = p->z;
        if (b->p[3].z < p->z)
            b->p[3].z = p->z;

        // Bottom plane
        if (b->p[4].z > p->z)
            b->p[4].z = p->z;
        if (b->p[5].z > p->z)
            b->p[5].z = p->z;
        if (b->p[6].z > p->z)
            b->p[6].z = p->z;
        if (b->p[7].z > p->z)
            b->p[7].z = p->z;
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
