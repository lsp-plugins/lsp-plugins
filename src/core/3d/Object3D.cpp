/*
 * Object3D.cpp
 *
 *  Created on: 9 апр. 2017 г.
 *      Author: sadko
 */

#include <core/alloc.h>
#include <core/status.h>
#include <core/3d/Object3D.h>

namespace lsp
{
    Object3D::Object3D()
    {
        sMaterial.speed         = 1.0f;
        sMaterial.damping       = 0.0f;
        sMaterial.absorption    = 0.0f;
        sMaterial.transparency  = 0.0f;
        sMaterial.refraction    = 1.0f;
        sMaterial.reflection    = 1.0f;
        sMaterial.diffuse       = 0.0f;

        sName                   = NULL;
        nTriangles              = 0;

        bVisible                = true;
        bTraceable              = true;

        dsp::init_matrix3d_identity(&sMatrix);
        dsp::init_point_xyz(&sCenter, 0.0f, 0.0f, 0.0f);
    }

    Object3D::~Object3D()
    {
        destroy();
    }
    
    void Object3D::destroy()
    {
        sVertexes.flush();
        sNormals.flush();
        sVxInd.flush();
        sNormInd.flush();
    }

    ssize_t Object3D::add_vertex(const point3d_t *p, const vector3d_t *n)
    {
        size_t index = sVertexes.size();
        if (!sVertexes.append(p))
            return -STATUS_NO_MEM;
        if (!sNormals.append(n))
        {
            sVertexes.remove(index);
            return -STATUS_NO_MEM;
        }
        return index;
    }

    ssize_t Object3D::add_vertex(const point3d_t *p)
    {
        size_t index = sVertexes.size();
        if (!sVertexes.append(p))
            return -STATUS_NO_MEM;
        return index;
    }

    ssize_t Object3D::add_vertex(float x, float y, float z)
    {
        point3d_t p = { x, y, z, 1.0f };
        size_t index = sVertexes.size();
        if (!sVertexes.append(&p))
            return -STATUS_NO_MEM;
        return index;
    }

    point3d_t *Object3D::create_vertex()
    {
        return sVertexes.append();
    }

    point3d_t *Object3D::create_vertex(size_t n)
    {
        return sVertexes.append_n(n);
    }

    point3d_t *Object3D::get_vertex(ssize_t idx)
    {
        return sVertexes[idx];
    }

    ssize_t Object3D::add_normal(const vector3d_t *n)
    {
        size_t index = sVertexes.size();
        if (!sNormals.append(n))
            return -STATUS_NO_MEM;
        return index;
    }

    ssize_t Object3D::add_normal(float dx, float dy, float dz)
    {
        vector3d_t v = { dx, dy, dz, 0.0f };
        size_t index = sVertexes.size();
        if (!sNormals.append(&v))
            return -STATUS_NO_MEM;
        return index;
    }

    vector3d_t *Object3D::get_normal(ssize_t idx)
    {
        return sNormals[idx];
    }

    status_t Object3D::add_triangle(
            ssize_t v1, ssize_t v2, ssize_t v3,
            ssize_t vn1, ssize_t vn2, ssize_t vn3
        )
    {
        // Check vertex index
        ssize_t v_limit  = sVertexes.size();
        if ((v1 >= v_limit) || (v2 >= v_limit) || (v3 >= v_limit))
            return -STATUS_INVALID_VALUE;
        if ((v1 < 0) || (v2 < 0) || (v3 < 0))
            return -STATUS_INVALID_VALUE;

        // Check normal index
        ssize_t n_limit  = sNormals.size();
        if ((vn1 >= n_limit) || (vn2 >= n_limit) || (vn3 >= n_limit))
            return -STATUS_INVALID_VALUE;
        if ((vn1 < 0) || (vn2 < 0) || (vn3 < 0))
        {
            // Add normal
            vector3d_t *xvn     = sNormals.append();
            point3d_t *p1       = sVertexes.at(v1);
            point3d_t *p2       = sVertexes.at(v2);
            point3d_t *p3       = sVertexes.at(v3);
            dsp::calc_normal3d_p3(xvn, p1, p2, p3);
        }
        if (vn1 < 0)
            vn1     = n_limit;
        if (vn2 < 0)
            vn2     = n_limit;
        if (vn3 < 0)
            vn3     = n_limit;

        // Append elements
        vertex_index_t *vi  = sVxInd.append_n(3);
        if (!vi)
            return STATUS_NO_MEM;
        vertex_index_t *ni  = sNormInd.append_n(3);
        if (!ni)
            return STATUS_NO_MEM;

        // Store indexes
        vi[0]   = v1;
        vi[1]   = v2;
        vi[2]   = v3;

        ni[0]   = vn1;
        ni[1]   = vn2;
        ni[2]   = vn3;

        // Update number of triangles
        nTriangles  ++;

        return STATUS_OK;
    }

    status_t Object3D::add_triangle(ssize_t *vv, ssize_t *vn)
    {
        return add_triangle(vv[0], vv[1], vv[2], vn[0], vn[1], vn[2]);
    }

    status_t Object3D::add_triangle(ssize_t *vv)
    {
        return add_triangle(vv[0], vv[1], vv[2], -1, -1, -1);
    }

    bool Object3D::set_name(const char *name)
    {
        if (name == NULL)
        {
            if (sName != NULL)
                free(sName);
            sName = NULL;
            return true;
        }
        else if (sName == NULL)
        {
            sName = strdup(name);
            return sName != NULL;
        }
        else if (strcmp(sName, name) == 0)
            return true;

        // Make copy of string
        char *dup = strdup(name);
        if (dup == NULL)
            return false;
        free(sName);
        sName = dup;

        return true;
    }

} /* namespace lsp */
