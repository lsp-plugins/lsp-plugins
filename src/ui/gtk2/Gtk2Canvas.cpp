/*
 * Gtk2Canvas.cpp
 *
 *  Created on: 27 нояб. 2015 г.
 *      Author: sadko
 */

#include <ui/gtk2/ui.h>

namespace lsp
{
    Gtk2Canvas::Gtk2Canvas(IGraph *graph):
        IGraphCanvas(graph)
    {
        pSurface    = NULL;
        pCR         = NULL;
    }

//    inline float Gtk2Canvas::preserve(float v)
//    {
//        if (isnan(v))
//            return 0.0f;
//        else if (isinf(v))
//            return 1e+10;
//        return v;
//    }

    void Gtk2Canvas::drop_canvas()
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

    void Gtk2Canvas::resize(ssize_t width, ssize_t height, ssize_t padding)
    {
        // Check the size
        ssize_t cwidth  = width - padding * 2;
        ssize_t cheight = height - padding * 2;
        if ((pCR != NULL) && (pSurface != NULL) && (cwidth == ssize_t(nWidth)) && (cheight == ssize_t(nHeight)))
            return;

        // Drop previous canvas
        drop_canvas();

        // Create new canvas
        pSurface    = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
        if (pSurface == NULL)
            return;
        pCR         = cairo_create(pSurface);
        if (pCR == NULL)
            return;

        // Add clipping
        cairo_rectangle(pCR, 0, 0, width, height);
        cairo_clip(pCR);

        // Initialize context
        cairo_set_antialias(pCR, CAIRO_ANTIALIAS_NONE);
        cairo_translate(pCR, width >> 1, height >> 1);
        cairo_scale(pCR, 1.0, 1.0);
        cairo_set_line_join(pCR, CAIRO_LINE_JOIN_BEVEL);

        // Resize the original canvas
        IGraphCanvas::set_size(cwidth, cheight);
    }

    Gtk2Canvas::~Gtk2Canvas()
    {
        drop_canvas();
    }

    void Gtk2Canvas::draw(cairo_t *cr, ssize_t x, ssize_t y)
    {
        if (pCR == NULL)
            return;
        cairo_set_source_surface (cr, pSurface, x, y);
        cairo_paint(cr);
    }

    void Gtk2Canvas::set_line_width(size_t width)
    {
        if (pCR == NULL)
            return;
        cairo_set_line_width(pCR, width);
    }

    void Gtk2Canvas::move_to(ssize_t x, ssize_t y)
    {
        if (pCR == NULL)
            return;
        cairo_move_to(pCR, x, - y);
    }

    void Gtk2Canvas::line_to(ssize_t x, ssize_t y)
    {
        if (pCR == NULL)
            return;
        cairo_line_to(pCR, x, - y);
    }

    void Gtk2Canvas::draw_lines(float *x, float *y, size_t count)
    {
        if ((count < 2) || (pCR == NULL))
            return;

        cairo_move_to(pCR, *(x++), - *(y++));
        for (size_t i=1; i < count; ++i)
            cairo_line_to(pCR, *(x++), - *(y++));
    }

    void Gtk2Canvas::draw_poly(float *x, float *y, size_t count, const Color &stroke, const Color &fill)
    {
        if ((count < 2) || (pCR == NULL))
            return;

        cairo_move_to(pCR, *(x++), - *(y++));
        for (size_t i=1; i < count; ++i)
            cairo_line_to(pCR, *(x++), - *(y++));

        cairo_set_source_rgba(pCR, fill.red(), fill.green(), fill.blue(), 1.0 - fill.alpha());
        cairo_fill_preserve(pCR);

        cairo_set_source_rgba(pCR, stroke.red(), stroke.green(), stroke.blue(), 1.0 - stroke.alpha());
        cairo_stroke(pCR);
    }

    void Gtk2Canvas::stroke()
    {
        if (pCR == NULL)
            return;
        cairo_stroke(pCR);
    }

    void Gtk2Canvas::set_color(const Color &c)
    {
        IGraphCanvas::set_color(c);
        if (pCR == NULL)
            return;
        cairo_set_source_rgb(pCR, sColor.red(), sColor.green(), sColor.blue());
    }

    void Gtk2Canvas::set_color_rgb(float r, float g, float b)
    {
        IGraphCanvas::set_color_rgb(r, g, b);
        if (pCR == NULL)
            return;
        cairo_set_source_rgb(pCR, sColor.red(), sColor.green(), sColor.blue());
    }

    void Gtk2Canvas::circle(ssize_t x, ssize_t y, ssize_t r)
    {
        if (pCR == NULL)
            return;
        cairo_arc(pCR, x, - y, r, 0, M_PI * 2);
        cairo_fill(pCR);
    }

    void Gtk2Canvas::clear()
    {
        if (pCR == NULL)
            return;
        cairo_paint(pCR);
    }

    void Gtk2Canvas::out_text(ssize_t x, ssize_t y, float h_pos, float v_pos, float size, const char *text)
    {
        if (pCR == NULL)
            return;

        // Draw text border
        cairo_text_extents_t extents;

        cairo_select_font_face(pCR, "Sans",
              CAIRO_FONT_SLANT_NORMAL,
              CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_font_size(pCR, size);
        cairo_text_extents(pCR, text, &extents);

        float r_w = extents.x_advance - extents.x_bearing;
        float r_h = extents.y_advance - extents.y_bearing;
        float fx = x - extents.x_bearing + (r_w + 4) * 0.5 * h_pos - r_w * 0.5;
        float fy = y - extents.y_advance + (r_h + 4) * 0.5 * v_pos - r_h * 0.5;

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

    void Gtk2Canvas::radial_gradient(ssize_t x, ssize_t y, const Color &c1, const Color &c2, ssize_t r)
    {
        if (pCR == NULL)
            return;
        // Draw light
        cairo_pattern_t *cp = cairo_pattern_create_radial (x, -y, 0, x, -y, r);
        if (cp == NULL)
            return;

        cairo_pattern_add_color_stop_rgba(cp, 0.0, c1.red(), c1.green(), c1.blue(), 1.0 - c1.alpha());
        cairo_pattern_add_color_stop_rgba(cp, 1.0, c1.red(), c1.green(), c1.blue(), 1.0 - c2.alpha());
        cairo_set_source (pCR, cp);
        cairo_arc(pCR, x, -y, r, 0, 2.0 * M_PI);
        cairo_fill(pCR);
        cairo_pattern_destroy(cp);
    }

    bool Gtk2Canvas::set_anti_aliasing(bool enable)
    {
        if (pCR == NULL)
            return false;

        bool old = IGraphCanvas::set_anti_aliasing(enable);
        if (enable)
            cairo_set_antialias(pCR, CAIRO_ANTIALIAS_DEFAULT);
        else
            cairo_set_antialias(pCR, CAIRO_ANTIALIAS_NONE);

        return old;
    }
} /* namespace lsp */
