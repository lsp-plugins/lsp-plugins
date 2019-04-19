/*
 * base_backend.h
 *
 *  Created on: 18 апр. 2019 г.
 *      Author: sadko
 */

#ifndef RENDERING_BASE_BACKEND_H_
#define RENDERING_BASE_BACKEND_H_

#include <dsp/dsp.h>
#include <core/status.h>
#include <core/stdlib/string.h>
#include <rendering/backend.h>

#define R3D_BASE_BACKEND_EXP(func)   export_func(r3d_backend_t::func, &r3d_base_backend_t::func);

typedef struct r3d_base_backend_t: public r3d_backend_t
{
    matrix3d_t  matProjection;
    matrix3d_t  matView;
    matrix3d_t  matWorld;

    color3d_t   colBackground;

    ssize_t     viewLeft;
    ssize_t     viewTop;
    ssize_t     viewWidth;
    ssize_t     viewHeight;

    void        build_vtable();

    static void init_matrix_identity(matrix3d_t *m)
    {
        ::bzero(m, sizeof(matrix3d_t));
        m->m[0]     = 1.0f;
        m->m[5]     = 1.0f;
        m->m[10]    = 1.0f;
        m->m[15]    = 1.0f;
    }

    status_t init()
    {
        init_matrix_identity(&matProjection);
        init_matrix_identity(&matView);
        init_matrix_identity(&matWorld);

        return STATUS_OK;
    }

    void destroy()
    {
        // Free pointer to self
        ::free(this);
    }

    status_t set_matrix(r3d_matrix_type_t type, const float *m)
    {
        if (m == NULL)
            return STATUS_BAD_ARGUMENTS;
        const matrix3d_t *xm = reinterpret_cast<const matrix3d_t *>(m);

        switch (type)
        {
            case R3D_MATRIX_PROJECTION: matProjection   = *xm;  break;
            case R3D_MATRIX_VIEW:       matView         = *xm;  break;
            case R3D_MATRIX_WORLD:      matWorld        = *xm;  break;
            default: return STATUS_INVALID_VALUE;
        }
        return STATUS_OK;
    }

    status_t get_matrix(r3d_matrix_type_t type, float *m)
    {
        if (m == NULL)
            return STATUS_BAD_ARGUMENTS;
        matrix3d_t *xm = reinterpret_cast<matrix3d_t *>(m);

        switch (type)
        {
            case R3D_MATRIX_PROJECTION: *xm = matProjection;    break;
            case R3D_MATRIX_VIEW:       *xm = matView;          break;
            case R3D_MATRIX_WORLD:      *xm = matWorld;         break;
            default: return STATUS_INVALID_VALUE;
        }
        return STATUS_OK;
    }

    static void matrix_mul(matrix3d_t *r, const matrix3d_t *s, const matrix3d_t *m)
    {
        const float *A      = s->m;
        const float *B      = m->m;
        float *R            = r->m;

        R[0]    = A[0] * B[0]   + A[4] * B[1]   + A[8] * B[2]   + A[12] * B[3];
        R[1]    = A[1] * B[0]   + A[5] * B[1]   + A[9] * B[2]   + A[13] * B[3];
        R[2]    = A[2] * B[0]   + A[6] * B[1]   + A[10] * B[2]  + A[14] * B[3];
        R[3]    = A[3] * B[0]   + A[7] * B[1]   + A[11] * B[2]  + A[15] * B[3];

        R[4]    = A[0] * B[4]   + A[4] * B[5]   + A[8] * B[6]   + A[12] * B[7];
        R[5]    = A[1] * B[4]   + A[5] * B[5]   + A[9] * B[6]   + A[13] * B[7];
        R[6]    = A[2] * B[4]   + A[6] * B[5]   + A[10] * B[6]  + A[14] * B[7];
        R[7]    = A[3] * B[4]   + A[7] * B[5]   + A[11] * B[6]  + A[15] * B[7];

        R[8]    = A[0] * B[8]   + A[4] * B[9]   + A[8] * B[10]  + A[12] * B[11];
        R[9]    = A[1] * B[8]   + A[5] * B[9]   + A[9] * B[10]  + A[13] * B[11];
        R[10]   = A[2] * B[8]   + A[6] * B[9]   + A[10] * B[10] + A[14] * B[11];
        R[11]   = A[3] * B[8]   + A[7] * B[9]   + A[11] * B[10] + A[15] * B[11];

        R[12]   = A[0] * B[12]  + A[4] * B[13]  + A[8] * B[14]  + A[12] * B[15];
        R[13]   = A[1] * B[12]  + A[5] * B[13]  + A[9] * B[14]  + A[13] * B[15];
        R[14]   = A[2] * B[12]  + A[6] * B[13]  + A[10] * B[14] + A[14] * B[15];
        R[15]   = A[3] * B[12]  + A[7] * B[13]  + A[11] * B[14] + A[15] * B[15];
    }

    status_t set_bg_color(const color3d_t *color)
    {
        colBackground   = *color;
        return STATUS_OK;
    }
} r3d_base_backend_t;

void r3d_base_backend_t::build_vtable()
{
    R3D_BASE_BACKEND_EXP(init);
    R3D_BASE_BACKEND_EXP(destroy);
    R3D_BASE_BACKEND_EXP(set_matrix);
    R3D_BASE_BACKEND_EXP(get_matrix);
}

#undef R3D_BASE_BACKEND_EXP

#endif /* RENDERING_BASE_BACKEND_H_ */
