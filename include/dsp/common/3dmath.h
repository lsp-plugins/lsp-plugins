/*
 * 3dmath.h
 *
 *  Created on: 9 апр. 2017 г.
 *      Author: sadko
 */

#ifndef DSP_COMMON_3DMATH_H_
#define DSP_COMMON_3DMATH_H_

#ifndef __DSP_DSP_DEFS
    #error "This header should not be included directly"
#endif /* __DSP_DSP_DEFS */

#include <dsp/common/3dmath/types.h>
#include <dsp/common/3dmath/tetra3d.h>

//-----------------------------------------------------------------------
// DSP 3D mathematic functions
namespace dsp
{
    /** Init point using coordinates
     *
     * @param p point to initialize
     * @param x X coordinate
     * @param y Y coordinate
     * @param z Z coordinate
     */
    extern void (* init_point_xyz)(point3d_t *p, float x, float y, float z);

    /** Init point using another point
     *
     * @param p point to initialize
     * @param s source point
     */
    extern void (* init_point)(point3d_t *p, const point3d_t *s);

    /** Normalize point coordinates
     *
     * @param p point to normalize
     */
    extern void (* normalize_point)(point3d_t *p);

    /** Normalize point coordinates to specified radius-vector length
     *
     * @param p point to scale
     * @param r radius-vector length
     */
    extern void (* scale_point1)(point3d_t *p, float r);

    /** Scale point coordinates to match specified radius-vector length
     *
     * @param p point to store result
     * @param s point to scale
     * @param r radius-vector length
     */
    extern void (* scale_point2)(point3d_t *p, const point3d_t *s, float r);

    /** Initialize vector
     *
     * @param v vector to initialize
     * @param dx delta coordinate X
     * @param dy delta coordinate Y
     * @param dz delta coordinate Y
     */
    extern void (* init_vector_dxyz)(vector3d_t *v, float dx, float dy, float dz);

    /** Initialize vector
     *
     * @param v destination vector
     * @param s source vector
     */
    extern void (* init_vector)(vector3d_t *v, const vector3d_t *s);

    /** Initialize vector
     *
     * @param v destination vector
     * @param p1 start point of vector
     * @param p2 end point of vector
     */
    extern void (* init_vector_p2)(vector3d_t *v, const point3d_t *p1, const point3d_t *p2);

    /** Initialize vector
     *
     * @param v destination vector
     * @param pv array of two points
     */
    extern void (* init_vector_pv)(vector3d_t *v, const point3d_t *pv);

    /** Initialize normal vector
     *
     * @param v vector to initialize
     * @param x1 point 1 x
     * @param y1 point 1 y
     * @param z1 point 1 z
     * @param x2 point 2 x
     * @param y2 point 2 y
     * @param z2 point 2 z
     */
    extern void (* init_normal3d_xyz)(vector3d_t *v, float x1, float y1, float z1, float x2, float y2, float z2);

    /** Initialize normal vector
     *
     * @param v vector to initialize
     * @param dx delta coordinate X
     * @param dy delta coordinate Y
     * @param dz delta coordinate Y
     */
    extern void (* init_normal3d_dxyz)(vector3d_t *v, float dx, float dy, float dz);

    /** Initialize normal vector
     *
     * @param p source vector
     * @param s destination vector
     */
    extern void (* init_normal3d)(vector3d_t *p, const vector3d_t *s);

    /** Normalize vector
     *
     * @param v vector to normalize
     */
    extern void (* normalize_vector)(vector3d_t *m);

    /** Scale vector coordinates to match specified radius-vector length
     *
     * @param v vector to scale
     * @param r radius-vector length
     */
    extern void (* scale_vector1)(vector3d_t *v, float r);

    /** Scale vector coordinates to match specified radius-vector length
     *
     * @param v vector to store result
     * @param s vector to scale
     * @param r radius-vector length
     */
    extern void (* scale_vector2)(vector3d_t *v, const vector3d_t *s, float r);

    /** Calculate vector multiplications
     *
     * @param r vector to store result
     * @param v1 vector 1
     * @param v2 vector 2
     */
    extern void (* vector_mul_v2)(vector3d_t *r, const vector3d_t *v1, const vector3d_t *v2);

