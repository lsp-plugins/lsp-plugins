/*
 * LSPSurface.h
 *
 *  Created on: 13 авг. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_UTIL_LSPSURFACE_H_
#define UI_TK_UTIL_LSPSURFACE_H_

namespace lsp
{
    namespace tk
    {
        class LSPSurface: public ISurface
        {
            protected:
                ws::ISurface       *pS;
                ssize_t             nX;
                ssize_t             nY;

            public:
                explicit LSPSurface(ISurface *ws, ssize_t dx = 0, ssize_t dy = 0);
                virtual ~LSPSurface();

            public:
                virtual ISurface *create(size_t width, size_t height);

                virtual IGradient *linear_gradient(float x0, float y0, float x1, float y1);

                virtual IGradient *radial_gradient
                (
                    float cx0, float cy0, float r0,
                    float cx1, float cy1, float r1
                );

                virtual void destroy();

                virtual void begin();

                virtual void end();

            public:
                virtual void draw(ISurface *s, float x, float y);

                virtual void fill_rect(float left, float top, float width, float height, const Color &color);

                virtual void fill_rect(float left, float top, float width, float height, IGradient *g);

                virtual void wire_rect(float left, float top, float width, float height, float line_width, const Color &color);

                virtual void wire_round_rect(float left, float top, float width, float height, float radius, size_t mask, float line_width, const Color &color);

                virtual void wire_round_rect(float left, float top, float width, float height, float radius, size_t mask, float line_width, IGradient *g);

                virtual void fill_round_rect(float left, float top, float width, float height, float radius, size_t mask, const Color &color);

                virtual void fill_round_rect(float left, float top, float width, float height, float radius, size_t mask, IGradient *g);

                virtual void full_rect(float left, float top, float width, float height, float line_width, const Color &color);

                virtual void fill_round_rect(float left, float top, float width, float height, float radius, const Color &color);

                virtual void fill_round_rect(float left, float top, float width, float height, float radius, IGradient *g);

                virtual void fill_sector(float cx, float cy, float radius, float angle1, float angle2, const Color &color);

                virtual void fill_triangle(float x0, float y0, float x1, float y1, float x2, float y2, IGradient *g);

                virtual void fill_triangle(float x0, float y0, float x1, float y1, float x2, float y2, const Color &color);

                virtual bool get_font_parameters(const Font &f, font_parameters_t *fp);

                virtual bool get_text_parameters(const Font &f, text_parameters_t *tp, const char *text);

                virtual void clear(const Color &color);

                virtual void clear_rgb(uint32_t color);

                virtual void clear_rgba(uint32_t color);

                virtual void out_text(const Font &f, float x, float y, const char *text, const Color &color);

                virtual void out_text_relative(const Font &f, float x, float y, float dx, float dy, const char *text, const Color &color);

                virtual void line(float x0, float y0, float x1, float y1, float width, const Color &color);

                virtual void parametric_line(float a, float b, float c, float width, const Color &color);

                virtual void parametric_line(float a, float b, float c, float left, float right, float top, float bottom, float width, const Color &color);

                virtual void wire_arc(float x, float y, float r, float a1, float a2, float width, const Color &color);

                virtual void fill_poly(const float *x, const float *y, size_t n, const Color & color);

                virtual void wire_poly(const float *x, const float *y, size_t n, float width, const Color & color);

                virtual void draw_poly(const float *x, const float *y, size_t n, float width, const Color &fill, const Color &wire);

                virtual void fill_circle(float x, float y, float r, const Color & color);

                virtual void fill_circle(float x, float y, float r, IGradient *g);

                virtual bool get_antialiasing();

                virtual bool set_antialiasing(bool set);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_UTIL_LSPSURFACE_H_ */
