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

    /** Normalize vector
     *
     * @param v vector to store normalized value
     * @param src source vector to normalize
     */
    extern void (* normalize_vector2)(vector3d_t *v, const vector3d_t *src);

    /**
     * Flip vector coordinates
     * @param v vector to flip
     */
    extern void (* flip_vector_v1)(vector3d_t *v);

    /**
     * Flip vector coordinates
     * @param v vector to store result
     * @param sv source vector
     */
    extern void (* flip_vector_v2)(vector3d_t *v, const vector3d_t *sv);

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

    /** Init matrix translation
     *
     * @param m matrix
     * @param p point that defines translation
     */
    extern void (* init_matrix3d_translate_p1)(matrix3d_t *m, const point3d_t *p);

    /**
     * Init matrix translation
     * @param m matrix
     * @param v vector that defines translation
     */
    extern void (* init_matrix3d_translate_v1)(matrix3d_t *m, const vector3d_t *v);

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

    /**
     * Initialize projection matrix according to the glFrustum() specification
     * @param m target matrix to store values
     * @param left coordinates for the left vertical clipping plane
     * @param right coordinates for the right vertical clipping plane
     * @param bottom coordinates for the bottom clipping plane
     * @param top coordinates for the top clipping plane
     * @param near distance to the near clipping plane
     * @param far distance to the far clipping plane
     */
    extern void (* init_matrix3d_frustum)(matrix3d_t *m, float left, float right, float bottom, float top, float near, float far);

    /**
     * Initialize matrix similar to gluPerspective()
     * @param m target matrix to store values
     * @param pov point-of view coordinates
     * @param fwd direction of view (vector)
     * @param up the up vector
     */
    extern void (* init_matrix3d_lookat_p1v2)(matrix3d_t *m, const point3d_t *pov, const vector3d_t *fwd, const vector3d_t *up);

    /**
     * Initialize matrix similar to gluPerspective()
     * @param m target matrix to store values
     * @param pov point-of view coordinates
     * @param pod point-of-destination coordinates
     * @param up the up vector
     */
    extern void (* init_matrix3d_lookat_p2v1)(matrix3d_t *m, const point3d_t *pov, const point3d_t *pod, const vector3d_t *up);

    /**
     * Initialize matrix that changes ortogonal orientation
     * @param m matrix to initialize
     * @param orientation axis orientation
     */
    extern void (* init_matrix3d_orientation)(matrix3d_t *m, axis_orientation_t orientation);

    /**
     * Compute tranfromation matrix from point and vector data which provides:
     *   - position of the object (point)
     *   - direction of the object (vector)
     *   - scale of the object (length of vector)
     * After applying this matrix, the point with coordinates (0, 0, 1)
     * will have coordinates (p.x + v.dx, p.y + v.dy, p.z + v.dz)
     *
     * @param m target matrix
     * @param p point that indicates position of the object
     * @param v vector that indicates rotation and size of the object
     */
    extern void (* calc_matrix3d_transform_p1v1)(matrix3d_t *m, const point3d_t *p, const vector3d_t *v);

    /**
     * Compute tranfromation matrix from ray data which provides:
     *   - position of the object (point)
     *   - direction of the object (vector)
     *   - scale of the object (length of vector)
     * After applying this matrix, the point with coordinates (0, 0, 1)
     * will have coordinates (r.z.x + r.v.dx, r.z.y + r.v.dy, r.z.z + r.v.dz)
     * @param m target matrix
     * @param r ray that indicates position, rotation and size of the object
     */
    extern void (* calc_matrix3d_transform_r1)(matrix3d_t *m, const ray3d_t *r);

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

    /**
     * Add vector to point
     * @param p point
     * @param dv vector to add
     */
    extern void (* add_vector_pv1)(point3d_t *p, const vector3d_t *dv);

    /**
     * Add vector to point
     * @param p point
     * @param dv vector to add
     */
    extern void (* add_vector_pv2)(point3d_t *p, const point3d_t *sp, const vector3d_t *dv);

    /**
     * Add scaled vector to point: p = p + dv * k
     * @param p target point
     * @param dv vector to add
     * @param k scale factor
     */
    extern void (* add_vector_pvk1)(point3d_t *p, const vector3d_t *dv, float k);

    /**
     * Add scaled vector to point: p = sp + dv * k
     * @param p point
     * @param sp source point
     * @param dv vector to add
     * @param k scale factor
     */
    extern void (* add_vector_pvk2)(point3d_t *p, const point3d_t *sp, const vector3d_t *dv, float k);

    /**
     * Compute bounding box around object
     * @param b bounding box object
     * @param p array of object vertexes
     * @param n number of vertexes in object
     */
    extern void (* calc_bound_box)(bound_box3d_t *b, const point3d_t *p, size_t n);

    /**
     * Compute plane equation using three points
     * @param v pointer to store plane equation
     * @param p0 point 0
     * @param p1 point 1
     * @param p2 point 2
     * @return the length of the original normal vector
     */
    extern float (* calc_plane_p3)(vector3d_t *v, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2);

    /**
     * Compute plane equation using three points
     * @param v pointer to store plane equation
     * @param pv array of three points that lay on the plane
     * @return the length of the original normal vector
     */
    extern float (* calc_plane_pv)(vector3d_t *v, const point3d_t *pv);

    /**
     * Compute plane equation using vector and two points
     * @param v vector to store plane equation
     * @param v0 vector
     * @param p0 point 0
     * @param p1 point 1
     * @return the length of the original normal vector
     */
    extern float (* calc_plane_v1p2)(vector3d_t *v, const vector3d_t *v0, const point3d_t *p0, const point3d_t *p1);

    /**
     * Orient plane to have source point below the plane
     * @param v target plane equation vector
     * @param sp source point
     * @param pl source plane equation vector
     * @return distance from point to the plane
     */
    extern float (* orient_plane_v1p1)(vector3d_t *v, const point3d_t *sp, const vector3d_t *pl);

    /**
     * Compute plane equation using three points and set the proper direction so the orienting point is always 'below'
     * the plane
     * @param v pointer to store plane equation
     * @param sp orienting point
     * @param p0 point 0
     * @param p1 point 1
     * @param p2 point 2
     * @return the length of the original normal vector
     */
    extern float (* calc_oriented_plane_p3)(vector3d_t *v, const point3d_t *sp, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2);

    /**
     * Compute plane equation using three points and set the proper direction so the orienting point is always 'below'
     * the plane
     * @param v pointer to store plane equation
     * @param sp orienting point
     * @param pv array of three points that lay on the plane
     * @return the length of the original normal vector
     */
    extern float (* calc_oriented_plane_pv)(vector3d_t *v, const point3d_t *sp, const point3d_t *pv);

    /**
     * Compute plane equation using three points and set the proper direction so the orienting point is always 'above'
     * the plane
     * @param v pointer to store plane equation
     * @param sp orienting point
     * @param p0 point 0
     * @param p1 point 1
     * @param p2 point 2
     * @return the length of the original normal vector
     */
    extern float (* calc_rev_oriented_plane_p3)(vector3d_t *v, const point3d_t *sp, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2);

    /**
     * Compute plane equation using three points and set the proper direction so the orienting point is always 'above'
     * the plane
     * @param v pointer to store plane equation
     * @param sp orienting point
     * @param pv array of three points that lay on the plane
     * @return the length of the original normal vector
     */
    extern float (* calc_rev_oriented_plane_pv)(vector3d_t *v, const point3d_t *sp, const point3d_t *pv);

    /**
     * Compute plane equation for parallel plane that contains sp and pp points and is parallel to the line formed from p0 and p1 points
     * @param v pointer to store plane equation
     * @param sp source (projection) point
     * @param pp point that lays on the plane
     * @param p0 line point 0
     * @param p1 line point 1
     * @return the length of the original normal vector
     */
    extern float (* calc_parallel_plane_p2p2)(vector3d_t *v, const point3d_t *sp, const point3d_t *pp, const point3d_t *p0, const point3d_t *p1);

    /**
     * Estimate the area of parallelogram formed by three points
     * @param p0 point 0
     * @param p1 point 1
     * @param p2 point 2
     * @return area of parallelogram
     */
    extern float (* calc_area_p3)(const point3d_t *p0, const point3d_t *p1, const point3d_t *p2);

    /**
     * Estimate the area of parallelogram formed by three points
     * @param pv array of three points that form prarallelogram
     * @return area of parallelogram
     */
    extern float (* calc_area_pv)(const point3d_t *pv);

    /**
     * Return length of the projection of the point on the line
     * @param p0 projection line point 0
     * @param p1 projection line point 1
     * @param pp projected point
     * @return length of the projection of the point on the line
     */
    extern float (* projection_length_p2)(const point3d_t *p0, const point3d_t *p1, const point3d_t *pp);

    /**
     * Return length of the projection of the vector on another vector
     * @param v projection vector
     * @param pv projected vector
     * @return length of the projection of the vector on another vector
     */
    extern float (* projection_length_v2)(const vector3d_t *v, const vector3d_t *pv);

    /**
     * Estimate the shortest distance to triangle
     * @param sp projection point
     * @param p0 point 0 of triangle
     * @param p1 point 1 of triangle
     * @param p2 point 2 of triangle
     * @return shortest distance
     */
    extern float (* calc_min_distance_p3)(const point3d_t *sp, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2);

    /**
     * Estimate the shortest distance to triangle
     * @param sp projection point
     * @param p0 point 0 of triangle
     * @param p1 point 1 of triangle
     * @param p2 point 2 of triangle
     * @return shortest distance
     */
    extern float (* calc_min_distance_pv)(const point3d_t *sp, const point3d_t *pv);

    /**
     * Estimate the average distance to triangle
     * @param sp projection point
     * @param p0 point 0 of triangle
     * @param p1 point 1 of triangle
     * @param p2 point 2 of triangle
     * @return average distance
     */
    extern float (* calc_avg_distance_p3)(const point3d_t *sp, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2);

    /**
     * Compute distance between two pointes
     * @param p1 point 1
     * @param p2 point 2
     * @return distance
     */
    extern float (* calc_distance_p2)(const point3d_t *p1, const point3d_t *p2);

    /**
     * Compute square of distance between two points
     * @param p1 point 1
     * @param p2 point 2
     * @return square value of distance
     */
    extern float (* calc_sqr_distance_p2)(const point3d_t *p1, const point3d_t *p2);

    /**
     * Compute distance between two points
     * @param pv array of two points
     * @return distance between two points
     */
    extern float (* calc_distance_pv)(const point3d_t *pv);

    /**
     * Compute the distance (actually, the length of the vector)
     * @param v vector
     * @return distance (vector length)
     */
    extern float (* calc_distance_v1)(const vector3d_t *v);

    /**
     * Compute square of distance between two points
     * @param pv array of two points
     * @return square of distance between two points
     */
    extern float (* calc_sqr_distance_pv)(const point3d_t *pv);

    /**
     * Compute intersection point of line and plane,
     * the method is safe from providing the same pointer of ip to l0 and/or l1
     * @param ip target point to store coordinates
     * @param l0 line point 0
     * @param l1 line point 1
     * @param pl vector containing plane equation
     */
    extern void  (* calc_split_point_p2v1)(point3d_t *ip, const point3d_t *l0, const point3d_t *l1, const vector3d_t *pl);

    /**
     * Compute intersection point of line and plane,
     * the method is safe from providing the same pointer of ip to l0 and/or l1
     * @param ip target point to store coordinates
     * @param lv line points (2 elements)
     * @param pl vector containing plane equation
     */
    extern void  (* calc_split_point_pvv1)(point3d_t *ip, const point3d_t *lv, const vector3d_t *pl);

    /**
     * Split raw triangle with plane, generates output set of triangles into out (triangles above split plane)
     * and in (triangles below split plane). For every triangle, points 1 and 2 are the points that
     * lay on the split plane, the first triangle ALWAYS has 2 common points with plane (1 and 2)
     *
     * @param out array of vertexes above plane
     * @param n_out counter of triangles above plane, should be initialized
     * @param in array of vertexes below plane
     * @param n_in counter of triangles below plane, should be initialized
     * @param pl plane equation
     * @param pv triangle to perform the split
     */
    extern void  (* split_triangle_raw)(
            raw_triangle_t *out,
            size_t *n_out,
            raw_triangle_t *in,
            size_t *n_in,
            const vector3d_t *pl,
            const raw_triangle_t *pv
        );

    /**
     * Cull raw triangle with plane, generates set of triangles below the split plane.
     * For every triangle, points 1 and 2 are the points that lay on the split plane,
     * the first triangle ALWAYS has 2 common points with split plane (1 and 2)
     *
     * @param in array of vertexes below plane
     * @param n_in counter of triangles below plane, should be initialized
     * @param pl plane equation
     * @param pv triangle to perform the split
     */
    extern void  (* cull_triangle_raw)(
            raw_triangle_t *in,
            size_t *n_in,
            const vector3d_t *pl,
            const raw_triangle_t *pv
        );

    /**
     * Check colocation of two points and a plane
     * @param v vector that contains plane equation
     * @param p1 point 1
     * @param p3 point 3
     * @return bit mask: 2 groups of 2 bits, describing state of each point, proper values are:
     *   00 - if point is above the plane
     *   01 - if point is on the plane
     *   10 - if point is below the plane
     *   11 - non-permitted value, won't be produced
     *   The example state:
     *   1001 - point 0 lays above the plane, point 1 lays on the plane
     */
    extern size_t (* colocation_x2_v1p2)(const vector3d_t *v, const point3d_t *p0, const point3d_t *p1);

    /**
     * Check colocation of three points and a plane
     * @param v vector that contains plane equation
     * @param pv array of two points
     * @return bit mask: 2 groups of 2 bits, describing state of each point, proper values are:
     *   00 - if point is above the plane
     *   01 - if point is on the plane
     *   10 - if point is below the plane
     *   11 - non-permitted value, won't be produced
     *   The example state:
     *   1001 - point 0 lays above the plane, point 1 lays on the plane
     */
    extern size_t (* colocation_x2_v1pv)(const vector3d_t *v, const point3d_t *pv);

    /**
     * Check colocation of three points and a plane
     * @param v vector that contains plane equation
     * @param p0 point 0
     * @param p1 point 1
     * @param p2 point 2
     * @return bit mask: 3 groups of 2 bits, describing state of each point, proper values are:
     *   00 - if point is above the plane
     *   01 - if point is on the plane
     *   10 - if point is below the plane
     *   11 - non-permitted value, won't be produced
     *   The example state:
     *   100100 - point 0 lays above the plane, point 1 lays on the plane, point 2 lays below the plane
     */
    extern size_t (* colocation_x3_v1p3)(const vector3d_t *v, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2);

    /**
     * Check colocation of three points and a plane
     * @param v vector that contains plane equation
     * @param pv array of three points
     * @return bit mask: 3 groups of 2 bits, describing state of each point, proper values are:
     *   00 - if point is above the plane
     *   01 - if point is on the plane
     *   10 - if point is below the plane
     *   11 - non-permitted value, won't be produced
     *   The example state:
     *   100100 - point 0 lays above the plane, point 1 lays on the plane, point 2 lays below the plane
     */
    extern size_t (* colocation_x3_v1pv)(const vector3d_t *v, const point3d_t *pv);

    /**
     * Check colocation of three planes and a point
     * @param v0 plane 0
     * @param v1 plane 1
     * @param v2 plane 2
     * @param p point
     * @return bit mask: 3 groups of 2 bits, describing state of point relative to each plane, proper values are:
     *   00 - if point is above the plane
     *   01 - if point is on the plane
     *   10 - if point is below the plane
     *   11 - non-permitted value, won't be produced
     *   The example state:
     *   100100 - point lays above the plane 0, on the plane 1 and below the plane 2
     */
    extern size_t (* colocation_x3_v3p1)(const vector3d_t *v0, const vector3d_t *v1, const vector3d_t *v2, const point3d_t *p);

    /**
     * Check colocation of three planes and a point
     * @param vv array of three vectors
     * @param p point
     * @return bit mask: 3 groups of 2 bits, describing state of point relative to each plane, proper values are:
     *   00 - if point is above the plane
     *   01 - if point is on the plane
     *   10 - if point is below the plane
     *   11 - non-permitted value, won't be produced
     *   The example state:
     *   100100 - point lays above the plane 0, on the plane 1 and below the plane 2
     */
    extern size_t (* colocation_x3_vvp1)(const vector3d_t *vv, const point3d_t *p);

    /**
     * Compute unit vector from source point to center of triangle
     * @param v target to store vector
     * @param sp source point
     * @param p0 point 0
     * @param p1 point 1
     * @param p2 point 2
     */
    extern void (* unit_vector_p1p3)(vector3d_t *v, const point3d_t *sp, const point3d_t *p0, const point3d_t *p1, const point3d_t *p2);

    /**
     * Compute unit vector from source point to center of triangle
     * @param v target to store vector
     * @param sp source point
     * @param pv array of three points
     */
    extern void (* unit_vector_p1pv)(vector3d_t *v, const point3d_t *sp, const point3d_t *pv);
} // dsp

#endif /* DSP_COMMON_3DMATH_H_ */
