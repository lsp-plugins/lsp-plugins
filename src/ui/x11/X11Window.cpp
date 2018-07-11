/*
 * X11Window.cpp
 *
 *  Created on: 10 окт. 2016 г.
 *      Author: sadko
 */

#include <ui/x11/ui.h>

namespace lsp
{
    namespace x11ui
    {
        X11Window::X11Window(X11Core *core, Window hwnd)
        {
            pCore       = core;
            hWindow     = hwnd;
            bVisible    = false;
        }

        X11Window::~X11Window()
        {
            pCore       = NULL;
        }

        int X11Window::init()
        {
            return 0;
        }

        void X11Window::destroy()
        {
        }

        void X11Window::setVisibility(bool visible)
        {
            if (visible)
                show();
            else
                hide();
        }

        void X11Window::show()
        {
            if (bVisible)
                return;
            XMapWindow(pCore->x11display(), hWindow);
            XFlush(pCore->x11display());
        }

        void X11Window::hide()
        {
            if (!bVisible)
                return;
            XUnmapWindow(pCore->x11display(), hWindow);
            XFlush(pCore->x11display());
        }

        void X11Window::handleEvent(const ui_event_t *ev)
        {
            lsp_trace("ui_event type=%d", int(ev->nType));
        }

    } /* namespace x11ui */
} /* namespace lsp */
