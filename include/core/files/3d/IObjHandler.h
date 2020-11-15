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

#ifndef CORE_FILES_3D_IOBJHANDLER_H_
#define CORE_FILES_3D_IOBJHANDLER_H_

#include <core/types.h>
#include <core/status.h>
#include <dsp/dsp.h>

namespace lsp
{
    namespace obj
    {
        /** File contents handler interface
         *
         */
        class IObjHandler
        {
            public:
                virtual ~IObjHandler();

            public:
                /** Start object
                 *
                 * @param id object id
                 * @param name object name
                 * @return status of operation
                 */
                virtual status_t begin_object(size_t id, const char *name);

                /** End object
                 *
                 * @param id object id
                 * @return status of operation
                 */
                virtual status_t end_object(size_t id);

                /**
                 * This callback is called when there is no more data at the input
                 * and allows to post-process/finalize loaded data structures
                 * @return status of operation
                 */
                virtual status_t end_of_data();

                /** Add vertex
                 *
                 * @param idx index
                 * @param p vertex to add
                 * @return number of added vertex or negative error code
                 */
                virtual ssize_t add_vertex(const point3d_t *p);

                /** Add parameter vertex
                 *
                 * @param idx index
                 * @param p vertex to add
                 * @return number of added vertex or negative error code
                 */
                virtual ssize_t add_parameter_vertex(const point3d_t *p);

                /** Add normal
                 *
                 * @param v normal vector to add
                 * @return number of added normal or negative error code
                 */
                virtual ssize_t add_normal(const vector3d_t *v);

                /** Add texture vertex
                 *
                 * @param v texture vector to add
                 * @return number of added texture vertex or negative error code
                 */
                virtual ssize_t add_texture_vertex(const point3d_t *v);

                /** Add face
                 *
                 * @param vv array of vertex indexes starting with 0 (negative if not present)
                 * @param vn array of normal indexes starting with 0 (negative if not present)
                 * @param vt array of texture vertex indexes starting with 0 (negative if not present)
                 * @param n number of elements in vv, vn and vt
                 * @return status of operation
                 */
                virtual status_t add_face(const ssize_t *vv, const ssize_t *vn, const ssize_t *vt, size_t n);

                /** Add points
                 *
                 * @param vv array of vertex indexes starting with 0 (negative if not present)
                 * @param n number of elements in vv
                 * @return status of operation
                 */
                virtual status_t add_points(const ssize_t *vv, size_t n);

                /** Add line
                 *
                 * @param vv array of vertex indexes starting with 0 (negative if not present)
                 * @param vt array of texture vertex indexes starting with 0 (negative if not present)
                 * @param n number of elements in vv
                 * @return status of operation
                 */
                virtual status_t add_line(const ssize_t *vv, const ssize_t *vt, size_t n);
        };
    }

} /* namespace lsp */

#endif /* CORE_FILES_3D_IOBJHANDLER_H_ */
