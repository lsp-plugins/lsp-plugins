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
        }

        X11Widget::~X11Widget()
        {
        }

        void X11Widget::handleEvent(const ui_event_t *ev)
        {
            lsp_trace("ui_event type=%d", int(ev->nType));
        }
    } /* namespace x11ui */
} /* namespace lsp */
