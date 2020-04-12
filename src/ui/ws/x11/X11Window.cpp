/*
 * X11Window.cpp
 *
 *  Created on: 10 окт. 2016 г.
 *      Author: sadko
 */

#include <ui/ws/x11/ws.h>

#ifdef USE_X11_DISPLAY

#include <limits.h>
#include <errno.h>
#include <string.h>
#include <dsp/endian.h>

namespace lsp
{
    namespace ws
    {
        namespace x11
        {
            X11Window::X11Window(X11Display *core, size_t screen, ::Window wnd, IEventHandler *handler, bool wrapper): INativeWindow(core, handler)
            {
                lsp_trace("hwindow = %x", int(wnd));
                pX11Display             = core;
                bWrapper                = wrapper;
                if (wrapper)
                {
                    hWindow                 = wnd;
                    hParent                 = None;
                }
                else
                {
                    hWindow                 = None;
                    hParent                 = wnd;
                }
                nScreen                 = screen;
                pSurface                = NULL;
                enBorderStyle           = BS_SIZABLE;
                vMouseUp[0].nType       = UIE_UNKNOWN;
                vMouseUp[1].nType       = UIE_UNKNOWN;
                nActions                = WA_SINGLE;
                nFlags                  = 0;
                enPointer               = MP_DEFAULT;

                sSize.nLeft             = 0;
                sSize.nTop              = 0;
                sSize.nWidth            = 32;
                sSize.nHeight           = 32;

                sConstraints.nMinWidth  = -1;
                sConstraints.nMinHeight = -1;
                sConstraints.nMaxWidth  = -1;
                sConstraints.nMaxHeight = -1;
            }

            void X11Window::do_create()
            {
            }

            X11Window::~X11Window()
            {
                pX11Display       = NULL;
            }

            status_t X11Window::init()
            {
                Display *dpy = pX11Display->x11display();
                Atom dnd_version    = 5;    // Version 5 of protocol is supported

                if (bWrapper)
                {
                    if (!pX11Display->addWindow(this))
                        return STATUS_NO_MEM;

                    // Now select input for the handle
                    lsp_trace("Issuing XSelectInput");
                    ::XSelectInput(dpy, hWindow,
                        KeyPressMask |
                        KeyReleaseMask |
                        ButtonPressMask |
                        ButtonReleaseMask |
                        EnterWindowMask |
                        LeaveWindowMask |
                        PointerMotionMask |
        //                PointerMotionHintMask |
                        Button1MotionMask |
                        Button2MotionMask |
                        Button3MotionMask |
                        Button4MotionMask |
                        Button5MotionMask |
                        ButtonMotionMask |
                        KeymapStateMask |
                        ExposureMask |
                        StructureNotifyMask |
                        FocusChangeMask |
                        PropertyChangeMask
                    );

                    /**
                     * In order for the user to be able to transfer data from any application to any other application
                     * via DND, every application that supports XDND version N must also support all previous versions
                     * (3 to N-1). The XdndAware property provides the highest version number supported by the target
                     * (Nt). If the source supports versions up to Ns, then the version that will actually be used is
                     * min(Ns,Nt). This is the version sent in the XdndEnter message. It is important to note that
                     * XdndAware allows this to be calculated before any messages are actually sent.
                     */
                    ::XChangeProperty(dpy, hWindow, pX11Display->atoms().X11_XdndAware, XA_ATOM, 32, PropModeReplace,
                                    reinterpret_cast<unsigned char *>(&dnd_version), 1);
                    /**
                     * The proxy window must have the XdndProxy property set to point to itself. If it doesn't or if the
                     * proxy window doesn't exist at all, one should ignore XdndProxy on the assumption that it is left
                     * over after a crash.
                     */
                    ::XChangeProperty(dpy, hWindow, pX11Display->atoms().X11_XdndProxy, XA_WINDOW, 32, PropModeReplace,
                                    reinterpret_cast<unsigned char *>(&hWindow), 1);

                    pX11Display->flush();
                }
                else
                {
                    // Try to create window
                    pX11Display->sync();

                    // Calculate window constraints
                    calc_constraints(&sSize, &sSize);

                    // Create window
                    Window wnd = 0;

                    if (hParent > 0)
                    {
                        XWindowAttributes atts;
                        XGetWindowAttributes(pX11Display->x11display(), hParent, &atts);
                        nScreen = pX11Display->get_screen(atts.root);

                        wnd = XCreateWindow(
                            dpy, hParent,
                            sSize.nLeft, sSize.nTop, sSize.nWidth, sSize.nHeight,
                            0, 0, CopyFromParent, CopyFromParent, 0, NULL);
                    }
                    else
                    {
                        size_t n = pX11Display->screens();
                        wnd = (nScreen < n) ? RootWindow(dpy, nScreen) : pX11Display->x11root();
                        nScreen = pX11Display->get_screen(wnd);

                        wnd = XCreateWindow(
                            dpy, wnd,
                            sSize.nLeft, sSize.nTop, sSize.nWidth, sSize.nHeight,
                            0, 0, CopyFromParent, CopyFromParent, 0, NULL);
                    }

                    lsp_trace("wnd=%x, external=%d, external_id=%x", int(wnd), int(hParent > 0), int(hParent));
                    if (wnd <= 0)
                        return STATUS_UNKNOWN_ERR;
                    pX11Display->flush();

                    // Get protocols
                    lsp_trace("Issuing XSetWMProtocols");
                    Atom atom_close     = pX11Display->atoms().X11_WM_DELETE_WINDOW;
                    ::XSetWMProtocols(dpy, wnd, &atom_close, 1);

                    /**
                     * In order for the user to be able to transfer data from any application to any other application
                     * via DND, every application that supports XDND version N must also support all previous versions
                     * (3 to N-1). The XdndAware property provides the highest version number supported by the target
                     * (Nt). If the source supports versions up to Ns, then the version that will actually be used is
                     * min(Ns,Nt). This is the version sent in the XdndEnter message. It is important to note that
                     * XdndAware allows this to be calculated before any messages are actually sent.
                     */
                    ::XChangeProperty(dpy, wnd, pX11Display->atoms().X11_XdndAware, XA_ATOM, 32, PropModeReplace,
                                    reinterpret_cast<unsigned char *>(&dnd_version), 1);
                    /**
                     * The proxy window must have the XdndProxy property set to point to itself. If it doesn't or if the
                     * proxy window doesn't exist at all, one should ignore XdndProxy on the assumption that it is left
                     * over after a crash.
                     */
                    ::XChangeProperty(dpy, wnd, pX11Display->atoms().X11_XdndProxy, XA_WINDOW, 32, PropModeReplace,
                                    reinterpret_cast<unsigned char *>(&wnd), 1);
                    pX11Display->flush();

                    // Now create X11Window instance
                    if (!pX11Display->addWindow(this))
                    {
                        XDestroyWindow(dpy, wnd);
                        pX11Display->flush();
                        return STATUS_NO_MEM;
                    }

                    // Now select input for new handle
                    lsp_trace("Issuing XSelectInput");
                    ::XSelectInput(dpy, wnd,
                        KeyPressMask |
                        KeyReleaseMask |
                        ButtonPressMask |
                        ButtonReleaseMask |
                        EnterWindowMask |
                        LeaveWindowMask |
                        PointerMotionMask |
        //                PointerMotionHintMask |
                        Button1MotionMask |
                        Button2MotionMask |
                        Button3MotionMask |
                        Button4MotionMask |
                        Button5MotionMask |
                        ButtonMotionMask |
                        KeymapStateMask |
                        ExposureMask |
        //                VisibilityChangeMask |
                        StructureNotifyMask |
        //                ResizeRedirectMask |
                        SubstructureNotifyMask |
                        SubstructureRedirectMask |
                        FocusChangeMask |
                        PropertyChangeMask |
                        ColormapChangeMask |
                        OwnerGrabButtonMask
                    );
                    if (hParent > 0)
                    {
                        ::XSelectInput(dpy, hParent,
                            PropertyChangeMask |
                            StructureNotifyMask
                        );
                    }

                    pX11Display->flush();

                    sMotif.flags        = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS | MWM_HINTS_INPUT_MODE | MWM_HINTS_STATUS;
                    sMotif.functions    = MWM_FUNC_ALL;
                    sMotif.decorations  = MWM_DECOR_ALL;
                    sMotif.input_mode   = MWM_INPUT_MODELESS;
                    sMotif.status       = 0;

                    hWindow = wnd;

                    // Initialize window border style and actions
                    set_border_style(BS_SIZABLE);
                    set_window_actions(WA_ALL);
                    set_mouse_pointer(MP_DEFAULT);
                }

                lsp_trace("init ok");

                return STATUS_OK;
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
                // Drop surface
                drop_surface();

                if (!bWrapper)
                {
                    // Remove window from registry
                    if (pX11Display != NULL)
                        pX11Display->remove_window(this);

                    // Destroy window
                    if (hWindow > 0)
                    {
                        XDestroyWindow(pX11Display->x11display(), hWindow);
                        hWindow = 0;
                    }
                    pX11Display->sync();
                }
                else
                {
                    hWindow = None;
                    hParent = None;
                }
            }

