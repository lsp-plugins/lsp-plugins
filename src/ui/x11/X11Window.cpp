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
        X11Window::X11Window(X11Core *core, Window hwnd, size_t width, size_t height)
        {
            pCore       = core;
            hWindow     = hwnd;
            bVisible    = false;
            pSurface    = NULL;
            nWidth      = width;
            nHeight     = height;
        }

        X11Window::~X11Window()
        {
            pCore       = NULL;
        }

        int X11Window::init()
        {
            return 0;
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

        void X11Window::draw(ISurface *surface)
        {
            surface->clear_rgb(0x404040);
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
                    if (pSurface != NULL)
                        draw(pSurface);
                    break;
                }

                case UIE_SIZE_REQUEST:
                {
                    lsp_trace("size request = %d x %d", int(ev->nWidth), int(ev->nHeight));
                    XResizeWindow(pCore->x11display(), hWindow, ev->nWidth, ev->nHeight);
                    pCore->x11sync();
                    break;
                }

                case UIE_RESIZE:
                {
                    lsp_trace("new window size = %d x %d", int(ev->nWidth), int(ev->nHeight));
                    nWidth      = ev->nWidth;
                    nHeight     = ev->nHeight;
                    if (pSurface != NULL)
                    {
                        pSurface->resize(nWidth, nHeight);
                        draw(pSurface);
                    }
                    break;
                }

                default:
                    break;
            }
        }

    } /* namespace x11ui */
} /* namespace lsp */
