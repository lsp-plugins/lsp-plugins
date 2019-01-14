/*
 * common.h
 *
 *  Created on: 14 янв. 2019 г.
 *      Author: sadko
 */

#ifndef INCLUDE_CORE_3D_COMMON_H_
#define INCLUDE_CORE_3D_COMMON_H_

#include <core/types.h>
#include <dsp/dsp.h>

namespace lsp
{
    typedef ssize_t         vertex_index_t; // Vertex index
    typedef ssize_t         normal_index_t; // Normal index
    typedef ssize_t         edge_index_t;   // Edge index

    struct obj_normal_t;
    struct obj_vertex_t;
    struct obj_edge_t;
    struct obj_triangle_t;

    typedef struct obj_normal_t: public vector3d_t
    {
    } obj_normal_t;

    typedef struct obj_vertex_t: public point3d_t
    {
        obj_edge_t         *ve;         // Edge list
    } obj_vertex_t;

    typedef struct obj_edge_t
    {
        obj_vertex_t       *v[2];       // Pointers to vertexes
        obj_edge_t         *vlnk[2];    // Link to next edge for the vertex v[i]
    } obj_edge_t;

    typedef struct obj_triangle_t
    {
        obj_vertex_t       *v[3];       // Vertexes
        obj_edge_t         *e[3];       // Edges
        obj_normal_t       *n[3];       // Normals
    } obj_triangle_t;
}

#endif /* INCLUDE_CORE_3D_COMMON_H_ */
