/*
 * LSPSurface.cpp
 *
 *  Created on: 13 авг. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        
        LSPSurface::LSPSurface(ISurface *ws, ssize_t dx, ssize_t dy):
                 ISurface(ws->width() - dx, ws->height() - dy, ST_PROXY)
        {
            pS  = ws;
            nX  = dx;
            nY  = dy;
        }

        LSPSurface::~LSPSurface()
        {
            pS  = NULL;
        }

        ISurface *LSPSurface::create(size_t width, size_t height)
        {
            return pS->create(width, height);
        }

        IGradient *LSPSurface::linear_gradient(float x0, float y0, float x1, float y1)
        {
            return pS->linear_gradient(x0 + nX, y0 + nY, x1 + nX, y1 + nY);
        }

        IGradient *LSPSurface::radial_gradient
        (
            float cx0, float cy0, float r0,
            float cx1, float cy1, float r1
        )
        {
            return pS->radial_gradient(
                    cx0 + nX, cy0 + nY, r0,
                    cx1 + nX, cy1 + nY, r1
                );
        }

        void LSPSurface::destroy()
        {
            pS = NULL;
        }

        void LSPSurface::begin()
        {
            pS->begin();
        }

        void LSPSurface::end()
        {
            pS->end();
        }

        void LSPSurface::draw(ISurface *s, float x, float y)
        {
            pS->draw(s, x + nX, y + nY);
        }

        void LSPSurface::fill_rect(float left, float top, float width, float height, const Color &color)
        {
            pS->fill_rect(left + nX, top + nY, width, height, color);
        }

        void LSPSurface::fill_rect(float left, float top, float width, float height, IGradient *g)
        {
            pS->fill_rect(left + nX, top + nY, width, height, g);
        }

        void LSPSurface::wire_rect(float left, float top, float width, float height, float line_width, const Color &color)
        {
            pS->wire_rect(left + nX, top + nY, width, height, line_width, color);
        }

        void LSPSurface::wire_round_rect(float left, float top, float width, float height, float radius, size_t mask, float line_width, const Color &color)
        {
            pS->wire_round_rect(left + nX, top + nY, width, height, radius, mask, line_width, color);
        }

        void LSPSurface::wire_round_rect(float left, float top, float width, float height, float radius, size_t mask, float line_width, IGradient *g)
        {
            pS->wire_round_rect(left + nX, top + nY, width, height, radius, mask, line_width, g);
        }

        void LSPSurface::fill_round_rect(float left, float top, float width, float height, float radius, size_t mask, const Color &color)
        {
            pS->fill_round_rect(left + nX, top + nY, width, height, radius, mask, color);
        }

        void LSPSurface::fill_round_rect(float left, float top, float width, float height, float radius, size_t mask, IGradient *g)
        {
            pS->fill_round_rect(left + nX, top + nY, width, height, radius, mask, g);
        }

        void LSPSurface::full_rect(float left, float top, float width, float height, float line_width, const Color &color)
        {
            pS->full_rect(left + nX, top + nY, width, height, line_width, color);
        }

        void LSPSurface::fill_round_rect(float left, float top, float width, float height, float radius, const Color &color)
        {
            pS->fill_round_rect(left + nX, top + nY, width, height, radius, color);
        }

        void LSPSurface::fill_round_rect(float left, float top, float width, float height, float radius, IGradient *g)
        {
            pS->fill_round_rect(left + nX, top + nY, width, height, radius, g);
        }

        void LSPSurface::fill_sector(float cx, float cy, float radius, float angle1, float angle2, const Color &color)
        {
            pS->fill_sector(cx + nX, cy + nY, radius, angle1, angle2, color);
        }

        void LSPSurface::fill_triangle(float x0, float y0, float x1, float y1, float x2, float y2, IGradient *g)
        {
            pS->fill_triangle(x0 + nX, y0 + nY, x1 + nX, y1 + nY, x2 + nX, y2 + nY, g);
        }

        void LSPSurface::fill_triangle(float x0, float y0, float x1, float y1, float x2, float y2, const Color &color)
        {
            pS->fill_triangle(x0 + nX, y0 + nY, x1 + nX, y1 + nY, x2 + nX, y2 + nY, color);
        }

        bool LSPSurface::get_font_parameters(const Font &f, font_parameters_t *fp)
        {
            return pS->get_font_parameters(f, fp);
        }

        bool LSPSurface::get_text_parameters(const Font &f, text_parameters_t *tp, const char *text)
        {
            return pS->get_text_parameters(f, tp, text);
        }

        void LSPSurface::clear(const Color &color)
        {
            pS->fill_rect(nX, nY, nWidth, nHeight, color);
        }

        void LSPSurface::clear_rgb(uint32_t color)
        {
            Color c(color & 0xffffff);
            pS->fill_rect(nX, nY, nWidth, nHeight, color);
        }

        void LSPSurface::clear_rgba(uint32_t color)
        {
            Color c(color & 0xffffff, (color >> 24) / 255.0f);
            pS->fill_rect(nX, nY, nWidth, nHeight, color);
        }

        void LSPSurface::out_text(const Font &f, float x, float y, const char *text, const Color &color)
        {
            pS->out_text(f, x + nX, y + nY, text, color);
        }

        void LSPSurface::out_text_relative(const Font &f, float x, float y, float dx, float dy, const char *text, const Color &color)
        {
            pS->out_text_relative(f, x + nX, y + nY, dx, dy, text, color);
        }

        void LSPSurface::line(float x0, float y0, float x1, float y1, float width, const Color &color)
        {
            pS->line(x0 + nX, y0 + nY, x1 + nX, y1 + nY, width, color);
        }

        void LSPSurface::parametric_line(float a, float b, float c, float width, const Color &color)
        {
            pS->parametric_line(a, b, c - nX*a - nY*b, width, color);
        }

        void LSPSurface::parametric_line(float a, float b, float c, float left, float right, float top, float bottom, float width, const Color &color)
        {
            pS->parametric_line(a, b, c - nX*a - nY*b, left + nX, right + nX, top + nY, bottom + nY, width, color);
        }

        void LSPSurface::wire_arc(float x, float y, float r, float a1, float a2, float width, const Color &color)
        {
            pS->wire_arc(x + nX, y + nY, r, a1, a2, width, color);
        }

        void LSPSurface::fill_poly(const float *x, const float *y, size_t n, const Color & color)
        {
            float *p = new float[n * 2];
            if (p == NULL)
                return;

            float *dx = p, *dy = &p[n];
            for (size_t i=0; i<n; ++i)
                dx[i] = x[i] + nX;
            for (size_t i=0; i<n; ++i)
                dy[i] = y[i] + nY;

            pS->fill_poly(dx, dy, n, color);

            delete [] p;
        }

        void LSPSurface::wire_poly(const float *x, const float *y, size_t n, float width, const Color & color)
        {
            float *p = new float[n * 2];
            if (p == NULL)
                return;

            float *dx = p, *dy = &p[n];
            for (size_t i=0; i<n; ++i)
                dx[i] = x[i] + nX;
            for (size_t i=0; i<n; ++i)
                dy[i] = y[i] + nY;

            pS->wire_poly(dx, dy, n, width, color);

            delete [] p;
        }

        void LSPSurface::draw_poly(const float *x, const float *y, size_t n, float width, const Color &fill, const Color &wire)
        {
            float *p = new float[n * 2];
            if (p == NULL)
                return;

            float *dx = p, *dy = &p[n];
            for (size_t i=0; i<n; ++i)
                dx[i] = x[i] + nX;
            for (size_t i=0; i<n; ++i)
                dy[i] = y[i] + nY;

            pS->draw_poly(dx, dy, n, width, fill, wire);

            delete [] p;
        }

        void LSPSurface::fill_circle(float x, float y, float r, const Color & color)
        {
            pS->fill_circle(x + nX, y + nY, r, color);
        }

        void LSPSurface::fill_circle(float x, float y, float r, IGradient *g)
        {
            pS->fill_circle(x + nX, y + nY, r, g);
        }

        bool LSPSurface::get_antialiasing()
        {
            return pS->get_antialiasing();
        }

        bool LSPSurface::set_antialiasing(bool set)
        {
            return pS->set_antialiasing(set);
        }
    
    } /* namespace tk */
} /* namespace lsp */
