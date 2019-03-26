/*
 * rt_mesh.h
 *
 *  Created on: 26 февр. 2019 г.
 *      Author: sadko
 */

#ifndef INCLUDE_CORE_3D_RT_MESH_H_
#define INCLUDE_CORE_3D_RT_MESH_H_

#include <core/3d/common.h>
#include <core/3d/Allocator3D.h>
#include <core/3d/Object3D.h>
#include <core/3d/Scene3D.h>
#include <core/3d/View3D.h>
#include <core/3d/rt_spline.h>

namespace lsp
{
    struct rt_debug_t;

    typedef struct rt_mesh_t
    {
        public:
            Allocator3D<rtm_vertex_t>   vertex;     // Collection of vertexes
            Allocator3D<rtm_edge_t>     edge;       // Collection of edges
            Allocator3D<rtm_triangle_t> triangle;   // Collection of triangles
            cstorage<rtm_material_t>    material;   // Additional materials

            IF_RT_TRACE_Y(
                rt_debug_t             *debug;      // Debug context
                View3D                 *view;       // Debug view
            )
        public:
            explicit rt_mesh_t();
            ~rt_mesh_t();

            IF_RT_TRACE_Y(
                inline void            set_debug_context(rt_debug_t *debug, View3D *view)
                {
                    this->debug     = debug;
                    this->view      = view;
                }
            )

        protected:
            bool            validate_list(rtm_edge_t *e);
            bool            validate_list(rtm_vertex_t *v);
            static ssize_t  linked_count(rtm_triangle_t *t, rtm_edge_t *e);
            static ssize_t  linked_count(rtm_edge_t *e, rtm_vertex_t *v);

            static bool     unlink_edge(rtm_edge_t *e, rtm_vertex_t *v);
            static bool     replace_edge(rtm_edge_t *e, rtm_edge_t *re);
            static bool     unlink_triangle(rtm_triangle_t *t, rtm_edge_t *e);
            static status_t arrange_triangle(rtm_triangle_t *ct, rtm_edge_t *e);

            rtm_vertex_t   *add_unique_vertex(const point3d_t *p);
            rtm_edge_t     *add_unique_edge(rtm_vertex_t *v1, rtm_vertex_t *v2);

            status_t        copy_object_data(Object3D *obj, ssize_t oid, const matrix3d_t *transform, rt_material_t *material, size_t itag);
            status_t        solve_conflicts(ssize_t oid);
            status_t        solve_vertex_edge_conflicts();
            status_t        remove_duplicate_edges();
            status_t        split_edge(rtm_edge_t* e, rtm_vertex_t* sp);
            status_t        split_triangle(rtm_triangle_t* t, rtm_vertex_t* sp);
            status_t        paint_triangles_internal(ssize_t oid);

            rt_material_t  *build_material(rt_material_t *from, rt_material_t *to);
            status_t        remove_obsolete_primitives();

            status_t        slice_data(cvector<rt_spline_t> &splines);

            status_t        walk_edge(rt_spline_t *spline, rtm_edge_t *xe, rtm_vertex_t *v);

        public:
            /**
             * Clear mesh: clear underlying structures
             */
            inline void     clear()
            {
                vertex.clear();
                edge.clear();
                triangle.clear();
            }

            /**
             * Flush mesh: clear underlying structures and release memory
             */
            inline void     flush()
            {
                vertex.flush();
                edge.flush();
                triangle.flush();
            }

            /**
             * Swap internal mesh contents with another context
             * @param dst destination context to perform swap
             */
            inline void     swap(rt_mesh_t *dst)
            {
                vertex.swap(&dst->vertex);
                edge.swap(&dst->edge);
                triangle.swap(&dst->triangle);
            }

            /**
             * Initialize mesh contents with object contents
             * @param obj object to initialize
             * @param oid unique object identifier
             * @return status of operation
             */
            status_t        init(Object3D *obj, ssize_t oid, const matrix3d_t *transform, rt_material_t *material = NULL);

            /**
             * Add object exclusively to context
             * @param obj object to add
             * @param oid unique id to identify the object
             * @param material material that describes behaviour of reflected rays
             * @return status of operation
             */
            inline status_t add_object(Object3D *obj, ssize_t oid, rt_material_t *material)
            {
                return add_object(obj, oid, obj->matrix(), material);
            }

            /**
             * Add object exclusively to context
             * @param obj object to add
             * @param oid unique id to identify the object
             * @param transform transformation matrix to apply to object
             * @param material material that describes behaviour of reflected rays
             * @return status of operation
             */
            status_t        add_object(Object3D *obj, ssize_t oid, const matrix3d_t *transform, rt_material_t *material);

            /**
             * Subtract object obj from mesh
             * @param obj object to subtract
             * @return status of operation
             */
            status_t        subtract(Object3D *obj, ssize_t oid, const matrix3d_t *transform);

            /**
             * Check consistency of the context: that all stored pointers are valid
             * @return true if context is in valid state
             */
            bool            validate();

            /**
             * Copy all data from the source mesh
             * @param src source mesh to copy
             * @return status of operation
             */
            status_t        copy(rt_mesh_t *src);
    } rt_mesh_t;
}

#endif /* INCLUDE_CORE_3D_RT_MESH_H_ */
