/*
 * X11CairoSurface.h
 *
 *  Created on: 25 окт. 2016 г.
 *      Author: sadko
 */

#ifndef UI_X11_X11CAIROSURFACE_H_
#define UI_X11_X11CAIROSURFACE_H_

#include <cairo.h>

namespace lsp
{
    namespace x11ui
    {
        class X11CairoSurface: public ISurface
        {
            protected:
                cairo_surface_t    *pSurface;
                cairo_t            *pCR;
                bool                bBegin;

            protected:
                void destroy_context();

            public:
                /** Create XLib surface
                 *
                 * @param dpy display
                 * @param drawable drawable
                 * @param visual visual
                 * @param width surface width
                 * @param height surface height
                 */
                X11CairoSurface(Display *dpy, Drawable drawable, Visual *visual, size_t width, size_t height);

                /** Create image surface
                 *
                 * @param width surface width
                 * @param height surface height
                 */
                X11CairoSurface(size_t width, size_t height);

                /** Destructor
                 *
                 */
                virtual ~X11CairoSurface();

            public:
                /** resize cairo surface if possible
                 *
                 * @param width new width
                 * @param height new height
                 * @return true on success
                 */
                bool resize(size_t width, size_t height);

                virtual ISurface *create(size_t width, size_t height);

                virtual void destroy();

            public:
                // Drawing methods
                virtual void draw(ISurface *s, float x, float y);

                virtual void begin();

                virtual void end();

                virtual void clear_rgb(uint32_t color);

                virtual void clear_rgba(uint32_t color);
        };
    }

} /* namespace lsp */

#endif /* UI_X11_X11CAIROSURFACE_H_ */
