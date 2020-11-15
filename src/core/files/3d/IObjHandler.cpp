/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 21 апр. 2017 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
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
