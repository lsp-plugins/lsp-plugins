/*
 * Gtk2Canvas.h
 *
 *  Created on: 27 нояб. 2015 г.
 *      Author: sadko
 */

#ifndef GTK2CANVAS_H_
#define GTK2CANVAS_H_

namespace lsp
{
    
    class Gtk2Canvas: public IGraphCanvas
    {
        private:
            cairo_surface_t        *pSurface;
            cairo_t                *pCR;

        public:
            Gtk2Canvas(ssize_t width, ssize_t height, ssize_t padding);
            virtual ~Gtk2Canvas();

        public:
            void draw(cairo_t *cr, ssize_t x, ssize_t y);

            virtual void clear();

            virtual void set_line_width(size_t width);

            virtual void move_to(ssize_t x, ssize_t y);

            virtual void line_to(ssize_t x, ssize_t y);

            virtual void stroke();

            virtual void set_color(const Color &c);
    };

} /* namespace lsp */

#endif /* GTK2CANVAS_H_ */
