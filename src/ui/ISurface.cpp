/*
 * ISurface.cpp
 *
 *  Created on: 25 окт. 2016 г.
 *      Author: sadko
 */

#include <ui/ISurface.h>

namespace lsp
{
    ISurface::ISurface(size_t width, size_t height, surface_type_t type)
    {
        nWidth      = width;
        nHeight     = height;
        nType       = type;
    }
    
    ISurface::ISurface()
    {
        nWidth      = 0;
        nHeight     = 0;
        nType       = ST_UNKNOWN;
    }
    
    ISurface::~ISurface()
    {
        nWidth      = 0;
        nHeight     = 0;
        nType       = ST_UNKNOWN;
    }

    ISurface *ISurface::create(size_t width, size_t height)
    {
        return new ISurface(width, height, ST_UNKNOWN);
    }

    void ISurface::destroy()
    {
    }

    void ISurface::draw(ISurface *s, float x, float y)
    {
    }

    void ISurface::begin()
    {
    }

    void ISurface::end()
    {
    }

    void ISurface::clear_rgb(uint32_t color)
    {
    }

    void ISurface::clear_rgba(uint32_t color)
    {
    }

} /* namespace lsp */
