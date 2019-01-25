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
#include <core/debug.h>

#ifdef LSP_DEBUG
    #define RT_TRACE(...) \
        __VA_ARGS__

    #define RT_TRACE_BREAK(ctx, action) \
        if ((ctx->shared->breakpoint >= 0) && ((ctx->shared->step++) == ctx->shared->breakpoint)) { \
            lsp_trace("Triggered breakpoint %d\n", int(ctx->shared->breakpoint)); \
            action; \
            return STATUS_BREAKPOINT; \
        }
#else
    #define RT_TRACE(...)

    #define RT_TRACE_BREAK(ctx, action)
#endif /* LSP_DEBUG */

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
        void               *ptag;       // Pointer tag, may be used by user for any data manipulation purpose
        ssize_t             itag;       // Integer tag, may be used by user for any data manipulation purpose
    } obj_normal_t;

    typedef struct obj_vertex_t: public point3d_t
    {
        vertex_index_t      id;         // Vertex index
        obj_edge_t         *ve;         // Edge list
        void               *ptag;       // Pointer tag, may be used by user for any data manipulation purpose
        ssize_t             itag;       // Integer tag, may be used by user for any data manipulation purpose
    } obj_vertex_t;

    typedef struct obj_edge_t
    {
        edge_index_t        id;         // Edge index
        obj_vertex_t       *v[2];       // Pointers to vertexes
        obj_edge_t         *vlnk[2];    // Link to next edge for the vertex v[i]
        void               *ptag;       // Pointer tag, may be used by user for any data manipulation purpose
        ssize_t             itag;       // Integer tag, may be used by user for any data manipulation purpose
    } obj_edge_t;

    typedef struct obj_triangle_t
    {
        triangle_index_t    id;         // Triangle index
        obj_vertex_t       *v[3];       // Vertexes
        obj_edge_t         *e[3];       // Edges
        obj_normal_t       *n[3];       // Normals
        void               *ptag;       // Pointer tag, may be used by user for any data manipulation purpose
        ssize_t             itag;       // Integer tag, may be used by user for any data manipulation purpose
    } obj_triangle_t;

    typedef struct obj_boundbox_t: public bound_box3d_t
    {
    } obj_boundbox_t;

    // Raytracing structures
    struct rt_vertex_t;
    struct rt_edge_t;
    struct rt_triangle_t;

    enum edge_flags_t
    {
        RT_EF_PLANE         = 1 << 0,       // The edge is part of split plane
        RT_EF_PROCESSED     = 1 << 8,       // The edge has been processed by split algorithm

        RT_EF_TEMP          = RT_EF_PROCESSED
    };

    typedef struct rt_vertex_t: public point3d_t
    {
        rt_edge_t          *ve;         // List of linked edges
        void               *ptag;       // Pointer tag, may be used by user for any data manipulation purpose
        ssize_t             itag;       // Integer tag, may be used by user for any data manipulation purpose
    } rt_vertex_t;

    typedef struct rt_edge_t
    {
        rt_vertex_t        *v[2];       // Pointers to vertexes
        rt_triangle_t      *vt;         // List of linked triangles
        rt_edge_t          *vlnk[2];    // Link to next edge for the vertex v[i]
        void               *ptag;       // Pointer tag, may be used by user for any data manipulation purpose
        ssize_t             itag;       // Integer tag, may be used by user for any data manipulation purpose
    } rt_edge_t;

    typedef struct rt_triangle_t
    {
        rt_vertex_t        *v[3];       // Vertexes
        rt_edge_t          *e[3];       // Edges
        rt_triangle_t      *elnk[3];    // Link to next triangle for the edge e[i]
        vector3d_t          n;          // Normal
        void               *ptag;       // Pointer tag, may be used by user for any data manipulation purpose
        ssize_t             itag;       // Integer tag, may be used by user for any data manipulation purpose
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
        color3d_t   c[2];   // Color
    } v_segment3d_t;

#pragma pack(pop)

#pragma pack(push, 1)
    typedef struct raw_triangle_t
    {
        point3d_t   p[3];
    } raw_triangle_t;
#pragma pack(pop)

#ifdef LSP_DEBUG
    extern const color3d_t C_RED;
    extern const color3d_t C_GREEN;
    extern const color3d_t C_DARKGREEN;
    extern const color3d_t C_BLUE;
    extern const color3d_t C_CYAN;
    extern const color3d_t C_MAGENTA;
    extern const color3d_t C_YELLOW;
    extern const color3d_t C_ORANGE;
    extern const color3d_t C_GRAY;
#endif /* LSP_DEBUG */

}

#endif /* INCLUDE_CORE_3D_COMMON_H_ */
