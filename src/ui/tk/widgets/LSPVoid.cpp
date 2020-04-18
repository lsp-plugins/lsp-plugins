/*
 * LSPVoid.cpp
 *
 *  Created on: 11 апр. 2020 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPVoid::metadata = { "LSPVoid", &LSPWidget::metadata };
        
        LSPVoid::LSPVoid(LSPDisplay *dpy):
            LSPWidget(dpy),
            sConstraints(this)
        {
            pClass          = &metadata;
        }
        
        LSPVoid::~LSPVoid()
        {
        }

        void LSPVoid::render(ISurface *s, bool force)
        {
            if ((sSize.nWidth > 0) && (sSize.nHeight > 0))
            {
                Color bgColor(sBgColor);
                s->fill_rect(sSize.nLeft, sSize.nTop, sSize.nWidth, sSize.nHeight, bgColor);
            }
        }

        void LSPVoid::size_request(size_request_t *r)
        {
            // Add external size constraints
            sConstraints.apply(r);
        }

    } /* namespace tk */
} /* namespace lsp */
