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

#ifndef UI_TK_LSPGROUP_H_
#define UI_TK_LSPGROUP_H_

namespace lsp
{
    namespace tk
    {
        class LSPGroup: public LSPWidgetContainer
        {
            public:
                static const w_class_t    metadata;

            protected:
                typedef struct dimensions_t
                {
                    size_t      nGapLeft;
                    size_t      nGapTop;
                    size_t      nGapRight;
                    size_t      nGapBottom;
                    size_t      nMinWidth;
                    size_t      nMinHeight;
                } dimensions_t;

            protected:
                LSPLocalString  sText;
                LSPColor        sColor;
                size_t          nRadius;
                size_t          nBorder;
                LSPWidget      *pWidget;
                LSPFont         sFont;
                bool            bEmbed;

            protected:
                virtual LSPWidget  *find_widget(ssize_t x, ssize_t y);
                void                query_dimensions(dimensions_t *d);
                void                do_destroy();

            public:
                inline LSPLocalString *text()               { return &sText; }
                inline const LSPLocalString *text() const   { return &sText; }
                inline LSPColor    *color()                 { return &sColor; }
                inline LSPColor    *text_color()            { return sFont.color(); }
                inline size_t       radius() const          { return nRadius; }
                inline size_t       border() const          { return nBorder; }
                inline LSPFont     *font()                  { return &sFont; }
                inline bool         embed() const           { return bEmbed; }

            public:
                void                set_radius(size_t value);
                void                set_border(size_t value);
                void                set_embed(bool embed);

            public:
                explicit LSPGroup(LSPDisplay *dpy);
                virtual ~LSPGroup();

                virtual status_t init();
                virtual void destroy();

            public:
                virtual void render(ISurface *s, bool force);

                virtual status_t add(LSPWidget *widget);

                virtual status_t remove(LSPWidget *widget);

                virtual void size_request(size_request_t *r);

                virtual void realize(const realize_t *r);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPGROUP_H_ */
