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

        private:
            status_t do_clone(Scene3D *s);

        public:
            /** Default constructor
             *
             */
            explicit Scene3D(size_t blk_size = 1024);

            /** Destructor
             *
             */
            ~Scene3D();

        public:
            /** Destroy scene
             *
             * @param recursive destroy attached objects
             */
            void destroy();

            /** Clear scene
             *
             */
            inline void clear() { destroy(); };

            /**
             * Clone contents from another scene
             * @param src
             */
            status_t clone_from(const Scene3D *src);

            /**
             * Swap contents with another scene
             * @param scene scene to perform swap
             */
            void swap(Scene3D *scene);

        public:
            /**
             * Do some post-processing after loading scene from file
             */
            void postprocess_after_loading();

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
             * Get object by it's index
             * @param idx object index
             * @return pointer to object or NULL
             */
            inline Object3D *object(size_t idx) { return vObjects.get(idx); }

            /**
             * Get object index
             * @param obj object index
             * @return object index
             */
            size_t index_of(Object3D *obj)      { return vObjects.index_of(obj);    }

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
            inline obj_normal_t *normal(size_t idx)
            {
                return (idx < vNormals.size()) ?
                        vNormals.get(idx) :
                        vXNormals.get(idx - vNormals.size());
            }

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
             * Add object with UTF8-encoded name
             * @param utf8_name UTF8-encoded name
             * @return pointer to object or NULL on error
             */
            Object3D *add_object(const char *utf8_name);

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

            /**
             * Initialize all tags (prepare for data manipulations)
             * @param ptag pointer tag
             * @param itag integer tag
             */
            void init_tags(void *ptag, ssize_t itag);

            /**
             * Get index of the object
             * @param obj object to search for
             * @return index of object
             */
            inline ssize_t get_object_index(Object3D *obj) { return vObjects.index_of(obj); }

            /**
             * Validate scene consistence
             * @return true if scene is self-consistent
             */
            bool validate();
    };

} /* namespace lsp */

#endif /* CORE_3D_SCENE3D_H_ */