    /** Calculate vector multiplications
     *
     * @param r vector to store result
     * @param vv array of two vectors to multiply
     */
    extern void (* vector_mul_vv)(vector3d_t *r, const vector3d_t *vv);

    /** Init segment using coordinates of 2 points
     *
     * @param s segment to initialize
     * @param x0 point 1 X coordinate
     * @param y0 point 1 Y coordinate
     * @param z0 point 1 Z coordinate
     * @param x1 point 2 X coordinate
     * @param y1 point 2 Y coordinate
     * @param z1 point 2 Z coordinate
     */
    extern void (* init_segment_xyz)(segment3d_t *s,
        float x0, float y0, float z0,
        float x1, float y1, float z1
    );

    /** Init segment using two points
     *
     * @param s segment to initialize
     * @param p1 point 1
     * @param p2 point 2
     */
    extern void (* init_segment_p2)(segment3d_t *s, const point3d_t *p1, const point3d_t *p2);

    /** Init segment using array of two points
     *
     * @param s segment to initialize
     * @param p array of points
     */
    extern void (* init_segment_pv)(segment3d_t *s, const point3d_t *p);

    /** Initialize matrix (make copy)
     *
     * @param dst destination matrix
     * @param src source matrix
     */
    extern void (* init_matrix3d)(matrix3d_t *dst, const matrix3d_t *src);

    /** Zero matrix
     *
     * @param m target matrix
     */
    extern void (* init_matrix3d_zero)(matrix3d_t *m);

    /** Fill matrix with ones
     *
     * @param m target matrix
     */
    extern void (* init_matrix3d_one)(matrix3d_t *m);

    /** Fill matrix with identity values
     *
     * @param m matrix
     */
    extern void (* init_matrix3d_identity)(matrix3d_t *m);

    /** Init matrix translation
     *
     * @param m matrix
     * @param dx translation X
     * @param dy translation Y
     * @param dz translation Z
     */
    extern void (* init_matrix3d_translate)(matrix3d_t *m, float dx, float dy, float dz);

    /** Init matrix scale
     *
     * @param m matrix
     * @param sx size X
     * @param sy size Y
     * @param sz size Z
     */
    extern void (* init_matrix3d_scale)(matrix3d_t *m, float sx, float sy, float sz);

    /** Initialize rotation matrix around X axis
     *
     * @param m matrix
     * @param angle angle
     */
    extern void (* init_matrix3d_rotate_x)(matrix3d_t *m, float angle);

    /** Initialize rotation matrix around Y axis
     *
     * @param m matrix
     * @param angle angle
     */
    extern void (* init_matrix3d_rotate_y)(matrix3d_t *m, float angle);

    /** Initialize rotation matrix around Z axis
     *
     * @param m matrix
     * @param angle angle
     */
    extern void (* init_matrix3d_rotate_z)(matrix3d_t *m, float angle);

    /** Initialize rotation matrix around vector
     *
     * @param m matrix
     * @param x vector X
     * @param y vector Y
     * @param z vector Z
     * @param angle angle
     */
    extern void (* init_matrix3d_rotate_xyz)(matrix3d_t *m, float x, float y, float z, float angle);

    /** Apply matrix to vector
     *
     * @param r target vector
     * @param v source vector
     * @param m matrix
     */
    extern void (* apply_matrix3d_mv2)(vector3d_t *r, const vector3d_t *v, const matrix3d_t *m);

    /** Apply matrix to vector
     *
     * @param r target vector
     * @param m matrix
     */
    extern void (* apply_matrix3d_mv1)(vector3d_t *r, const matrix3d_t *m);

    /** Apply matrix to point
     *
     * @param r target point
     * @param v source point
     * @param m matrix
     */
    extern void (* apply_matrix3d_mp2)(point3d_t *r, const point3d_t *p, const matrix3d_t *m);

    /** Apply matrix to point
     *
     * @param r target point
     * @param m matrix
     */
    extern void (* apply_matrix3d_mp1)(point3d_t *r, const matrix3d_t *m);

