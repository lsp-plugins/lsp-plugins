/*
 * X11CustomWidget.cpp
 *
 *  Created on: 12 дек. 2016 г.
 *      Author: sadko
 */

#include <ui/x11/ui.h>

namespace lsp
{
    namespace x11ui
    {
        X11CustomWidget::X11CustomWidget(plugin_ui *ui, IWidgetCore *core):
            X11Widget(ui, core->getClass())
        {
            pCore       = core;
        }

        X11CustomWidget::~X11CustomWidget()
        {
            pCore       = NULL;
        }

        void X11CustomWidget::handleEvent(const ui_event_t *ev)
        {
        }
    }
} /* namespace lsp */
