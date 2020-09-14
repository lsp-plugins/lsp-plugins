/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 18 апр. 2019 г.
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
        Display        *pDisplay;
        Window          hWnd;
        GLXPbuffer      hPBuffer;
        GLXContext      hContext;
        GLXFBConfig    *pFBConfig;
        bool            bVisible;
        bool            bDrawing;
        bool            bPBuffer;

        static void     destroy(glx_backend_t *_this);
        static status_t init_window(glx_backend_t *_this, void **out_window);
        static status_t init_offscreen(glx_backend_t *_this);

        static status_t locate(glx_backend_t *_this, ssize_t left, ssize_t top, ssize_t width, ssize_t height);
        static status_t start(glx_backend_t *_this);
        static status_t set_matrix(glx_backend_t *_this, r3d_matrix_type_t type, const matrix3d_t *m);
        static status_t set_lights(glx_backend_t *_this, const r3d_light_t *lights, size_t count);
        static status_t draw_primitives(glx_backend_t *_this, const r3d_buffer_t *buffer);
        static status_t sync(glx_backend_t *_this);
        static status_t read_pixels(glx_backend_t *_this, void *buf, size_t stride, r3d_pixel_format_t format);
        static status_t finish(glx_backend_t *_this);

        explicit glx_backend_t();

    } glx_backend_t;
}

#endif /* RENDERING_GLX_BACKEND_H_ */
