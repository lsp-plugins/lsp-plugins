/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 18 июл. 2017 г.
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

#ifndef UI_TK_LSPGRAPHITEM_H_
#define UI_TK_LSPGRAPHITEM_H_

namespace lsp
{
    namespace tk
    {
        class LSPGraph;
        
        class LSPGraphItem: public LSPWidget
        {
            public:
                static const w_class_t    metadata;

            protected:
                bool        bSmooth;

            public:
                explicit LSPGraphItem(LSPDisplay *dpy);
                virtual ~LSPGraphItem();

            public:
                LSPGraph        *graph();

            public:
                inline bool is_smooth() const   { return bSmooth;               };

            public:
                void        set_smooth(bool value = true);

                inline void set_sharp(bool value = true)    { set_smooth(!value); }

            public:
                virtual bool inside(ssize_t x, ssize_t y);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPGRAPHITEM_H_ */