    /** Apply matrix to matrix (calculate matrix multiplication)
     *
     * @param r target matrix
     * @param s source matrix
     * @param m matrix
     */
    extern void (* apply_matrix3d_mm2)(matrix3d_t *r, const matrix3d_t *s, const matrix3d_t *m);

    /** Apply matrix to matrix (calculate matrix multiplication)
     *
     * @param r target matrix
     * @param m matrix
     */
    extern void (* apply_matrix3d_mm1)(matrix3d_t *r, const matrix3d_t *m);

    /** Transpose matrix
     *
     * @param r target matrix
     */
    extern void (* transpose_matrix3d1)(matrix3d_t *r);

    /** Transpose matrix
     *
     * @param r target matrix
     * @param m source matrix
     */
    extern void (* transpose_matrix3d2)(matrix3d_t *r, const matrix3d_t *m);

    /** Initialize ray using coordinates of 2 points
     *
     * @param l ray to initialize
     * @param x0 source point X coordinate
     * @param y0 source point Y coordinate
     * @param z0 source point Z coordinate
     * @param x1 destination point X coordinate
     * @param y1 destination point X coordinate
     * @param z1 destination point X coordinate
     */
    extern void (* init_ray_xyz)(ray3d_t *l, float x0, float y0, float z0, float x1, float y1, float z1);

    /** Initialize ray using coordinate of start point and direction vector
     *
     * @param l ray to initialize
     * @param x0 source point X coordinate
     * @param y0 source point Y coordinate
     * @param z0 source point Z coordinate
     * @param dx direction vector X projection
     * @param dy direction vector Y projection
     * @param dz direction vector Z projection
     */
    extern void (* init_ray_dxyz)(ray3d_t *l, float x0, float y0, float z0, float dx, float dy, float dz);

    /** Initialize ray using point and vector object
     *
     * @param l ray to initialize
     * @param p source point
     * @param v direction vector
     */
    extern void (* init_ray_pdv)(ray3d_t *l, const point3d_t *p, const vector3d_t *m);

    /** Initialize ray using two points
     *
     * @param l ray to initialize
     * @param p1 source point
     * @param p2 destination point
     */
    extern void (* init_ray_p2)(ray3d_t *l, const point3d_t *p1, const point3d_t *p2);

    /** Initialize ray using array of two points
     *
     * @param l ray to initialize
     * @param p array of two points to initialize
     */
    extern void (* init_ray_pv)(ray3d_t *l, const point3d_t *p);

    /** Initialize ray using another ray
     *
     * @param l ray to initialize
     * @param r source ray
     */
    extern void (* init_ray)(ray3d_t *l, const ray3d_t *r);

    /** Calculate ray using coordinates of 2 points
     *
     * @param l ray to initialize
     * @param x0 source point X coordinate
     * @param y0 source point Y coordinate
     * @param z0 source point Z coordinate
     * @param x1 destination point X coordinate
     * @param y1 destination point X coordinate
     * @param z1 destination point X coordinate
     */
    extern void (* calc_ray_xyz)(ray3d_t *l, float x0, float y0, float z0, float x1, float y1, float z1);

    /** Calculate ray using coordinate of start point and direction vector
     *
     * @param l ray to initialize
     * @param x0 source point X coordinate
     * @param y0 source point Y coordinate
     * @param z0 source point Z coordinate
     * @param dx direction vector X projection
     * @param dy direction vector Y projection
     * @param dz direction vector Z projection
     */
    extern void (* calc_ray_dxyz)(ray3d_t *l, float x0, float y0, float z0, float dx, float dy, float dz);

    /** Calculate ray using another ray
     *
     * @param l ray to initialize
     * @param r source ray
     */
    extern void (* calc_ray_pdv)(ray3d_t *l, const point3d_t *p, const vector3d_t *m);

    /** Calculate ray using two points
     *
     * @param l ray to initialize
     * @param p1 source point
     * @param p2 destination point
     */
    extern void (* calc_ray_p2)(ray3d_t *l, const point3d_t *p1, const point3d_t *p2);

    /** Calculate ray using array of two points
     *
     * @param l ray to initialize
     * @param p array of two points to initialize
     */
    extern void (* calc_ray_pv)(ray3d_t *l, const point3d_t *p);

