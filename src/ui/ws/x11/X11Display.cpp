/*
 * UICore.cpp
 *
 *  Created on: 10 окт. 2016 г.
 *      Author: sadko
 */

#include <ui/ws/x11/ws.h>

#ifdef USE_X11_DISPLAY

#include <sys/poll.h>
#include <errno.h>
#include <stdlib.h>

#define X11IOBUF_SIZE       0x10000

namespace lsp
{
    namespace ws
    {
        namespace x11
        {
            static unsigned int cursor_shapes[] =
            {
                (unsigned int)(-1), // MP_NONE
                XC_left_ptr, // MP_ARROW +++
                XC_hand1, // MP_HAND +++
                XC_cross, // MP_CROSS +++
                XC_xterm, // MP_IBEAM +++
                XC_pencil, // MP_DRAW +++
                XC_plus, // MP_PLUS +++
                XC_bottom_right_corner, // MP_SIZE_NESW ---
                XC_sb_v_double_arrow, // MP_SIZE_NS +++
                XC_sb_h_double_arrow, // MP_SIZE_WE +++
                XC_bottom_left_corner, // MP_SIZE_NWSE ---
                XC_center_ptr, // MP_UP_ARROW ---
                XC_watch, // MP_HOURGLASS +++
                XC_fleur, // MP_DRAG +++
                XC_circle, // MP_NO_DROP +++
                XC_pirate, // MP_DANGER +++
                XC_right_side, // MP_HSPLIT ---
                XC_bottom_side, // MP_VPSLIT ---
                XC_exchange, // MP_MULTIDRAG ---
                XC_watch, // MP_APP_START ---
                XC_question_arrow // MP_HELP +++
            };

// Cursors matched to KDE:
//XC_X_cursor,
//XC_left_ptr,
//XC_hand1,
//XC_hand2,
//XC_cross,
//XC_xterm,
//XC_pencil,
//XC_plus,
//XC_sb_v_double_arrow,
//XC_sb_h_double_arrow,
//XC_watch,
//XC_fleur,
//XC_circle,
//XC_pirate,
//XC_question_arrow,

// Possible useful cursors:
//XC_based_arrow_down,
//XC_based_arrow_up,
//XC_bottom_left_corner,
//XC_bottom_right_corner,
//XC_bottom_side,
//XC_bottom_tee,
//XC_center_ptr,
//XC_cross_reverse,
//XC_crosshair,
//XC_double_arrow,
//XC_exchange,
//XC_left_side,
//XC_left_tee,
//XC_ll_angle,
//XC_lr_angle,
//XC_right_side,
//XC_right_tee,
//XC_sb_down_arrow,
//XC_sb_left_arrow,
//XC_sb_right_arrow,
//XC_sb_up_arrow,
//XC_tcross,
//XC_top_left_corner,
//XC_top_right_corner,
//XC_top_side,
//XC_top_tee,
//XC_ul_angle,
//XC_ur_angle,



            X11Display::X11Display()
            {
                bExit           = false;
                pDisplay        = NULL;
                hRootWnd        = -1;
                hClipWnd        = None;
                nBlackColor     = 0;
                nWhiteColor     = 0;
                pIOBuf          = NULL;

                for (size_t i=0; i<_CBUF_TOTAL; ++i)
                    pClipboard[i]           = NULL;
            }

            X11Display::~X11Display()
            {
                do_destroy();
            }

            int X11Display::init(int argc, const char **argv)
            {
                // Enable multi-threading
                XInitThreads();

                // Open the display
                pDisplay        = XOpenDisplay(NULL);
                if (pDisplay == NULL)
                {
                    lsp_error("Can not open display");
                    return STATUS_NO_DEVICE;
                }

                // Get Root window and screen
                hRootWnd        = DefaultRootWindow(pDisplay);
                int dfl         = DefaultScreen(pDisplay);
                nBlackColor     = BlackPixel(pDisplay, dfl);
                nWhiteColor     = WhitePixel(pDisplay, dfl);

                pIOBuf          = new uint8_t[X11IOBUF_SIZE];

                // Create invisible clipboard window
                hClipWnd        = XCreateWindow(pDisplay, hRootWnd, 0, 0, 1, 1, 0, 0, CopyFromParent, CopyFromParent, 0, NULL);
                if (hClipWnd == None)
                    return STATUS_UNKNOWN_ERR;

                // Initialize atoms
                int result = init_atoms(pDisplay, &sAtoms);
                if (result != STATUS_SUCCESS)
                    return result;

                // Initialize cursors
                for (size_t i=0; i<__MP_COUNT; ++i)
                {
                    unsigned int id = cursor_shapes[i];
                    if (id == (unsigned int)(-1))
                    {
                        Pixmap blank;
                        XColor dummy;
                        char data[1] = {0};

                        /* make a blank cursor */
                        blank = XCreateBitmapFromData (pDisplay, hRootWnd, data, 1, 1);
                        if (blank == None)
                            return STATUS_NO_MEM;
                        vCursors[i] = XCreatePixmapCursor(pDisplay, blank, blank, &dummy, &dummy, 0, 0);
                        XFreePixmap(pDisplay, blank);
                    }
                    else
                        vCursors[i] = XCreateFontCursor(pDisplay, id);
                }

                return IDisplay::init(argc, argv);
            }

