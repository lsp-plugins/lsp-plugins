/*
 * LSPProgressBar.cpp
 *
 *  Created on: 2 июл. 2019 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPProgressBar::metadata = { "LSPProgressBar", &LSPWidget::metadata };

        
        LSPProgressBar::LSPProgressBar(LSPDisplay *dpy):
            LSPWidget(dpy)
        {
            fMin        = 0.0f;
            fMax        = 100.0f;
            fValue      = 50.0f;

            pClass      = &metadata;
        }
        
        LSPProgressBar::~LSPProgressBar()
        {
        }
    
        status_t LSPProgressBar::set_text(const char *text)
        {
            return (sText.set_utf8(text)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t LSPProgressBar::set_text(const LSPString *text)
        {
            return (sText.set(text)) ? STATUS_OK : STATUS_NO_MEM;
        }

        void LSPProgressBar::draw(ISurface *s)
        {
        }

        void LSPProgressBar::size_request(size_request_t *r)
        {
            LSPWidget::size_request(r);


        }
    } /* namespace tk */
} /* namespace lsp */
