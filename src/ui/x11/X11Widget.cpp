/*
 * X11Widget.cpp
 *
 *  Created on: 10 окт. 2016 г.
 *      Author: sadko
 */

#include <ui/x11/ui.h>

namespace lsp
{
    namespace x11ui
    {
        X11Widget::X11Widget(plugin_ui *ui, widget_t w_class)
        {
            nLeft           = 0;
            nTop            = 0;
            nWidth          = 32;
            nHeight         = 32;
            bVisible        = true;
        }

        X11Widget::~X11Widget()
        {
        }

        void X11Widget::add(IWidget *widget)
        {
        }

        void X11Widget::render(ISurface *s)
        {
        }

        void X11Widget::size_request(ui_size_request_t *r)
        {
            r->nMinWidth        = 0;
            r->nMinHeight       = 0;
            r->nMaxWidth        = -1;
            r->nMaxHeight       = -1;
        }

        void X11Widget::realize(const ui_realize_t *r)
        {
            nLeft               = r->nLeft;
            nTop                = r->nTop;
            nWidth              = r->nWidth;
            nHeight             = r->nHeight;
        }

        void X11Widget::handleEvent(const ui_event_t *ev)
        {
            lsp_trace("ui_event type=%d", int(ev->nType));
        }
    } /* namespace x11ui */
} /* namespace lsp */
