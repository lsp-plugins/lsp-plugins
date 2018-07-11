/*
 * X11Widget.h
 *
 *  Created on: 10 окт. 2016 г.
 *      Author: sadko
 */

#ifndef UI_X11_X11WIDGET_H_
#define UI_X11_X11WIDGET_H_

namespace lsp
{
    namespace x11ui
    {
        class X11Widget
        {
            protected:
                ssize_t     nLeft;
                ssize_t     nTop;
                size_t      nWidth;
                size_t      nHeight;
                bool        bVisible;

            public:
                X11Widget(plugin_ui *ui, widget_t w_class);
                virtual ~X11Widget();

            public:
                virtual void add(IWidget *widget);

                virtual void render(ISurface *s);

                virtual void size_request(ui_size_request_t *r);

                virtual void realize(const ui_realize_t *r);

            public:
                virtual void handleEvent(const ui_event_t *ev);
        };
    
    } /* namespace x11ui */
} /* namespace lsp */

#endif /* UI_X11_X11WIDGET_H_ */