            INativeWindow *X11Display::createWindow()
            {
                return new X11Window(this, DefaultScreen(pDisplay), 0);
            }

            INativeWindow *X11Display::createWindow(size_t screen)
            {
                return new X11Window(this, screen, 0);
            }

            INativeWindow *X11Display::createWindow(void *handle)
            {
                lsp_trace("handle = %p", handle);
                return new X11Window(this, DefaultScreen(pDisplay), Window(uintptr_t(handle)));
            }

            ISurface *X11Display::createSurface(size_t width, size_t height)
            {
                return new X11CairoSurface(width, height);
            }

            void X11Display::do_destroy()
            {
                for (size_t i=0; i< vWindows.size(); )
                {
                    X11Window *wnd  = vWindows.at(i);
                    if (wnd != NULL)
                    {
                        wnd->destroy();
                        wnd = NULL;
                    }
                    else
                        i++;
                }

                if (hClipWnd != None)
                {
                    XDestroyWindow(pDisplay, hClipWnd);
                    hClipWnd = None;
                }

                size_t n            = sCbRequests.size();
                for (size_t i=0; i < n; ++i)
                {
                    cb_request_t *req   = sCbRequests.at(i);

                    // Remove the request
                    if (req->pIn != NULL)
                    {
                        req->pIn->close();
                        delete req->pIn;
                        req->pIn = NULL;
                    }
                    if (req->pCB != NULL)
                        req->pCB->close();
                }

                sCbRequests.flush();
                vWindows.flush();
                sPending.flush();
                sGrab.clear();
                sTargets.clear();

                if (pIOBuf != NULL)
                {
                    delete [] pIOBuf;
                    pIOBuf = NULL;
                }

                if (pDisplay != NULL)
                {
                    XFlush(pDisplay);
                    XCloseDisplay(pDisplay);
                    pDisplay = NULL;
                }
            }

            void X11Display::destroy()
            {
                do_destroy();
            }

