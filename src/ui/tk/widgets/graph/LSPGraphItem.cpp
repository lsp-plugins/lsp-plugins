/*
 * LSPGraphItem.cpp
 *
 *  Created on: 18 июл. 2017 г.
 *      Author: sadko
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
