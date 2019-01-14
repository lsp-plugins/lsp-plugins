/*
 * common.h
 *
 *  Created on: 14 янв. 2019 г.
 *      Author: sadko
 */

#ifndef INCLUDE_CORE_3D_COMMON_H_
#define INCLUDE_CORE_3D_COMMON_H_

#include <core/types.h>

namespace lsp
{
    typedef ssize_t         vertex_index_t; // Vertex index
    typedef ssize_t         normal_index_t; // Normal index
    typedef ssize_t         edge_index_t;   // Edge index

    typedef struct obj_edge_t
    {
        vertex_index_t  vi[2];      // Index of start and end vertex
    } obj_edge_t;

    typedef struct obj_triangle_t
    {
        vertex_index_t  v[3];       // Vertex indexes
        normal_index_t  n[3];       // Normal indexes
        edge_index_t    e[3];       // Edge indexes
    } obj_triangle_t;
}

#endif /* INCLUDE_CORE_3D_COMMON_H_ */
