/*
 * IFileHandler3D.h
 *
 *  Created on: 21 апр. 2017 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_3D_IFILEHANDLER3D_H_
#define CORE_FILES_3D_IFILEHANDLER3D_H_

#include <core/types.h>
#include <core/status.h>
#include <dsp/dsp.h>

namespace lsp
{
    /** File contents handler interface
     *
     */
    class IFileHandler3D
    {
        public:
            virtual ~IFileHandler3D();

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

} /* namespace lsp */

#endif /* CORE_FILES_3D_IFILEHANDLER3D_H_ */
