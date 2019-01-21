/*
 * RTContext.h
 *
 *  Created on: 15 янв. 2019 г.
 *      Author: sadko
 */

#ifndef INCLUDE_CORE_3D_RT_CONTEXT_H_
#define INCLUDE_CORE_3D_RT_CONTEXT_H_

#include <core/3d/common.h>
#include <core/3d/Allocator3D.h>
#include <core/3d/Scene3D.h>
#include <core/3d/View3D.h>
#include <data/cstorage.h>

namespace lsp
{
    typedef struct rt_shared_t
    {
        cstorage<v_triangle3d_t>    matched;    // List of matched triangles (for debug)
        cstorage<v_triangle3d_t>    ignored;    // List of ignored triangles (for debug)
        ssize_t                     step;       // Trace step
        ssize_t                     breakpoint; // Trace breakpoint
        Scene3D                    *scene;
        View3D                     *view;
    } rt_shared_t;

    enum rt_context_state_t
    {
        S_SCAN_OBJECTS,
        S_CULL_VIEW,
        S_PARTITION
    };

    typedef struct rt_context_t
    {
        public:
            rt_view_t                   view;       // Ray tracing point of view
            rt_shared_t                *shared;     // Shared settings
            rt_context_state_t          state;      // Context state
            size_t                      index;      // Plane index

            Allocator3D<rt_vertex_t>    vertex;     // Collection of vertexes
            Allocator3D<rt_edge_t>      edge;       // Collection of edges
            Allocator3D<rt_triangle_t>  triangle;   // Collection of triangles

        public:
            // Construction/destruction
            explicit rt_context_t(rt_shared_t *shared);
            ~rt_context_t();

        protected:
            static status_t arrange_triangle(rt_triangle_t *ct, rt_edge_t *e);
            static void     unlink_edge(rt_edge_t *e, rt_vertex_t *v);
            static void     unlink_triangle(rt_triangle_t *t, rt_edge_t *e);

            bool            validate_list(rt_vertex_t *v);
            bool            validate_list(rt_edge_t *e);

            status_t        split_edge(rt_edge_t* e, rt_vertex_t* sp);
            status_t        split_edges(const vector3d_t *pl);
            status_t        split_triangles(rt_context_t *out, rt_context_t *in);

        public:
            // Methods
            /**
             * Swap internal mesh contents with another context
             * @param src source context to perform swap
             */
            void            swap(rt_context_t *src);

            /**
             * Add object to context
             * @param obj object to add
             * @return
             */
            status_t        add_object(Object3D *obj);

            /**
             * Split context space into two spaces with plane
             * @param out context data above the plane
             * @param in context data below the plane
             * @param pl plane equation
             * @return status of operation
             */
            status_t        split(rt_context_t *out, rt_context_t *in, const vector3d_t *pl);

            /**
             * Check consistency of the context: that all stored pointers are valid
             * @return true if context is in valid state
             */
            bool            validate();
    } rt_context_t;

} /* namespace mtest */

#endif /* INCLUDE_CORE_3D_RT_CONTEXT_H_ */
