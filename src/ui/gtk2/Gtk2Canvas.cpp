/*
 * Gtk2Canvas.cpp
 *
 *  Created on: 27 нояб. 2015 г.
 *      Author: sadko
 */

#include <ui/gtk2/ui.h>

namespace lsp
{
    Gtk2Canvas::Gtk2Canvas(ssize_t width, ssize_t height, ssize_t padding):
        IGraphCanvas(width - padding * 2, height - padding * 2)
    {
        pSurface    = NULL;
        pCR         = NULL;
        pSurface    = cairo_image_surface_create(CAIRO_FORMAT_RGB24, width, height);
        pCR         = cairo_create(pSurface);

        cairo_set_antialias(pCR, CAIRO_ANTIALIAS_NONE);
        cairo_translate(pCR, width >> 1, height >> 1);
        cairo_scale(pCR, 1.0, -1.0);
    }

    Gtk2Canvas::~Gtk2Canvas()
    {
        if (pCR != NULL)
        {
            cairo_destroy(pCR);
            pCR         = NULL;
        }
        if (pSurface != NULL)
        {
            cairo_surface_destroy(pSurface);
            pSurface    = NULL;
        }
    }

    void Gtk2Canvas::draw(cairo_t *cr, ssize_t x, ssize_t y)
    {
        cairo_set_source_surface (cr, pSurface, x, y);
        cairo_paint(cr);
    }

    void Gtk2Canvas::set_line_width(size_t width)
    {
        cairo_set_line_width(pCR, width);
    }

    void Gtk2Canvas::move_to(ssize_t x, ssize_t y)
    {
        cairo_move_to(pCR, x, y);
    }

    void Gtk2Canvas::line_to(ssize_t x, ssize_t y)
    {
        cairo_line_to(pCR, x, y);
    }

    void Gtk2Canvas::stroke()
    {
        cairo_stroke(pCR);
    }

    void Gtk2Canvas::set_color(const Color &c)
    {
        IGraphCanvas::set_color(c);
        cairo_set_source_rgb(pCR, sColor.red(), sColor.green(), sColor.blue());
    }

    void Gtk2Canvas::clear()
    {
        cairo_paint (pCR);
    }
} /* namespace lsp */
