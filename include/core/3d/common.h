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
    typedef ssize_t         vertex_index_t;     // Vertex index
    typedef ssize_t         normal_index_t;     // Normal index
    typedef ssize_t         edge_index_t;       // Edge index
    typedef ssize_t         triangle_index_t;   // Triangle index

    // Object structures
    struct obj_normal_t;
    struct obj_vertex_t;
    struct obj_edge_t;
    struct obj_triangle_t;
    struct obj_boundbox_t;

    typedef struct obj_normal_t: public vector3d_t
    {
        normal_index_t      id;         // Normal index
        void               *ptag;       // Pointer tag
        ssize_t             itag;       // Integer tag
    } obj_normal_t;

    typedef struct obj_vertex_t: public point3d_t
    {
        vertex_index_t      id;         // Vertex index
        obj_edge_t         *ve;         // Edge list
        void               *ptag;       // Pointer tag
        ssize_t             itag;       // Integer tag
    } obj_vertex_t;

    typedef struct obj_edge_t
    {
        edge_index_t        id;         // Edge index
        obj_vertex_t       *v[2];       // Pointers to vertexes
        obj_edge_t         *vlnk[2];    // Link to next edge for the vertex v[i]
        void               *ptag;       // Pointer tag
        ssize_t             itag;       // Integer tag
    } obj_edge_t;

    typedef struct obj_triangle_t
    {
        triangle_index_t    id;         // Triangle index
        obj_vertex_t       *v[3];       // Vertexes
        obj_edge_t         *e[3];       // Edges
        obj_normal_t       *n[3];       // Normals
        void               *ptag;       // Pointer tag
        ssize_t             itag;       // Integer tag
    } obj_triangle_t;

    typedef struct obj_boundbox_t: public bound_box3d_t
    {
    } obj_boundbox_t;

    // Raytracing structures
    struct rt_vertex_t;
    struct rt_edge_t;
    struct rt_triangle_t;

    typedef struct rt_vertex_t: public point3d_t
    {
        rt_edge_t          *ve;
        rt_vertex_t        *split[3];   // The replacement pointers
    } rt_vertex_t;

    typedef struct rt_edge_t
    {
        rt_vertex_t        *v[2];       // Pointers to vertexes
        rt_vertex_t        *vlnk[2];    // Link to next edge for the vertex v[i]
        rt_edge_t          *split[3];   // The replacement pointers
    } rt_edge_t;

    typedef struct rt_triangle_t
    {
        rt_vertex_t        *v[3];       // Vertexes
        rt_edge_t          *e[3];       // Edges
        vector3d_t          n;          // Normal
    } rt_triangle_t;

    typedef struct rt_view_t
    {
        point3d_t           s;          // Source point
        point3d_t           p[3];       // View points
    } rt_front_t;

    // Viewing structures
#pragma pack(push, 1)
    typedef struct v_vertex3d_t
    {
        point3d_t   p;      // Position
        vector3d_t  n;      // Normal
        color3d_t   c;      // Color
    } v_vertex3d_t;

    typedef struct v_triangle3d_t
    {
        point3d_t   p[3];   // Positions
        vector3d_t  n[3];   // Normals
    } v_triangle3d_t;

    typedef struct v_point3d_t
    {
        point3d_t   p;      // Position
        color3d_t   c;      // Color
    } v_point3d_t;

    typedef struct v_ray3d_t
    {
        point3d_t   p;      // Position
        vector3d_t  v;      // Direction
        color3d_t   c;      // Color
    } v_ray3d_t;

    typedef struct v_segment3d_t
    {
        point3d_t   p[2];   // Position
        color3d_t   c;      // Color
    } v_segment3d_t;
#pragma pack(pop)
}

#endif /* INCLUDE_CORE_3D_COMMON_H_ */
