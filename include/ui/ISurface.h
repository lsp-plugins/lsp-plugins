/*
 * ISurface.h
 *
 *  Created on: 25 окт. 2016 г.
 *      Author: sadko
 */

#ifndef UI_ISURFACE_H_
#define UI_ISURFACE_H_

#include <core/types.h>

namespace lsp
{
    enum surface_type_t
    {
        ST_UNKNOWN,
        ST_IMAGE,
        ST_XLIB
    };

    /** Common drawing surface interface
     *
     */
    class ISurface
    {
        protected:
            size_t          nWidth;
            size_t          nHeight;
            surface_type_t  nType;

        protected:
            ISurface(size_t width, size_t height, surface_type_t type);

        public:
            ISurface();
            virtual ~ISurface();

        public:
            /** Get surface width
             *
             * @return surface width
             */
            inline size_t width() const { return nWidth; }

            /** Get surface height
             *
             * @return surface height
             */
            inline size_t height() const { return nHeight; }

            /** Get type of surface
             *
             * @return type of surface
             */
            inline surface_type_t type()  const { return nType; }

        public:
            /** Create child surface for drawing
             * @param width surface width
             * @param height surface height
             * @return created surface or NULL
             */
            virtual ISurface *create(size_t width, size_t height);

            /** Destroy surface
             *
             */
            virtual void destroy();

            /** Start drawing on the surface
             *
             */
            virtual void begin();

            /** Complete drawing, synchronize surface with underlying device
             *
             */
            virtual void end();

        public:
            /** Draw surface
             *
             * @param s surface to draw
             * @param x offset from left
             * @param y offset from top
             */
            virtual void draw(ISurface *s, float x, float y);

            /** Clear surface with specified color
             *
             * @param color RGB color to use for clear
             */
            virtual void clear_rgb(uint32_t color);

            /** Clear surface with specified color
             *
             * @param color RGBA color to use for clear
             */
            virtual void clear_rgba(uint32_t color);

    };

} /* namespace lsp */

#endif /* UI_ISURFACE_H_ */