    /** Calculate ray using another ray
     *
     * @param l ray to initialize
     * @param r source ray
     */
    extern void (* calc_ray)(ray3d_t *l, const ray3d_t *r);

    /** Calculate triangle normal and edge lengths
     *
     * @param t triangle
     */
    extern void (* calc_triangle3d_params)(triangle3d_t *t);

    /** Initialize triangle using coordinates of 3 points,
     *  fill normal vector with zeros
     *
     * @param t triangle to initialize
     * @param x0 point 1 X coordinate
     * @param y0 point 1 Y coordinate
     * @param z0 point 1 Z coordinate
     * @param x1 point 2 X coordinate
     * @param y1 point 2 Y coordinate
     * @param z1 point 2 Z coordinate
     * @param x2 point 3 X coordinate
     * @param y2 point 3 Y coordinate
     * @param z2 point 3 Z coordinate
     */
    extern void (* init_triangle3d_xyz)(triangle3d_t *t,
            float x0, float y0, float z0,
            float x1, float y1, float z1,
            float x2, float y2, float z2
        );

    /** Initialize triangle using 3 points,
     *  fill normal vector with zeros
     *
     * @param t triangle to initialize
     * @param p1 point 1
     * @param p2 point 2
     * @param p3 point 3
     */
    extern void (* init_triangle3d_p3)(
            triangle3d_t *t,
            const point3d_t *p1,
            const point3d_t *p2,
            const point3d_t *p3
        );

    /** Initialize triangle using array of 3 points,
     *  fill normal vector with zeros
     *
     * @param t triangle to initialize
     * @param p array of 3 points
     */
    extern void (* init_triangle3d_pv)(
            triangle3d_t *t,
            const point3d_t *p
        );

    /** Initialize triangle from another triangle
     *
     * @param dst destination triangle
     * @param src source triangle
     */
    extern void (* init_triangle3d)(triangle3d_t *dst, const triangle3d_t *src);

    /** Initialize triangle using coordinates of 3 points,
     *  calculate normal vector
     *
     * @param t triangle to initialize
     * @param x0 point 1 X coordinate
     * @param y0 point 1 Y coordinate
     * @param z0 point 1 Z coordinate
     * @param x1 point 2 X coordinate
     * @param y1 point 2 Y coordinate
     * @param z1 point 2 Z coordinate
     * @param x2 point 3 X coordinate
     * @param y2 point 3 Y coordinate
     * @param z2 point 3 Z coordinate
     */
    extern void (* calc_triangle3d_xyz)(triangle3d_t *t,
            float x0, float y0, float z0,
            float x1, float y1, float z1,
            float x2, float y2, float z2
        );

    /** Initialize triangle using 3 points,
     *  calculate normal vector
     *
     * @param t triangle to initialize
     * @param p1 point 1
     * @param p2 point 2
     * @param p3 point 3
     */
    extern void (* calc_triangle3d_p3)(
            triangle3d_t *t,
            const point3d_t *p1,
            const point3d_t *p2,
            const point3d_t *p3
        );

    /** Initialize triangle using array of 3 points,
     *  calculate normal vector
     *
     * @param t triangle to initialize
     * @param p array of 3 points
     */
    extern void (* calc_triangle3d_pv)(
            triangle3d_t *t,
            const point3d_t *p
        );

    /** Init triangle from another triangle,
     *  calculate normal vector
     *
     * @param dst destination triangle
     * @param src source triangle
     */
    extern void (* calc_triangle3d)(triangle3d_t *dst, const triangle3d_t *src);

    /** Clear intersection primitive
     *
     * @param is pointer to intersection primitive
     */
    extern void (* init_intersection3d)(intersection3d_t *is);

    /** Initialize raytrace primitive
     *
     * @param rt raytrace primitive
     * @param r source raytrace
     */
    extern void (* init_raytrace3d)(raytrace3d_t *rt, const raytrace3d_t *r);

    /** Initialize raytrace primitive
     *
     * @param rt raytrace primitive
     * @param r ray
     */
    extern void (* init_raytrace3d_r)(raytrace3d_t *rt, const ray3d_t *r);

