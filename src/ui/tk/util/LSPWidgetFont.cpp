/*
 * LSPWidgetFont.cpp
 *
 *  Created on: 7 нояб. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        
        LSPWidgetFont::LSPWidgetFont(LSPDisplay *dpy): LSPFont(dpy)
        {
            pWidget = NULL;
        }

        LSPWidgetFont::LSPWidgetFont(LSPWidget *widget): LSPFont(widget->display())
        {
            pWidget = widget;
        }

        LSPWidgetFont::LSPWidgetFont(LSPDisplay *dpy, LSPWidget *widget): LSPFont(dpy)
        {
            pWidget = widget;
        }
        
        LSPWidgetFont::~LSPWidgetFont()
        {
            pWidget = NULL;
        }

        void LSPWidgetFont::on_change()
        {
            if (pWidget != NULL)
                pWidget->query_resize();
        }
    
    } /* namespace tk */
} /* namespace lsp */