            void X11Window::calc_constraints(realize_t *dst, const realize_t *req)
            {
                *dst    = *req;

                if ((sConstraints.nMaxWidth >= 0) && (dst->nWidth > sConstraints.nMaxWidth))
                    dst->nWidth         = sConstraints.nMaxWidth;
                if ((sConstraints.nMaxHeight >= 0) && (dst->nHeight > sConstraints.nMaxHeight))
                    dst->nHeight        = sConstraints.nMaxHeight;
                if ((sConstraints.nMinWidth >= 0) && (dst->nWidth < sConstraints.nMinWidth))
                    dst->nWidth         = sConstraints.nMinWidth;
                if ((sConstraints.nMinHeight >= 0) && (dst->nHeight < sConstraints.nMinHeight))
                    dst->nHeight        = sConstraints.nMinHeight;
            }

            ISurface *X11Window::get_surface()
            {
                if (bWrapper)
                    return NULL;
                return pSurface;
            }

            status_t X11Window::do_update_constraints()
            {
                if (hWindow == 0)
                    return STATUS_BAD_STATE;

                XSizeHints sz;
                sz.flags        = USPosition | USSize | PMinSize | PMaxSize;
                sz.x            = sSize.nLeft;
                sz.y            = sSize.nTop;
                sz.width        = sSize.nWidth;
                sz.height       = sSize.nHeight;

                if (nActions & WA_RESIZE)
                {
                    sz.min_width    = (sConstraints.nMinWidth > 0) ? sConstraints.nMinWidth : 0;
                    sz.min_height   = (sConstraints.nMinHeight > 0) ? sConstraints.nMinHeight : 0;
                    sz.max_width    = (sConstraints.nMaxWidth > 0) ? sConstraints.nMaxWidth : INT_MAX;
                    sz.max_height   = (sConstraints.nMaxHeight > 0) ? sConstraints.nMaxHeight : INT_MAX;
                }
                else
                {
                    sz.min_width    = sSize.nWidth;
                    sz.min_height   = sSize.nHeight;
                    sz.max_width    = sSize.nWidth;
                    sz.max_height   = sSize.nHeight;
                }

                lsp_trace("Window constraints: min_width=%d, min_height=%d, max_width=%d, max_height=%d",
                        int(sz.min_width), int(sz.min_height), int(sz.max_width), int(sz.max_height)
                    );

                XSetWMNormalHints(pX11Display->x11display(), hWindow, &sz);
//                pX11Display->sync();
                return STATUS_OK;
            }

            bool X11Window::check_double_click(const ws_event_t *pe, const ws_event_t *ce)
            {
                if ((pe->nType != UIE_MOUSE_UP) || (ce->nType != UIE_MOUSE_UP))
                    return false;
                if ((pe->nState != ce->nState) || (pe->nCode != ce->nCode))
                    return false;
                if (((ce->nTime - pe->nTime) > 400) || (ce->nTime < pe->nTime))
                    return false;

                return (ce->nLeft == pe->nLeft) && (ce->nTop == pe->nTop);
            }

