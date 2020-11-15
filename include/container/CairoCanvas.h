/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 24 авг. 2016 г.
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

#ifndef CONTAINER_CAIRO_CANVAS_H_
#define CONTAINER_CAIRO_CANVAS_H_

#include <core/ICanvas.h>
#if defined(PLATFORM_WINDOWS)
    // TODO
#else
    #include <cairo/cairo.h>
#endif /* PLATFORM_WINDOWS */

namespace lsp
{
    class CairoCanvas: public ICanvas
    {
        protected:
            cairo_surface_t    *pSurface;
            cairo_t            *pCR;
            bool                bLocked;

        public:
            CairoCanvas();
            virtual ~CairoCanvas();

        protected:
            void destroy_data();
            void draw_border();

        public:
            virtual bool init(size_t width, size_t height);
            virtual void destroy();
            virtual void set_color(float r, float g, float b, float a=1.0f);
            virtual void paint();
            virtual void set_line_width(float w);
            virtual void line(float x1, float y1, float x2, float y2);
            virtual void draw_poly(float *x, float *y, size_t count, const Color &stroke, const Color &fill);
            virtual bool set_anti_aliasing(bool enable);
            virtual void draw_lines(float *x, float *y, size_t count);
            virtual void circle(ssize_t x, ssize_t y, ssize_t r);
            virtual void radial_gradient(ssize_t x, ssize_t y, const Color &c1, const Color &c2, ssize_t r);

            virtual void draw_alpha(ICanvas *s, float x, float y, float sx, float sy, float a);

            virtual void *data();
            virtual void *row(size_t row);
            virtual void *start_direct();
            virtual void end_direct();

        public:
            canvas_data_t *get_data();
    };
}

#endif /* CONTAINER_CAIRO_CANVAS_H_ */
