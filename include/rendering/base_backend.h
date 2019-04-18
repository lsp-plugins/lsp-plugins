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
