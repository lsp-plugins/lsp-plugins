/*
 * ICanvas.h
 *
 *  Created on: 24 авг. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_ICANVAS_H_
#define CORE_ICANVAS_H_

#include <core/types.h>
#include <core/util/Color.h>

namespace lsp
{
    typedef struct canvas_data_t
    {
        size_t      nWidth;         // Width in pixels
        size_t      nHeight;        // Height in pixels
        size_t      nStride;        // Stride in bytes
        uint8_t    *pData;          // ARGB32 data, 4 bytes per pixel
    } canvas_data_t;
    
    class ICanvas
    {
        protected:
            canvas_data_t       sData;

        public:
            ICanvas();
            virtual ~ICanvas();

        public:
            /** Get canvas width
             *
             * @return canvas width
             */
            inline size_t width() const     { return sData.nWidth;      }

            /** Get canvas height
             *
             * @return canvas height
             */
            inline size_t height() const    { return sData.nHeight;     }

            /** Return difference (in bytes) between two sequential rows
             *
             * @return stride between rows
             */
            inline size_t stride() const    { return sData.nStride;     }

        public:
            /** Initialize canvas
             *
             * @param width canvas width
             * @param height canvas height
             */
            virtual bool init(size_t width, size_t height);

            /** Destroy canvas
             *
             *
             */
            virtual void destroy();

            /** Set current color
             *
             * @param r red
             * @param g green
             * @param b blue
             * @param a alpha
             */
            virtual void set_color(float r, float g, float b, float a=1.0f);

            /** Set current color
             *
             * @param rgb RGB value
             */
            virtual void set_color_rgb(uint32_t rgb);

            /** Set current color
             *
             * @param rgb RGB value
             * @param a alpha value
             */
            virtual void set_color_rgb(uint32_t rgb, float a);

            /** Set current color
             *
             * @param argb ARGB value
             */
            virtual void set_color_argb(uint32_t argb);

            /** Set current color
             *
             * @param c color objet
             */
            virtual void set_color(const Color &c);

            /** Set line width
             *
             * @param w width
             */
            virtual void set_line_width(float w);

            /** Draw line
             *
             * @param x1 point 1 x coordinate
             * @param y1 point 1 y coordinate
             * @param x2 point 2 x coordinate
             * @param y2 point 2 y coordinate
             */
            virtual void line(float x1, float y1, float x2, float y2);

            /** Draw poly
             *
             * @param x x coordinates
             * @param y y coordinates
             * @param count number of dots
             * @param stroke stroke color
             * @param fill fill color
             */
            virtual void draw_poly(float *x, float *y, size_t count, const Color &stroke, const Color &fill);

            /** Perform painting
             *
             */
            virtual void paint();

            /** Enable/disable anti-aliasing
             *
             * @param enable flag that enables anti-aliasing
             * @return previous anti-aliasing state
             */
            virtual bool set_anti_aliasing(bool enable);

            /** Draw lines
             *
             * @param x x coordinates
             * @param y y coordinates
             * @param count number of dots
             */
            virtual void draw_lines(float *x, float *y, size_t count);

            /** Draw circle
             *
             * @param x circle center x
             * @param y circle center y
             * @param r radius
             */
            virtual void circle(ssize_t x, ssize_t y, ssize_t r);

            /** Draw radial gradient
             *
             * @param x center of gradient x
             * @param y center of gradient y
             * @param c1 color 1
             * @param c2 color 2
             * @param r radius of gradient
             */
            virtual void radial_gradient(ssize_t x, ssize_t y, const Color &c1, const Color &c2, ssize_t r);

            /**
             * Draw another surface with applied alpha channel
             * @param s source surface to draw
             * @param x x-axis position
             * @param y y-axis poisition
             * @param sx x-axis scale
             * @param sy y-axis scale
             * @param a alpha
             */
            virtual void draw_alpha(ICanvas *s, float x, float y, float sx, float sy, float a);

            /**
             * Return raw buffer data for direct rendering
             *
             * @return raw buffer data
             */
            virtual void *data();

            /**
             * Return pointer to the beginning of the specified row
             * @param row row number
             */
            virtual void *row(size_t row);

            /**
             * Start direct access to the surface
             * @return pointer to surface buffer or NULL if error/not possible
             */
            virtual void *start_direct();

            /**
             * End direct access to the surface
             */
            virtual void end_direct();
    };

} /* namespace lsp */

#endif /* CORE_ICANVAS_H_ */
