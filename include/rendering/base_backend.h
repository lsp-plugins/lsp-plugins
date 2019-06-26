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

namespace lsp
{
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

        static void init_matrix_identity(matrix3d_t *m);
        static void matrix_mul(matrix3d_t *r, const matrix3d_t *s, const matrix3d_t *m);

        explicit r3d_base_backend_t();

        static status_t init(r3d_base_backend_t *_this);
        static void destroy(r3d_base_backend_t *_this);

        static status_t locate(r3d_base_backend_t *_this, ssize_t left, ssize_t top, ssize_t width, ssize_t height);
        static status_t set_matrix(r3d_base_backend_t *_this, r3d_matrix_type_t type, const matrix3d_t *m);
        static status_t get_matrix(r3d_base_backend_t *_this, r3d_matrix_type_t type, matrix3d_t *m);
        static status_t set_bg_color(r3d_base_backend_t *_this, const color3d_t *color);
        static status_t get_bg_color(r3d_base_backend_t *_this, color3d_t *color);
        static status_t get_location(r3d_base_backend_t *_this, ssize_t *left, ssize_t *top, ssize_t *width, ssize_t *height);

    } r3d_base_backend_t;
}

#endif /* RENDERING_BASE_BACKEND_H_ */
