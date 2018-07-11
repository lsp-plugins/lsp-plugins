/*
 * UICore.cpp
 *
 *  Created on: 10 окт. 2016 г.
 *      Author: sadko
 */

#include <ui/x11/ui.h>

namespace lsp
{
    namespace x11ui
    {
        X11Core::X11Core()
        {
            bExit           = false;
            pDisplay        = NULL;
            hRootWnd        = -1;
            hDflScreen      = -1;
            nBlackColor     = 0;
            nWhiteColor     = 0;
        }

        X11Core::~X11Core()
        {
        }

        int X11Core::init(int argc, const char **argv)
        {
            // Open the display
            pDisplay        = XOpenDisplay(NULL);
            if (pDisplay == NULL)
            {
                lsp_error("Can not open display");
                return -1;
            }

            // Get Root window and screen
            hRootWnd        = DefaultRootWindow(pDisplay);
            hDflScreen      = DefaultScreen(pDisplay);
            nBlackColor     = BlackPixel(pDisplay, hDflScreen);
            nWhiteColor     = WhitePixel(pDisplay, hDflScreen);

            // Return success
            return 0;
        }

        void X11Core::destroy()
        {
        }

        int X11Core::main()
        {
            while (!bExit)
            {
                // Do iteration
                int result = main_iteration();
                if (result < 0)
                    return result;

                // Make a pause
            }

            return 0;
        }

        int X11Core::main_iteration()
        {
            XEvent event;
            int pending = XPending(pDisplay);

            for (int i=0; i<pending; i++)
            {
                if (XNextEvent(pDisplay, &event) != Success)
                {
                    lsp_error("Failed to fetch next event");
                    return -1;
                }

                handleEvent(&event);
            }

            // Flush & sync display
            XFlush(pDisplay);
            XSync(pDisplay, False);

            // Return number of processed events
            return pending;
        }

        void X11Core::x11sync()
        {
            // Flush & sync display
            XFlush(pDisplay);
            XSync(pDisplay, False);
        }

        void X11Core::handleEvent(XEvent *ev)
        {
            lsp_trace("Received event: %d (%s)", int(ev->type), event_name(ev->type));

            ui_event_t ue;
            ue.nType    = UIE_UNKNOWN;
            ue.nLeft    = 0;
            ue.nTop     = 0;
            ue.nWidth   = 0;
            ue.nHeight  = 0;
            ue.nCode    = 0;
            ue.nState   = 0;

            // Decode event
            switch (ev->type)
            {
                case KeyPress:
                case KeyRelease:
                    ue.nType    = (ev->type == KeyPress) ? UIE_KEY_DOWN : UIE_KEY_UP;
                    ue.nLeft    = ev->xkey.x;
                    ue.nTop     = ev->xkey.y;
                    ue.nState   = ev->xkey.state;
                    ue.nCode    = ev->xkey.keycode;
                    break;

                case ButtonPress:
                case ButtonRelease:
                    ue.nType    = (ev->type == ButtonPress) ? UIE_MOUSE_DOWN : UIE_MOUSE_UP;
                    ue.nLeft    = ev->xbutton.x;
                    ue.nTop     = ev->xbutton.y;
                    ue.nState   = ev->xbutton.state;
                    ue.nCode    = ev->xbutton.button;
                    break;

                case MotionNotify:
                    ue.nType    = UIE_MOUSE_MOVE;
                    ue.nLeft    = ev->xmotion.x;
                    ue.nTop     = ev->xmotion.y;
                    ue.nState   = ev->xmotion.state;
                    ue.nCode    = 0;
                    break;

                case Expose:
                    ue.nType    = UIE_REDRAW;
                    ue.nLeft    = ev->xexpose.x;
                    ue.nTop     = ev->xexpose.y;
                    ue.nWidth   = ev->xexpose.width;
                    ue.nHeight  = ev->xexpose.height;
                    break;

                case ResizeRequest:
                    ue.nType    = UIE_SIZE_REQUEST;
                    ue.nWidth   = ev->xresizerequest.width;
                    ue.nHeight  = ev->xresizerequest.height;
                    break;

                case ConfigureNotify:
                    ue.nType    = UIE_RESIZE;
                    ue.nLeft    = ev->xconfigure.x;
                    ue.nTop     = ev->xconfigure.y;
                    ue.nWidth   = ev->xconfigure.width;
                    ue.nHeight  = ev->xconfigure.height;
                    break;

                case MapNotify:
                    ue.nType    = UIE_SHOW;
                    break;
                case UnmapNotify:
                    ue.nType    = UIE_HIDE;
                    break;

                default:
                    return;
            }

            // Analyze event type
            if (ue.nType == UIE_UNKNOWN)
                return;

            // Find relative window and pass event to it
            size_t nwnd = vWindows.size();
            for (size_t i=0; i<nwnd; ++i)
            {
                X11Window *wnd = vWindows[i];
                if (wnd == NULL)
                    continue;
                wnd->handleEvent(&ue);
            }
        }

        void X11Core::quit_main()
        {
            bExit = true;
        }

        X11Window *X11Core::createWindow(size_t width, size_t height)
        {
            // Try to create window
            Window wnd = XCreateSimpleWindow(pDisplay, hRootWnd, 0, 0, width, height, 0, 0, 0);
            if (wnd <= 0)
                return NULL;

            // Now select input for new handle
            XSelectInput(pDisplay, wnd,
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
                VisibilityChangeMask |
                StructureNotifyMask |
                ResizeRedirectMask |
                SubstructureNotifyMask |
                SubstructureRedirectMask |
                FocusChangeMask |
                PropertyChangeMask |
                ColormapChangeMask |
                OwnerGrabButtonMask
            );

            // Now create X11Window instance
            X11Window *x11wnd = new X11Window(this, wnd, width, height);
            if (x11wnd != NULL)
            {
                // Add window to list
                if (vWindows.add(x11wnd))
                {
                    XFlush(pDisplay);
                    return x11wnd;
                }
                delete x11wnd;
            }

            XDestroyWindow(pDisplay, wnd);
            return NULL;
        }

        const char *X11Core::event_name(int xev_code)
        {
            #define E(x) case x: return #x;
            switch (xev_code)
            {
                E(KeyPress)
                E(KeyRelease)
                E(ButtonPress)
                E(ButtonRelease)
                E(MotionNotify)
                E(EnterNotify)
                E(LeaveNotify)
                E(FocusIn)
                E(FocusOut)
                E(KeymapNotify)
                E(Expose)
                E(GraphicsExpose)
                E(NoExpose)
                E(VisibilityNotify)
                E(CreateNotify)
                E(DestroyNotify)
                E(UnmapNotify)
                E(MapNotify)
                E(MapRequest)
                E(ReparentNotify)
                E(ConfigureNotify)
                E(ConfigureRequest)
                E(GravityNotify)
                E(ResizeRequest)
                E(CirculateNotify)
                E(CirculateRequest)
                E(PropertyNotify)
                E(SelectionClear)
                E(SelectionRequest)
                E(SelectionNotify)
                E(ColormapNotify)
                E(ClientMessage)
                E(MappingNotify)
                E(GenericEvent)
                default: return "Unknown";
            }
            return "Unknown";
            #undef E
        }
    }
}


