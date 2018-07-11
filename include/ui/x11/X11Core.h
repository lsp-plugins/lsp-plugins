/*
 * UICore.h
 *
 *  Created on: 10 окт. 2016 г.
 *      Author: sadko
 */

#ifndef UI_X11_UICORE_H_
#define UI_X11_UICORE_H_

#include <time.h>

namespace lsp
{
    namespace x11ui
    {
        class X11Window;

        class X11Core
        {
            protected:
                volatile bool   bExit;
                Display        *pDisplay;
                Window          hRootWnd;
                int             hDflScreen;
                int             nBlackColor;
                int             nWhiteColor;
                cvector<X11Window> vWindows;
                timespec        sLastRender;
                x11_atoms_t     sAtoms;

            protected:
                void            handleEvent(XEvent *ev);
                int             do_main_iteration(bool redraw);

            public:
                X11Core();
                ~X11Core();

            public:
                int init(int argc, const char **argv);
                void destroy();

                int main();
                int main_iteration();
                void quit_main();

            public:
                bool                addWindow(X11Window *wnd);
                bool                removeWindow(X11Window *wnd);

                inline Display             *x11display() const  { return pDisplay; }
                inline Window               x11root() const     { return hRootWnd; }
                inline int                  x11screen() const   { return hDflScreen; }
                inline const x11_atoms_t   &atoms() const       { return sAtoms; }
                void                        x11sync();

            public:
                static const char *event_name(int xev_code);
        };
    }
}

#endif /* UI_X11_UICORE_H_ */
