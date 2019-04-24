/*
 * backend.h
 *
 *  Created on: 18 апр. 2019 г.
 *      Author: sadko
 */

#ifndef RENDERING_GLX_BACKEND_H_
#define RENDERING_GLX_BACKEND_H_

// Standard libraries
#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>

#include <rendering/base_backend.h>

namespace lsp
{
    typedef struct glx_backend_t: public r3d_base_backend_t
    {
        Display    *pDisplay;
        Window      hParent;
        Window      hWnd;
        GLXContext  hContext;
        bool        bVisible;
        bool        bDrawing;

        static void     destroy(glx_backend_t *_this);
        static status_t init(glx_backend_t *_this, Window window, void **out_window);
        static status_t show(glx_backend_t *_this);
        static status_t hide(glx_backend_t *_this);
        static status_t locate(glx_backend_t *_this, ssize_t left, ssize_t top, ssize_t width, ssize_t height);
        static status_t start(glx_backend_t *_this);
        static status_t set_matrix(glx_backend_t *_this, r3d_matrix_type_t type, const matrix3d_t *m);
        static status_t set_lights(glx_backend_t *_this, const r3d_light_t *lights, size_t count);
        static status_t draw_primitives(glx_backend_t *_this, const r3d_buffer_t *buffer);
        static status_t finish(glx_backend_t *_this);

        explicit glx_backend_t();

    } glx_backend_t;
}

#endif /* RENDERING_GLX_BACKEND_H_ */
