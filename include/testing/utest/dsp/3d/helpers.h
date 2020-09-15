/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 30 авг. 2018 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef TEST_UTEST_3D_HELPERS_H_
#define TEST_UTEST_3D_HELPERS_H_

#include <test/helpers.h>
#include <dsp/dsp.h>

namespace test
{
    bool point3d_ck(const point3d_t *p1, const point3d_t *p2);
    bool point3d_sck(const point3d_t *p1, const point3d_t *p2);
    bool point3d_ack(const point3d_t *p1, const point3d_t *p2, float tolerance = DSP_3D_TOLERANCE);
    bool vector3d_sck(const vector3d_t *v1, const vector3d_t *v2);
    bool vector3d_ack(const vector3d_t *v1, const vector3d_t *v2, float tolerance = DSP_3D_TOLERANCE);
    bool matrix3d_ck(const matrix3d_t *m1, const matrix3d_t *m2);

    void dump_point(const char *text, const point3d_t *p);
    void dump_vector(const char *text, const vector3d_t *v);
}

#endif /* TEST_UTEST_3D_HELPERS_H_ */
