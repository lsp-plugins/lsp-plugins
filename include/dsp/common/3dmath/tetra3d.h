/*
 * tetra3d.h
 *
 *  Created on: 29 мая 2017 г.
 *      Author: sadko
 */

#ifndef DSP_COMMON_3DMATH_TETRA3D_H_
#define DSP_COMMON_3DMATH_TETRA3D_H_

namespace dsp
{
    /** Calculate tetrahedron
     *
     * @param t tetrahedron to calculate
     * @param p array of four points, the first element is a source point, three others describe rays
     */
    extern void (* calc_tetra3d_pv)(tetra3d_t *t, const point3d_t *p);

    /** Calculate tetrahedron
     *
     * @param t tetrahedron to calculate
     * @param p source point
     * @param v1 ray 1
     * @param v2 ray 2
     * @param v3 ray 3
     */
    extern void (* calc_tetra3d_pv3)(tetra3d_t *t, const point3d_t *p, const vector3d_t *v1, const vector3d_t *v2, const vector3d_t *v3);

    /** Calculate tetrahedron
     *
     * @param t tetrahedron to calculate
     * @param p source point
     * @param v array of three vectors that describe rays
     */
    extern void (* calc_tetra3d_pvv)(tetra3d_t *t, const point3d_t *p, const vector3d_t *v);

    /** Find intersection between tetrahedron and triangle
     *
     * @param r array of three rays to store the result.
     *      The v.dw component will be negative if there is no intersection, all other values are undefined
     *      0 if it lies on the intersection line of planes, all other parameters define line equations
     *      positive if there is intersection, all other parameters define line equations
     * @param t tetrahedron to test
     * @param tr triangle to test
     * @return non-negative value on success
     */
    extern float (* find_tetra3d_intersections)(ray3d_t *r, const tetra3d_t *t, const triangle3d_t *tr);
}

#endif /* DSP_COMMON_3DMATH_TETRA3D_H_ */
