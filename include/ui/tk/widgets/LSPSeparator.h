/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 9 июл. 2017 г.
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

#ifndef UI_TK_LSPSEPARATOR_H_
#define UI_TK_LSPSEPARATOR_H_

namespace lsp
{
    namespace tk
    {
        class LSPSeparator: public LSPWidget
        {
            public:
                static const w_class_t    metadata;

            protected:
                LSPColor        sColor;
                ssize_t         nSize;
                size_t          nBorder;
                size_t          nPadding;
                size_t          nLineWidth;
                orientation_t   enOrientation;

            public:
                explicit LSPSeparator(LSPDisplay *dpy, bool horizontal = false);
                virtual ~LSPSeparator();

                virtual status_t init();

            public:
                inline LSPColor    *color()             { return &sColor; }

                inline ssize_t      size() const        { return nSize; }

                inline size_t       border() const      { return nBorder; }

                inline size_t       padding() const     { return nPadding; }

                inline size_t       line_width() const  { return nLineWidth; }

                inline bool         horizontal() const  { return enOrientation == O_HORIZONTAL; }

                inline bool         vertical() const    { return enOrientation == O_VERTICAL; }

                inline orientation_t    orientation() const { return enOrientation; }

            public:
                void                set_size(ssize_t value);

                void                set_border(size_t value);

                void                set_padding(size_t value);

                void                set_line_width(size_t value);

                void                set_horizontal(bool value=true);

                void                set_vertical(bool value=true);

                void                set_orientation(orientation_t value);

            public:
                virtual void render(ISurface *s, bool force);

                virtual void size_request(size_request_t *r);

        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPSEPARATOR_H_ */
