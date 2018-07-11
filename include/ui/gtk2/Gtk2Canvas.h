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

        protected:
            void            drop_canvas();

        public:
            Gtk2Canvas(IGraph *graph);
//            Gtk2Canvas(IGraph *graph, ssize_t width, ssize_t height, ssize_t padding);
            virtual ~Gtk2Canvas();

            void resize(ssize_t width, ssize_t height, ssize_t padding);

            inline bool valid() const { return pCR != NULL; };
//            static inline float preserve(float v);

        public:
            void draw(cairo_t *cr, ssize_t x, ssize_t y);

            virtual void clear();

            virtual void set_line_width(size_t width);

            virtual void move_to(ssize_t x, ssize_t y);

            virtual void line_to(ssize_t x, ssize_t y);

            virtual void draw_lines(float *x, float *y, size_t count);

            virtual void draw_poly(float *x, float *y, size_t count, const Color &stroke, const Color &fill);

            virtual void stroke();

            virtual void set_color(const Color &c);

            virtual void set_color_rgb(float r, float g, float b);

            virtual void circle(ssize_t x, ssize_t y, ssize_t r);

            virtual void out_text(ssize_t x, ssize_t y, float h_pos, float v_pos, float size, const char *text);

            virtual void radial_gradient(ssize_t x, ssize_t y, const Color &c1, const Color &c2, ssize_t r);

            virtual bool set_anti_aliasing(bool enable);
    };

} /* namespace lsp */

#endif /* GTK2CANVAS_H_ */
