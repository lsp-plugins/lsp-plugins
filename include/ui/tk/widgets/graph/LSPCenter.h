/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 19 июл. 2017 г.
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

#ifndef UI_TK_LSPCENTER_H_
#define UI_TK_LSPCENTER_H_

namespace lsp
{
    namespace tk
    {
        class LSPCenter: public LSPGraphItem
        {
            public:
                static const w_class_t    metadata;

            private:
                float           fLeft;
                float           fTop;
                float           fRadius;
                LSPColor        sColor;

            public:
                explicit LSPCenter(LSPDisplay *dpy);
                virtual ~LSPCenter();

                virtual status_t init();

            public:
                inline LSPColor *color()            { return &sColor;   };
                inline float radius() const         { return fRadius;   };
                inline float canvas_left() const    { return fLeft;     };
                inline float canvas_top() const     { return fTop;      };

            public:
                void set_radius(float value);
                void set_canvas_left(float value);
                void set_canvas_top(float value);

            public:
                virtual void render(ISurface *s, bool force);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPCENTER_H_ */
