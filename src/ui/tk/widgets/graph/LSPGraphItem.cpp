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

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPGraphItem::metadata = { "LSPGraphItem", &LSPWidget::metadata };

        LSPGraphItem::LSPGraphItem(LSPDisplay *dpy): LSPWidget(dpy)
        {
            bSmooth     = true;

            pClass          = &metadata;
        }

        LSPGraphItem::~LSPGraphItem()
        {
        }

        LSPGraph *LSPGraphItem::graph()
        {
            LSPComplexWidget *parent = pParent;

            while (parent != NULL)
            {
                LSPGraph *graph = widget_cast<LSPGraph>(parent);
                if (graph != NULL)
                    return graph;

                parent  = parent->parent();
            }
            return NULL;
        }

        void LSPGraphItem::set_smooth(bool value)
        {
            if (bSmooth == value)
                return;
            bSmooth     = value;
            query_draw();
        }

        bool LSPGraphItem::inside(ssize_t x, ssize_t y)
        {
            return false;
        }
    } /* namespace tk */
} /* namespace lsp */
