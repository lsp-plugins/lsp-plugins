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
        cstorage<rt_triangle_t>   matched;    // List of matched triangles (for debug)
        cstorage<rt_triangle_t>   ignored;    // List of ignored triangles (for debug)
        ssize_t                   step;       // Trace step
        ssize_t                   breakpoint; // Trace breakpoint
        View3D                   *view;
    } rt_shared_t;

    typedef struct rt_context_t
    {
        rt_front_t                  front;          // Raytracing front
        rt_shared_t                *shared;         // Shared settings

        Allocator3D<rt_vertex_t>    vertex;     // Collection of vertexes
        Allocator3D<rt_edge_t>      edge;       // Collection of eges
        Allocator3D<rt_triangle_t>  triangle;   // Collection of triangles

        // Construction/destruction
        explicit rt_context_t(rt_shared_t *shared);
        ~rt_context_t();

        // Methods
        /**
         * Swap internal mesh contents with another context
         * @param src source context to perform swap
         */
        void swap_mesh(rt_context_t *src);

        /**
         * Swap internal contents with another context
         * and prepare data structures for splitting
         *
         * @param src source context to perform swap
         */
        void swap_mesh_for_split(rt_context_t *src);
    } rt_context_t;

} /* namespace mtest */

#endif /* INCLUDE_CORE_3D_RT_CONTEXT_H_ */
