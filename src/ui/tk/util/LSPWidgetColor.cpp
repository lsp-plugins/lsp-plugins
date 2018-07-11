/*
 * LSPWidgetColor.cpp
 *
 *  Created on: 5 нояб. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        
        LSPWidgetColor::LSPWidgetColor()
        {
            pWidget = NULL;
        }
        
        LSPWidgetColor::LSPWidgetColor(LSPWidget *widget)
        {
            pWidget = widget;
        }

        LSPWidgetColor::~LSPWidgetColor()
        {
            pWidget = NULL;
        }

        void LSPWidgetColor::color_changed()
        {
            if (pWidget != NULL)
                pWidget->query_draw();
        }
    
    } /* namespace tk */
} /* namespace lsp */
