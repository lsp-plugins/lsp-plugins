/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 7 июл. 2017 г.
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

#ifndef UI_TK_LSPINDICATOR_H_
#define UI_TK_LSPINDICATOR_H_

#include <core/buffer.h>

namespace lsp
{
    namespace tk
    {
        class LSPIndicator: public LSPWidget
        {
            public:
                static const w_class_t    metadata;

            protected:
                static const size_t DIGITS_DFL          = 5;
                static const size_t ITEMS_MAX           = 16;

                enum format_t
                {
                    F_UNKNOWN,
                    F_FLOAT,
                    F_INT,
                    F_TIME
                };

                enum flags_t
                {
                    F_SIGN          = 1 << 0,
                    F_PLUS          = 1 << 1,
                    F_PAD_ZERO      = 1 << 2,
                    F_FIXED_PREC    = 1 << 3,
                    F_NO_ZERO       = 1 << 4,
                    F_DOT           = 1 << 5,
                    F_TOLERANCE     = 1 << 6
                };

                typedef struct item_t
                {
                    char        type;
                    size_t      digits;
                    ssize_t     precision;
                } item_t;

                LSPColor            sColor;
                LSPColor            sTextColor;
                float               fValue;

                // Format
                char               *sFormat;
                format_t            nFormat;
                size_t              sDigits;
                size_t              nFlags;
                cstorage<item_t>    vItems;

            private:
                bool    parse_format(const char *format);
                void    draw_digit(ISurface *s, int x, int y, char ch, char mod, const Color &on, const Color &off);

                bool    fmt_time(buffer_t *buf, double value);
                bool    fmt_float(buffer_t *buf, double value);
                bool    fmt_int(buffer_t *buf, ssize_t value);
                bool    format(buffer_t *buf, double value);

                static bool parse_long(char *p, char **ret, long *value);

                void    drop_data();

            //---------------------------------------------------------------------------------
            // Construction and destruction
            public:
                explicit LSPIndicator(LSPDisplay *dpy);
                virtual ~LSPIndicator();

                virtual status_t init();
                virtual void destroy();

            //---------------------------------------------------------------------------------
            // Properties
            public:

                inline LSPColor        *color()         { return &sColor; }

                inline LSPColor        *text_color()    { return &sTextColor; }

                inline float            value() const   { return fValue; }

                inline const char      *format() const  { return sFormat; }

            //---------------------------------------------------------------------------------
            // Manipulation
            public:

                status_t                set_format(const char *fmt);

                void                    set_value(float value);

            //---------------------------------------------------------------------------------
            // Event handling
            public:
                virtual void size_request(size_request_t *r);

                virtual void draw(ISurface *s);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPINDICATOR_H_ */
