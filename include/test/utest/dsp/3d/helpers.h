/*
 * helpers.h
 *
 *  Created on: 30 авг. 2018 г.
 *      Author: sadko
 */

#ifndef TEST_UTEST_3D_HELPERS_H_
#define TEST_UTEST_3D_HELPERS_H_

#include <test/helpers.h>
#include <dsp/dsp.h>

namespace test
{
    bool point3d_ck(const point3d_t *p1, const point3d_t *p2);
    bool point3d_sck(const point3d_t *p1, const point3d_t *p2);
    bool point3d_ack(const point3d_t *p1, const point3d_t *p2, float tolerance);
    bool vector3d_sck(const vector3d_t *v1, const vector3d_t *v2);
    bool vector3d_ack(const vector3d_t *v1, const vector3d_t *v2);
    bool matrix3d_ck(const matrix3d_t *m1, const matrix3d_t *m2);

    void dump_point(const char *text, const point3d_t *p);
    void dump_vector(const char *text, const vector3d_t *v);
}

#endif /* TEST_UTEST_3D_HELPERS_H_ */
