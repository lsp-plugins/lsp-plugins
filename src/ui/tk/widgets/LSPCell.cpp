/*
 * LSPCell.cpp
 *
 *  Created on: 21 июн. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPCell::metadata = { "LSPCell", &LSPWidgetProxy::metadata };

        LSPCell::LSPCell(LSPDisplay *dpy): LSPWidgetProxy(dpy)
        {
            nRowSpan    = 1;
            nColSpan    = 1;

            pClass      = &metadata;
        }
        
        LSPCell::~LSPCell()
        {
        }
    
    } /* namespace tk */
} /* namespace lsp */
