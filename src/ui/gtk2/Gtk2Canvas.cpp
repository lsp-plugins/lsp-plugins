/*
 * Gtk2Canvas.cpp
 *
 *  Created on: 27 нояб. 2015 г.
 *      Author: sadko
 */

#include <ui/gtk2/ui.h>

namespace lsp
{
    Gtk2Canvas::Gtk2Canvas(IGraph *graph, ssize_t width, ssize_t height, ssize_t padding):
        IGraphCanvas(graph, width - padding * 2, height - padding * 2)
    {
        pSurface    = NULL;
        pCR         = NULL;
        pSurface    = cairo_image_surface_create(CAIRO_FORMAT_RGB24, width, height);
        pCR         = cairo_create(pSurface);

        cairo_set_antialias(pCR, CAIRO_ANTIALIAS_NONE);
        cairo_translate(pCR, width >> 1, height >> 1);
        cairo_scale(pCR, 1.0, 1.0);
        cairo_set_line_join(pCR, CAIRO_LINE_JOIN_BEVEL);
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
        cairo_move_to(pCR, x, - y);
    }

    void Gtk2Canvas::line_to(ssize_t x, ssize_t y)
    {
        cairo_line_to(pCR, x, - y);
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

    void Gtk2Canvas::circle(ssize_t x, ssize_t y, ssize_t r)
    {
        cairo_arc(pCR, x, - y, r, 0, M_PI * 2);
        cairo_fill(pCR);
    }

    void Gtk2Canvas::clear()
    {
        cairo_paint (pCR);
    }

    void Gtk2Canvas::out_text(ssize_t x, ssize_t y, float h_pos, float v_pos, float size, const char *text)
    {
        // Draw text border
        cairo_text_extents_t extents;

        cairo_select_font_face(pCR, "Sans",
              CAIRO_FONT_SLANT_NORMAL,
              CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_font_size(pCR, size);
        cairo_text_extents(pCR, text, &extents);

//        cairo_arc(pCR, x, - y, 2, 0, M_PI * 2);
//        cairo_fill(pCR);

        float r_w = extents.x_advance - extents.x_bearing;
        float r_h = extents.y_advance - extents.y_bearing;
        float fx = x - extents.x_bearing + (r_w + 4) * 0.5 * h_pos - r_w * 0.5;// + (h_pos) * r_w;
        float fy = y - extents.y_advance + (r_h + 4) * 0.5 * v_pos - r_h * 0.5;// + (v_pos) * r_h;

        cairo_move_to(pCR, fx, -fy);
        cairo_show_text (pCR, text);
//
//        cairo_set_line_width(pCR, 1);
//        cairo_set_source_rgb(pCR, 1.0, 0, 0);
//
//        cairo_move_to(pCR, fx + extents.x_bearing, -fy - 10);
//        cairo_line_to(pCR, fx + extents.x_bearing, -fy + 10);
//        cairo_stroke(pCR);
//
//        cairo_move_to(pCR, fx + extents.x_advance, -fy - 10);
//        cairo_line_to(pCR, fx + extents.x_advance, -fy + 10);
//        cairo_stroke(pCR);
//
//        cairo_move_to(pCR, fx - 10, -fy + extents.y_bearing);
//        cairo_line_to(pCR, fx + 10, -fy + extents.y_bearing);
//        cairo_stroke(pCR);
//
//        cairo_move_to(pCR, fx - 10, -fy + extents.y_advance);
//        cairo_line_to(pCR, fx + 10, -fy + extents.y_advance);
//        cairo_stroke(pCR);
//
//        cairo_move_to(pCR, fx - 10, -fy - extents.y_advance);
//        cairo_line_to(pCR, fx + 10, -fy - extents.y_advance);
//        cairo_stroke(pCR);
    }
} /* namespace lsp */
