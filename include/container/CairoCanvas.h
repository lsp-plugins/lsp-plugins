/*
 * cairo_canvas.h
 *
 *  Created on: 24 авг. 2016 г.
 *      Author: sadko
 */

#ifndef CONTAINER_CAIRO_CANVAS_H_
#define CONTAINER_CAIRO_CANVAS_H_

#include <core/ICanvas.h>
#include <cairo/cairo.h>

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

        public:
            canvas_data_t *get_data();
    };
}

#endif /* CONTAINER_CAIRO_CANVAS_H_ */
