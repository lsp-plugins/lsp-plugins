/*
 * cairo_canvas.h
 *
 *  Created on: 24 авг. 2016 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <core/debug.h>

#include <container/CairoCanvas.h>


namespace lsp
{
    CairoCanvas::CairoCanvas()
    {
        pSurface        = NULL;
        pCR             = NULL;
        bLocked         = false;
    }

    CairoCanvas::~CairoCanvas()
    {
        destroy_data();
    }

    void CairoCanvas::destroy_data()
    {
        if (pCR != NULL)
        {
            lsp_trace("destroy cairo=%p", pCR);
            cairo_destroy(pCR);
            pCR         = NULL;
        }
        if (pSurface != NULL)
        {
            lsp_trace("destroy surface=%p", pSurface);
            cairo_surface_destroy(pSurface);
            pSurface    = NULL;
        }
    }

    bool CairoCanvas::init(size_t width, size_t height)
    {
//        lsp_trace("initializing canvas width=%d, height=%d", int(width), int(height));

        // Check parameters
        if ((pCR == NULL) || (pSurface == NULL))
            destroy_data();
        if ((sData.nWidth != width) || (sData.nHeight != height))
        {
            if (!bLocked)
                destroy_data();
            else
            {
                width   = sData.nWidth;
                height  = sData.nHeight;
            }
        }

        // Create surface
        if (pSurface == NULL)
        {
            pSurface    = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
            if (pSurface == NULL)
                return false;
        }

        // Create cairo
        if (pCR == NULL)
        {
            pCR         = cairo_create(pSurface);
            if (pCR == NULL)
                return false;
        }

        // All seems to be OK
        sData.nWidth        = width;
        sData.nHeight       = height;
        sData.nStride       = cairo_image_surface_get_stride(pSurface);
        sData.pData         = NULL;
        bLocked             = true;     // Lock size update

        // Save state of Cairo
        cairo_save(pCR);

        // Clear surface
        cairo_set_source_rgb(pCR, 0.0f, 0.0f, 0.0f);
        cairo_paint(pCR);
        cairo_set_antialias(pCR, CAIRO_ANTIALIAS_NONE);
        cairo_set_line_join(pCR, CAIRO_LINE_JOIN_BEVEL);

        return true;
    }

    void CairoCanvas::destroy()
    {
        lsp_trace("this = %p", this);
        destroy_data();
    }

    void CairoCanvas::set_color(float r, float g, float b, float a)
    {
        if (pCR == NULL)
            return;
        cairo_set_source_rgba(pCR, r, g, b, 1.0f - a);
    }

    void CairoCanvas::paint()
    {
        if (pCR == NULL)
            return;
        cairo_paint(pCR);
    }

    void CairoCanvas::set_line_width(float w)
    {
        if (pCR == NULL)
            return;
        cairo_set_line_width(pCR, w);
    }

    void CairoCanvas::line(float x1, float y1, float x2, float y2)
    {
        if (pCR == NULL)
            return;

        cairo_move_to(pCR, x1, y1);
        cairo_line_to(pCR, x2, y2);
        cairo_stroke(pCR);
    }

    void CairoCanvas::draw_poly(float *x, float *y, size_t count, const Color &stroke, const Color &fill)
    {
        if ((count < 2) || (pCR == NULL))
            return;

        cairo_move_to(pCR, *(x++), *(y++));
        for (size_t i=1; i < count; ++i)
            cairo_line_to(pCR, *(x++), *(y++));

        cairo_set_source_rgba(pCR, fill.red(), fill.green(), fill.blue(), 1.0 - fill.alpha());
        cairo_fill_preserve(pCR);

        cairo_set_source_rgba(pCR, stroke.red(), stroke.green(), stroke.blue(), 1.0 - stroke.alpha());
        cairo_stroke(pCR);
    }

    void CairoCanvas::draw_lines(float *x, float *y, size_t count)
    {
        if ((count < 2) || (pCR == NULL))
            return;

        cairo_move_to(pCR, *(x++), *(y++));
        for (size_t i=1; i < count; ++i)
            cairo_line_to(pCR, *(x++), *(y++));
        cairo_stroke(pCR);
    }

    canvas_data_t *CairoCanvas::get_data()
    {
        if (pCR == NULL)
            return NULL;

        // Restore state
        cairo_restore(pCR);

        // Flush surface
        cairo_surface_flush(pSurface);

        // Return data
        sData.nStride       = cairo_image_surface_get_stride (pSurface);
        sData.pData         = reinterpret_cast<uint8_t *>(cairo_image_surface_get_data (pSurface));

        // Unlock size update
        bLocked             = false;

        return &sData;
    }

    bool CairoCanvas::set_anti_aliasing(bool enable)
    {
        if (pCR == NULL)
            return false;

        bool old = cairo_get_antialias(pCR) != CAIRO_ANTIALIAS_NONE;
        if (enable)
            cairo_set_antialias(pCR, CAIRO_ANTIALIAS_DEFAULT);
        else
            cairo_set_antialias(pCR, CAIRO_ANTIALIAS_NONE);

        return old;
    }

    void CairoCanvas::circle(ssize_t x, ssize_t y, ssize_t r)
    {
        if (pCR == NULL)
            return;
        cairo_arc(pCR, x, y, r, 0, M_PI * 2);
        cairo_fill(pCR);
    }

    void CairoCanvas::radial_gradient(ssize_t x, ssize_t y, const Color &c1, const Color &c2, ssize_t r)
    {
        if (pCR == NULL)
            return;
        // Draw light
        cairo_pattern_t *cp = cairo_pattern_create_radial (x, y, 0, x, y, r);
        if (cp == NULL)
            return;

        cairo_pattern_add_color_stop_rgba(cp, 0.0, c1.red(), c1.green(), c1.blue(), 1.0 - c1.alpha());
        cairo_pattern_add_color_stop_rgba(cp, 1.0, c1.red(), c1.green(), c1.blue(), 1.0 - c2.alpha());
        cairo_set_source (pCR, cp);
        cairo_arc(pCR, x, y, r, 0, 2.0 * M_PI);
        cairo_fill(pCR);
        cairo_pattern_destroy(cp);
    }

    void CairoCanvas::draw_alpha(ICanvas *s, float x, float y, float sx, float sy, float a)
    {
        if (pCR == NULL)
            return;
        CairoCanvas *cs = static_cast<CairoCanvas *>(s);
        if (cs->pSurface == NULL)
            return;

        // Draw one surface on another
        cairo_save(pCR);
        if (sx < 0.0f)
            x       -= sx * s->width();
        if (sy < 0.0f)
            y       -= sy * s->height();
        cairo_translate(pCR, x, y);
        cairo_scale(pCR, sx, sy);
        cairo_set_source_surface(pCR, cs->pSurface, 0.0f, 0.0f);
        cairo_paint_with_alpha(pCR, 1.0f - a);
        cairo_restore(pCR);
    }

    void *CairoCanvas::data()
    {
        return sData.pData;
    }

    void *CairoCanvas::row(size_t row)
    {
        return (sData.pData != NULL) ? &sData.pData[row * sData.nStride] : NULL;
    }

    void *CairoCanvas::start_direct()
    {
        if ((pCR == NULL) || (pSurface == NULL))
            return NULL;

        sData.nStride = cairo_image_surface_get_stride(pSurface);
        return sData.pData = reinterpret_cast<uint8_t *>(cairo_image_surface_get_data(pSurface));
    }

    void CairoCanvas::end_direct()
    {
        if ((pCR == NULL) || (pSurface == NULL) || (sData.pData == NULL))
            return;

        cairo_surface_mark_dirty(pSurface);
        sData.pData = NULL;
    }
}
