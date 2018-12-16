/*
 * ICanvas.cpp
 *
 *  Created on: 24 авг. 2016 г.
 *      Author: sadko
 */

#include <core/ICanvas.h>

namespace lsp
{
    ICanvas::ICanvas()
    {
        sData.nWidth    = 0;
        sData.nHeight   = 0;
        sData.nStride   = 0;
        sData.pData     = NULL;
    }

    ICanvas::~ICanvas()
    {
        destroy();
    }

    bool ICanvas::init(size_t width, size_t height)
    {
        return true;
    }

    void ICanvas::destroy()
    {
        sData.nWidth    = 0;
        sData.nHeight   = 0;
        sData.nStride   = 0;
        sData.pData     = NULL;
    }

    void ICanvas::set_color(float r, float g, float b, float a)
    {
    }

    void ICanvas::paint()
    {
    }

    void ICanvas::set_color(const Color &c)
    {
        set_color(c.red(), c.green(), c.blue(), c.alpha());
    }

    void ICanvas::set_color_rgb(uint32_t rgb)
    {
        set_color(
            float((rgb >> 16) & 0xff)/255.0f,
            float((rgb >> 8) & 0xff)/255.0f,
            float(rgb & 0xff)/255.0f,
            0.0f
        );
    }

    void ICanvas::set_color_rgb(uint32_t rgb, float a)
    {
        set_color(
            float((rgb >> 16) & 0xff)/255.0f,
            float((rgb >> 8) & 0xff)/255.0f,
            float(rgb & 0xff)/255.0f,
            a
        );
    }

    void ICanvas::set_color_argb(uint32_t argb)
    {
        set_color(
            float((argb >> 16) & 0xff)/255.0f,
            float((argb >> 8) & 0xff)/255.0f,
            float(argb & 0xff)/255.0f,
            float(argb >> 24)/255.0f
        );
    }

    void ICanvas::set_line_width(float w)
    {
    }

    void ICanvas::line(float x1, float y1, float x2, float y2)
    {
    }

    void ICanvas::draw_poly(float *x, float *y, size_t count, const Color &stroke, const Color &fill)
    {
    }

    bool ICanvas::set_anti_aliasing(bool enable)
    {
        return false;
    }

    void ICanvas::draw_lines(float *x, float *y, size_t count)
    {
    }

    void ICanvas::circle(ssize_t x, ssize_t y, ssize_t r)
    {
    }

    void ICanvas::radial_gradient(ssize_t x, ssize_t y, const Color &c1, const Color &c2, ssize_t r)
    {
    }

    void ICanvas::draw_alpha(ICanvas *s, float x, float y, float sx, float sy, float a)
    {
    }

    void *ICanvas::data()
    {
        return NULL;
    }

    void *ICanvas::row(size_t row)
    {
        return NULL;
    }

    void *ICanvas::start_direct()
    {
        return NULL;
    }

    void ICanvas::end_direct()
    {
    }

} /* namespace lsp */
