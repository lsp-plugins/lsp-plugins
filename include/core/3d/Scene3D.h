/*
 * Scene3D.h
 *
 *  Created on: 9 апр. 2017 г.
 *      Author: sadko
 */

#ifndef CORE_3D_SCENE3D_H_
#define CORE_3D_SCENE3D_H_

#include <dsp/dsp.h>
#include <data/cvector.h>
#include <data/cstorage.h>
#include <core/3d/common.h>
#include <core/3d/Allocator3D.h>
#include <core/3d/Object3D.h>

namespace lsp
{
    /** 3D scene
     *
     */
    class Scene3D
    {
        protected:
            cvector<Object3D>           vObjects;
            Allocator3D<obj_vertex_t>   vVertexes;      // Vertex allocator
            Allocator3D<obj_normal_t>   vNormals;       // Normal allocator
            Allocator3D<obj_normal_t>   vXNormals;      // Extra normal allocator
            Allocator3D<obj_edge_t>     vEdges;         // Edge allocator
            Allocator3D<obj_triangle_t> vTriangles;     // Triangle allocator

            friend class Object3D;

        public:
            /** Default constructor
             *
             */
            explicit Scene3D();

            /** Destructor
             *
             */
            ~Scene3D();

            /** Destroy scene
             *
             * @param recursive destroy attached objects
             */
            void destroy();

            /** Clear scene
             *
             */
            inline void clear() { destroy(); };

            /** Return number of objects in scene
             *
             * @return number of objects in scene
             */
            inline size_t num_objects() const { return vObjects.size(); }

            /**
             * Return number of vertexes in scene
             * @return number of vertexes in scene
             */
            inline size_t num_vertexes() const { return vVertexes.size(); }

            /**
             * Return number of normals in scene
             * @return number of normals in scene
             */
            inline size_t num_normals() const { return vNormals.size(); }

            /**
             * Return number of edges in scene
             * @return number of edges in scene
             */
            inline size_t num_edges() const { return vEdges.size(); }

            /**
             * Return number of triangles in scene
             * @return number of triangles in scene
             */
            inline size_t num_triangles() const { return vTriangles.size(); }

            /**
             * Get vertex by specified index
             * @param idx vertex index
             * @return vertex or NULL
             */
            inline obj_vertex_t *vertex(size_t idx) { return vVertexes.get(idx); }

            /**
             * Get normal by specified index
             * @param idx normal index
             * @return normal or NULL
             */
            inline obj_normal_t *normal(size_t idx) { return vNormals.get(idx); }

            /**
             * Get edge by specified index
             * @param idx edge index
             * @return edge or NULL
             */
            inline obj_edge_t *edge(size_t idx) { return vEdges.get(idx); }

            /**
             * Get triangle by specified index
             * @param idx triangle index
             * @return triangle or NULL
             */
            inline obj_triangle_t *triangle(size_t idx) { return vTriangles.get(idx); }

            /**
             * Add object
             * @param name name of object
             * @return pointer to object or NULL on error
             */
            Object3D *add_object(const LSPString *name);

            /**
             * Add vertex
             * @param p vertex to add
             * @return index of vertex or negative status of operation
             */
            ssize_t add_vertex(const point3d_t *p);

            /**
             * Add vertex
             * @param n normal to add
             * @return index of normal or negative status of operation
             */
            ssize_t add_normal(const vector3d_t *n);

            /**
             * Get object by it's index
             * @param index object index
             * @return pointer to object or NULL
             */
            Object3D *get_object(size_t index) { return vObjects.get(index); }
    };

} /* namespace lsp */

#endif /* CORE_3D_SCENE3D_H_ */
