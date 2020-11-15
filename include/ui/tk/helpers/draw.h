/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 29 окт. 2017 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef UI_TK_HELPERS_DRAW_H_
#define UI_TK_HELPERS_DRAW_H_

#include <ui/ws/ws.h>

namespace lsp
{
    namespace tk
    {
        /** Draw border
         *
         * @param s surface to draw the border
         * @param left the left position
         * @param top the top position
         * @param width the width of the border
         * @param height the height of the border
         * @param thick the thickness of the border
         * @param iradius inner radius of the border
         * @param mask the rounding mask
         * @param c the color of the border
         */
        void draw_border(ISurface *s, ssize_t left, ssize_t top, ssize_t width, ssize_t height, ssize_t thick, size_t iradius, size_t mask, const Color &c);

        /** Create glass
         *
         * @param s the factory surface
         * @param g pointer to pointer that stores address of the surface object
         * @param width the width of the glass
         * @param height the height of the glass
         * @param radius the radius of the glass
         * @param mask the radius drawing mask
         * @return pointer to the glass on succes or null on error
         */
        ISurface *create_glass(ISurface *s, ISurface **g, size_t width, size_t height, size_t radius, size_t mask);

        /** Create glass with border
         *
         * @param s the factory surface
         * @param g pointer to pointer that stores address of the surface object
         * @param width the width of the glass
         * @param height the height of the glass
         * @param radius the radius of the glass
         * @param mask the radius drawing mask
         * @param c the color of the border
         * @return pointer to the glass on succes or null on error
         */
        ISurface * create_border_glass(
            ISurface *s, ISurface **g,
            size_t width, size_t height,
            ssize_t thick, size_t radius,
            size_t mask, const Color &c);

        void draw_border_back(
            ISurface *s,
            ssize_t left, ssize_t top,
            size_t width, size_t height,
            ssize_t thick, size_t radius,
            size_t mask, const Color &c
        );
    }
}

#endif /* UI_TK_HELPERS_DRAW_H_ */
