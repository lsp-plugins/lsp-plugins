/*
 * X11CustomWidget.h
 *
 *  Created on: 12 дек. 2016 г.
 *      Author: sadko
 */

#ifndef UI_X11_X11CUSTOMWIDGET_H_
#define UI_X11_X11CUSTOMWIDGET_H_

namespace lsp
{
    namespace x11ui
    {
        class X11CustomWidget: public X11Widget
        {
            private:
                IWidgetCore    *pCore;

            public:
                X11CustomWidget(plugin_ui *ui, IWidgetCore *core);
                virtual ~X11CustomWidget();

            public:
                virtual void handleEvent(const ui_event_t *ev);
        };
    }
} /* namespace lsp */

#endif /* UI_X11_X11CUSTOMWIDGET_H_ */