            int X11Display::main()
            {
                // Make a pause
                struct pollfd x11_poll;
                struct timespec ts;

                int x11_fd          = ConnectionNumber(pDisplay);
                lsp_trace("x11fd = %d(int)", x11_fd);
                XSync(pDisplay, false);

                while (!bExit)
                {
                    // Get current time
                    clock_gettime(CLOCK_REALTIME, &ts);
                    timestamp_t xts     = (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
                    int wtime           = 50; // How many milliseconds to wait

                    if (sTasks.size() > 0)
                    {
                        dtask_t *t          = sTasks.first();
                        ssize_t delta       = t->nTime - xts;
                        if (delta <= 0)
                            wtime               = -1;
                        else if (delta <= wtime)
                            wtime               = delta;
                    }

                    // Try to poll input data for a 100 msec period
                    x11_poll.fd         = x11_fd;
                    x11_poll.events     = POLLIN | POLLPRI | POLLHUP;
                    x11_poll.revents    = 0;

                    errno               = 0;
                    int poll_res        = (wtime > 0) ? poll(&x11_poll, 1, wtime) : 0;
                    if (poll_res < 0)
                    {
                        int err_code = errno;
                        lsp_trace("Poll returned error: %d, code=%d", poll_res, err_code);
                        if (err_code != EINTR)
                            return -1;
                    }
                    else if ((wtime <= 0) || ((poll_res > 0) && (x11_poll.events > 0)))
                    {
                        // Do iteration
                        status_t result = do_main_iteration(xts);
                        if (result != STATUS_OK)
                            return result;
                    }
                }

                return 0;
            }

            status_t X11Display::do_main_iteration(timestamp_t ts)
            {
                XEvent event;
                int pending     = XPending(pDisplay);
                status_t result = STATUS_OK;

                // Process pending x11 events
                for (int i=0; i<pending; i++)
                {
                    if (XNextEvent(pDisplay, &event) != Success)
                    {
                        lsp_error("Failed to fetch next event");
                        return STATUS_UNKNOWN_ERR;
                    }

                    handleEvent(&event);
                }

                // Generate list of tasks for processing
                sPending.clear();

                while (true)
                {
                    // Get next task
                    dtask_t *t  = sTasks.first();
                    if (t == NULL)
                        break;

                    // Do we need to process this task ?
                    if (t->nTime > ts)
                        break;

                    // Allocate task in pending queue
                    t   = sPending.append();
                    if (t == NULL)
                        return STATUS_NO_MEM;

                    // Remove the task from the queue
                    if (!sTasks.remove(0, t))
                    {
                        result = STATUS_UNKNOWN_ERR;
                        break;
                    }
                }

                // Process pending tasks
                if (result == STATUS_OK)
                {
                    // Execute all tasks in pending queue
                    for (size_t i=0; i<sPending.size(); ++i)
                    {
                        dtask_t *t  = sPending.at(i);

                        // Process task
                        result  = t->pHandler(ts, t->pArg);
                        if (result != STATUS_OK)
                            break;
                    }
                }

                // Flush & sync display
                XFlush(pDisplay);
//                XSync(pDisplay, False);

                // Return number of processed events
                return result;
            }

            void X11Display::sync()
            {
                if (pDisplay == NULL)
                    return;
                XFlush(pDisplay);
                XSync(pDisplay, False);
            }

            void X11Display::flush()
            {
                if (pDisplay == NULL)
                    return;
                XFlush(pDisplay);
            }

            int X11Display::main_iteration()
            {
                // Get current time to determine if need perform a rendering
                struct timespec ts;
                clock_gettime(CLOCK_REALTIME, &ts);
                timestamp_t xts = (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);

                // Do iteration
                return do_main_iteration(xts);
            }

            void X11Display::compress_long_data(void *data, size_t nitems)
            {
                uint32_t *dst  = static_cast<uint32_t *>(data);
                long *src      = static_cast<long *>(data);

                while (nitems--)
                    *(dst++)    = *(src++);
            }

            X11Display::cb_request_t *X11Display::find_request(Window requestor, Atom selection, Time time)
            {
                size_t n    = sCbRequests.size();

                for (size_t i=0; i<n; ++i)
                {
                    cb_request_t *r = sCbRequests.at(i);
                    if (requestor != hClipWnd)
                        continue;
                    if (selection != r->hSelection)
                        continue;
                    if (time != r->nTime)
                        continue;
                    return r;
                }

                return NULL;
            }

            X11Window *X11Display::find_window(Window wnd)
            {
                size_t n    = vWindows.size();

                for (size_t i=0; i<n; ++i)
                {
                    X11Window *w = vWindows.at(i);
                    if (w == NULL)
                        continue;
                    if (w->x11handle() == wnd)
                        return w;
                }

                return NULL;
            }

            status_t X11Display::bufid_to_atom(size_t bufid, Atom *atom)
            {
                switch (bufid)
                {
                    case CBUF_PRIMARY:      *atom = sAtoms.X11_XA_PRIMARY; return STATUS_OK;
                    case CBUF_SECONDARY:    *atom = sAtoms.X11_XA_SECONDARY; return STATUS_OK;
                    case CBUF_CLIPBOARD:    *atom = sAtoms.X11_CLIPBOARD; return STATUS_OK;
                    default : break;
                }
                return STATUS_BAD_ARGUMENTS;
            }

            status_t X11Display::atom_to_bufid(Atom x, size_t *bufid)
            {
                if (x == sAtoms.X11_XA_PRIMARY)
                    *bufid  = CBUF_PRIMARY;
                else if (x == sAtoms.X11_XA_SECONDARY)
                    *bufid  = CBUF_SECONDARY;
                else if (x == sAtoms.X11_CLIPBOARD)
                    *bufid  = CBUF_CLIPBOARD;
                else
                    return STATUS_BAD_ARGUMENTS;
                return STATUS_OK;
            }

            bool X11Display::handleClipboardEvent(XEvent *ev)
            {
                switch (ev->type)
                {
                    case SelectionClear:
                    {
                        // Free the corresponding data
                        XSelectionClearEvent *sc    = &ev->xselectionclear;
                        lsp_trace("XSelectionClearEvent for window %x, selection %d", int(sc->window), int(sc->selection));

                        // Find the owner window
                        if (sc->window != hClipWnd)
                            return true;

                        // Get clipboard buffer identifier
                        size_t bufid;
                        status_t res        = atom_to_bufid(sc->selection, &bufid);
                        if (res != STATUS_OK)
                            return true;

                        // Clear the garbage
                        IClipboard *cb      = pClipboard[bufid];
                        if (cb == NULL)
                            return true;
                        else
                            pClipboard[bufid]   = NULL;

                        // Remove all requests related to this cb
                        size_t n            = sCbRequests.size();
                        for (size_t i=0; i < n; )
                        {
                            cb_request_t *req   = sCbRequests.at(i);
                            if (req->pCB != cb)
                            {
                                ++i;
                                continue;
                            }

                            // Remove the request
                            if (req->pIn != NULL)
                            {
                                req->pIn->close();
                                delete req->pIn;
                                req->pIn = NULL;
                            }
                            if (req->pCB != NULL)
                                req->pCB->close();

                            sCbRequests.remove(i);
                            --n;
                        }

                        // Close the clipboard object
                        cb->close();
                        return true;
                    }
                    case SelectionRequest:
                    {
                        XSelectionRequestEvent *sr   = &ev->xselectionrequest;
                        lsp_trace("SelectionRequest requestor = %x, selection=%d (%s), target=%d (%s), property=%d (%s), time=%ld",
                                     int(sr->requestor),
                                     int(sr->selection), XGetAtomName(pDisplay, sr->selection),
                                     int(sr->target), XGetAtomName(pDisplay, sr->target),
                                     int(sr->property), XGetAtomName(pDisplay, sr->property),
                                     long(sr->time));

                        XEvent response;
                        XSelectionEvent *se = &response.xselection;

                        se->type        = SelectionNotify;
                        se->send_event  = True;
                        se->display     = pDisplay;
                        se->requestor   = sr->requestor;
                        se->selection   = sr->selection;
                        se->target      = sr->target;
                        se->property    = sr->property;
                        se->time        = sr->time;

                        // Get buffer identifier
                        size_t bufid;
                        status_t res        = atom_to_bufid(sr->selection, &bufid);

                        // Find window
                        IClipboard *cb      = ((res == STATUS_OK) && (sr->owner == hClipWnd)) ? pClipboard[bufid] : NULL;
                        if (cb == NULL)
                        {
                            se->property        = None;
                            XSendEvent(pDisplay, sr->requestor, True, NoEventMask, &response);
                            XFlush(pDisplay);
                            return true;
                        }
                        else
                            cb->acquire();

                        // Special case ?
                        if (sr->target == sAtoms.X11_TARGETS)
                        {
                            size_t n_targets    = cb->targets();
                            Atom *atoms         = reinterpret_cast<Atom *>(alloca(sizeof(Atom) * (n_targets + 1)));
                            if (atoms == NULL)
                            {
                                se->property        = None;
                                cb->close();
                                XSendEvent(pDisplay, sr->requestor, True, NoEventMask, &response);
                                XFlush(pDisplay);
                                return true;
                            }

                            atoms[0]    = sAtoms.X11_TARGETS;
                            for (size_t i=0; i<n_targets; ++i)
                            {
                                const char *target  = cb->target(i);
                                atoms[i+1]  = XInternAtom(pDisplay, target, False);
                                lsp_trace("Supported format: %s", XGetAtomName(pDisplay, atoms[i+1]));
                            }

                            // Update property and return
                            XChangeProperty(
                                pDisplay, // display
                                sr->requestor, // window
                                sr->property, // property
                                sAtoms.X11_XA_ATOM, // type
                                32, // format
                                PropModeReplace, // mode
                                reinterpret_cast<unsigned char *>(atoms), // data
                                n_targets+1 // nelements
                            );

                            XFlush(pDisplay);

                            // DEBUG start
//                            {
//                                Atom type, *targets;
//                                int di;
//                                unsigned long i, nitems, dul;
//                                unsigned char *prop_ret = NULL;
//                                char *an = NULL;
//
//                                XGetWindowProperty(pDisplay, sr->requestor, sr->property, 0, 1024 * sizeof (Atom), False, XA_ATOM,
//                                                   &type, &di, &nitems, &dul, &prop_ret);
//
//                                lsp_trace("Targets (actual type = %d %s):", int(type), XGetAtomName(pDisplay, type));
//                                targets = (Atom *)prop_ret;
//                                for (i = 0; i < nitems; i++)
//                                {
//                                    lsp_trace("    id = %d", int(targets[i]));
//                                    an = XGetAtomName(pDisplay, targets[i]);
//                                    lsp_trace("    name '%s'", an);
//                                    if (an)
//                                        XFree(an);
//                                }
//                                XFree(prop_ret);
//                            }
                            // DEBUG end

                            XSendEvent(pDisplay, sr->requestor, True, NoEventMask, &response);
                            XFlush(pDisplay);

                            cb->close();
                            return true;
                        }

                        // Now get content type
                        char *ctype             = XGetAtomName(pDisplay, sr->target);
                        io::IInputStream *is    = (ctype != NULL) ? cb->read(ctype) : NULL;
                        lsp_trace("requested content type: %s", ctype);
                        if (ctype != NULL)
                            XFree(ctype);

                        if (is == NULL)
                        {
                            lsp_trace("returned NULL input stream");
                            cb->close();
                            se->property        = None;
                            XSendEvent(pDisplay, sr->requestor, True, NoEventMask, &response);
                            XFlush(pDisplay);
                            return true;
                        }

                        // The description of INCR protocol is here:
                        // https://tronche.com/gui/x/icccm/sec-2.html#s-2.7.2
                        if (is->avail() > 0x10000)
                        {
                            lsp_trace("Too large data, need INCR protocol implementation");

                            se->property        = None;
                            XSendEvent(pDisplay, sr->requestor, True, NoEventMask, &response);
                            XFlush(pDisplay);

                            is->close();
                            cb->close();
                            return true;
                        }

                        // Change property
                        se->property    = sr->property;
                        size_t count    = is->read(pIOBuf, X11IOBUF_SIZE);

                        if (se->property == None)
                            se->property        = XInternAtom(pDisplay, "LSP_SELECTION_DATA", False);
                        lsp_trace("target property: %s, bytes=%d", XGetAtomName(pDisplay, se->property), int(count));

                        XChangeProperty(
                            pDisplay, // display
                            sr->requestor, // window
                            sr->property, // property
                            sr->type, // type
                            8, // format
                            PropModeReplace, // mode
                            reinterpret_cast<unsigned char *>(pIOBuf), // data
                            count // nelements
                        );

                        XFlush(pDisplay);
                        XSendEvent(pDisplay, sr->requestor, True, NoEventMask, &response);
                        XFlush(pDisplay);
                        cb->close();

                        return true;
                    }
                    case SelectionNotify:
                    {
                        // Check that it's proper selection event
                        XSelectionEvent *se = &ev->xselection;
                        lsp_trace("SelectionNotify");

                        // Find the request
                        cb_request_t *req   = find_request(se->requestor, se->selection, se->time);
                        if (req == NULL)
                            return true;

                        if (se->property != req->hProperty)
                        {
                            // Close clipboard objects
                            if (req->pCB != NULL)
                                req->pCB->close();

                            // Call handler and remove request
                            req->pHandler(req->pArgument, STATUS_IO_ERROR, NULL);
                            sCbRequests.remove(req);
                            return true;
                        }

                        // Now we can append data of the event to the output stream
                        Atom p_type = None;
                        int p_fmt = 0;
                        unsigned long p_nitems = 0, p_size = 0, p_offset = 0;
                        unsigned char *p_data = NULL;
                        status_t status = STATUS_OK;

                        // Read with 64k chunks
                        XGetWindowProperty(
                            pDisplay, hClipWnd, req->hProperty,
                            p_offset, X11IOBUF_SIZE/4, False, AnyPropertyType,
                            &p_type, &p_fmt, &p_nitems, &p_size, &p_data
                        );
                        size_t multiplier = p_fmt / 8;

                        do
                        {
                            // Analyze property type
                            if (p_type == sAtoms.X11_INCR)
                            {
                                // Incremental change?
                                lsp_error("Incremental mechanism not implemented currently");
                                if (p_data != NULL)
                                    XFree(p_data);
                                status = STATUS_NOT_SUPPORTED;
                                break;
                            }
                            else
                            {
                                // Compress data if format is 32
                                if ((p_fmt == 32) && (sizeof(long) != 4))
                                    compress_long_data(p_data, p_nitems);

                                // Simply write contents
                                size_t count = req->pCB->append(p_data, p_nitems * multiplier);
                                if (count < (p_nitems * multiplier))
                                {
                                    status      = req->pCB->error_code();
                                    break;
                                }
                            }

                            // Free buffer and update read position
                            if (p_data != NULL)
                                XFree(p_data);
                            p_offset       += p_nitems;
                        } while ((p_size > 0) && (p_nitems > 0));

                        // Remove the property
                        XDeleteProperty(pDisplay, hClipWnd, req->hProperty);

                        // Check status
                        if (status == STATUS_OK)
                        {
                            io::IInputStream *is = req->pCB->read(NULL);
                            if (is == NULL)
                                req->pHandler(req->pArgument, req->pCB->error_code(), NULL);
                            else
                                req->pHandler(req->pArgument, STATUS_OK, is);

                            // Close stream
                            if (is != NULL)
                                is->close();
                        }
                        else
                            req->pHandler(req->pArgument, status, NULL);

                        // Close clipboard storage object (will automatically delete self if there are no references)
                        // Also deregister request
                        if (req->pCB != NULL)
                            req->pCB->close();
                        sCbRequests.remove(req);

                        return true;
                    }
                    default:
                        break;
                }

                return false;
            }

            void X11Display::handleEvent(XEvent *ev)
            {
                if (ev->type > LASTEvent)
                    return;

                // Special case for buffers
                if (handleClipboardEvent(ev))
                    return;

//                lsp_trace("Received event: %d (%s), serial = %ld, window = %x",
//                    int(ev->type), event_name(ev->type), long(ev->xany.serial), int(ev->xany.window));

                // Find the target window
                X11Window *target = NULL;
                for (size_t i=0, nwnd=vWindows.size(); i<nwnd; ++i)
                {
                    X11Window *wnd = vWindows[i];
                    if (wnd == NULL)
                        continue;
                    if (wnd->x11handle() == ev->xany.window)
                    {
                        target      = wnd;
                        break;
                    }
                }

                ws_event_t ue;
                ue.nType        = UIE_UNKNOWN;
                ue.nLeft        = 0;
                ue.nTop         = 0;
                ue.nWidth       = 0;
                ue.nHeight      = 0;
                ue.nCode        = 0;
                ue.nState       = 0;
                ue.nTime        = 0;

                // Decode event
                switch (ev->type)
                {
                    case KeyPress:
                    case KeyRelease:
                    {
                        char ret[32];
                        KeySym ksym;
                        XComposeStatus status;

                        XLookupString(&ev->xkey, ret, sizeof(ret), &ksym, &status);
                        ws_code_t key   = decode_keycode(ksym);

                        if (key != WSK_UNKNOWN)
                        {
                            ue.nType        = (ev->type == KeyPress) ? UIE_KEY_DOWN : UIE_KEY_UP;
                            ue.nLeft        = ev->xkey.x;
                            ue.nTop         = ev->xkey.y;
                            ue.nCode        = key;
                            ue.nState       = decode_state(ev->xkey.state);
                            ue.nTime        = ev->xkey.time;
                        }
                        break;
                    }

                    case ButtonPress:
                    case ButtonRelease:
                        lsp_trace("button time = %ld, x=%d, y=%d up=%s", long(ev->xbutton.time),
                            int(ev->xbutton.x), int(ev->xbutton.y),
                            (ev->type == ButtonRelease) ? "true" : "false");

                        // Check if it is a button press/release
                        ue.nCode        = decode_mcb(ev->xbutton.button);
                        if (ue.nCode != MCB_NONE)
                        {
                            ue.nType        = (ev->type == ButtonPress) ? UIE_MOUSE_DOWN : UIE_MOUSE_UP;
                            ue.nLeft        = ev->xbutton.x;
                            ue.nTop         = ev->xbutton.y;
                            ue.nState       = decode_state(ev->xbutton.state);
                            ue.nTime        = ev->xbutton.time;
                            break;
                        }

                        // Check that it is a scrolling
                        ue.nCode        = decode_mcd(ev->xbutton.button);
                        if ((ue.nCode != MCD_NONE) && (ev->type == ButtonPress))
                        {
                            // Skip ButtonRelease
                            ue.nType        = UIE_MOUSE_SCROLL;
                            ue.nLeft        = ev->xbutton.x;
                            ue.nTop         = ev->xbutton.y;
                            ue.nState       = decode_state(ev->xbutton.state);
                            ue.nTime        = ev->xbutton.time;
                            break;
                        }

                        // Unknown button
                        break;

                    case MotionNotify:
                        ue.nType        = UIE_MOUSE_MOVE;
                        ue.nLeft        = ev->xmotion.x;
                        ue.nTop         = ev->xmotion.y;
                        ue.nState       = decode_state(ev->xmotion.state);
                        ue.nTime        = ev->xmotion.time;
                        break;

                    case Expose:
                        ue.nType        = UIE_REDRAW;
                        ue.nLeft        = ev->xexpose.x;
                        ue.nTop         = ev->xexpose.y;
                        ue.nWidth       = ev->xexpose.width;
                        ue.nHeight      = ev->xexpose.height;
                        break;

                    case ResizeRequest:
                        ue.nType        = UIE_SIZE_REQUEST;
                        ue.nWidth       = ev->xresizerequest.width;
                        ue.nHeight      = ev->xresizerequest.height;
                        break;

                    case ConfigureNotify:
                        ue.nType        = UIE_RESIZE;
                        ue.nLeft        = ev->xconfigure.x;
                        ue.nTop         = ev->xconfigure.y;
                        ue.nWidth       = ev->xconfigure.width;
                        ue.nHeight      = ev->xconfigure.height;
                        break;

                    case MapNotify:
                        ue.nType        = UIE_SHOW;
                        break;
                    case UnmapNotify:
                        ue.nType        = UIE_HIDE;
                        break;

                    case EnterNotify:
                    case LeaveNotify:
                        ue.nType        = (ev->type == EnterNotify) ? UIE_MOUSE_IN : UIE_MOUSE_OUT;
                        ue.nLeft        = ev->xcrossing.x;
                        ue.nTop         = ev->xcrossing.y;
                        break;

                    case FocusIn:
                    case FocusOut:
                        ue.nType        = (ev->type == FocusIn) ? UIE_FOCUS_IN : UIE_FOCUS_OUT;
                        // TODO: maybe useful could be decoding of mode and detail
                        break;

                    case KeymapNotify:
                        lsp_trace("The keyboard state was changed!");
                        break;

                    case MappingNotify:
                        if ((ev->xmapping.request == MappingKeyboard) || (ev->xmapping.request == MappingModifier))
                        {
                            lsp_trace("The keyboard mapping was changed!");
                            XRefreshKeyboardMapping(&ev->xmapping);
                        }

                        break;

                    case ClientMessage:
                        if (ev->xclient.message_type == sAtoms.X11_WM_PROTOCOLS)
                        {
                            if (ev->xclient.data.l[0] == long(sAtoms.X11_WM_DELETE_WINDOW))
                                ue.nType        = UIE_CLOSE;
                            else
                                lsp_trace("received client WM_PROTOCOLS message with argument %s",
                                        XGetAtomName(pDisplay, ev->xclient.data.l[0]));
                        }
                        else
                            lsp_trace("received client message of type %s",
                                    XGetAtomName(pDisplay, ev->xclient.message_type));
                        break;

                    default:
                        return;
                }

                // Analyze event type
                if (ue.nType != UIE_UNKNOWN)
                {
                    Window child        = None;
                    ws_event_t se       = ue;

                    // Clear the collection
                    sTargets.clear();

                    switch (se.nType)
                    {
                        case UIE_CLOSE:
                            if ((target != NULL) && (get_locked(target) == NULL))
                                sTargets.add(target);
                            break;

                        case UIE_MOUSE_DOWN:
                        case UIE_MOUSE_UP:
                        case UIE_MOUSE_SCROLL:
                        case UIE_MOUSE_MOVE:
                        case UIE_KEY_DOWN:
                        case UIE_KEY_UP:
                        {
                            // Check if there is grab enabled
                            if (sGrab.size() > 0)
                            {
                                // Add listeners from grabbing windows
                                for (size_t i=0, nwnd = vWindows.size(); i<nwnd; ++i)
                                {
                                    X11Window *wnd = vWindows.at(i);
                                    if (wnd == NULL)
                                        continue;
                                    if (sGrab.index_of(wnd) < 0)
                                        continue;
                                    sTargets.add(wnd);
//                                    lsp_trace("Grabbing window: %p", wnd);
                                }

                                // Allow event replay
                                if ((se.nType == UIE_KEY_DOWN) || (se.nType == UIE_KEY_UP))
                                    XAllowEvents(pDisplay, ReplayKeyboard, CurrentTime);
                                else if (se.nType != UIE_CLOSE)
                                    XAllowEvents(pDisplay, ReplayPointer, CurrentTime);
                            }
                            else if (target != NULL)
                                sTargets.add(target);

                            // Get the final window
                            for (size_t i=0, nwnd=sTargets.size(); i<nwnd; ++i)
                            {
                                // Get target window
                                X11Window *wnd = sTargets.at(i);
                                if (wnd == NULL)
                                    continue;

                                // Get the locking window
                                X11Window *redirect = get_redirect(wnd);
                                if (wnd != redirect)
                                {
//                                    lsp_trace("Redirect window: %p", wnd);
                                    sTargets.set(i, wnd);
                                }
                            }

                            break;
                        }
                        default:
                            if (target != NULL)
                                sTargets.add(target);
                            break;
                    }

                    // Deliver the message to target windows
                    for (size_t i=0, nwnd = sTargets.size(); i<nwnd; ++i)
                    {
                        X11Window *wnd = sTargets.at(i);

                        // Translate coordinates if originating and target window differs
                        if (wnd != target)
                        {
                            int x, y;
                            XTranslateCoordinates(pDisplay,
                                ev->xany.window, wnd->x11handle(),
                                ue.nLeft, ue.nTop,
                                &x, &y, &child);
                            se.nLeft    = x;
                            se.nTop     = y;
                        }

//                        lsp_trace("Sending event to target=%p", wnd);
                        wnd->handle_event(&se);
                    }
                }
            }

            void X11Display::quit_main()
            {
                bExit = true;
            }

            bool X11Display::addWindow(X11Window *wnd)
            {
                return vWindows.add(wnd);
            }

            size_t X11Display::screens()
            {
                if (pDisplay == NULL)
                    return STATUS_BAD_STATE;
                return ScreenCount(pDisplay);
            }

            size_t X11Display::default_screen()
            {
                if (pDisplay == NULL)
                    return STATUS_BAD_STATE;
                return DefaultScreen(pDisplay);
            }

            status_t X11Display::screen_size(size_t screen, ssize_t *w, ssize_t *h)
            {
                if (pDisplay == NULL)
                    return STATUS_BAD_STATE;

                Screen *s = ScreenOfDisplay(pDisplay, screen);
                if (w != NULL)
                    *w = WidthOfScreen(s);
                if (h != NULL)
                    *h = HeightOfScreen(s);

                return STATUS_OK;
            }

            bool X11Display::remove_window(X11Window *wnd)
            {
                if (!vWindows.remove(wnd))
                    return false;

                // Check if need to leave main cycle
                if (vWindows.size() <= 0)
                    bExit = true;
                return true;
            }

            const char *X11Display::event_name(int xev_code)
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

            status_t X11Display::grab_events(X11Window *wnd)
            {
                if (sGrab.index_of(wnd) >= 0)
                {
                    lsp_trace("grab duplicated");
                    return STATUS_DUPLICATED;
                }

                size_t screen = wnd->screen();
                bool set_grab = true;
                size_t n = sGrab.size();
                for (size_t i=0; i<n; ++i)
                {
                    X11Window *wnd = sGrab.get(i);
                    if (wnd->screen() == screen)
                    {
                        set_grab = false;
                        break;
                    }
                }

                if (!sGrab.add(wnd))
                    return STATUS_NO_MEM;

                if (set_grab)
                {
                    lsp_trace("setting grab for screen=%d", int(screen));
                    Window root     = RootWindow(pDisplay, screen);
                    XGrabPointer(pDisplay, root, True, PointerMotionMask | ButtonPressMask | ButtonReleaseMask, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
                    XGrabKeyboard(pDisplay, root, True, GrabModeAsync, GrabModeAsync, CurrentTime);

                    XFlush(pDisplay);
//                    XSync(pDisplay, False);
                }

                return STATUS_OK;
            }

            status_t X11Display::lock_events(X11Window *wnd, X11Window *lock)
            {
                if (wnd == NULL)
                    return STATUS_BAD_ARGUMENTS;
                if (lock == NULL)
                    return STATUS_OK;

                size_t n = sLocks.size();
                for (size_t i=0; i<n; ++i)
                {
                    wnd_lock_t *lk = sLocks.at(i);
                    if ((lk != NULL) && (lk->pOwner == wnd) && (lk->pWaiter == lock))
                    {
                        lk->nCounter++;
                        return STATUS_OK;
                    }
                }

                wnd_lock_t *lk = sLocks.append();
                if (lk == NULL)
                    return STATUS_NO_MEM;

                lk->pOwner      = wnd;
                lk->pWaiter     = lock;
                lk->nCounter    = 1;

                return STATUS_OK;
            }

            status_t X11Display::unlock_events(X11Window *wnd)
            {
                for (size_t i=0; i<sLocks.size();)
                {
                    wnd_lock_t *lk = sLocks.get(i);
                    if ((lk == NULL) || (lk->pOwner != wnd))
                    {
                        ++i;
                        continue;
                    }
                    if ((--lk->nCounter) <= 0)
                        sLocks.remove(i);
                }

                return STATUS_OK;
            }

            X11Window *X11Display::get_locked(X11Window *wnd)
            {
                size_t n = sLocks.size();
                for (size_t i=0; i<n; ++i)
                {
                    wnd_lock_t *lk = sLocks.at(i);
                    if ((lk != NULL) && (lk->pWaiter == wnd) && (lk->nCounter > 0))
                        return lk->pOwner;
                }
                return NULL;
            }

            X11Window *X11Display::get_redirect(X11Window *wnd)
            {
                X11Window *redirect = get_locked(wnd);
                if (redirect == NULL)
                    return wnd;

                do
                {
                    wnd         = redirect;
                    redirect    = get_locked(wnd);
                } while (redirect != NULL);

                return wnd;
            }

            status_t X11Display::ungrab_events(X11Window *wnd)
            {
                size_t screen = wnd->screen();
                bool kill_grab = true;

                if (!sGrab.remove(wnd))
                {
                    lsp_trace("grab window not found");
                    return STATUS_NOT_FOUND;
                }

                size_t n = sGrab.size();
                for (size_t i=0; i<n; ++i)
                {
                    X11Window *wnd = sGrab.get(i);
                    if (wnd->screen() == screen)
                    {
                        kill_grab = false;
                        break;
                    }
                }

                if (kill_grab)
                {
                    lsp_trace("removing grab for screen=%d", int(screen));
                    XUngrabPointer(pDisplay, CurrentTime);
                    XUngrabKeyboard(pDisplay, CurrentTime);

                    XFlush(pDisplay);
//                    XSync(pDisplay, False);
                }

                return STATUS_OK;
            }

            size_t X11Display::get_screen(Window root)
            {
                size_t n = ScreenCount(pDisplay);

                for (size_t i=0; i<n; ++i)
                {
                    if (RootWindow(pDisplay, i) == root)
                        return i;
                }

                return 0;
            }

            Cursor X11Display::get_cursor(mouse_pointer_t pointer)
            {
                return vCursors[pointer];
            }

            Atom X11Display::gen_selection_id()
            {
                char prop_id[32];

                for (size_t id = 0;; ++id)
                {
                    sprintf(prop_id, "LSP_SELECTION_%d", int(id));
                    Atom atom = XInternAtom(pDisplay, prop_id, False);

                    size_t n = sCbRequests.size();
                    for (size_t i=0; i<n; ++i)
                    {
                        cb_request_t *req = sCbRequests.at(i);
                        if (req->hProperty == atom)
                        {
                            atom = None;
                            break;
                        }
                    }

                    if (atom != None)
                        return atom;
                }
                return None;
            }

            status_t X11Display::fetchClipboard(size_t id, const char *ctype, clipboard_handler_t handler, void *arg = NULL)
            {
                if (ctype == NULL)
                    return STATUS_BAD_ARGUMENTS;

                Atom aid;
                status_t result = bufid_to_atom(id, &aid);
                if (result != STATUS_OK)
                    return result;

                // First, check that it's our window to avoid X11 transfers
                Window wnd  = XGetSelectionOwner(pDisplay, aid);
                if (wnd == hClipWnd)
                {
                    IClipboard *cb          = pClipboard[id];
                    io::IInputStream *is    = (cb != NULL) ? cb->read(ctype) : NULL;
                    if (is != NULL)
                        return handler(arg, STATUS_OK, is);
                    else
                        return handler(arg, STATUS_NO_DATA, is);
                }

                // Are there any X11 windows present?
                Atom type, prop_id;
                if (!strcasecmp(ctype, "text/plain"))
                    type = sAtoms.X11_UTF8_STRING;
                else if (!strcasecmp(ctype, "utf8_string"))
                    type = sAtoms.X11_UTF8_STRING;
                else
                    type = XInternAtom(pDisplay, ctype, False);
                prop_id = gen_selection_id();
                if (prop_id == None)
                    return STATUS_UNKNOWN_ERR;

                // Create clipboard request
                X11Clipboard *cb    = new X11Clipboard();
                if (cb == NULL)
                    return STATUS_NO_MEM;
                cb_request_t *req   = sCbRequests.add();
                if (req == NULL)
                {
                    cb->close();
                    return STATUS_NO_MEM;
                }

                // Fill the pending request structure
                struct timespec ts;
                clock_gettime(CLOCK_REALTIME, &ts);
                Time xts            = ts.tv_sec;
                req->hProperty      = prop_id;
                req->hSelection     = aid;
                req->nTime          = xts;
                req->pCB            = cb;
                req->pIn            = NULL;
                req->pHandler       = handler;
                req->pArgument      = arg;

                // This is not our window, we need to request selection data from X server
                XConvertSelection(pDisplay, aid, type, prop_id, hClipWnd, xts);
                XFlush(pDisplay);

                return STATUS_OK;
            }

            status_t X11Display::writeClipboard(size_t id, IClipboard *c)
            {
                // Check arguments
                if ((id < 0) || (id >= _CBUF_TOTAL) || (c == NULL))
                    return STATUS_BAD_ARGUMENTS;

                // Acquire clipboard object
                status_t res = c->acquire();
                if (res != STATUS_OK)
                    return res;

                // Release previously bound resource
                if (pClipboard[id] != NULL)
                {
                    res = pClipboard[id]->close();
                    if (res != STATUS_OK)
                    {
                        c->close();
                        return res;
                    }
                    pClipboard[id] = NULL;
                }

                // Try to set clipboard owner
                Atom aid;
                status_t result = bufid_to_atom(id, &aid);
                if (result != STATUS_OK)
                    return result;

                // Notify that our window is owning a selection
                pClipboard[id]  = c;
                XSetSelectionOwner(pDisplay, aid, hClipWnd, CurrentTime);
                XFlush(pDisplay);

                return STATUS_OK;
            }

        } /* namespace x11 */
    } /* namespace ws */
} /* namespace lsp */

#endif /* USE_X11_DISPLAY */

