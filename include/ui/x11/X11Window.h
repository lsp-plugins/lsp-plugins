/*
 * Window.h
 *
 *  Created on: 10 окт. 2016 г.
 *      Author: sadko
 */

#ifndef UI_X11_WINDOW_H_
#define UI_X11_WINDOW_H_

namespace lsp
{
    namespace x11ui
    {
        class X11Window
        {
            protected:
                X11Core    *pCore;
                Window      hWindow;
                bool        bVisible;

            public:
                X11Window(X11Core *core, Window hwnd);
                virtual ~X11Window();

            public:
                int init();
                void destroy();

                inline Window x11handle() const { return hWindow; };
                void setVisibility(bool visible);
                inline bool getVisibility() const { return bVisible; }
                void show();
                void hide();

            public:
                virtual void handleEvent(const ui_event_t *ev);
        };
    
    } /* namespace x11ui */
} /* namespace lsp */

#endif /* UI_X11_WINDOW_H_ */
