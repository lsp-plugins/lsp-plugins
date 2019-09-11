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
            bDefault    = true;
        }

        LSPColor::LSPColor(LSPWidget *widget)
        {
            pWidget     = widget;
            bDefault    = true;
        }
        
        LSPColor::~LSPColor()
        {
        }

        void LSPColor::color_changed()
        {
        }

        void LSPColor::trigger_change(bool reset_default)
        {
            if (reset_default)
                bDefault    = false;

            color_changed();

            if (pWidget != NULL)
                pWidget->query_draw();
        }

        void LSPColor::override(const LSPColor &c)
        {
            if (!bDefault)
                return;
            sColor.copy(c.sColor);
            trigger_change(false);
        }

        void LSPColor::override(const LSPColor *c)
        {
            if (!bDefault)
                return;
            sColor.copy(c->sColor);
            trigger_change(false);
        }

        void LSPColor::override(const Color &c)
        {
            if (!bDefault)
                return;
            sColor.copy(c);
            trigger_change(false);
        }

        void LSPColor::override(const Color *c)
        {
            if (!bDefault)
                return;
            sColor.copy(c);
            trigger_change(false);
        }
    
    } /* namespace tk */
} /* namespace lsp */
