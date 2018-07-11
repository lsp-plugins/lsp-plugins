/*
 * UICore.cpp
 *
 *  Created on: 10 окт. 2016 г.
 *      Author: sadko
 */

#include <ui/x11/ui.h>
#include <sys/poll.h>
#include <core/status.h>

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
            sLastRender.tv_nsec = 0;
            sLastRender.tv_sec  = 0;
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

            // Initialize atoms
            int result = init_atoms(pDisplay, &sAtoms);
            if (result != STATUS_SUCCESS)
                return result;

            return STATUS_SUCCESS;
        }

        void X11Core::destroy()
        {
        }

        int X11Core::main()
        {
            // Make a pause
            struct pollfd x11_poll;
            struct timespec ts;

            int x11_fd          = ConnectionNumber(pDisplay);
            XSync(pDisplay, false);

            while (!bExit)
            {
                // Get current time
                clock_gettime(CLOCK_REALTIME, &ts);
                ssize_t dmsec   = (ts.tv_nsec - sLastRender.tv_nsec) / 1000000;
                ssize_t dsec    = (ts.tv_sec - sLastRender.tv_sec);
                dmsec          += dsec * 1000;
                bool force      = dmsec > 40; // each 40 msec render request

                // Try to poll input data for a 100 msec period
                x11_poll.fd         = x11_fd;
                x11_poll.events     = POLLIN | POLLPRI | POLLHUP;
                x11_poll.revents    = 0;

                errno               = 0;
                int poll_res = poll(&x11_poll, 1, 40);
                if (poll_res < 0)
                {
                    int err_code = errno;
                    lsp_trace("Poll returned error: %d, code=%d", poll_res, err_code);
                    if (err_code != EINTR)
                        return -1;
                }
                else if ((force) || ((poll_res > 0) && (x11_poll.revents > 0)))
                {
                    // Do iteration
                    int result = do_main_iteration(force);
                    if (result < 0)
                        return result;
                    // Store last render time
                    if (force)
                        sLastRender = ts;
                }
            }

            return 0;
        }

        int X11Core::do_main_iteration(bool redraw)
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

        int X11Core::main_iteration()
        {
            return do_main_iteration(true);
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

                case ClientMessage:
                    if (ev->xclient.message_type == sAtoms.X11_WM_PROTOCOLS)
                    {
                        if (ev->xclient.data.l[0] == long(sAtoms.X11_WM_DELETE_WINDOW))
                            ue.nType        = UIE_CLOSE;
                    }
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

        bool X11Core::addWindow(X11Window *wnd)
        {
            return vWindows.add(wnd);
        }

        bool X11Core::removeWindow(X11Window *wnd)
        {
            if (!vWindows.remove(wnd))
                return false;

            // Call window destroy
            wnd->destroy();
            delete wnd;

            // Check if need to leave main cycle
            if (vWindows.size() <= 0)
                bExit = true;
            return true;
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


