/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 17 июл. 2017 г.
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

#ifndef UI_TK_LSPALIGN_H_
#define UI_TK_LSPALIGN_H_

namespace lsp
{
    namespace tk
    {
        class LSPAlign: public LSPWidgetContainer
        {
            public:
                static const w_class_t    metadata;

            protected:
                float           nVertPos;
                float           nHorPos;
                float           nVertScale;
                float           nHorScale;
                LSPWidget      *pWidget;

            protected:
                virtual LSPWidget      *find_widget(ssize_t x, ssize_t y);
                void                    do_destroy();

            public:
                explicit LSPAlign(LSPDisplay *dpy);
                virtual ~LSPAlign();

                virtual status_t init();
                virtual void destroy();

            public:
                inline float            vpos() const            { return nVertPos; }
                inline float            hpos() const            { return nHorPos; }
                inline float            vscale() const          { return nVertScale; }
                inline float            hscale() const          { return nHorScale; }

            public:
                void                    set_vpos(float value);
                void                    set_hpos(float value);
                void                    set_pos(float h, float v);
                inline void             set_position(float h, float v) { set_pos(h, v); };
                void                    set_vscale(float value);
                void                    set_hscale(float value);
                void                    set_scale(float h, float v);

            public:
                virtual void render(ISurface *s, bool force);

                virtual status_t add(LSPWidget *widget);

                virtual status_t remove(LSPWidget *widget);

                virtual void size_request(size_request_t *r);

                virtual void realize(const realize_t *r);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPALIGN_H_ */
