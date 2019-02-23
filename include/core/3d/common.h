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

#define LSP_RT_TRACE

#if defined(LSP_DEBUG) && defined(LSP_RT_TRACE)
    #define IF_RT_TRACE_Y(...)          __VA_ARGS__
    #define IF_RT_TRACE_N(...)

    #define RT_TRACE(debug, ...)        if (debug != NULL) { __VA_ARGS__ }

//    #define RT_VALIDATE(...)    __VA_ARGS__
    #define RT_VALIDATE(...)

    #define RT_TRACE_BREAK(debug, action) \
        if ((debug != NULL) && (debug->breakpoint >= 0) && ((debug->step++) == debug->breakpoint)) { \
            lsp_trace("Triggered breakpoint %d\n", int(debug->breakpoint)); \
            action; \
            return STATUS_BREAKPOINT; \
        }

    #define RT_CALL_DEBUGGER(debug, xstep, ...) \
        if ((debug != NULL) && (debug->step == xstep)) { \
            __VA_ARGS__; \
        }
#else
    #define IF_RT_TRACE_Y(...)
    #define IF_RT_TRACE_N(...)          __VA_ARGS__

    #define RT_TRACE(...)

    #define RT_TRACE_BREAK(debug, action)

    #define RT_CALL_DEBUGGER(debug, xstep, ...)
#endif /* LSP_DEBUG */

namespace lsp
{
    typedef ssize_t         vertex_index_t;     // Vertex index
    typedef ssize_t         normal_index_t;     // Normal index
    typedef ssize_t         edge_index_t;       // Edge index
    typedef ssize_t         triangle_index_t;   // Triangle index
    typedef ssize_t         face_index_t;       // Face index

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
        face_index_t        face;       // Face number
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
        RT_EF_APPLY         = 1 << 1,       // The flag that requires the edge to be applied
    };

    struct rt_vertex_t;
    struct rt_edge_t;
    struct rt_triangle_t;
    struct rt_view_t;
    struct rt_material_t;

    /**
     * Capture function
     * @param v the ray-tracing view that captured the energy
     * @param data user data
     * @return status of operation
     */
    typedef status_t (*rt_capture_t)(const rt_view_t *v, void *data);

    /**
     * Progress reporting function
     * @param progress the progress value between 0 and 1
     * @param data user data
     * @return status of operation
     */
    typedef status_t (*rt_progress_t)(float progress, void *data);

    typedef struct rt_vertex_t: public point3d_t
    {
        rt_edge_t          *ve;         // List of linked edges
        void               *ptag;       // Pointer tag, may be used by user for any data manipulation purpose
        ssize_t             itag;       // Integer tag, may be used by user for any data manipulation purpose
        ssize_t             __pad;      // Alignment to be multiple of 16
    } rt_vertex_t;

    typedef struct rt_edge_t
    {
        rt_vertex_t        *v[2];       // Pointers to vertexes
        rt_edge_t          *vlnk[2];    // Link to next edge for the vertex v[i]
        rt_triangle_t      *vt;         // List of linked triangles
        void               *ptag;       // Pointer tag, may be used by user for any data manipulation purpose
        ssize_t             itag;       // Integer tag, may be used by user for any data manipulation purpose
        ssize_t             __pad;      // Alignment to be multiple of 16
    } rt_edge_t;

    typedef struct rt_triangle_t
    {
        vector3d_t          n;          // Normal
        rt_vertex_t        *v[3];       // Vertexes
        rt_edge_t          *e[3];       // Edges
        rt_triangle_t      *elnk[3];    // Link to next triangle for the edge e[i]
        void               *ptag;       // Pointer tag, may be used by user for any data manipulation purpose
        ssize_t             itag;       // Integer tag, may be used by user for any data manipulation purpose
        ssize_t             oid;        // Object identifier
        ssize_t             face;       // Object's face identifier
        rt_material_t      *m;          // Material
        // Alignment to be multiple of 16
        __IF_32(uint32_t    __pad[2]);
    } rt_triangle_t;

    typedef struct rt_view_t
    {
        point3d_t           s;          // Source point
        point3d_t           p[3];       // View points
        ssize_t             oid;        // Object identifier
        ssize_t             face;       // Object's face to ignore
        float               time[3];    // The corresponding start time for each source point
        float               amplitude;  // The amplitude of the signal
//        float               energy;     // The energy of the wave, can have both positive and negative signs (if reflected)
        float               speed;      // This value indicates the current sound speed [m/s]
        ssize_t             rnum;       // The reflection number
    } rt_view_t;

    typedef struct rt_material_t
    {
        float           absorption[2];      // The amount of energy that will be absorpted
        float           dispersion[2];      // The dispersion coefficients for reflected signal
        float           dissipation[2];     // The dissipation coefficients for refracted signal
        float           transparency[2];    // The amount of energy that will be passed-through the material
        float           permeability;       // Sound permeability of the object (inner sound speed / outer sound speed)
    } rt_material_t;

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
