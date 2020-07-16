/*
 * IObjHandler.cpp
 *
 *  Created on: 21 апр. 2017 г.
 *      Author: sadko
 */

#include <core/files/3d/IObjHandler.h>

namespace lsp
{
    namespace obj
    {
        IObjHandler::~IObjHandler()
        {
        }

        status_t IObjHandler::begin_object(size_t index, const char *name)
        {
            return STATUS_OK;
        }

        status_t IObjHandler::end_object(size_t index)
        {
            return STATUS_OK;
        }

        status_t IObjHandler::end_of_data()
        {
            return STATUS_OK;
        }

        ssize_t IObjHandler::add_vertex(const point3d_t *p)
        {
            return 0;
        }

        ssize_t IObjHandler::add_parameter_vertex(const point3d_t *p)
        {
            return 0;
        }

        ssize_t IObjHandler::add_normal(const vector3d_t *v)
        {
            return 0;
        }

        ssize_t IObjHandler::add_texture_vertex(const point3d_t *v)
        {
            return 0;
        }

        status_t IObjHandler::add_face(const ssize_t *vv, const ssize_t *vn, const ssize_t *vt, size_t n)
        {
            return STATUS_OK;
        }

        status_t IObjHandler::add_points(const ssize_t *vv, size_t n)
        {
            return STATUS_OK;
        }

        status_t IObjHandler::add_line(const ssize_t *vv, const ssize_t *vt, size_t n)
        {
            return STATUS_OK;
        }
    }
} /* namespace lsp */
