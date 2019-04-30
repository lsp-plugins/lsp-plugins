/*
 * base_backend.cpp
 *
 *  Created on: 24 апр. 2019 г.
 *      Author: sadko
 */

#include <rendering/base_backend.h>

namespace lsp
{

    void r3d_base_backend_t::init_matrix_identity(matrix3d_t *m)
    {
        ::bzero(m, sizeof(matrix3d_t));
        m->m[0]     = 1.0f;
        m->m[5]     = 1.0f;
        m->m[10]    = 1.0f;
        m->m[15]    = 1.0f;
    }

    status_t r3d_base_backend_t::init(r3d_base_backend_t *_this)
    {
        _this->viewLeft         = 0;
        _this->viewTop          = 0;
        _this->viewWidth        = 1;
        _this->viewHeight       = 1;
        _this->colBackground.r  = 0.0f;
        _this->colBackground.g  = 0.0f;
        _this->colBackground.b  = 0.0f;
        _this->colBackground.a  = 1.0f;

        init_matrix_identity(&_this->matProjection);
        init_matrix_identity(&_this->matView);
        init_matrix_identity(&_this->matWorld);

        return STATUS_OK;
    }

    void r3d_base_backend_t::destroy(r3d_base_backend_t *_this)
    {
        // Free pointer to self
        delete _this;
    }

    status_t r3d_base_backend_t::set_matrix(r3d_base_backend_t *_this, r3d_matrix_type_t type, const matrix3d_t *m)
    {
        if (m == NULL)
            return STATUS_BAD_ARGUMENTS;

        switch (type)
        {
            case R3D_MATRIX_PROJECTION: _this->matProjection    = *m;   break;
            case R3D_MATRIX_VIEW:       _this->matView          = *m;   break;
            case R3D_MATRIX_WORLD:      _this->matWorld         = *m;   break;
            default: return STATUS_INVALID_VALUE;
        }
        return STATUS_OK;
    }

    status_t r3d_base_backend_t::get_matrix(r3d_base_backend_t *_this, r3d_matrix_type_t type, matrix3d_t *m)
    {
        if (m == NULL)
            return STATUS_BAD_ARGUMENTS;

        switch (type)
        {
            case R3D_MATRIX_PROJECTION: *m  = _this->matProjection;     break;
            case R3D_MATRIX_VIEW:       *m  = _this->matView;           break;
            case R3D_MATRIX_WORLD:      *m  = _this->matWorld;          break;
            default: return STATUS_INVALID_VALUE;
        }
        return STATUS_OK;
    }

    status_t r3d_base_backend_t::get_location(r3d_base_backend_t *_this, ssize_t *left, ssize_t *top, ssize_t *width, ssize_t *height)
    {
        if (left != NULL)
            *left   = _this->viewLeft;
        if (top != NULL)
            *top    = _this->viewTop;
        if (width != NULL)
            *width  = _this->viewWidth;
        if (height != NULL)
            *height = _this->viewHeight;

        return STATUS_OK;
    }

    void r3d_base_backend_t::matrix_mul(matrix3d_t *r, const matrix3d_t *s, const matrix3d_t *m)
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

    status_t r3d_base_backend_t::set_bg_color(r3d_base_backend_t *_this, const color3d_t *color)
    {
        if (color == NULL)
            return STATUS_BAD_ARGUMENTS;

        _this->colBackground   = *color;
        return STATUS_OK;
    }

    status_t r3d_base_backend_t::get_bg_color(r3d_base_backend_t *_this, color3d_t *color)
    {
        if (color == NULL)
            return STATUS_BAD_ARGUMENTS;

        *color  = _this->colBackground;
        return STATUS_OK;
    }

    status_t r3d_base_backend_t::locate(r3d_base_backend_t *_this, ssize_t left, ssize_t top, ssize_t width, ssize_t height)
    {
        _this->viewLeft    = left;
        _this->viewTop     = top;
        _this->viewWidth   = width;
        _this->viewHeight  = height;

        return STATUS_OK;
    }

    r3d_base_backend_t::r3d_base_backend_t()
    {
        viewLeft    = 0;
        viewTop     = 0;
        viewWidth   = 0;
        viewHeight  = 0;

        // Initialize vtable
        #define R3D_BASE_BACKEND_EXP(func)   export_func(r3d_backend_t::func, &r3d_base_backend_t::func);
        R3D_BASE_BACKEND_EXP(destroy);
        R3D_BASE_BACKEND_EXP(locate);
        R3D_BASE_BACKEND_EXP(set_matrix);
        R3D_BASE_BACKEND_EXP(get_matrix);
        R3D_BASE_BACKEND_EXP(get_location);
        R3D_BASE_BACKEND_EXP(get_bg_color);
        R3D_BASE_BACKEND_EXP(set_bg_color);
        #undef R3D_BASE_BACKEND_EXP
    }
}
