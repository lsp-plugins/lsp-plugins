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

#define X11IOBUF_SIZE       0x1000

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
                hDndSource      = None;

                for (size_t i=0; i<_CBUF_TOTAL; ++i)
                {
                    pClipboard[i]           = NULL;
                    pCbOwner[i]              = NULL;
                }
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
                hClipWnd        = ::XCreateWindow(pDisplay, hRootWnd, 0, 0, 1, 1, 0, 0, CopyFromParent, CopyFromParent, 0, NULL);
                if (hClipWnd == None)
                    return STATUS_UNKNOWN_ERR;
                ::XSelectInput(pDisplay, hClipWnd, PropertyChangeMask);
                ::XFlush(pDisplay);

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
                return new X11Window(this, DefaultScreen(pDisplay), 0, NULL, false);
            }

            INativeWindow *X11Display::createWindow(size_t screen)
            {
                return new X11Window(this, screen, 0, NULL, false);
            }

            INativeWindow *X11Display::createWindow(void *handle)
            {
                lsp_trace("handle = %p", handle);
                return new X11Window(this, DefaultScreen(pDisplay), Window(uintptr_t(handle)), NULL, false);
            }

            INativeWindow *X11Display::wrapWindow(void *handle)
            {
                return new X11Window(this, DefaultScreen(pDisplay), Window(uintptr_t(handle)), NULL, true);
            }

            ISurface *X11Display::createSurface(size_t width, size_t height)
            {
                return new X11CairoSurface(width, height);
            }

            void X11Display::do_destroy()
            {
                // Perform resource release
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
                drop_mime_types(&vDndMimeTypes);

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
                IDisplay::destroy();
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
                        status_t result = IDisplay::main_iteration();
                        if (result == STATUS_OK)
                            result = do_main_iteration(xts);
                        if (result != STATUS_OK)
                            return result;
                    }
                }

                return STATUS_OK;
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

            status_t X11Display::main_iteration()
            {
                // Call parent class for iteration
                status_t result = IDisplay::main_iteration();
                if (result != STATUS_OK)
                    return result;

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
                    case CBUF_PRIMARY:
                        *atom       = sAtoms.X11_XA_PRIMARY;
                        return STATUS_OK;

                    case CBUF_SECONDARY:
                        *atom       = sAtoms.X11_XA_SECONDARY;
                        return STATUS_OK;

                    case CBUF_CLIPBOARD:
                        *atom       = sAtoms.X11_CLIPBOARD;
                        return STATUS_OK;

                    default:
                        break;
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
                    case PropertyNotify:
                    {
                        XPropertyEvent *sc          = &ev->xproperty;
                        char *name                  = ::XGetAtomName(pDisplay, sc->atom);
                        lsp_trace("XPropertyEvent for window 0x%lx, property %ld (%s)", long(sc->window), long(sc->atom), name);
                        ::XFree(name);
                        handle_property_notify(sc);
                        return true;
                    }

                    case SelectionClear:
                    {
                        // Free the corresponding data
                        XSelectionClearEvent *sc    = &ev->xselectionclear;
                        lsp_trace("XSelectionClearEvent for window 0x%lx, selection %ld", long(sc->window), long(sc->selection));

                        handle_selection_clear(sc);

#if 0
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
#endif
                        return true;
                    }
                    case SelectionRequest:
                    {
                        XSelectionRequestEvent *sr   = &ev->xselectionrequest;
                        lsp_trace("SelectionRequest requestor = 0x%x, selection=%d (%s), target=%d (%s), property=%d (%s), time=%ld",
                                     int(sr->requestor),
                                     int(sr->selection), XGetAtomName(pDisplay, sr->selection),
                                     int(sr->target), XGetAtomName(pDisplay, sr->target),
                                     int(sr->property), XGetAtomName(pDisplay, sr->property),
                                     long(sr->time));

                        handle_selection_request(sr);
#if 0
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
                        io::IInStream *is    = (ctype != NULL) ? cb->read(ctype) : NULL;
                        lsp_trace("requested content type: %s", (ctype != NULL) ? ctype : "(null)");
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
                            sr->target, // type
                            8, // format
                            PropModeReplace, // mode
                            reinterpret_cast<unsigned char *>(pIOBuf), // data
                            count // nelements
                        );

                        XFlush(pDisplay);
                        XSendEvent(pDisplay, sr->requestor, True, NoEventMask, &response);
                        XFlush(pDisplay);
                        cb->close();
#endif
                        return true;
                    }
                    case SelectionNotify:
                    {
                        // Check that it's proper selection event
                        XSelectionEvent *se = &ev->xselection;
                        char *aname = ::XGetAtomName(pDisplay, se->property);
                        lsp_trace("SelectionNotify for window=0x%lx, selection=%ld, property=%ld (%s)",
                                long(se->requestor), long(se->selection), long(se->property), aname);
                        if (aname != NULL)
                            ::XFree(aname);
                        handle_selection_notify(se);

                        // Find the request (legacy code)

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
                        ::XDeleteProperty(pDisplay, hClipWnd, req->hProperty);
                        ::XFlush(pDisplay);

                        // Check status
                        if (status == STATUS_OK)
                        {
                            io::IInStream *is = req->pCB->read(NULL);
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

            status_t X11Display::read_property(Window wnd, Atom property, Atom ptype, uint8_t **data, size_t *size, Atom *type)
            {
                int p_fmt = 0;
                unsigned long p_nitems = 0, p_size = 0, p_offset = 0;
                unsigned char *p_data = NULL;
                uint8_t *ptr        = NULL;
                size_t capacity     = 0;

                while (true)
                {
                    // Get window property
                    ::XGetWindowProperty(
                        pDisplay, hClipWnd, property,
                        p_offset / 4, X11IOBUF_SIZE/4, False, ptype,
                        type, &p_fmt, &p_nitems, &p_size, &p_data
                    );

                    // Compress data if format is 32
                    if ((p_fmt == 32) && (sizeof(long) != 4))
                        compress_long_data(p_data, p_nitems);

                    // No more data?
                    if ((p_nitems <= 0) || (p_data == NULL))
                    {
                        if (p_data != NULL)
                            ::XFree(p_data);
                        break;
                    }

                    // Append data to the memory buffer
                    size_t multiplier   = p_fmt / 8;
                    size_t ncap         = capacity + p_nitems * multiplier;
                    uint8_t *nptr       = reinterpret_cast<uint8_t *>(::realloc(ptr, ncap));
                    if (nptr == NULL)
                    {
                        ::XFree(p_data);
                        if (ptr != NULL)
                            ::free(ptr);

                        return STATUS_NO_MEM;
                    }
                    ::memcpy(&nptr[capacity], p_data, p_nitems * multiplier);
                    ::XFree(p_data);
                    p_offset           += p_nitems;

                    // Update buffer pointer and capacity
                    capacity            = ncap;
                    ptr                 = nptr;

                    // There are no remaining bytes?
                    if (p_size <= 0)
                        break;
                };

                // Return successful result
                *size       = capacity;
                *data       = ptr;

                return STATUS_OK;
            }

            status_t X11Display::decode_mime_types(cvector<char> *ctype, const uint8_t *data, size_t size)
            {
                // Fetch long list of supported MIME types
                const uint32_t *list = reinterpret_cast<const uint32_t *>(data);
                for (size_t i=0, n=size/sizeof(uint32_t); i<n; ++i)
                {
                    // Get atom name
                    char *a_name = ::XGetAtomName(pDisplay, list[i]);
                    if (a_name == NULL)
                        continue;
                    char *a_dup = ::strdup(a_name);
                    if (a_dup == NULL)
                    {
                        ::XFree(a_name);
                        return STATUS_NO_MEM;
                    }

                    if (!ctype->add(a_dup))
                    {
                        ::free(a_name);
                        ::free(a_dup);
                        return STATUS_NO_MEM;
                    }
                }

                return STATUS_OK;
            }

            void X11Display::handle_selection_notify(XSelectionEvent *ev)
            {
                for (size_t i=0, n=sAsync.size(); i<n; ++i)
                {
                    x11_async_t *task = sAsync.at(i);

                    // Notify all possible tasks about the event
                    switch (task->type)
                    {
                        case X11ASYNC_CB_RECV:
                            if (task->cb_recv.hProperty == ev->property)
                                task->result = handle_selection_notify(&task->cb_recv, ev);
                            break;
                        default:
                            break;
                    }
                }

                complete_tasks();
            }

            void X11Display::handle_selection_clear(XSelectionClearEvent *ev)
            {
                // Get the selection identifier
                size_t bufid = 0;
                status_t res = atom_to_bufid(ev->selection, &bufid);
                if (res != STATUS_OK)
                    return;

                // Cleanup tasks
//                for (size_t i=0, n=sAsync.size(); i<n; ++i)
//                {
//                    x11_async_t *task = sAsync.at(i);
//
//                    // Notify all possible tasks about the event
//                    switch (task->type)
//                    {
//                        case X11ASYNC_CB_SEND:
//                            if (task->cb_send.hSelection == ev->selection)
//                            {
//                                task->result    = STATUS_CANCELLED;
//                                task->cb_common.bComplete = true;
//                            }
//                            break;
//                        default:
//                            break;
//                    }
//                }
//
//                complete_tasks();

                // Unbind data source
                if (pCbOwner[bufid] != NULL)
                {
                    pCbOwner[bufid]->release();
                    pCbOwner[bufid] = NULL;
                }
            }

            void X11Display::handle_property_notify(XPropertyEvent *ev)
            {
                for (size_t i=0, n=sAsync.size(); i<n; ++i)
                {
                    x11_async_t *task = sAsync.at(i);

                    // Notify all possible tasks about the event
                    switch (task->type)
                    {
                        case X11ASYNC_CB_RECV:
                            if (task->cb_recv.hProperty == ev->atom)
                                task->result = handle_property_notify(&task->cb_recv, ev);
                            break;
                        case X11ASYNC_CB_SEND:
                            if ((task->cb_send.hProperty == ev->atom) &&
                                (task->cb_send.hRequestor == ev->window))
                                task->result = handle_property_notify(&task->cb_send, ev);
                            break;
                        default:
                            break;
                    }
                }

                complete_tasks();
            }

            void X11Display::complete_tasks()
            {
                for (size_t i=0; i<sAsync.size(); )
                {
                    // Skip non-complete tasks
                    x11_async_t *task = sAsync.get(i);
                    if (!task->cb_common.bComplete)
                    {
                        ++i;
                        continue;
                    }

                    // Analyze how to finalize the task
                    switch (task->type)
                    {
                        case X11ASYNC_CB_RECV:
                            // Close and release data sink
                            if (task->cb_recv.pSink != NULL)
                            {
                                task->cb_recv.pSink->close(task->result);
                                task->cb_recv.pSink->release();
                                task->cb_recv.pSink = NULL;
                            }
                            break;
                        case X11ASYNC_CB_SEND:
                            // Close associated stream
                            if (task->cb_send.pStream != NULL)
                            {
                                task->cb_send.pStream->close();
                                task->cb_send.pStream = NULL;
                            }
                            // Release data source
                            if (task->cb_send.pSource != NULL)
                            {
                                task->cb_send.pSource->release();
                                task->cb_send.pSource = NULL;
                            }
                            break;
                        default:
                            break;
                    }

                    // Remove the async task from the queue
                    sAsync.remove(task);
                }
            }

            status_t X11Display::handle_property_notify(cb_recv_t *task, XPropertyEvent *ev)
            {
                status_t res    = STATUS_OK;
                uint8_t *data   = NULL;
                size_t bytes    = 0;
                Atom type       = None;

                switch (task->enState)
                {
                    case CB_RECV_INCR:
                    {
                        // Read incrementally property contents
                        if (ev->state == PropertyNewValue)
                        {
                            res = read_property(hClipWnd, task->hProperty, task->hType, &data, &bytes, &type);
                            if (res == STATUS_OK)
                            {
                                // Check property type
                                if (bytes <= 0) // End of transfer?
                                {
                                    // Complete the INCR transfer
                                    task->pSink->close(res);
                                    task->bComplete = true;
                                }
                                else if (type == task->hType)
                                {
                                    res = task->pSink->write(data, bytes); // Append data to the sink
                                    ::XDeleteProperty(pDisplay, hClipWnd, task->hProperty); // Request next chunk
                                    ::XFlush(pDisplay);
                                }
                                else
                                    res     = STATUS_UNSUPPORTED_FORMAT;
                            }
                        }

                        break;
                    }

                    default:
                        break;
                }

                // Free allocated data
                if (data != NULL)
                    ::free(data);

                return res;
            }

            status_t X11Display::handle_property_notify(cb_send_t *task, XPropertyEvent *ev)
            {
                // Look only at PropertyDelete events
                if ((ev->state != PropertyDelete) || (task->pStream == NULL))
                    return STATUS_OK;

                // How many bytes are available?
                wssize_t avail  = task->pStream->avail();
                if (avail < 0)
                {
                    if (avail == -STATUS_NOT_IMPLEMENTED)
                        avail = X11IOBUF_SIZE;
                    else
                        return status_t(-avail);
                }
                else if (avail > X11IOBUF_SIZE)
                    avail = X11IOBUF_SIZE;
                else if (avail == 0)
                {
                    // Complete the transfer
                    ::XSelectInput(pDisplay, task->hRequestor, None);
                    ::XChangeProperty(
                        pDisplay, task->hRequestor, task->hProperty,
                        task->hType, 8, PropModeReplace, NULL, 0
                    );
                    ::XFlush(pDisplay);
                    task->bComplete = true;
                    return STATUS_OK;
                }

                // Allocate buffer and perform transfer
                uint8_t *data   = reinterpret_cast<uint8_t *>(::malloc(avail));
                if (data == NULL)
                {
                    ::XSelectInput(pDisplay, task->hRequestor, None);
                    ::XFlush(pDisplay);
                    return STATUS_NO_MEM;
                }

                // Read data from the stream
                ssize_t nread   = task->pStream->read_fully(data, avail);
                status_t res    = STATUS_OK;
                if (nread > 0)
                {
                    // Write the property to re requestor
                    ::XChangeProperty(
                        pDisplay, task->hRequestor, task->hProperty,
                        task->hType, 8, PropModeReplace,
                        reinterpret_cast<unsigned char *>(data), nread
                    );
                }
                else if ((nread == 0) || (nread == -STATUS_EOF))
                {
                    ::XSelectInput(pDisplay, task->hRequestor, None);
                    ::XChangeProperty(
                        pDisplay, task->hRequestor, task->hProperty,
                        task->hType, 8, PropModeReplace, NULL, 0
                    );
                    task->bComplete = true;
                }
                else
                {
                    ::XSelectInput(pDisplay, task->hRequestor, None);
                    res = status_t(-nread);
                }
                ::XFlush(pDisplay);

                // Release the data and return
                ::free(data);

                return res;
            }

            status_t X11Display::handle_selection_notify(cb_recv_t *task, XSelectionEvent *ev)
            {
                uint8_t *data   = NULL;
                size_t bytes    = 0;
                Atom type       = None;
                status_t res    = STATUS_OK;

                // Analyze state
                switch (task->enState)
                {
                    case CB_RECV_CTYPE:
                    {
                        // Here we expect list of content types, of type XA_ATOM
                        res = read_property(hClipWnd, task->hProperty, sAtoms.X11_XA_ATOM, &data, &bytes, &type);
                        if ((res == STATUS_OK) && (type == sAtoms.X11_XA_ATOM) && (data != NULL))
                        {
                            // Decode list of mime types and pass to sink
                            cvector<char> mimes;
                            res = decode_mime_types(&mimes, data, bytes);
                            if (res == STATUS_OK)
                            {
                                ssize_t idx = task->pSink->open(mimes.get_array());
                                if ((idx >= 0) && (idx < ssize_t(mimes.size())))
                                {
                                    lsp_trace("Requesting data of mime type %s", mimes.get(idx));

                                    // Submit next XConvertSelection request
                                    task->enState   = CB_RECV_SIMPLE;
                                    task->hType     = ::XInternAtom(pDisplay, mimes.get(idx), True);
                                    if (task->hType != None)
                                    {
                                        // Request selection data of selected type
                                        ::XDeleteProperty(pDisplay, hClipWnd, task->hProperty);
                                        ::XConvertSelection(
                                            pDisplay, task->hSelection, task->hType, task->hProperty,
                                            hClipWnd, CurrentTime
                                        );
                                        ::XFlush(pDisplay);
                                    }
                                    else
                                        res         = STATUS_INVALID_VALUE;
                                }
                                else
                                    res = -idx;
                            }
                            drop_mime_types(&mimes);
                        }
                        else
                            res = STATUS_BAD_FORMAT;

                        break;
                    }

                    case CB_RECV_SIMPLE:
                    {
                        // We expect property of type INCR or of type task->hType
                        res = read_property(hClipWnd, task->hProperty, task->hType, &data, &bytes, &type);
                        if (res == STATUS_OK)
                        {
                            if (type == sAtoms.X11_INCR)
                            {
                                // Initiate INCR mode transfer
                                ::XDeleteProperty(pDisplay, hClipWnd, task->hProperty);
                                ::XFlush(pDisplay);
                                task->enState       = CB_RECV_INCR;
                            }
                            else if (type == task->hType)
                            {
                                ::XDeleteProperty(pDisplay, hClipWnd, task->hProperty); // Remove property
                                ::XFlush(pDisplay);

                                if (bytes > 0)
                                    res = task->pSink->write(data, bytes);
                                task->bComplete = true;
                            }
                            else
                                res     = STATUS_UNSUPPORTED_FORMAT;
                        }

                        break;
                    }

                    case CB_RECV_INCR:
                    {
                        // Read incrementally property contents
                        res = read_property(hClipWnd, task->hProperty, task->hType, &data, &bytes, &type);
                        if (res == STATUS_OK)
                        {
                            // Check property type
                            if (bytes <= 0) // End of transfer?
                            {
                                // Complete the INCR transfer
                                ::XDeleteProperty(pDisplay, hClipWnd, task->hProperty); // Delete the property
                                ::XFlush(pDisplay);
                                task->bComplete = true;
                            }
                            else if (type == task->hType)
                            {
                                ::XDeleteProperty(pDisplay, hClipWnd, task->hProperty); // Request next chunk
                                ::XFlush(pDisplay);
                                res     = task->pSink->write(data, bytes); // Append data to the sink
                            }
                            else
                                res     = STATUS_UNSUPPORTED_FORMAT;
                        }

                        break;
                    }

                    default:
                        // Invalid state, report as error
                        res         = STATUS_IO_ERROR;
                        break;
                }

                // Free allocated data
                if (data != NULL)
                    ::free(data);

                return res;
            }

            void X11Display::handle_selection_request(XSelectionRequestEvent *ev)
            {
                // Get the selection identifier
                size_t bufid = 0;
                status_t res = atom_to_bufid(ev->selection, &bufid);
                if (res != STATUS_OK)
                    return;

                // Now check that selection is present
                bool found = false;

                for (size_t i=0, n=sAsync.size(); i<n; ++i)
                {
                    x11_async_t *task = sAsync.at(i);

                    // Notify all possible tasks about the event
                    switch (task->type)
                    {
                        case X11ASYNC_CB_SEND:
                            if ((task->cb_send.hProperty == ev->property) &&
                                (task->cb_send.hSelection == ev->selection) &&
                                (task->cb_send.hRequestor == ev->requestor))
                            {
                                task->result    = handle_selection_request(&task->cb_send, ev);
                                found           = true;
                            }
                            break;
                        default:
                            break;
                    }
                }

                // The transfer has not been found?
                if (!found)
                {
                    // Do we have a data source?
                    IDataSource *ds = pCbOwner[bufid];
                    if (ds == NULL)
                        return;

                    // Create async task if possible
                    x11_async_t *task   = sAsync.add();
                    if (task == NULL)
                        return;

                    task->type          = X11ASYNC_CB_SEND;
                    task->result        = STATUS_OK;

                    cb_send_t *param    = &task->cb_send;
                    param->bComplete    = false;
                    param->hProperty    = ev->property;
                    param->hSelection   = ev->selection;
                    param->hRequestor   = ev->requestor;
                    param->pSource      = ds;
                    param->pStream      = NULL;
                    ds->acquire();

                    // Call for processing
                    task->result        = handle_selection_request(&task->cb_send, ev);
                }

                complete_tasks();
            }

            status_t X11Display::handle_selection_request(cb_send_t *task, XSelectionRequestEvent *ev)
            {
                status_t res    = STATUS_OK;

                // Prepare SelectionNotify event
                XEvent response;
                XSelectionEvent *se = &response.xselection;

                se->type        = SelectionNotify;
                se->send_event  = True;
                se->display     = pDisplay;
                se->requestor   = ev->requestor;
                se->selection   = ev->selection;
                se->target      = ev->target;
                se->property    = ev->property;
                se->time        = ev->time;

                if (ev->target == sAtoms.X11_TARGETS)
                {
                    // Special case: send all supported targets
                    lsp_trace("Requested TARGETS for selection");
                    const char *const *mime = task->pSource->mime_types();

                    // Estimate number of mime types
                    size_t n = 1; // also return X11_TARGETS
                    for (const char *const *p = mime; *p != NULL; ++p, ++n) { /* nothing */ }

                    // Allocate memory and initialize MIME types
                    Atom *data  = reinterpret_cast<Atom *>(::malloc(sizeof(Atom) * n));
                    if (data != NULL)
                    {
                        Atom *dst   = data;
                        *dst        = sAtoms.X11_TARGETS;
                        lsp_trace("  supported target: TARGETS (%ld)", long(*dst));

                        for (const char *const *p = mime; *p != NULL; ++p, ++dst)
                        {
                            *dst    = ::XInternAtom(pDisplay, *p, False);
                            lsp_trace("  supported target: %s (%ld)", *p, long(*dst));
                        }

                        // Write property to the target window and send SelectionNotify event
                        ::XChangeProperty(pDisplay, task->hRequestor, task->hProperty,
                                sAtoms.X11_XA_ATOM, 32, PropModeReplace,
                                reinterpret_cast<unsigned char *>(data), n);
                        ::XFlush(pDisplay);
                        ::XSendEvent(pDisplay, ev->requestor, True, NoEventMask, &response);
                        ::XFlush(pDisplay);

                        // Free allocated buffer
                        ::free(data);
                    }
                    else
                        res = STATUS_NO_MEM;
                }
                else
                {
                    char *mime  = ::XGetAtomName(pDisplay, ev->target);
                    lsp_trace("Requested MIME type is 0x%lx (%s)", long(ev->target), mime);

                    if (mime != NULL)
                    {
                        // Open the input stream
                        io::IInStream *in   = task->pSource->open(mime);
                        if (in != NULL)
                        {
                            // Store stream and data type
                            task->hType     = ev->target;

                            // Determine the used method for transfer
                            wssize_t avail  = in->avail();
                            if (avail == -STATUS_NOT_IMPLEMENTED)
                                avail = 2 * X11IOBUF_SIZE;

                            if (avail > X11IOBUF_SIZE)
                            {
                                // Do incremental transfer
                                task->pStream   = in;   // Save the stream
                                ::XSelectInput(pDisplay, task->hRequestor, PropertyChangeMask);
                                ::XChangeProperty(
                                    pDisplay, task->hRequestor, task->hProperty,
                                    sAtoms.X11_INCR, 32, PropModeReplace, NULL, 0
                                );
                                ::XFlush(pDisplay);
                                ::XSendEvent(pDisplay, ev->requestor, True, NoEventMask, &response);
                                ::XFlush(pDisplay);
                            }
                            else if (avail > 0)
                            {
                                // One-time write()
                                uint8_t *ptr    = reinterpret_cast<uint8_t *>(::malloc(avail));
                                if (ptr != NULL)
                                {
                                    // Fetch data from stream
                                    avail   = in->read_fully(ptr, avail);
                                    if (avail == -STATUS_EOF)
                                        avail   = 0;

                                    if (avail >= 0)
                                    {
                                        // All is OK, set the property and complete transfer
                                        ::XChangeProperty(
                                            pDisplay, task->hRequestor, task->hProperty,
                                            task->hType, 8, PropModeReplace,
                                            reinterpret_cast<unsigned char *>(ptr), avail
                                        );
                                        ::XFlush(pDisplay);
                                        ::XSendEvent(pDisplay, ev->requestor, True, NoEventMask, &response);
                                        ::XFlush(pDisplay);
                                        task->bComplete = true;
                                    }
                                    else
                                        res     = -avail;
                                    ::free(ptr);
                                }
                                else
                                    res = STATUS_NO_MEM;

                                // Close the stream
                                in->close();
                                delete in;
                            }
                            else
                                res = status_t(-avail);
                        }
                        else
                            res = STATUS_UNSUPPORTED_FORMAT;

                        ::XFree(mime);
                    }
                    else
                        res = STATUS_UNSUPPORTED_FORMAT;
                }

                return res;
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
                Window src_wnd  = ev->xany.window;
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
                    {
                        XClientMessageEvent *ce = &ev->xclient;
                        Atom type = ce->message_type;

                        if (type == sAtoms.X11_WM_PROTOCOLS)
                        {
                            if (ce->data.l[0] == long(sAtoms.X11_WM_DELETE_WINDOW))
                                ue.nType        = UIE_CLOSE;
                            else
                                lsp_trace("received client WM_PROTOCOLS message with argument %s",
                                        XGetAtomName(pDisplay, ce->data.l[0]));
                        }
                        else if (type == sAtoms.X11_XdndEnter)
                        {
                            /**
                            data.l[0] contains the XID of the source window.

                            data.l[1]:

                               Bit 0 is set if the source supports more than three data types.
                               The high byte contains the protocol version to use (minimum of the source's
                               and target's highest supported versions).
                               The rest of the bits are reserved for future use.

                            data.l[2,3,4] contain the first three types that the source supports.
                               Unused slots are set to None. The ordering is arbitrary since, in general,
                               the source cannot know what the target prefers.
                            */
                            lsp_trace("Received XdndEnter: wnd=0x%lx, ext=%s",
                                    long(ev->xclient.data.l[0]),
                                    ((ev->xclient.data.l[1] & 1) ? "true" : "false")
                                );

                            // Remember mime types
                            cvector<char> mime_types;
                            status_t res = read_dnd_mime_types(ce, &mime_types);
                            for (size_t i=0, n=mime_types.size(); i<n; ++i)
                            {
                                char *mime = mime_types.at(i);
                                lsp_trace("Supported MIME type: %s", mime);
                            }

                            drop_mime_types(&vDndMimeTypes);
                            if (res == STATUS_OK)
                            {
                                vDndMimeTypes.swap_data(&mime_types);
                                ue.nType        = UIE_DRAG_ENTER;
                                ue.nState       = vDndMimeTypes.size();
                                hDndSource      = ce->data.l[0];
                                src_wnd         = hRootWnd;
                            }
                            else
                                hDndSource      = None;
                            drop_mime_types(&mime_types);
                        }
                        else if (type == sAtoms.X11_XdndLeave)
                        {
                            /**
                            Sent from source to target to cancel the drop.

                               data.l[0] contains the XID of the source window.
                               data.l[1] is reserved for future use (flags).
                            */
                            lsp_trace("Received XdndLeave");
                            hDndSource          = None;
                            ue.nType            = UIE_DRAG_LEAVE;
                        }
                        else if (type == sAtoms.X11_XdndPosition)
                        {
                            /**
                            Sent from source to target to provide mouse location.

                               data.l[0] contains the XID of the source window.
                               data.l[1] is reserved for future use (flags).
                               data.l[2] contains the coordinates of the mouse position relative to the root window.
                                   data.l[2] = (x << 16) | y;
                               data.l[3] contains the time stamp for retrieving the data. (new in version 1)
                               data.l[4] contains the action requested by the user. (new in version 2)
                            */

                            long x = (ce->data.l[2] >> 16), y = (ce->data.l[2] & 0xffff);
                            lsp_trace("Received XdndPosition: wnd=0x%lx, flags=0x%lx, x=%ld, y=%ld, timestamp=%ld action=%ld (%s)",
                                    ce->data.l[0], ce->data.l[1], x, y, ce->data.l[3], ce->data.l[4], XGetAtomName(pDisplay, ce->data.l[4])
                                    );

                            hDndSource          = ev->xclient.data.l[0];
                            src_wnd             = hRootWnd;

                            // Form the notification event
                            Atom act            = ce->data.l[4];

                            ue.nType            = UIE_DRAG_REQUEST;
                            ue.nLeft            = x;
                            ue.nTop             = y;
                            ue.nTime            = ce->data.l[3];

                            if (act == sAtoms.X11_XdndActionCopy)
                                ue.nState           = DRAG_COPY;
                            else if (act == sAtoms.X11_XdndActionMove)
                                ue.nState           = DRAG_MOVE;
                            else if (act == sAtoms.X11_XdndActionLink)
                                ue.nState           = DRAG_LINK;
                            else if (act == sAtoms.X11_XdndActionAsk)
                                ue.nState           = DRAG_ASK;
                            else if (act == sAtoms.X11_XdndActionPrivate)
                                ue.nState           = DRAG_PRIVATE;
                            else if (act == sAtoms.X11_XdndActionDirectSave)
                                ue.nState           = DRAG_DIRECT_SAVE;
                            else
                                ue.nState           = DRAG_NONE;
                        }
                        else if (type == sAtoms.X11_XdndDrop)
                        {
                            /**
                            Sent from source to target to complete the drop.
                            data.l[0] contains the XID of the source window.
                            data.l[1] is reserved for future use (flags).
                            data.l[2] contains the time stamp for retrieving the data. (new in version 1)
                            */
                            lsp_trace("Received XdndDrop wnd=0x%lx, ts=%ld", ce->data.l[0], ce->data.l[2]);
                            if (ce->data.l[0] == hDndSource)
                            {
                                ue.nType            = UIE_DRAG_DROP;
                                ue.nTime            = ce->data.l[2];
                                src_wnd             = hRootWnd;
                            }
                        }
                        else
                            lsp_trace("received client message of type %s",
                                    XGetAtomName(pDisplay, ev->xclient.message_type));
                        break;
                    }

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
                    } // switch(se.nType)

                    // Deliver the message to target windows
                    for (size_t i=0, nwnd = sTargets.size(); i<nwnd; ++i)
                    {
                        X11Window *wnd = sTargets.at(i);

                        // Translate coordinates if originating and target window differs
                        int x, y;
                        XTranslateCoordinates(pDisplay,
                            src_wnd, wnd->x11handle(),
                            ue.nLeft, ue.nTop,
                            &x, &y, &child);
                        se.nLeft    = x;
                        se.nTop     = y;

//                        lsp_trace("Sending event to target=%p", wnd);
                        wnd->handle_event(&se);
                    }
                }
            }

            status_t X11Display::read_dnd_mime_types(XClientMessageEvent *ev, cvector<char> *ctype)
            {
                // Do not need to fetch long list?
                if (!(ev->data.l[1] & 1))
                {
                    for (size_t i=2; i<5; ++i)
                    {
                        if (ev->data.l[i] == None)
                            continue;
                        char *a_name = XGetAtomName(pDisplay, ev->data.l[i]);
                        if (a_name == NULL)
                            continue;

                        // Add atom name to list
                        if ((a_name = strdup(a_name)) == NULL)
                            return STATUS_NO_MEM;
                        if (!ctype->add(a_name))
                        {
                            free(a_name);
                            return STATUS_NO_MEM;
                        }
                    }

                    return STATUS_OK;
                }

                // Fetch long list of supported MIME types
                Atom p_type = None;
                int p_fmt = 0;
                unsigned long p_nitems = 0, p_size = 0, p_offset = 0;
                unsigned char *p_data = NULL;

                do
                {
                    // Read with 64k chunks
                    XGetWindowProperty(
                        pDisplay, ev->data.l[0], sAtoms.X11_XdndTypeList,
                        p_offset, X11IOBUF_SIZE/4, False, sAtoms.X11_XA_ATOM,
                        &p_type, &p_fmt, &p_nitems, &p_size, &p_data
                    );

                    // Analyze property type
                    if ((p_type != sAtoms.X11_XA_ATOM) || (p_fmt != 32))
                        break;

                    long *ids = reinterpret_cast<long *>(p_data);
                    for (unsigned long i=0; i<p_nitems; ++i)
                    {
                        if (ids == None)
                            continue;
                        char *a_name = XGetAtomName(pDisplay, ids[i]);
                        if (a_name == NULL)
                            continue;

                        // Add atom name to list
                        if ((a_name = strdup(a_name)) == NULL)
                            return STATUS_NO_MEM;
                        if (!ctype->add(a_name))
                        {
                            free(a_name);
                            return STATUS_NO_MEM;
                        }
                    }

                    // Free buffer and update read position
                    if (p_data != NULL)
                        XFree(p_data);
                    p_offset       += p_nitems;
                } while ((p_size > 0) && (p_nitems > 0));

                return STATUS_OK;
            }

            void X11Display::drop_mime_types(cvector<char> *ctype)
            {
                for (size_t i=0, n=ctype->size(); i<n; ++i)
                {
                    char *mime = ctype->at(i);
                    if (mime != NULL)
                        ::free(mime);
                }
                ctype->flush();
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

                    // Check legacy clipboard requests
                    for (size_t i=0, n=sCbRequests.size(); i<n; ++i)
                    {
                        cb_request_t *req = sCbRequests.at(i);
                        if (req->hProperty == atom)
                        {
                            atom = None;
                            break;
                        }
                    }
                    if (atom == None)
                        continue;

                    // Check pending async tasks
                    for (size_t i=0, n=sAsync.size(); i<n; ++i)
                    {
                        x11_async_t *task   = sAsync.at(i);
                        if ((task->type == X11ASYNC_CB_RECV) && (task->cb_recv.hProperty == atom))
                            atom = None;
                        else if ((task->type == X11ASYNC_CB_SEND) && (task->cb_send.hProperty == atom))
                            atom = None;
                        else if ((task->type == X11ASYNC_DND_RECV) && (task->dnd_recv.hProperty == atom))
                            atom = None;
                        if (atom == None)
                            break;
                    }

                    if (atom != None)
                        return atom;
                }
                return None;
            }

            status_t X11Display::setClipboard(size_t id, IDataSource *ds)
            {
                // Acquire reference
                if (ds != NULL)
                    ds->acquire();

                // Check arguments
                if ((id < 0) || (id >= _CBUF_TOTAL))
                    return STATUS_BAD_ARGUMENTS;

                // Try to set clipboard owner
                Atom aid;
                status_t res        = bufid_to_atom(id, &aid);
                if (res != STATUS_OK)
                {
                    if (ds != NULL)
                        ds->release();
                    return res;
                }

                // Release previous placeholder
                if (pCbOwner[id] != NULL)
                {
                    pCbOwner[id]->release();
                    pCbOwner[id]    = NULL;
                }

                // There is no selection owner?
                if (ds == NULL)
                {
                    ::XSetSelectionOwner(pDisplay, aid, None, CurrentTime);
                    ::XFlush(pDisplay);
                    return STATUS_OK;
                }

                // Notify that our window is owning a selection
                pCbOwner[id]    = ds;
                ::XSetSelectionOwner(pDisplay, aid, hClipWnd, CurrentTime);
                ::XFlush(pDisplay);

                return STATUS_OK;
            }

            status_t X11Display::sink_data_source(IDataSink *dst, IDataSource *src)
            {
                status_t result = STATUS_OK;

                // Fetch list of MIME types
                src->acquire();

                const char *const *mimes = src->mime_types();
                if (mimes != NULL)
                {
                    // Open sink
                    ssize_t idx = dst->open(mimes);
                    if (idx >= 0)
                    {
                        // Open source
                        io::IInStream *s = src->open(mimes[idx]);
                        if (s != NULL)
                        {
                            // Perform data copy
                            uint8_t buf[1024];
                            while (true)
                            {
                                // Read the buffer from the stream
                                ssize_t nread = s->read(buf, sizeof(buf));
                                if (nread < 0)
                                {
                                    if (nread != -STATUS_EOF)
                                        result = -nread;
                                    break;
                                }

                                // Write the buffer to the sink
                                result = dst->write(buf, nread);
                                if (result != STATUS_OK)
                                    break;
                            }

                            // Close the stream
                            if (result == STATUS_OK)
                                result = s->close();
                            else
                                s->close();
                        }
                        else
                            result = STATUS_UNKNOWN_ERR;

                        // Close sink
                        dst->close(result);
                    }
                    else
                        result  = -idx;
                }
                else
                    result = STATUS_NO_DATA;

                src->release();

                return result;
            }

            status_t X11Display::getClipboard(size_t id, IDataSink *dst)
            {
                // Acquire data sink
                if (dst == NULL)
                    return STATUS_BAD_ARGUMENTS;
                dst->acquire();

                // Convert clipboard type to atom
                Atom sel_id;
                status_t result = bufid_to_atom(id, &sel_id);
                if (result != STATUS_OK)
                {
                    dst->release();
                    return STATUS_BAD_ARGUMENTS;
                }

                // First, check that it's our window to avoid X11 transfers
                Window wnd  = ::XGetSelectionOwner(pDisplay, sel_id);
                if (wnd == hClipWnd)
                {
                    // Perform direct data transfer because we're owner of the selection
                    result = (pCbOwner[id] != NULL) ?
                            sink_data_source(dst, pCbOwner[id]) : STATUS_NO_DATA;
                    dst->release();
                    return result;
                }

                // Release previously used placeholder
                if (pCbOwner[id] != NULL)
                {
                    pCbOwner[id]->release();
                    pCbOwner[id]    = NULL;
                }

                // Generate property identifier
                Atom prop_id = gen_selection_id();
                if (prop_id == None)
                    return STATUS_UNKNOWN_ERR;

                // Create async task
                x11_async_t *task   = sAsync.add();
                task->type          = X11ASYNC_CB_RECV;
                task->result        = STATUS_OK;

                cb_recv_t *param    = &task->cb_recv;
                param->bComplete    = false;
                param->hProperty    = prop_id;
                param->hSelection   = sel_id;
                param->hType        = None;
                param->enState      = CB_RECV_CTYPE;
                param->pSink        = dst;

                // Request conversion
                ::XConvertSelection(pDisplay, sel_id, sAtoms.X11_TARGETS, prop_id, hClipWnd, CurrentTime);
                ::XFlush(pDisplay);

                return STATUS_OK;
            }

        } /* namespace x11 */
    } /* namespace ws */
} /* namespace lsp */

#endif /* USE_X11_DISPLAY */