    /** Initialize raytrace primitive
     *
     * @param rt raytrace primitive
     * @param r ray
     * @param ix last instersection
     */
    extern void (* init_raytrace3d_ix)(raytrace3d_t *rt, const ray3d_t *r, const intersection3d_t *ix);

    /** Analyze that two vectors and the normal vector organize the left triplet
     *
     * @param p1 start point of the first vector
     * @param p2 end point of the first vector, start point of the second vector
     * @param p3 end point of the second vector
     * @param n normal vector
     * @return value greater than zero if left triplet, less than zero if right triplet, zero if not triplet
     */
    extern float (* check_triplet3d_p3n)(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3, const vector3d_t *n);

    /** Analyze that two vectors and the normal vector organize the left triplet
     *
     * @param pv array of three points, each previous point is start of vector, each next point is end of vector
     * @param n normal vector
     * @return value greater than zero if left triplet, less than zero if right triplet, zero if not triplet
     */
    extern float (* check_triplet3d_pvn)(const point3d_t *pv, const vector3d_t *n);

    /** Analyze that two vectors and the normal vector organize the left triplet
     *
     * @param v1 first vector
     * @param v2 second vector
     * @param n normal vector
     * @return value greater than zero if left triplet, less than zero if right triplet, zero if not triplet
     */
    extern float (* check_triplet3d_v2n)(const vector3d_t *v1, const vector3d_t *v2, const vector3d_t *n);

    /** Analyze that two vectors and the normal vector organize the left triplet
     *
     * @param v array of two vectors
     * @param n normal vector
     * @return value greater than zero if left triplet, less than zero if right triplet, zero if not triplet
     */
    extern float (* check_triplet3d_vvn)(const vector3d_t *v, const vector3d_t *n);

    /** Analyze that three vectors organize the left triplet
     *
     * @param v array of three vectors
     * @param n normal vector
     * @return value greater than zero if left triplet, less than zero if right triplet, zero if not triplet
     */
    extern float (* check_triplet3d_vv)(const vector3d_t *v);

    /** Analyze that triangle vectors and normal vector of triangle organize the left triplet
     *
     * @param t triganle
     * @return value greater than zero if left triplet, less than zero if right triplet, zero if not triplet
     */
    extern float (* check_triplet3d_t)(const triangle3d_t *t);

    /** Analyze that triangle vectors and normal vector organize the left triplet
     *
     * @param t triganle
     * @param n normal vector
     * @return value greater than zero if left triplet, less than zero if right triplet, zero if not triplet
     */
    extern float (* check_triplet3d_tn)(const triangle3d_t *t, const vector3d_t *n);

    /** Analyze point location relative to the triangle
     *
     * @param t triangle
     * @param p point
     * @return value > 0 if point is candidate to be inside the triangle,
     *         value < 0 if point is candidate to be outside the triangle,
     *         value = 0 if point is on the edge of triangle
     */
    extern float (* check_point3d_on_triangle_tp)(const triangle3d_t *t, const point3d_t *p);

    /** Analyze point location relative to the triangle of three points
     *
     * @param t array of three triangle points
     * @param p point
     * @return value > 0 if point is candidate to be inside the triangle,
     *         value < 0 if point is candidate to be outside the triangle,
     *         value = 0 if point is on the edge of triangle
     */
    extern float (* check_point3d_on_triangle_pvp)(const point3d_t *t, const point3d_t *p);

    /** Analyze point location relative to the triangle of three points
     *
     * @param p1 triangle point 1
     * @param p2 triangle point 2
     * @param p3 triangle point 3
     * @param p point
     * @return value > 0 if point is candidate to be inside the triangle,
     *         value < 0 if point is candidate to be outside the triangle,
     *         value = 0 if point is on the edge of triangle
     */
    extern float (* check_point3d_on_triangle_p3p)(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3, const point3d_t *p);

    /** Check that point lies on the edge
     *
     * @param p1 edge point 1
     * @param p2 edge point 2
     * @param p point to check
     * @return value >= 0 if point lies within edge
     */
    extern float (* check_point3d_on_edge_p2p)(const point3d_t *p1, const point3d_t *p2, const point3d_t *p);

    /** Check that point lies on the edge
     *
     * @param p array of two edges
     * @param p point to check
     * @return value >= 0 if point lies within edge
     */
    extern float (* check_point3d_on_edge_pvp)(const point3d_t *pv, const point3d_t *p);