            status_t X11Window::handle_event(const ws_event_t *ev)
            {
                // Additionally generated event
                ws_event_t gen;
                gen.nType       = UIE_UNKNOWN;
    //            lsp_trace("ui_event type=%d", int(ev->nType));

                switch (ev->nType)
                {
                    case UIE_SHOW:
                    {
                        if (bWrapper)
                            break;

                        // Drop previously existed drawing surface
                        drop_surface();

                        // Create surface
                        Display *dpy    = pX11Display->x11display();
                        Visual *v       = DefaultVisual(dpy, screen());
                        pSurface        = new X11CairoSurface(dpy, hWindow, v, sSize.nWidth, sSize.nHeight);
                        break;
                    }

                    case UIE_HIDE:
                    {
                        if (bWrapper)
                            break;

                        // Drop previously existed drawing surface
                        drop_surface();
                        break;
                    }

                    case UIE_REDRAW:
                    {
//                        lsp_trace("redraw location = %d x %d, size = %d x %d",
//                                int(ev->nLeft), int(ev->nTop),
//                                int(ev->nWidth), int(ev->nHeight));
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
                        if (bWrapper)
                            break;

                        lsp_trace("new window location = %d x %d, size = %d x %d",
                                int(ev->nLeft), int(ev->nTop),
                                int(ev->nWidth), int(ev->nHeight));
                        sSize.nLeft         = ev->nLeft;
                        sSize.nTop          = ev->nTop;
                        sSize.nWidth        = ev->nWidth;
                        sSize.nHeight       = ev->nHeight;
                        if (pSurface != NULL)
                            pSurface->resize(sSize.nWidth, sSize.nHeight);
                        break;
                    }

                    case UIE_MOUSE_MOVE:
                    {
    //                    lsp_trace("mouse move = %d x %d", int(ev->nLeft), int(ev->nTop));
                        break;
                    }

                    case UIE_MOUSE_UP:
                    {
                        // Check that there was correct previous mouse up event
                        if (check_double_click(&vMouseUp[1], ev))
                        {
                            // Generate mouse double click event
                            gen.nType       = (check_double_click(&vMouseUp[0], &vMouseUp[1])) ? UIE_MOUSE_TRI_CLICK : UIE_MOUSE_DBL_CLICK;
                            gen.nLeft       = ev->nLeft;
                            gen.nTop        = ev->nTop;
                            gen.nWidth      = ev->nWidth;
                            gen.nHeight     = ev->nHeight;
                            gen.nCode       = ev->nCode;
                            gen.nState      = ev->nState;
                            gen.nTime       = ev->nTime;
                        }

                        // Do not allow series of triple-clicks
                        if (gen.nType != UIE_MOUSE_TRI_CLICK)
                        {
                            vMouseUp[0]         = vMouseUp[1];
                            vMouseUp[1]         = *ev;
                        }
                        else
                        {
                            vMouseUp[0].nType   = UIE_UNKNOWN;
                            vMouseUp[1].nType   = UIE_UNKNOWN;
                        }
                        break;
                    }

                    case UIE_CLOSE:
                    {
                        lsp_trace("close request on window");
                        if (pHandler == NULL)
                        {
                            this->destroy();
                            delete this;
                        }
                        break;
                    }

                    default:
                        break;
                }

                // Pass event to event handler
                if (pHandler != NULL)
                {
                    pHandler->handle_event(ev);
                    if (gen.nType != UIE_UNKNOWN)
                        pHandler->handle_event(&gen);
                }

                return STATUS_OK;
            }

            status_t X11Window::set_border_style(border_style_t style)
            {
                // Update state
                enBorderStyle = style;

                switch (style)
                {
                    case BS_DIALOG:
                        sMotif.decorations  = MWM_DECOR_BORDER | MWM_DECOR_TITLE;
                        sMotif.input_mode   = MWM_INPUT_APPLICATION_MODAL;
                        sMotif.status       = 0;
                        break;

                    case BS_NONE:
                    case BS_POPUP:
                    case BS_COMBO:
                        sMotif.decorations  = 0;
                        sMotif.input_mode   = MWM_INPUT_FULL_APPLICATION_MODAL;
                        sMotif.status       = 0;
                        break;

                    case BS_SINGLE:
                    case BS_SIZABLE:
                        sMotif.decorations  = MWM_DECOR_ALL;
                        sMotif.input_mode   = MWM_INPUT_MODELESS;
                        sMotif.status       = 0;
                        break;

                    default:
                        break;
                }

                if (hWindow == 0)
                {
                    nFlags |= F_SYNC_WM;
                    return STATUS_OK;
                }

                // Send changes to X11
                const x11_atoms_t &a = pX11Display->atoms();

                Atom atoms[32];
                int n_items = 0;

                // Set window type
                switch (style)
                {
                    case BS_DIALOG:
                        atoms[n_items++] = a.X11__NET_WM_WINDOW_TYPE_DIALOG;
                        atoms[n_items++] = a.X11__NET_WM_WINDOW_TYPE_NOTIFICATION;
                        break;

                    case BS_NONE:
                        break;

                    case BS_POPUP:
                        atoms[n_items++] = a.X11__NET_WM_WINDOW_TYPE_MENU;
                        atoms[n_items++] = a.X11__NET_WM_WINDOW_TYPE_POPUP_MENU;
                        break;

                    case BS_COMBO:
                        atoms[n_items++] = a.X11__NET_WM_WINDOW_TYPE_MENU;
                        atoms[n_items++] = a.X11__NET_WM_WINDOW_TYPE_POPUP_MENU;
                        atoms[n_items++] = a.X11__NET_WM_WINDOW_TYPE_COMBO;
                        break;

                    case BS_SINGLE:
                    case BS_SIZABLE:
                    default:
                        atoms[n_items++] = a.X11__NET_WM_WINDOW_TYPE_NORMAL;
                        break;
                }

                lsp_trace("Setting _NET_WM_WINDOW_TYPE...");
                XChangeProperty(
                    pX11Display->x11display(),
                    hWindow,
                    a.X11__NET_WM_WINDOW_TYPE,
                    a.X11_XA_ATOM,
                    32,
                    PropModeReplace,
                    reinterpret_cast<unsigned char *>(&atoms[0]),
                    n_items
                );

                // Set window state
                n_items = 0;
                switch (style)
                {
                    case BS_DIALOG:         // Not resizable; no minimize/maximize menu
                        atoms[n_items++] = a.X11__NET_WM_STATE_MODAL;
                        break;
                    case BS_NONE:           // Not resizable; no visible border line
                    case BS_POPUP:
                    case BS_COMBO:
                        atoms[n_items++] = a.X11__NET_WM_STATE_ABOVE;
                        break;

                    case BS_SINGLE:         // Not resizable; minimize/maximize menu
                    case BS_SIZABLE:       // Standard resizable border
                        break;
                }

                lsp_trace("Setting _NET_WM_STATE...");
                XChangeProperty(
                    pX11Display->x11display(),
                    hWindow,
                    a.X11__NET_WM_STATE,
                    a.X11_XA_ATOM,
                    32,
                    PropModeReplace,
                    reinterpret_cast<unsigned char *>(&atoms[0]),
                    n_items
                );


                // Set MOTIF hints
                lsp_trace("Setting _MOTIF_WM_HINTS...");
                XChangeProperty(
                    pX11Display->x11display(),
                    hWindow,
                    a.X11__MOTIF_WM_HINTS,
                    a.X11__MOTIF_WM_HINTS,
                    32,
                    PropModeReplace,
                    reinterpret_cast<unsigned char *>(&sMotif),
                    sizeof(sMotif)/sizeof(long)
                );


                lsp_trace("All seems to be OK");

                status_t result = do_update_constraints();
                pX11Display->flush();
                return result;
            }

