/*
 * IGraphCanvas.cpp
 *
 *  Created on: 27 нояб. 2015 г.
 *      Author: sadko
 */

#include <ui/ui.h>

namespace lsp
{
    IGraphCanvas::IGraphCanvas(ssize_t width, ssize_t height)
    {
        nWidth      = width;
        nHeight     = height;
        nLeft       = - (nWidth >> 1);
        nTop        = (nHeight >> 1);
        sColor.set_rgb(1.0, 1.0, 1.0);
    }

    IGraphCanvas::~IGraphCanvas()
    {
    }

    void IGraphCanvas::line(ssize_t x1, ssize_t y1, ssize_t x2, ssize_t y2)
    {
        move_to(x1, y1);
        line_to(x2, y2);
        stroke();
    }

    void IGraphCanvas::move_to(ssize_t x, ssize_t y)
    {
    }

    void IGraphCanvas::line_to(ssize_t x, ssize_t y)
    {
    }

    void IGraphCanvas::stroke()
    {
    }

    void IGraphCanvas::set_color(const Color &c)
    {
        sColor.copy(c);
    }

    void IGraphCanvas::set_line_width(size_t width)
    {
    }

    void IGraphCanvas::clear()
    {
    }

} /* namespace lsp */
