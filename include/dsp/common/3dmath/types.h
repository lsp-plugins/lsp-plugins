/*
 * types.h
 *
 *  Created on: 29 мая 2017 г.
 *      Author: sadko
 */

#ifndef DSP_COMMON_3DMATH_TYPES_H_
#define DSP_COMMON_3DMATH_TYPES_H_

#ifndef __DSP_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __DSP_DSP_DEFS */


#define DSP_3D_TOLERANCE        1e-5f
#define DSP_3D_SQR_TOLERANCE    0.00316227766017f
#define DSP_3D_MAXVALUE         1e+20f
#define DSP_3D_MAXISECT         8

#pragma pack(push, 1)
typedef struct point3d_t
{
    float x, y, z, w;
} point3d_t;

typedef struct vector3d_t
{
    float dx, dy, dz, dw;
} vector3d_t;

typedef struct color3d_t
{
    float r, g, b, a;
} color3d_t;

typedef struct ray3d_t
{
    point3d_t   z;          // The start point: x, y, z = point coordinates
    vector3d_t  v;          // The spread vector: dx, dy, dz = direction
} ray3d_t;

typedef struct bound_box3d_t
{
    point3d_t   p[8];       // Bounding-box contains 8 points
} bound_box3d_t;

typedef struct segment3d_t
{
    point3d_t   p[2];
} segment3d_t;

typedef struct triangle3d_t
{
    point3d_t   p[3];       // Vertexes: x, y, z = vertex coordinates, w = length of the corresponding edge
    vector3d_t  n;          // Normal: dx, dy, dz = normal coordinates, dw = plane equation coefficient
} triangle3d_t;

typedef struct matrix3d_t
{
    float       m[16];      // Matrix data
} matrix3d_t;

typedef struct raw_triangle_t
{
    point3d_t           v[3];
} raw_triangle_t;

#pragma pack(pop)

enum axis_orientation_t
{
    AO3D_POS_X_FWD_POS_Y_UP,
    AO3D_POS_X_FWD_POS_Z_UP,
    AO3D_POS_X_FWD_NEG_Y_UP,
    AO3D_POS_X_FWD_NEG_Z_UP,
    AO3D_NEG_X_FWD_POS_Y_UP,
    AO3D_NEG_X_FWD_POS_Z_UP,
    AO3D_NEG_X_FWD_NEG_Y_UP,
    AO3D_NEG_X_FWD_NEG_Z_UP,

    AO3D_POS_Y_FWD_POS_X_UP,
    AO3D_POS_Y_FWD_POS_Z_UP,
    AO3D_POS_Y_FWD_NEG_X_UP,
    AO3D_POS_Y_FWD_NEG_Z_UP,
    AO3D_NEG_Y_FWD_POS_X_UP,
    AO3D_NEG_Y_FWD_POS_Z_UP,
    AO3D_NEG_Y_FWD_NEG_X_UP,
    AO3D_NEG_Y_FWD_NEG_Z_UP,

    AO3D_POS_Z_FWD_POS_X_UP,
    AO3D_POS_Z_FWD_POS_Y_UP,
    AO3D_POS_Z_FWD_NEG_X_UP,
    AO3D_POS_Z_FWD_NEG_Y_UP,
    AO3D_NEG_Z_FWD_POS_X_UP,
    AO3D_NEG_Z_FWD_POS_Y_UP,
    AO3D_NEG_Z_FWD_NEG_X_UP,
    AO3D_NEG_Z_FWD_NEG_Y_UP
};

#endif /* DSP_COMMON_3DMATH_TYPES_H_ */