            status_t X11Window::get_border_style(border_style_t *style)
            {
                if (style != NULL)
                    *style = enBorderStyle;
                return STATUS_OK;
            }

            ssize_t X11Window::left()
            {
                return sSize.nLeft;
            }

            ssize_t X11Window::top()
            {
                return sSize.nTop;
            }

            ssize_t X11Window::width()
            {
                return sSize.nWidth;
            }

            ssize_t X11Window::height()
            {
                return sSize.nHeight;
            }

            bool X11Window::is_visible()
            {
                return pSurface != NULL;
            }

            void *X11Window::handle()
            {
                return (void *)(hWindow);
            }

            size_t X11Window::screen()
            {
                return nScreen;
            }

            status_t X11Window::move(ssize_t left, ssize_t top)
            {
                if (hWindow == 0)
                    return STATUS_BAD_STATE;

                sSize.nLeft     = left;
                sSize.nTop      = top;

                lsp_trace("left=%d, top=%d", int(left), int(top));

//                if (hParent > 0)
//                    XMoveWindow(pX11Display->x11display(), hParent, sSize.nLeft, sSize.nTop);
//                else
                status_t result = do_update_constraints();
                if (hParent <= 0)
                    XMoveWindow(pX11Display->x11display(), hWindow, sSize.nLeft, sSize.nTop);
                if (result != STATUS_OK)
                    return result;
                pX11Display->flush();

                return STATUS_OK;
            }

            status_t X11Window::resize(ssize_t width, ssize_t height)
            {
                if (hWindow == 0)
                    return STATUS_BAD_STATE;

                sSize.nWidth    = width;
                sSize.nHeight   = height;

                calc_constraints(&sSize, &sSize);

                lsp_trace("width=%d, height=%d", int(width), int(height));

                status_t result = do_update_constraints();
                XResizeWindow(pX11Display->x11display(), hWindow, sSize.nWidth, sSize.nHeight);
//                if (hParent > 0)
//                    XResizeWindow(pX11Display->x11display(), hParent, sSize.nWidth, sSize.nHeight);
                if (result != STATUS_OK)
                    return result;
                pX11Display->flush();

                return STATUS_OK;
            }

            status_t X11Window::set_geometry(const realize_t *realize)
            {
                if (hWindow == 0)
                    return STATUS_BAD_STATE;

                calc_constraints(&sSize, realize);

                lsp_trace("left=%d, top=%d, width=%d, height=%d", int(sSize.nLeft), int(sSize.nTop), int(sSize.nWidth), int(sSize.nHeight));

                status_t result = do_update_constraints();
                if (hParent > 0)
                {
                    XResizeWindow(pX11Display->x11display(), hWindow, sSize.nWidth, sSize.nHeight);
//                    XMoveResizeWindow(pX11Display->x11display(), hParent, sSize.nLeft, sSize.nTop, sSize.nWidth, sSize.nHeight);
                }
                else
                    XMoveResizeWindow(pX11Display->x11display(), hWindow, sSize.nLeft, sSize.nTop, sSize.nWidth, sSize.nHeight);
                if (result != STATUS_OK)
                    return result;

                pX11Display->flush();

                return STATUS_OK;
            }

            status_t X11Window::get_geometry(realize_t *realize)
            {
                if (realize != NULL)
                    *realize    = sSize;
                return STATUS_OK;
            }

            status_t X11Window::get_absolute_geometry(realize_t *realize)
            {
                if (realize == NULL)
                    return STATUS_BAD_ARGUMENTS;
                if (hWindow == 0)
                {
                    realize->nLeft      = 0;
                    realize->nTop       = 0;
                    realize->nWidth     = sSize.nWidth;
                    realize->nHeight    = sSize.nHeight;
                    return STATUS_BAD_STATE;
                }

                int x, y;
                Window child;
                Display *dpy = pX11Display->x11display();
                // We do not trust XGetWindowAttributes since it can always return (0, 0) coordinates
                XTranslateCoordinates(dpy, hWindow, pX11Display->hRootWnd, 0, 0, &x, &y, &child);
                lsp_trace("xy = {%d, %d}", int(x), int(y));

                realize->nLeft      = x;
                realize->nTop       = y;
                realize->nWidth     = sSize.nWidth;
                realize->nHeight    = sSize.nHeight;

                return STATUS_OK;
            }

            status_t X11Window::hide()
            {
                if (hWindow == 0)
                    return STATUS_BAD_STATE;

                Display *dpy = pX11Display->x11display();
                if (nFlags & F_GRABBING)
                {
                    pX11Display->ungrab_events(this);
                    nFlags &= ~F_GRABBING;
                }
                if (nFlags & F_LOCKING)
                {
                    pX11Display->unlock_events(this);
                    nFlags &= ~F_LOCKING;
                }

                if (pSurface != NULL)
                    ::XUnmapWindow(dpy, hWindow);

                pX11Display->flush();
                return STATUS_OK;
            }

            status_t X11Window::ungrab_events()
            {
                if (!(nFlags & F_GRABBING))
                    return STATUS_NO_GRAB;
                return pX11Display->ungrab_events(this);
            }

            status_t X11Window::grab_events(grab_t group)
            {
                if (!(nFlags & F_GRABBING))
                {
                    pX11Display->grab_events(this, group);
                    nFlags |= F_GRABBING;
                }
                return STATUS_OK;
            }

            status_t X11Window::show(INativeWindow *over)
            {
                if (hWindow == 0)
                    return STATUS_BAD_STATE;
                if (pSurface != NULL)
                    return STATUS_OK;

                ::Window transient_for = None;
                X11Window *wnd = NULL;
                if (over != NULL)
                {
                    wnd = static_cast<X11Window *>(over);
                    if (wnd->hWindow > 0)
                        transient_for = wnd->hWindow;
                }

                lsp_trace("Showing window %lx as transient for %lx", hWindow, transient_for);
                ::XSetTransientForHint(pX11Display->x11display(), hWindow, transient_for);
                ::XRaiseWindow(pX11Display->x11display(), hWindow);
                ::XMapWindow(pX11Display->x11display(), hWindow);
                pX11Display->flush();
//                XWindowAttributes atts;
//                XGetWindowAttributes(pX11Display->x11display(), hWindow, &atts);
//                lsp_trace("window x=%d, y=%d", atts.x, atts.y);

                if (nFlags & F_SYNC_WM)
                {
                    nFlags      &= ~F_SYNC_WM;
                    set_border_style(enBorderStyle);
                    set_window_actions(nActions);
                };

                switch (enBorderStyle)
                {
                    case BS_POPUP:
                    case BS_COMBO:
//                        pX11Display->grab_events(this);
//                        nFlags |= F_GRABBING;
                        break;
                    case BS_DIALOG:
                        if (wnd != NULL)
                        {
                            pX11Display->lock_events(this, wnd);
                            nFlags |= F_LOCKING;
                        }
                        break;
                    default:
                        break;
                }
                return STATUS_OK;
            }

