/*
 * X11CairoSurface.cpp
 *
 *  Created on: 25 окт. 2016 г.
 *      Author: sadko
 */

#include <ui/x11/ui.h>
#include <cairo-xlib.h>

namespace lsp
{
    namespace x11ui
    {
        X11CairoSurface::X11CairoSurface(Display *dpy, Drawable drawable, Visual *visual, size_t width, size_t height):
            ISurface(width, height, ST_XLIB)
        {
            pSurface        = cairo_xlib_surface_create(dpy, drawable, visual, width, height);
            if (pSurface == NULL)
                return;
            pCR             = cairo_create(pSurface);
            if (pCR == NULL)
                return;
            bBegin          = false;
        }
        
        X11CairoSurface::X11CairoSurface(size_t width, size_t height):
            ISurface(width, height, ST_IMAGE)
        {
            pSurface        = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
            if (pSurface == NULL)
                return;
            pCR             = cairo_create(pSurface);
            if (pCR == NULL)
                return;
            bBegin          = false;
        }

        ISurface *X11CairoSurface::create(size_t width, size_t height)
        {
            X11CairoSurface *s = new X11CairoSurface(width, height);
            return ((s == NULL) || (s->pCR == NULL)) ? NULL : s;
        }

        X11CairoSurface::~X11CairoSurface()
        {
            destroy_context();
        }

        void X11CairoSurface::destroy_context()
        {
            if (pCR != NULL)
            {
                cairo_destroy(pCR);
                pCR             = NULL;
            }
            if (pSurface != NULL)
            {
                cairo_surface_destroy(pSurface);
                pSurface        = NULL;
            }
        }

        void X11CairoSurface::destroy()
        {
            destroy_context();
        }

        bool X11CairoSurface::resize(size_t width, size_t height)
        {
            if (nType == ST_XLIB)
            {
                cairo_xlib_surface_set_size(pSurface, width, height);
                return true;
            }
            else if (nType == ST_IMAGE)
            {
                // Create new surface and cairo
                cairo_surface_t *s  = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
                if (s == NULL)
                    return false;
                cairo_t *cr         = cairo_create(s);
                if (cr == NULL)
                {
                    cairo_surface_destroy(s);
                    return false;
                }

                // Draw previous content
                cairo_set_source_surface(cr, pSurface, 0, 0);
                cairo_fill(cr);

                // Destroy previously used context
                destroy_context();

                // Update context
                pSurface            = s;
                pCR                 = cr;
            }

            return false;
        }

        void X11CairoSurface::draw(ISurface *s, float x, float y)
        {
            surface_type_t type = s->type();
            if ((type != ST_XLIB) && (type != ST_IMAGE))
                return;
            if (pCR == NULL)
                return;
            X11CairoSurface *cs = static_cast<X11CairoSurface *>(s);
            if (cs->pSurface == NULL)
                return;

            // Draw one surface on another
            cairo_set_source_surface(pCR, cs->pSurface, x, y);
            cairo_fill(pCR);
        }

        void X11CairoSurface::begin()
        {
            if (nType != ST_XLIB)
                return;
            if (bBegin)
                end();

            // TODO
        }

        void X11CairoSurface::end()
        {
            if ((nType != ST_XLIB) || (!bBegin))
                return;

            // TODO

            bBegin = true;
        }

        void X11CairoSurface::clear_rgb(uint32_t rgb)
        {
            clear_rgba(rgb & 0xffffff);
        }

        void X11CairoSurface::clear_rgba(uint32_t rgba)
        {
            if (pCR == NULL)
                return;

            cairo_set_source_rgb(pCR,
                float((rgba >> 16) & 0xff)/255.0f,
                float((rgba >> 8) & 0xff)/255.0f,
                float(rgba & 0xff)/255.0f
            );
            cairo_paint(pCR);
        }

    }

} /* namespace lsp */
