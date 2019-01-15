/*
 * RTContext.cpp
 *
 *  Created on: 15 янв. 2019 г.
 *      Author: sadko
 */

#include <core/3d/rt_context.h>

namespace lsp
{
    rt_context_t::rt_context_t(rt_shared_t *shared):
        vertex(256),
        edge(256),
        triangle(256)
    {
        this->state     = S_SCAN_OBJECTS;
        this->shared    = shared;
    }
    
    rt_context_t::~rt_context_t()
    {
        shared          = NULL;

        vertex.destroy();
        edge.destroy();
        triangle.destroy();
    }

    void rt_context_t::swap_mesh(rt_context_t *src)
    {
        // Do swap
        vertex.swap(&src->vertex);
        edge.swap(&src->edge);
        triangle.swap(&src->triangle);
    }

    void rt_context_t::swap_mesh_for_split(rt_context_t *src)
    {
        // Do swap
        vertex.swap(&src->vertex);
        edge.swap(&src->edge);
        triangle.swap(&src->triangle);

        // Prepare all data structures for splitting
        for (size_t i=0, n=vertex.size(); i<n; ++i)
        {
            rt_vertex_t *v  = vertex.get(i);
            v->split[0]     = NULL;
            v->split[1]     = NULL;
            v->split[2]     = NULL;
        }

        for (size_t i=0, n=edge.size(); i<n; ++i)
        {
            rt_edge_t *e    = edge.get(i);
            e->split[0]     = NULL;
            e->split[1]     = NULL;
            e->split[2]     = NULL;
            e->split[3]     = NULL;
        }
    }
} /* namespace mtest */
