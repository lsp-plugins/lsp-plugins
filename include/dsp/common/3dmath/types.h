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

typedef struct ray3d_t
{
    point3d_t   z;          // The start point: x, y, z = point coordinates
    vector3d_t  v;          // The spread vector: dx, dy, dz = direction
} ray3d_t;

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

typedef struct material3d_t
{
    float       speed;          // The sound speed in the space relative to the normal speed of sound in air
    float       damping;        // Power damping, the amount of energy lost when interacting with barrier
    float       absorption;     // The absorption made by material
    float       transparency;   // Material transparency, the amount of energy passed through the barrier
    float       reflection;     // The refraction coefficient of the wave reflected from the barrier
    float       refraction;     // The refraction coefficient of the wave passed through the barrier
    float       diffuse;        // Material diffuse, the amount of energy spread into different directions
    float       __pad[1];       // Padding
} material3d_t;

typedef struct intersection3d_t
{
    point3d_t               p;          // Intersection point
    const triangle3d_t     *t[DSP_3D_MAXISECT];     // Triangle
    const material3d_t     *m[DSP_3D_MAXISECT];     // Material
    size_t                  n;          // Number of records in t and m arrays
    uint8_t                 __pad[DEFAULT_ALIGN - sizeof(size_t)]; // padding
} intersection3d_t;

typedef struct octant3d_t
{
    point3d_t           min;            // Minimum coordinates
    point3d_t           max;            // Maximum coordinates
    point3d_t           bounds[8];      // 8 bounds for checking
} octant3d_t;

typedef struct raytrace3d_t
{
    ray3d_t             r;              // Ray
    float               amplitude;      // amplitude of the signal
    float               delay;          // delay of the signal
    float             __pad[2];         // padding
    intersection3d_t    x;              // Last intersection of the ray
} raytrace3d_t;

typedef struct tetra3d_t
{
    point3d_t           s;              // Source point
    vector3d_t          r[3];           // Three rays coming from source point, counter-clockwise order
    vector3d_t          n[3];           // Three normals for planes formed by three rays
} tetra3d_t;

typedef uint32_t        vertex_index_t;

#pragma pack(pop)

#endif /* DSP_COMMON_3DMATH_TYPES_H_ */