            status_t X11Window::show()
            {
                return show(NULL);
            }

            status_t X11Window::set_left(ssize_t left)
            {
                return move(left, sSize.nTop);
            }

            status_t X11Window::set_top(ssize_t top)
            {
                return move(sSize.nLeft, top);
            }

            ssize_t X11Window::set_width(ssize_t width)
            {
                return resize(width, sSize.nHeight);
            }

            ssize_t X11Window::set_height(ssize_t height)
            {
                return resize(sSize.nWidth, height);
            }

            status_t X11Window::set_size_constraints(const size_request_t *c)
            {
                sConstraints    = *c;

                calc_constraints(&sSize, &sSize);

                lsp_trace("width=%d, height=%d", int(sSize.nWidth), int(sSize.nHeight));

                XResizeWindow(pX11Display->x11display(), hWindow, sSize.nWidth, sSize.nHeight);
//                if (hParent > 0)
//                    XResizeWindow(pX11Display->x11display(), hParent, sSize.nWidth, sSize.nHeight);

                status_t result = do_update_constraints();
                if (result != STATUS_OK)
                    return result;
                pX11Display->flush();

                return STATUS_OK;
            }

            status_t X11Window::get_size_constraints(size_request_t *c)
            {
                *c = sConstraints;
                return STATUS_OK;
            }

            status_t X11Window::check_constraints()
            {
                realize_t rs;

                calc_constraints(&rs, &sSize);
                if ((rs.nWidth == sSize.nWidth) && (rs.nHeight == sSize.nHeight))
                    return STATUS_OK;

                lsp_trace("width=%d, height=%d", int(sSize.nWidth), int(sSize.nHeight));

                XResizeWindow(pX11Display->x11display(), hWindow, sSize.nWidth, sSize.nHeight);
//                if (hParent > 0)
//                    XResizeWindow(pX11Display->x11display(), hParent, sSize.nWidth, sSize.nHeight);
                pX11Display->flush();
                return STATUS_OK;
            }

            status_t X11Window::set_focus(bool focus)
            {
                if (hWindow == 0)
                {
                    lsp_trace("focusing window: bad handle");
                    return STATUS_BAD_STATE;
                }
                if (pSurface == NULL)
                {
                    lsp_trace("focusing window: bad surface");
                    return STATUS_OK;
                }

                lsp_trace("focusing window: focus=%s", (focus) ? "true" : "false");
                if (focus)
                    XSetInputFocus(pX11Display->x11display(), hWindow,  RevertToPointerRoot, CurrentTime);
                else
                    XSetInputFocus(pX11Display->x11display(), PointerRoot,  RevertToPointerRoot, CurrentTime);
                pX11Display->flush();
                return STATUS_OK;
            }

            status_t X11Window::toggle_focus()
            {
                if (hWindow == 0)
                    return STATUS_BAD_STATE;
                if (pSurface == NULL)
                    return STATUS_OK;

                Window wnd;
                int ret;
                XGetInputFocus(pX11Display->x11display(), &wnd, &ret);

                if (wnd != hWindow)
                    XSetInputFocus(pX11Display->x11display(), hWindow,  RevertToPointerRoot, CurrentTime);
                else
                    XSetInputFocus(pX11Display->x11display(), PointerRoot,  RevertToPointerRoot, CurrentTime);
                pX11Display->flush();
                return STATUS_OK;
            }


            status_t X11Window::set_caption(const char *ascii, const char *utf8)
            {
                if (ascii == NULL)
                    return STATUS_BAD_ARGUMENTS;
                if (hWindow == None)
                    return STATUS_OK;

                if (utf8 == NULL)
                    utf8 = ascii;

                const x11_atoms_t &a = pX11Display->atoms();

                ::XChangeProperty(
                    pX11Display->x11display(),
                    hWindow,
                    a.X11_XA_WM_NAME,
                    a.X11_XA_STRING,
                    8,
                    PropModeReplace,
                    reinterpret_cast<const unsigned char *>(ascii),
                    ::strlen(ascii)
                );
                ::XChangeProperty(
                    pX11Display->x11display(),
                    hWindow,
                    a.X11__NET_WM_NAME,
                    a.X11_UTF8_STRING,
                    8,
                    PropModeReplace,
                    reinterpret_cast<const unsigned char *>(utf8),
                    ::strlen(utf8)
                );
                ::XChangeProperty(
                    pX11Display->x11display(),
                    hWindow,
                    a.X11__NET_WM_ICON_NAME,
                    a.X11_UTF8_STRING,
                    8,
                    PropModeReplace,
                    reinterpret_cast<const unsigned char *>(utf8),
                    ::strlen(utf8)
                );

                pX11Display->flush();

                return STATUS_OK;
            }

            status_t X11Window::get_caption(char *text, size_t len)
            {
                if (len < 1)
                    return STATUS_TOO_BIG;

                unsigned long count = 0, left = 0;
                Atom ret;
                int fmt;
                unsigned char* data;

                const x11_atoms_t &a = pX11Display->atoms();

                int result = XGetWindowProperty(
                    pX11Display->x11display() /* display */,
                    hWindow /* window */,
                    a.X11__NET_WM_NAME /* property */,
                    0           /* long_offset */,
                    ~0L         /* long_length */,
                    False       /* delete */,
                    a.X11_UTF8_STRING  /* req_type */,
                    &ret        /* actual_type_return */,
                    &fmt        /* actual_format_return */,
                    &count      /* nitems_return */,
                    &left       /* bytes_after_return */,
                    &data       /* prop_return */
                );

                if (result != Success)
                    return STATUS_UNKNOWN_ERR;

                if ((ret != a.X11_UTF8_STRING) || (count <= 0) || (data == NULL))
                {
                    XFree(data);
                    text[0] = '\0';
                    return STATUS_OK;
                }
                else if (count >= len)
                {
                    XFree(data);
                    return STATUS_TOO_BIG;
                }

                memcpy(text, data, count);
                text[count] = '\0';
                return STATUS_OK;
            }

