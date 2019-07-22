/*
 * LSPColor.cpp
 *
 *  Created on: 5 нояб. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        LSPColor::LSPColor()
        {
            pWidget     = NULL;
        }

        LSPColor::LSPColor(LSPWidget *widget)
        {
            pWidget     = widget;
        }
        
        LSPColor::~LSPColor()
        {
        }

        void LSPColor::color_changed()
        {
            if (pWidget != NULL)
                pWidget->query_draw();
        }
    
    } /* namespace tk */
} /* namespace lsp */