    /** Return the index of longest edge between three points
     *
     * @param p1 point 1
     * @param p2 point 2
     * @param p3 point 3
     * @return 0 if edge between points 1 and 2 is longest, 1 if between points 2 and 3, 2 if between ponts 3 and 1
     */
    extern size_t (* longest_edge3d_p3)(const point3d_t *p1, const point3d_t *p2, const point3d_t *p3);

    /** Return the index of longest edge between three points
     *
     * @param p array of points
     * @return 0 if edge between points 0 and 1 is longest, 1 if between points 1 and 2, 2 if between ponts 2 and 0
     */
    extern size_t (* longest_edge3d_pv)(const point3d_t *p);

    /** Find intersection of ray and triangle
     *
     * @param ip intersection point to store result
     * @param l ray to test intersection
     * @param t pre-calculated triangle to check (with plane equation)
     * @return actual distance between ray start point and intersection point.
     *         If value is less than zero, then there is no intersection
     */
    extern float (* find_intersection3d_rt)(point3d_t *ip, const ray3d_t *l, const triangle3d_t *t);

    /** Reflect the ray from the surface specified by the intersection and update raytrace object
     *
     * @param rt raytrace object, will contain the information about reflected raytrace after call
     * @param rf ray object, will contain the information about refracted raytrace after call
     * @param ix intersection primitive
     */
    extern void (* reflect_ray)(raytrace3d_t *rt, raytrace3d_t *rf, const intersection3d_t *ix);

    /** Calculate angle between two vectors
     *
     * @param v1 vector 1
     * @param v2 vector 2
     * @return cosine of angle between two vectors [-1..1]
     */
    extern float (* calc_angle3d_v2)(const vector3d_t *v1, const vector3d_t *v2);

    /** Calculate angle between two vectors
     *
     * @param v array of two vectors
     * @return cosine of angle between two vectors [-1..1]
     */
    extern float (* calc_angle3d_vv)(const vector3d_t *v);

    /** Calculate normal for triangle described by three points
     *
     * @param n normal
     * @param p1 point 1
     * @param p2 point 2
     * @param p3 point 3
     */
    extern void (* calc_normal3d_p3)(vector3d_t *n, const point3d_t *p1, const point3d_t *p2, const point3d_t *p3);

    /** Calculate normal for triangle described by array of three points
     *
     * @param n normal
     * @param pv array of three points
     */
    extern void (* calc_normal3d_pv)(vector3d_t *n, const point3d_t *pv);

    /** Calculate normal for triangle described by two vectors
     *
     * @param n normal
     * @param v1 vector 1
     * @param v2 vector 2
     */
    extern void (* calc_normal3d_v2)(vector3d_t *n, const vector3d_t *v1, const vector3d_t *v2);

    /** Calculate normal for triangle described by array two vectors
     *
     * @param n normal
     * @param vv array of two vectors
     */
    extern void (* calc_normal3d_vv)(vector3d_t *n, const vector3d_t *vv);

    /** Move point between two other points: p = p1 + (p2 - p1) * k
     *
     * @param p target to store result
     * @param p1 point 1
     * @param p2 point 2
     * @param k movement
     */
    extern void (* move_point3d_p2)(point3d_t *p, const point3d_t *p1, const point3d_t *p2, float k);

    /** Move point between two other points: p = p1 + (p2 - p1) * k
     *
     * @param p point to move
     * @param pv array of two points
     * @param k movement
     */
    extern void (* move_point3d_pv)(point3d_t *p, const point3d_t *pv, float k);

    /** Initialize octant
     *
     * @param o octant to initialize
     * @param t list of points to analyze bounds
     * @param n number of points
     */
    extern void (* init_octant3d_v)(octant3d_t *o, const point3d_t *t, size_t n);

    /** Check that
     *
     * @param o octant to check
     * @param r ray to check
     * @return true if intersection of ray with object in octant is possible
     */
    extern bool (* check_octant3d_rv)(const octant3d_t *o, const ray3d_t *r);
} // dsp

#endif /* DSP_COMMON_3DMATH_H_ */