            status_t X11Window::set_icon(const void *bgra, size_t width, size_t height)
            {
                if (hWindow <= 0)
                    return STATUS_BAD_STATE;

                size_t n = width * height;
                unsigned long *buffer = new unsigned long [n + 2];
                if (buffer == NULL)
                    return STATUS_NO_MEM;

                buffer[0] = width;
                buffer[1] = height;

                const uint32_t *ptr = reinterpret_cast<const uint32_t *>(bgra);
                unsigned long *dst  = &buffer[2];

                for (size_t i=0; i<n; ++i)
                    *(dst++) = LE_TO_CPU(*(ptr++));

                const x11_atoms_t &a = pX11Display->atoms();

                XChangeProperty(
                        pX11Display->x11display(), hWindow,
                        a.X11__NET_WM_ICON, a.X11_XA_CARDINAL, 32, PropModeReplace,
                        reinterpret_cast<unsigned char *>(buffer), n + 2);

                delete [] buffer; // Drop buffer

                return STATUS_OK;
            }

            status_t X11Window::get_window_actions(size_t *actions)
            {
                if (actions == NULL)
                    return STATUS_BAD_ARGUMENTS;
                *actions = nActions;
                return STATUS_OK;
            }

            status_t X11Window::set_window_actions(size_t actions)
            {
                nActions            = actions;

                // Update legacy _MOTIF_WM_HINTS
                sMotif.functions    = 0;

                // Functions
                if (actions & WA_MOVE)
                    sMotif.functions       |= MWM_FUNC_MOVE;
                if (actions & WA_RESIZE)
                    sMotif.functions       |= MWM_FUNC_RESIZE;
                if (actions & WA_MINIMIZE)
                    sMotif.functions       |= MWM_FUNC_MINIMIZE;
                if (actions & WA_MAXIMIZE)
                    sMotif.functions       |= MWM_FUNC_MAXIMIZE;
                if (actions & WA_CLOSE)
                    sMotif.functions       |= MWM_FUNC_CLOSE;

                if (hWindow == 0)
                {
                    nFlags |= F_SYNC_WM;
                    return STATUS_OK;
                }

                // Set window actions
                Atom atoms[10];
                size_t n_items = 0;
                const x11_atoms_t &a = pX11Display->atoms();

                // Update _NET_WM_ALLOWED_ACTIONS
                #define TR_ACTION(from, to)    \
                    if (actions & from) \
                        atoms[n_items++] = a.X11__NET_WM_ACTION_ ## to;

                TR_ACTION(WA_MOVE, MOVE);
                TR_ACTION(WA_RESIZE, RESIZE);
                TR_ACTION(WA_MINIMIZE, MINIMIZE);
                TR_ACTION(WA_MAXIMIZE, MAXIMIZE_HORZ);
                TR_ACTION(WA_MAXIMIZE, MAXIMIZE_VERT);
                TR_ACTION(WA_CLOSE, CLOSE);
                TR_ACTION(WA_STICK, STICK);
                TR_ACTION(WA_SHADE, SHADE);
                TR_ACTION(WA_FULLSCREEN, FULLSCREEN);
                TR_ACTION(WA_CHANGE_DESK, CHANGE_DESKTOP);

                #undef TR_ACTION

                lsp_trace("Setting _NET_WM_ALLOWED_ACTIONS...");
                XChangeProperty(
                    pX11Display->x11display(),
                    hWindow,
                    a.X11__NET_WM_ALLOWED_ACTIONS,
                    a.X11_XA_ATOM,
                    32,
                    PropModeReplace,
                    reinterpret_cast<unsigned char *>(&atoms[0]),
                    n_items
                );


                // Send _MOTIF_WM_HINTS
                lsp_trace("Setting _MOTIF_WM_HINTS...");
                XChangeProperty(
                    pX11Display->x11display(),
                    hWindow,
                    a.X11__MOTIF_WM_HINTS,
                    a.X11__MOTIF_WM_HINTS,
                    32,
                    PropModeReplace,
                    reinterpret_cast<unsigned char *>(&sMotif),
                    sizeof(sMotif)/sizeof(long)
                );

                pX11Display->flush();

                return STATUS_OK;
            }

            status_t X11Window::set_mouse_pointer(mouse_pointer_t pointer)
            {
                if (hWindow <= 0)
                    return STATUS_BAD_STATE;

                Cursor cur = pX11Display->get_cursor(pointer);
                if (cur == None)
                    return STATUS_UNKNOWN_ERR;
                XDefineCursor(pX11Display->x11display(), hWindow, cur);
                XFlush(pX11Display->x11display());
                enPointer = pointer;
                return STATUS_OK;
            }

            mouse_pointer_t X11Window::get_mouse_pointer()
            {
                return enPointer;
            }

            status_t X11Window::set_class(const char *instance, const char *wclass)
            {
                if ((instance == NULL) || (wclass == NULL))
                    return STATUS_BAD_ARGUMENTS;

                size_t l1 = ::strlen(instance);
                size_t l2 = ::strlen(wclass);

                char *dup = reinterpret_cast<char *>(::malloc((l1 + l2 + 2) * sizeof(char)));
                if (dup == NULL)
                    return STATUS_NO_MEM;

                ::memcpy(dup, instance, l1+1);
                ::memcpy(&dup[l1+1], wclass, l2+1);

                const x11_atoms_t &a = pX11Display->atoms();
                ::XChangeProperty(
                    pX11Display->x11display(),
                    hWindow,
                    a.X11_XA_WM_CLASS,
                    a.X11_XA_STRING,
                    8,
                    PropModeReplace,
                    reinterpret_cast<unsigned char *>(dup),
                    (l1 + l2 + 2)
                );

                ::free(dup);
                return STATUS_OK;
            }

            status_t X11Window::set_role(const char *wrole)
            {
                if (wrole == NULL)
                    return STATUS_BAD_ARGUMENTS;

                const x11_atoms_t &a = pX11Display->atoms();
                ::XChangeProperty(
                    pX11Display->x11display(),
                    hWindow,
                    a.X11_WM_WINDOW_ROLE,
                    a.X11_XA_STRING,
                    8,
                    PropModeReplace,
                    reinterpret_cast<const unsigned char *>(wrole),
                    ::strlen(wrole)
                );

                return STATUS_OK;
            }
        }
    } /* namespace ws */
} /* namespace lsp */

