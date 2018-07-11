/*
 * UICore.h
 *
 *  Created on: 10 окт. 2016 г.
 *      Author: sadko
 */

#ifndef UI_X11_UICORE_H_
#define UI_X11_UICORE_H_

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

            protected:
                void            handleEvent(XEvent *ev);

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
                X11Window      *createWindow(size_t width, size_t height);

                Display        *x11display() const  { return pDisplay; }
                Window          x11root() const     { return hRootWnd; }
                int             x11screen() const   { return hDflScreen; }

            public:
                static const char *event_name(int xev_code);
        };
    }
}

#endif /* UI_X11_UICORE_H_ */
