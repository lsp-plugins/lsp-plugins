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

//        lsp_trace("drawing final border");

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
}
