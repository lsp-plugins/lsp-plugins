/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 10 июл. 2017 г.
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

#ifndef UI_TK_LSPLED_H_
#define UI_TK_LSPLED_H_

namespace lsp
{
    namespace tk
    {
        class LSPLed: public LSPWidget
        {
            public:
                static const w_class_t    metadata;

            protected:
                LSPColor            sColor;
                LSPColor            sHoleColor;
                LSPColor            sGlassColor;
                size_t              nSize;
                bool                bOn;

            public:
                explicit LSPLed(LSPDisplay *dpy);
                virtual ~LSPLed();

                virtual status_t init();

            public:
                inline LSPColor *color()        { return &sColor; }

                inline LSPColor *hole_color()   { return &sHoleColor; }

                inline LSPColor *glass_color()  { return &sGlassColor; }

                inline bool on() const      { return bOn; }

                inline bool off() const     { return !bOn; }

                inline size_t size() const  { return nSize; }

            public:
                void set_on(bool on = true);

                void set_off(bool off = true);

                void set_size(size_t size);

            public:
                virtual void draw(ISurface *s);

                virtual void size_request(size_request_t *r);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPLED_H_ */
