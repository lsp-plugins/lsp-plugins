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
#include <core/3d/rt_plan.h>
#include <core/3d/rt_mesh.h>
#include <data/cstorage.h>

namespace lsp
{
    typedef struct rt_debug_t
    {
        ssize_t                     step;       // Trace step
        ssize_t                     breakpoint; // Trace breakpoint
        cstorage<v_triangle3d_t>    matched;    // List of matched triangles (for debug)
        cstorage<v_triangle3d_t>    ignored;    // List of ignored triangles (for debug)
        View3D                      trace;
    } rt_debug_t;

    enum rt_context_state_t
    {
        S_SCAN_OBJECTS,
        S_CULL_VIEW,
        S_SPLIT,
        S_CULL_BACK,
        S_REFLECT
    };

    typedef struct rt_context_t
    {
        protected:
            typedef struct rt_edge_sort_t
            {
                rtm_edge_t         *e;          // Pointer to edge
                float               w;          // Weight of edge
            } rt_edge_sort_t;

            typedef struct rt_triangle_sort_t
            {
                rtm_triangle_t     *t;          // Pointer to triangle
                float               w;          // Weight of edge
            } rt_triangle_sort_t;

        public:
            rt_view_t                   view;       // Ray tracing point of view
            rt_context_state_t          state;      // Context state

            rt_plan_t                   plan;       // Split plan
            Allocator3D<rt_triangle_t>  triangle;   // Triangle for raytracint

            IF_RT_TRACE_Y(
                rt_debug_t                 *debug;      // Debug context
                cstorage<v_triangle3d_t>    ignored;    // List of ignored triangles (for debug)
                View3D                      trace;      // The state of the context
            )

        public:
            // Construction/destruction
            explicit rt_context_t();
            explicit rt_context_t(const rt_view_t *view);
            explicit rt_context_t(const rt_view_t *view, rt_context_state_t state);

            ~rt_context_t();

            IF_RT_TRACE_Y(
                inline void            set_debug_context(rt_debug_t *debug) { this->debug     = debug; }
            )

        public:

            /**
             * Clear context: clear underlying structures
             */
            inline void     clear()
            {
                plan.clear();
                triangle.clear();
                IF_RT_TRACE_Y(
                    ignored.clear();
                    trace.clear_all();
                )
            }

            /**
             * Flush context: clear underlying structures and release memory
             */
            inline void     flush()
            {
                plan.flush();
                triangle.flush();
                IF_RT_TRACE_Y(
                    ignored.flush();
                    trace.clear_all();
                )
            }

            /**
             * Swap internal mesh contents with another context
             * @param dst target context to perform swap
             */
            inline void     swap(rt_context_t *dst)
            {
                plan.swap(&dst->plan);
                triangle.swap(&dst->triangle);
            }

            /**
             * Fetch data for all objects identified by specified identifiers
             * @param src source context to perform fetch
             * @param n number of object identifiers in array
             * @param ids pointer to array that contains object identifiers
             * @return status of operation
             */
            status_t        fetch_objects(rt_mesh_t *src, size_t n, const size_t *ids);

            /**
             * Keep the only triangles below the specified plane
             * @param pl plane equation
             * @return status of operation
             */
            status_t        cut(const vector3d_t *pl);

            /**
             * Keep the only triangles below the specified plane or on the plane
             * @param pl plane equation
             * @return status of operation
             */
            status_t        cullback(const vector3d_t *pl);

            /**
             * Split context into two separate contexts
             * @return status of operation
             */
            status_t        split(rt_context_t *out, const vector3d_t *pl);

            /**
             * Perform depth-testing cullback of faces and remove invalid faces
             * @return status of operation
             */
            status_t        depth_test(const point3d_t *sp);

            /**
             * Add triangle to list of ignored
             * @param t triangle to add to list of ignored
             * @return status of operation
             */
            IF_RT_TRACE_Y(
                status_t        ignore(const rtm_triangle_t *t);
                status_t        ignore(const rt_triangle_t *t);
            )

    } rt_context_t;

} /* namespace mtest */

#endif /* INCLUDE_CORE_3D_RT_CONTEXT_H_ */