/* Example of changing window's icon
 https://stackoverflow.com/questions/10699927/xlib-argb-window-icon

    // gcc -Wall -Wextra -Og -lX11 -lstdc++ -L/usr/X11/lib -o native foo.c
    #include <stdlib.h>
    #include <X11/Xlib.h>
    int main( int argc, char **argv )
    {
        unsigned long buffer[] = {
                16, 16,
                4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 338034905, 3657433343, 0, 184483840, 234881279, 3053453567, 3221225727, 1879048447, 0, 0, 0, 0, 0, 0, 0, 1224737023, 3305111807, 3875537151,0, 0, 2063597823, 1291845887, 0, 67109119, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 50266112, 3422552319, 0, 0, 3070230783, 2063597823, 2986344703, 771752191, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3422552319, 0, 0, 3372220671, 1509949695, 704643327, 3355443455, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 0, 3422552319, 0, 134152192, 3187671295, 251658495, 0, 3439329535, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3422552319, 0, 0, 2332033279, 1342177535, 167772415, 3338666239, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 0, 3422552319, 0, 0, 436207871, 3322085628, 3456106751, 1375731967, 4278255360, 4026597120, 3758161664, 3489726208, 3204513536, 2952855296, 2684419840, 2399207168, 2130771712, 1845559040, 1593900800, 1308688128, 1040252672, 755040000, 486604544, 234946304, 4278255360, 4043374336, 3774938880, 3506503424, 3221290752, 2952855296, 2667642624, 2399207168, 2130771712, 1862336256, 1627453957, 1359017481, 1073805064, 788591627, 503379721, 218169088, 4278255360, 4043374336, 3758161664, 3506503424, 3221290752, 2952855296, 2684419840, 2415984384, 2130771712, 1862336256, 1577123584, 1308688128, 1040252672, 755040000, 486604544, 218169088, 4278190335, 4026532095, 3758096639, 3489661183, 3221225727, 2952790271, 2667577599, 2415919359, 2130706687, 1862271231, 1593835775, 1325400319, 1056964863, 771752191, 520093951, 234881279, 4278190335, 4026532095, 3758096639, 3489661183, 3221225727, 2952790271, 2667577599, 2415919359, 2130706687, 1862271231, 1593835775, 1325400319, 1056964863, 771752191, 503316735, 234881279, 4278190335, 4026532095, 3758096639, 3489661183, 3221225727, 2952790271, 2684354815, 2399142143, 2130706687, 1862271231, 1593835775, 1325400319, 1040187647, 771752191, 520093951, 234881279, 4294901760, 4043243520, 3774808064, 3506372608, 3221159936, 2952724480, 2684289024, 2399076352, 2147418112, 1862205440, 1593769984, 1308557312, 1040121856, 771686400, 503250944, 234815488, 4294901760, 4060020736, 3758030848, 3506372608, 3221159936, 2952724480, 2684289024, 2415853568, 2130640896, 1862205440, 1593769984, 1308557312, 1040121856, 771686400, 503250944, 234815488, 4294901760, 4043243520, 3774808064, 3489595392, 3237937152, 2952724480, 2684289024, 2415853568, 2147418112, 1862205440, 1593769984, 1325334528, 1056899072, 788463616, 503250944, 234815488,
                32, 32,
                4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 0, 0, 0, 0, 0, 0, 0, 0, 0, 268369920, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 1509949695, 3120562431, 4009754879, 4194304255, 3690987775, 2130706687, 83886335, 0, 50331903, 1694499071, 3170894079, 3992977663, 4211081471, 3657433343, 1879048447, 16777471, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3087007999, 2281701631, 1191182591, 1040187647, 2030043391, 4127195391, 2566914303, 0, 16777471, 3254780159, 2181038335, 1191182591, 973078783, 2030043391,4177527039, 2130706687, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 0, 0, 0, 0, 0, 2214592767, 4093640959, 0, 0, 0, 0, 0, 0, 0, 2298478847, 3909091583, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2214592767, 3607101695, 0, 0, 0, 0, 0, 0, 0, 1946157311, 4093640959, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 0, 0, 536871167, 1191182591, 2281701631,3019899135, 637534463, 0, 0, 0, 100597760, 251592704, 33488896, 0, 3321889023, 2919235839, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2550137087, 4278190335, 4278190335, 3405775103, 570425599, 0, 0, 0, 0, 0, 0, 2046820607, 4043309311, 620757247, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 33488896, 0, 0, 218104063, 1291845887, 3841982719, 3388997887, 0, 0, 0, 0, 0, 1996488959, 4093640959, 1073742079, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1761607935, 4278190335, 150995199, 0, 0, 67109119, 2550137087, 3909091583, 889192703, 0, 0, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 0, 0, 0, 0, 0, 2181038335, 3925868799, 0, 0, 218104063, 3070230783, 3623878911, 570425599, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 805306623, 3288334591, 1795162367, 1040187647, 1023410431, 2231369983, 4211081471, 1694499071, 0, 369099007, 3456106751, 3825205503, 1174405375, 872415487, 872415487, 872415487, 872415487, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4293984270, 2046951677, 3422552319, 4110418175, 4177527039, 3405775103, 1409286399, 0, 0, 1409286399, 4278190335, 4278190335, 4278190335, 4278190335, 4278190335, 4278190335, 4278190335, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760,4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 4294901760, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4278255360, 4144037632, 4009819904, 3875602176, 3741384448, 3607166720, 3472948992, 3338731264, 3204513536, 3053518592, 2936078080, 2801860352, 2650865408, 2516647680, 2382429952, 2264989440, 2113994496, 1996553984, 1862336256, 1728118528, 1577123584, 1459683072, 1325465344, 1191247616, 1040252672, 922812160, 771817216, 637599488, 503381760, 385941248, 234946304, 100728576, 4278255360, 4144037632, 4009819904, 3875602176, 3724607232, 3607166720, 3472948992, 3338731264, 3204513536, 3070295808, 2936078080, 2801860352, 2667642624, 2516647680, 2399207168, 2264989440, 2130771712, 1996553984, 1845559040, 1728118528, 1593900800, 1459683072, 1308688128, 1191247616, 1057029888, 922812160, 788594432, 637599488, 503381760, 369164032, 234946304, 117505792, 4278255360, 4144037632, 4009819904, 3875602176, 3741384448, 3607166720, 3472948992, 3338731264, 3204513536, 3053518592, 2919300864, 2801860352, 2650865408, 2533424896, 2399207168, 2264989440, 2113994496, 1996553984, 1862336256, 1728118528,1593900800, 1459683072, 1325465344, 1191247616, 1040252672, 906034944, 771817216, 654376704, 503381760, 369164032, 234946304, 117505792, 4278255360, 4144037632, 4009819904, 3858824960, 3741384448, 3607166720, 3472948992, 3338731264, 3204513536, 3070295808, 2936078080, 2801860352, 2667642624, 2533424896, 2382429952, 2264989440, 2130771712, 1979776768, 1862336256, 1728118528, 1577123584, 1442905856, 1325465344, 1191247616, 1040252672, 922812160, 771817216, 637599488, 503381760, 369164032, 234946304, 100728576, 4278255360, 4144037632, 4009819904, 3875602176, 3741384448, 3607166720, 3472948992, 3338731264, 3204513536, 3070295808, 2919300864, 2801860352, 2667642624, 2533424896, 2399207168, 2264989440, 2113994496, 1996553984, 1862336256, 1728118528, 1593900800, 1442905856, 1342241795, 1174470400, 1057029888, 906034944, 788594432, 654376704, 503381760, 385941248, 251723520, 100728576, 4278190335, 4160749823, 4026532095, 3892314367, 3741319423, 3623878911, 3472883967, 3338666239, 3221225727, 3070230783, 2952790271, 2818572543, 2667577599, 2533359871, 2399142143, 2264924415, 2147483903, 1996488959, 1862271231, 1728053503, 1593835775, 1459618047, 1325400319, 1191182591, 1056964863, 922747135, 788529407, 654311679, 520093951,385876223, 251658495, 117440767, 4278190335, 4160749823, 4026532095, 3892314367, 3741319423, 3623878911, 3489661183, 3355443455, 3221225727, 3087007999, 2936013055, 2801795327, 2667577599, 2533359871, 2399142143, 2281701631, 2130706687, 1996488959, 1862271231, 1728053503, 1593835775,1459618047, 1325400319, 1191182591, 1056964863, 922747135, 788529407, 654311679, 520093951, 385876223, 234881279, 100663551, 4278190335, 4160749823, 4026532095, 3892314367, 3758096639, 3623878911, 3489661183, 3355443455, 3221225727, 3087007999, 2936013055, 2801795327, 2667577599, 2550137087, 2415919359, 2264924415, 2130706687, 1996488959, 1862271231, 1728053503, 1593835775, 1459618047, 1325400319, 1191182591, 1056964863, 922747135, 788529407, 654311679, 503316735, 369099007, 251658495, 100663551, 4278190335, 4160749823, 4026532095, 3892314367, 3758096639, 3623878911, 3489661183, 3355443455, 3204448511, 3087007999, 2936013055, 2818572543, 2667577599, 2533359871, 2399142143, 2264924415, 2130706687, 1996488959, 1879048447, 1728053503, 1593835775, 1459618047, 1325400319, 1191182591, 1056964863, 922747135, 788529407, 654311679, 520093951, 385876223, 251658495, 117440767, 4278190335, 4160749823, 4026532095, 3892314367, 3758096639, 3623878911, 3489661183, 3355443455, 3221225727, 3087007999, 2952790271, 2818572543, 2667577599, 2533359871, 2399142143, 2264924415, 2147483903, 2013266175, 1862271231, 1744830719, 1610612991, 1476395263, 1342177535, 1191182591, 1056964863, 922747135, 788529407, 654311679, 520093951, 385876223, 251658495, 100663551, 4294901760, 4160684032, 4026466304, 3909025792, 3774808064, 3623813120, 3489595392, 3355377664, 3237937152, 3103719424, 2952724480, 2818506752, 2684289024, 2550071296, 2415853568, 2281635840, 2147418112, 2013200384, 1878982656, 1744764928, 1593769984, 1476329472,1325334528, 1207894016, 1056899072, 939458560, 788463616, 654245888, 520028160, 385810432, 251592704, 117374976, 4294901760, 4177461248, 4043243520, 3909025792, 3774808064, 3640590336, 3506372608, 3355377664, 3221159936, 3086942208, 2952724480, 2818506752, 2701066240, 2550071296, 2415853568, 2281635840, 2147418112, 2013200384, 1878982656, 1727987712, 1610547200, 1476329472, 1325334528, 1191116800, 1073676288, 922681344, 788463616, 654245888, 520028160, 385810432, 251592704, 100597760, 4294901760, 4177461248, 4043243520, 3909025792, 3774808064, 3640590336, 3489595392, 3372154880, 3237937152, 3103719424, 2952724480, 2818506752, 2700935170, 2550071296, 2415853568, 2281635840, 2147418112, 2013200384, 1878982656, 1744764928, 1610547200, 1459552256, 1342111744, 1191116800, 1056899072, 922681344, 788463616, 671023104, 520028160, 385810432, 251592704, 100597760, 4294901760, 4177461248, 4043243520, 3909025792, 3774808064, 3640590336, 3489595392, 3372154880, 3237937152, 3086942208, 2969501696, 2818506752, 2684289024, 2550071296, 2432630784, 2281635840, 2147418112, 2013200384, 1862205440, 1744764928, 1610547200, 1476329472, 1342111744, 1191116800, 1056899072, 922681344, 788463616, 654245888, 520028160, 385810432, 251592704, 117374976, 4294901760, 4177461248, 4043243520, 3909025792, 3774808064, 3623813120, 3506372608, 3372154880, 3237937152, 3103719424, 2952724480, 2835283968, 2684289024, 2550071296, 2432630784, 2281635840, 2147418112, 2046492676, 1862205440, 1744764928, 1610547200, 1476329472, 1342111744,1207894016, 1056899072, 939458560, 788463616, 654245888, 536281096, 385810432, 251592704, 134152192,
        };
        Display *d = XOpenDisplay(0);
        int s = DefaultScreen(d);
        Atom net_wm_icon = XInternAtom(d, "_NET_WM_ICON", False);
        Atom cardinal = XInternAtom(d, "CARDINAL", False);
        Window w;
        XEvent e; w = XCreateWindow(d, RootWindow(d, s), 0, 0, 200, 200, 0,
        CopyFromParent, InputOutput, CopyFromParent, 0, 0);
        int length = 2 + 16 * 16 + 2 + 32 * 32;
        XChangeProperty(d, w, net_wm_icon, cardinal, 32, PropModeReplace, (const unsigned char*) buffer, length);
        XMapWindow(d, w);
        while(1)
        XNextEvent(d, &e);
        (void)argc, (void)argv;
    }
 */

#endif /* USE_X11_DISPLAY */
