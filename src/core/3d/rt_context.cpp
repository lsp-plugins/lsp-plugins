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
        this->index     = 0;
    }
    
    rt_context_t::~rt_context_t()
    {
        shared          = NULL;

        vertex.destroy();
        edge.destroy();
        triangle.destroy();
    }

    void rt_context_t::swap(rt_context_t *src)
    {
        // Do swap
        vertex.swap(&src->vertex);
        edge.swap(&src->edge);
        triangle.swap(&src->triangle);
    }

} /* namespace mtest */
