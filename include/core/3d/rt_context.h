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
        S_FILTER_VIEW,
        S_CULL_VIEW,
        S_PARTITION,
        S_CUTOFF,
        S_REFLECT
    };

    typedef struct rt_context_t
    {
        public:
            rt_view_t                   view;       // Ray tracing point of view
            rt_shared_t                *shared;     // Shared settings
            rt_context_state_t          state;      // Context state
            size_t                      loop;       // Loop counter

            Allocator3D<rt_vertex_t>    vertex;     // Collection of vertexes
            Allocator3D<rt_edge_t>      edge;       // Collection of edges
            Allocator3D<rt_triangle_t>  triangle;   // Collection of triangles

        public:
            // Construction/destruction
            explicit rt_context_t(rt_shared_t *shared);
            ~rt_context_t();

        protected:
            static status_t arrange_triangle(rt_triangle_t *ct, rt_edge_t *e);
            static bool     unlink_edge(rt_edge_t *e, rt_vertex_t *v);
            static bool     unlink_triangle(rt_triangle_t *t, rt_edge_t *e);

            bool            validate_list(rt_vertex_t *v);
            bool            validate_list(rt_edge_t *e);
            static ssize_t  linked_count(rt_edge_t *e, rt_vertex_t *v);
            static ssize_t  linked_count(rt_triangle_t *t, rt_edge_t *e);
            bool            check_crossing(rt_triangle_t *ct, rt_triangle_t *st);

            status_t        split_edge(rt_edge_t* e, rt_vertex_t* sp);
            status_t        split_edges(const vector3d_t *pl);

            void            cleanup_tag_pointers();
            status_t        fetch_triangle(rt_context_t *dst, rt_triangle_t *st);
            status_t        fetch_triangle_safe(rt_context_t *dst, rt_triangle_t *st);
            status_t        fetch_triangles(rt_context_t *dst, ssize_t itag);
            status_t        fetch_triangles_safe(rt_context_t *dst, ssize_t itag);
            void            complete_fetch(rt_context_t *dst);
            void            calc_partition_itag(rt_triangle_t *ct);

            void            dump_edge_list(size_t lvl, rt_edge_t *e);
            void            dump_triangle_list(size_t lvl, rt_triangle_t *t);

        public:
            // Methods

            /**
             * Clear context: clear underlying structures
             */
            void            clear();

            /**
             * Flush context: clear underlying structures and release memory
             */
            void            flush();

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
             * Filter context state with set of planes
             * @param out all triangles that are 'outside' the planes
             * @param in all triangles that are candidates to be inside
             * @param pl culling plane
             * @return status of operation
             */
            status_t        filter(rt_context_t *out, rt_context_t *in, const vector3d_t *pl);

            /**
             * Partition space using random-selected triangle
             * @param out ouside contexts (3 items)
             * @param ign ignore context
             * @param in inside context
             * @return status of operation
             */
            status_t        partition(rt_context_t **out, rt_context_t *ign, rt_context_t *in);

            /**
             * Split context space into two spaces with plane
             * @param out context data above the plane
             * @param in context data below the plane
             * @param pl plane equation
             * @return status of operation
             */
            status_t        split(rt_context_t *out, rt_context_t *in, const vector3d_t *pl);

            /**
             * Perform cutoff by current triangle
             * @param out outside context
             * @param in inside context
             * @return status of operation
             */
            status_t        cutoff(rt_context_t *out, rt_context_t *in);

            /**
             * Check consistency of the context: that all stored pointers are valid
             * @return true if context is in valid state
             */
            bool            validate();

            /**
             * Add triangle to list of ignored
             * @param t triangle to add to list of ignored
             * @return status of operation
             */
            status_t        ignore(const rt_triangle_t *t);

            /**
             * Add triangle to list of matched
             * @param t triangle to add to list of matched
             * @return status of operation
             */
            status_t        match(const rt_triangle_t *t);

            /**
             * Dump context
             */
            void            dump();
    } rt_context_t;

} /* namespace mtest */

#endif /* INCLUDE_CORE_3D_RT_CONTEXT_H_ */
