/*
 * X11Window.cpp
 *
 *  Created on: 10 окт. 2016 г.
 *      Author: sadko
 */

#include <core/status.h>
#include <ui/x11/ui.h>

namespace lsp
{
    namespace x11ui
    {
        X11Window::X11Window(plugin_ui *ui, X11Core *core):
            X11Widget(ui, W_WINDOW)
        {
            pCore       = core;
            hWindow     = 0;
            pSurface    = NULL;

            bVisible    = false;
        }

        X11Window::~X11Window()
        {
            pCore       = NULL;
        }

        int X11Window::init()
        {
            Display *dpy = pCore->x11display();

            // Try to create window
            Window wnd = XCreateSimpleWindow(dpy, pCore->x11root(), 0, 0, nWidth, nHeight, 0, 0, 0);
            if (wnd <= 0)
                return STATUS_UNKNOWN_ERR;
            XFlush(dpy);

            // Get protocols
            Atom atom_close = pCore->atoms().X11_WM_DELETE_WINDOW;
            XSetWMProtocols(dpy, wnd, &atom_close, 1);

            // Now create X11Window instance
            if (!pCore->addWindow(this))
            {
                XDestroyWindow(dpy, wnd);
                XFlush(dpy);
                return STATUS_NO_MEM;
            }

            // Now select input for new handle
            XSelectInput(dpy, wnd,
                KeyPressMask |
                KeyReleaseMask |
                ButtonPressMask |
                ButtonReleaseMask |
                EnterWindowMask |
                LeaveWindowMask |
                PointerMotionMask |
                /*PointerMotionHintMask | */
                Button1MotionMask |
                Button2MotionMask |
                Button3MotionMask |
                Button4MotionMask |
                Button5MotionMask |
                ButtonMotionMask |
                KeymapStateMask |
                ExposureMask |
                /*VisibilityChangeMask | */
                StructureNotifyMask |
                /*ResizeRedirectMask | */
                SubstructureNotifyMask |
                SubstructureRedirectMask |
                FocusChangeMask |
                PropertyChangeMask |
                ColormapChangeMask |
                OwnerGrabButtonMask
            );
            XFlush(dpy);
            hWindow = wnd;

            if (bVisible)
                do_mapping(true);

            return STATUS_OK;
        }

        void X11Window::do_mapping(bool mapped)
        {
            bVisible = mapped;
            if (hWindow == 0)
                return;

            if (mapped)
                XMapWindow(pCore->x11display(), hWindow);
            else
                XUnmapWindow(pCore->x11display(), hWindow);

            XFlush(pCore->x11display());
        }

        void X11Window::setWidth(size_t width)
        {
            nWidth = width;
            if (hWindow == 0)
                return;
        }

        void X11Window::setHeight(size_t height)
        {
            nHeight = height;
            if (hWindow == 0)
                return;
        }

        void X11Window::drop_surface()
        {
            if (pSurface != NULL)
            {
                pSurface->destroy();
                delete pSurface;
                pSurface = NULL;
            }
        }

        void X11Window::destroy()
        {
            drop_surface();

            // Destroy window
            if (hWindow > 0)
            {
                do_mapping(false);
                XDestroyWindow(pCore->x11display(), hWindow);
                hWindow = 0;
            }
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
            do_mapping(true);
        }

        void X11Window::hide()
        {
            if (!bVisible)
                return;
            do_mapping(false);
        }

        void X11Window::render(ISurface *s)
        {
            if (s == NULL)
                s = pSurface;

            size_t b = ((nWidth*nHeight) >> 4) & 0xff;
            s->clear_rgb(b);
        }

        void X11Window::handleEvent(const ui_event_t *ev)
        {
            lsp_trace("ui_event type=%d", int(ev->nType));

            switch (ev->nType)
            {
                case UIE_SHOW:
                {
                    // Drop previously existed drawing surface
                    drop_surface();

                    // Create surface
                    Display *dpy    = pCore->x11display();
                    int scr         = pCore->x11screen();
                    Visual *v       = DefaultVisual(dpy, scr);
                    pSurface        = new X11CairoSurface(dpy, hWindow, v, nWidth, nHeight);
                    break;
                }

                case UIE_HIDE:
                {
                    // Drop previously existed drawing surface
                    drop_surface();
                    break;
                }

                case UIE_REDRAW:
                {
                    lsp_trace("redraw location = %d x %d, size = %d x %d",
                            int(ev->nLeft), int(ev->nTop),
                            int(ev->nWidth), int(ev->nHeight));
                    if (pSurface != NULL)
                        render(pSurface);
                    break;
                }

                case UIE_SIZE_REQUEST:
                {
                    lsp_trace("size request = %d x %d", int(ev->nWidth), int(ev->nHeight));
//                    XResizeWindow(pCore->x11display(), hWindow, ev->nWidth, ev->nHeight);
//                    pCore->x11sync();
                    break;
                }

                case UIE_RESIZE:
                {
                    lsp_trace("new window location = %d x %d, size = %d x %d",
                            int(ev->nLeft), int(ev->nTop),
                            int(ev->nWidth), int(ev->nHeight));
                    nWidth      = ev->nWidth;
                    nHeight     = ev->nHeight;
                    if (pSurface != NULL)
                    {
                        pSurface->resize(nWidth, nHeight);
                        render(pSurface);
                    }
                    break;
                }

                case UIE_MOUSE_MOVE:
                {
                    lsp_trace("mouse move = %d x %d", int(ev->nLeft), int(ev->nTop));
                    break;
                }

                case UIE_CLOSE:
                {
                    lsp_trace("close request on window");
                    pCore->removeWindow(this);
                    break;
                }

                default:
                    break;
            }
        }

    } /* namespace x11ui */
} /* namespace lsp */
