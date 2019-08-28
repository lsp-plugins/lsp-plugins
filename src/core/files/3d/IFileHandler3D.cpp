/*
 * IFileHandler3D.cpp
 *
 *  Created on: 21 апр. 2017 г.
 *      Author: sadko
 */

#include <core/files/3d/IFileHandler3D.h>

namespace lsp
{
    IFileHandler3D::~IFileHandler3D()
    {
    }

    status_t IFileHandler3D::begin_object(size_t index, const char *name)
    {
        return STATUS_OK;
    }

    status_t IFileHandler3D::end_object(size_t index)
    {
        return STATUS_OK;
    }

    status_t IFileHandler3D::end_of_data()
    {
        return STATUS_OK;
    }

    ssize_t IFileHandler3D::add_vertex(const point3d_t *p)
    {
        return 0;
    }

    ssize_t IFileHandler3D::add_parameter_vertex(const point3d_t *p)
    {
        return 0;
    }

    ssize_t IFileHandler3D::add_normal(const vector3d_t *v)
    {
        return 0;
    }

    ssize_t IFileHandler3D::add_texture_vertex(const point3d_t *v)
    {
        return 0;
    }

    status_t IFileHandler3D::add_face(const ssize_t *vv, const ssize_t *vn, const ssize_t *vt, size_t n)
    {
        return STATUS_OK;
    }

    status_t IFileHandler3D::add_points(const ssize_t *vv, size_t n)
    {
        return STATUS_OK;
    }

    status_t IFileHandler3D::add_line(const ssize_t *vv, const ssize_t *vt, size_t n)
    {
        return STATUS_OK;
    }

} /* namespace lsp */
