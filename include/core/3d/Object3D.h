/*
 * Object3D.h
 *
 *  Created on: 9 апр. 2017 г.
 *      Author: sadko
 */

#ifndef INCLUDE_CORE_3D_OBJECT3D_H_
#define INCLUDE_CORE_3D_OBJECT3D_H_

#include <dsp/dsp.h>
#include <core/status.h>
#include <data/cvector.h>
#include <core/LSPString.h>
#include <core/3d/common.h>

namespace lsp
{
    class Scene3D;

    /** One scene object in the 3D space
     *
     */
    class Object3D
    {
        protected:
            enum flags_t
            {
                OF_VISIBLE      = 1 << 0,
                OF_TRACEABLE    = 1 << 1,
                OF_HAS_
            };

        protected:
            LSPString                   sName;
            cvector<obj_triangle_t>     vTriangles;

            matrix3d_t                  sMatrix;
            bool                        bVisible;
            Scene3D                    *pScene;

            friend class Scene3D;

        protected:
            /** Default constructor
             *
             */
            explicit Object3D(Scene3D *scene, const LSPString *name);

            /** Destructor
             *
             */
            ~Object3D();

        public:
            /** Destroy object's contents
             *
             */
            void destroy();

            /** Add triangle
             *
             * @param v1 index of vertex 1, non-negative
             * @param v2 index of vertex 2, non-negative
             * @param v3 index of vertex 3, non-negative
             * @param vn1 index of normal 1, can be negative
             * @param vn2 index of normal 2, can be negative
             * @param vn3 index of normal 3, can be negative
             * @return status of operation
             */
            status_t add_triangle(
                    ssize_t v1, ssize_t v2, ssize_t v3,
                    ssize_t vn1=-1, ssize_t vn2=-1, ssize_t vn3=-1
                );

            /** Add triangle
             *
             * @param vv array of three vertex indexes
             * @param vn array of three normal indexes
             * @return status of operation
             */
            status_t add_triangle(ssize_t *vv, ssize_t *vn);

            /**
             * Return number of triangles
             * @return number of triangles
             */
            inline size_t num_triangles() const { return vTriangles.size(); }

            /**
             * Get triangle
             * @param index triangle index
             * @return pointer to triangle or NULL
             */
            inline obj_triangle_t *triangle(size_t index) { return vTriangles.get(index); }

            /** Add triangle
             *
             * @param vv array of three vertex indexes
             * @return status of operation
             */
            status_t add_triangle(ssize_t *vv);

            /** Check if object is visible
             *
             * @return true if object is visible
             */
            inline bool is_visible() const
            {
                return bVisible;
            }
            
            /** Set object visible
             *
             * @param visible visibility flag
             */
            inline void set_visible(bool visible)
            {
                bVisible = visible;
            }

            /**
             * Get name of object
             * @param name pointer to store name
             * @return true on success
             */
            inline bool get_name(LSPString *name) const { return name->set(&sName); }

            /**
             * Get name of object in UTF-8 character set
             * @return name of object in UTF-8 character set
             */
            inline const char *get_name() const { return sName.get_utf8(); }
    };

} /* namespace lsp */

#endif /* INCLUDE_CORE_3D_OBJECT3D_H_ */
