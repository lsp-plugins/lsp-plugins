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
#include <data/cstorage.h>

namespace lsp
{
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

        private:
            cstorage<point3d_t>         sVertexes;
            cstorage<vector3d_t>        sNormals;
            cstorage<vertex_index_t>    sVxInd;
            cstorage<vertex_index_t>    sNormInd;

            material3d_t                sMaterial;
            matrix3d_t                  sMatrix;
            point3d_t                   sCenter;
            size_t                      nTriangles;
            char                       *sName;
            bool                        bVisible;
            bool                        bTraceable;

        public:
            /** Default constructor
             *
             */
            Object3D();

            /** Destructor
             *
             */
            ~Object3D();

            /** Destroy object's contents
             *
             */
            void destroy();

            /** Get object's material
             *
             * @param m object's material
             * @return pointer to object's material
             */
            inline material3d_t *get_material() { return &sMaterial; };

            /** Set material
             *
             * @param m material
             */
            inline void set_material(const material3d_t *m) { sMaterial = *m; };

            /** Get number of vertex
             *
             * @return number of vertex
             */
            inline size_t get_vertex_count() const { return sVertexes.size(); }

            /** Get capacity in vertexes
             *
             * @return capacity in vertexes
             */
            inline size_t get_vertex_capacity() const { return sVertexes.capacity(); }

            /** Get number of normals
             *
             * @return number of normals
             */
            inline size_t get_normals_count() const { return sNormals.size(); };

            /** Get capacity in normals
             *
             * @return capacity in normals
             */
            inline size_t get_normals_capacity() const { return sNormals.capacity(); };

            /** Get number of indices in object
             *
             * @return number of indices in object
             */
            inline size_t get_indices_count() const { return sVxInd.size(); };

            /** Get capacity in indices
             *
             * @return capacity in indices
             */
            inline size_t get_indices_capacity() const { return sVxInd.capacity(); };

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

            /** Add triangle
             *
             * @param vv array of three vertex indexes
             * @return status of operation
             */
            status_t add_triangle(ssize_t *vv);

            /** Add vertex with normal
             *
             * @param p vertex to add
             * @param n normal to add
             * @return index of added vertex or negative value of status
             */
            ssize_t add_vertex(const point3d_t *p, const vector3d_t *n);

            /** Add vertex
             *
             * @param p vertex to add
             * @return index of added vertex or negative value of status
             */
            ssize_t add_vertex(const point3d_t *p);

            /** Add vertex
             *
             * @param x x coordinate
             * @param y y coordinate
             * @param z z coordinate
             * @return index of added vertex or negative value of status
             */
            ssize_t add_vertex(float x, float y, float z);

            /** Create vertex and return pointer to it
             *
             * @return pointer to vertex or NULL
             */
            point3d_t *create_vertex();

            /** Create vertexes and return pointer to the first of it
             *
             * @return pointer to the first vertex or NULL
             */
            point3d_t *create_vertex(size_t n);

            /** Get vertex at specified position
             *
             * @param idx vertex identifier
             * @return normal pointer or NULL
             */
            point3d_t *get_vertex(ssize_t idx);

            /** Add normal
             *
             * @param n normal to add
             * @return index of added normal or negative value of status
             */
            ssize_t add_normal(const vector3d_t *n);

            /** Add normal
             *
             * @param dx x delta
             * @param dy y delta
             * @param dz z delta
             * @return index of added normal or negative value of status
             */
            ssize_t add_normal(float dx, float dy, float dz);

            /** Get normal at specified position
             *
             * @param idx normal identifier
             * @return normal pointer or NULL
             */
            vector3d_t *get_normal(ssize_t idx);

            /** Get array of vertexes
             *
             * @return array of vertexes
             */
            inline point3d_t *get_vertexes() { return sVertexes.get_array(); };

            /** Get array of normals
             *
             * @return array of normals
             */
            inline vector3d_t *get_normals() { return sNormals.get_array(); };

            /** Get array of vertex indexes
             *
             * @return array of vertex indexes
             */
            inline vertex_index_t *get_vertex_indexes() { return sVxInd.get_array(); };

            /** Get array of normal indexes
             *
             * @return array of normal indexes
             */
            inline vertex_index_t *get_normal_indexes() { return sNormInd.get_array(); };

            /** Get number of triangles
             *
             * @return number of triangles
             */
            inline size_t get_triangles_count() const { return nTriangles; };

            /** Get matrix
             *
             * @return object matrix
             */
            inline matrix3d_t *get_matrix() { return &sMatrix; }

            /** Get object name
             *
             * @return object name
             */
            inline const char *get_name() const { return sName; }

            /** Set object name
             *
             * @param name
             * @return
             */
            bool set_name(const char *name);

            /** Check if object is ray-traceable
             *
             * @return true if object is ray-traceable
             */
            inline bool is_traceable() const
            {
                return bTraceable;
            }
            
            /** Set object ray-traceable
             *
             * @param traceable traceable flag
             */
            inline void set_traceable(bool traceable)
            {
                bTraceable = traceable;
            }
            
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

            /** Get center of the object
             *
             * @return center of the object
             */
            inline point3d_t *get_center()
            {
                return &sCenter;
            }
    };

} /* namespace lsp */

#endif /* INCLUDE_CORE_3D_OBJECT3D_H_ */
