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

#define X11IOBUF_SIZE               0x100000

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

            volatile atomic_t X11Display::hLock     = 0;
            X11Display  *X11Display::pHandlers      = NULL;

            X11Display::X11Display()
            {
                pNextHandler    = NULL;
                bExit           = false;
                pDisplay        = NULL;
                hRootWnd        = -1;
                hClipWnd        = None;
                nBlackColor     = 0;
                nWhiteColor     = 0;
                nIOBufSize      = X11IOBUF_SIZE;
                pIOBuf          = NULL;

                for (size_t i=0; i<_CBUF_TOTAL; ++i)
                    pCbOwner[i]             = NULL;
            }

            X11Display::~X11Display()
            {
                do_destroy();
            }

            int X11Display::init(int argc, const char **argv)
            {
                // Enable multi-threading
                ::XInitThreads();

                // Set-up custom handler
                while (true)
                {
                    if (atomic_cas(&hLock, 0, 1))
                    {
                        pNextHandler    = pHandlers;
                        pHandlers       = this;
                        hLock           = 0;
                        break;
                    }
                }

                // Open the display
                pDisplay        = ::XOpenDisplay(NULL);
                if (pDisplay == NULL)
                {
                    lsp_error("Can not open display");
                    return STATUS_NO_DEVICE;
                }

                // Get Root window and screen
                size_t screens  = ScreenCount(pDisplay);
                hRootWnd        = DefaultRootWindow(pDisplay);
                int dfl         = DefaultScreen(pDisplay);
                nBlackColor     = BlackPixel(pDisplay, dfl);
                nWhiteColor     = WhitePixel(pDisplay, dfl);

                for (size_t i=0; i<screens; ++i)
                {
                    x11_screen_t *s     = vScreens.add();
                    if (s == NULL)
                        return STATUS_NO_MEM;
                    s->id           = i;
                    s->grabs        = 0;
                    s->width        = DisplayWidth(pDisplay, i);
                    s->height       = DisplayHeight(pDisplay, i);
                    s->mm_width     = DisplayWidthMM(pDisplay, i);
                    s->mm_height    = DisplayHeightMM(pDisplay, i);

                    lsp_trace("Added display #%d: %ldx%ld (%ldx%ld mm)", int(i),
                            long(s->width), long(s->height), long(s->mm_width), long(s->mm_height));
                }


                // Allocate I/O buffer
                nIOBufSize      = ::XExtendedMaxRequestSize(pDisplay) / 4;
                if (nIOBufSize == 0)
                    nIOBufSize      = ::XMaxRequestSize(pDisplay) / 4;
                if (nIOBufSize == 0)
                    nIOBufSize      = 0x1000; // Guaranteed IO buf size
                if (nIOBufSize > X11IOBUF_SIZE)
                    nIOBufSize      = X11IOBUF_SIZE;
                pIOBuf          = reinterpret_cast<uint8_t *>(::malloc(nIOBufSize));
                if (pIOBuf == NULL)
                    return STATUS_NO_MEM;

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
                        blank = ::XCreateBitmapFromData (pDisplay, hRootWnd, data, 1, 1);
                        if (blank == None)
                            return STATUS_NO_MEM;
                        vCursors[i] = ::XCreatePixmapCursor(pDisplay, blank, blank, &dummy, &dummy, 0, 0);
                        ::XFreePixmap(pDisplay, blank);
                    }
                    else
                        vCursors[i] = ::XCreateFontCursor(pDisplay, id);
                }

                return IDisplay::init(argc, argv);
            }

            int X11Display::x11_error_handler(Display *dpy, XErrorEvent *ev)
            {
                while (!atomic_cas(&hLock, 0, 1)) { /* Wait */ }

                // Dispatch errors between Displays
                for (X11Display *dp = pHandlers; dp != NULL; ++dp)
                    if (dp->pDisplay == dpy)
                        dp->handle_error(ev);

                hLock   = 0;
                return 0;
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
                // Cancel async tasks
                for (size_t i=0, n=sAsync.size(); i<n; ++i)
                {
                    x11_async_t *task           = sAsync.get(i);
                    if (!task->cb_common.bComplete)
                    {
                        task->result                = STATUS_CANCELLED;
                        task->cb_common.bComplete   = true;
                    }
                }
                complete_async_tasks();

                // Drop clipboard data sources
                for (size_t i=0; i<_CBUF_TOTAL; ++i)
                {
                    if (pCbOwner[i] != NULL)
                    {
                        pCbOwner[i]->release();
                        pCbOwner[i] = NULL;
                    }
                }

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

                vWindows.flush();
                sPending.flush();
                for (size_t i=0; i<__GRAB_TOTAL; ++i)
                    vGrab[i].clear();
                sTargets.clear();
                drop_mime_types(&vDndMimeTypes);

                if (pIOBuf != NULL)
                {
                    ::free(pIOBuf);
                    pIOBuf = NULL;
                }

                Display *dpy = pDisplay;
                if (dpy != NULL)
                {
                    pDisplay        = NULL;
                    ::XFlush(dpy);
                    ::XCloseDisplay(dpy);
                }

                // Remove custom handler
                while (true)
                {
                    if (atomic_cas(&hLock, 0, 1))
                    {
                        X11Display **pd = &pHandlers;
                        while (*pd != NULL)
                        {
                            if (*pd == this)
                                *pd = (*pd)->pNextHandler;
                            else
                                pd  = &((*pd)->pNextHandler);
                        }
                        hLock   = 0;
                        break;
                    }
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

                // Call for main task
                call_main_task(ts);

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

            bool X11Display::handle_clipboard_event(XEvent *ev)
            {
                switch (ev->type)
                {
                    case PropertyNotify:
                    {
                        XPropertyEvent *sc          = &ev->xproperty;
                        #ifdef LSP_TRACE
                        char *name                  = ::XGetAtomName(pDisplay, sc->atom);
//                        lsp_trace("XPropertyEvent for window 0x%lx, property %ld (%s), state=%d", long(sc->window), long(sc->atom), name, int(sc->state));
                        ::XFree(name);
                        #endif
                        handle_property_notify(sc);
                        return true;
                    }
                    case SelectionClear:
                    {
                        // Free the corresponding data
                        XSelectionClearEvent *sc    = &ev->xselectionclear;
                        lsp_trace("XSelectionClearEvent for window 0x%lx, selection %ld", long(sc->window), long(sc->selection));

                        handle_selection_clear(sc);
                        return true;
                    }
                    case SelectionRequest:
                    {
                        XSelectionRequestEvent *sr   = &ev->xselectionrequest;
                        #ifdef LSP_TRACE
                        char *asel = XGetAtomName(pDisplay, sr->selection);
                        char *atar = XGetAtomName(pDisplay, sr->target);
                        char *aprop = XGetAtomName(pDisplay, sr->property);
                        lsp_trace("SelectionRequest requestor = 0x%x, selection=%d (%s), target=%d (%s), property=%d (%s), time=%ld",
                                     int(sr->requestor),
                                     int(sr->selection), asel,
                                     int(sr->target), atar,
                                     int(sr->property), aprop,
                                     long(sr->time));
                        ::XFree(asel);
                        ::XFree(atar);
                        ::XFree(aprop);
                        #endif
                        handle_selection_request(sr);
                        return true;
                    }
                    case SelectionNotify:
                    {
                        // Check that it's proper selection event
                        XSelectionEvent *se = &ev->xselection;
                        if (se->property == None)
                            return true;

                        #ifdef LSP_TRACE
                        char *aname = ::XGetAtomName(pDisplay, se->property);
                        lsp_trace("SelectionNotify for window=0x%lx, selection=%ld, property=%ld (%s)",
                                long(se->requestor), long(se->selection), long(se->property), aname);
                        if (aname != NULL)
                            ::XFree(aname);
                        #endif
                        handle_selection_notify(se);

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
                        pDisplay, wnd, property,
                        p_offset / 4, nIOBufSize / 4, False, ptype,
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
                    if (list[i] == None)
                        continue;
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
                        ::XFree(a_name);
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
                    if (task->cb_common.bComplete)
                        continue;

                    // Notify all possible tasks about the event
                    switch (task->type)
                    {
                        case X11ASYNC_CB_RECV:
                            if (task->cb_recv.hProperty == ev->property)
                                task->result = handle_selection_notify(&task->cb_recv, ev);
                            break;
                        case X11ASYNC_DND_RECV:
                            if ((task->dnd_recv.hProperty == ev->property) &&
                                (task->dnd_recv.hTarget == ev->requestor))
                                task->result = handle_selection_notify(&task->dnd_recv, ev);
                            break;
                        default:
                            break;
                    }

                    if (task->result != STATUS_OK)
                        task->cb_common.bComplete   = true;
                }
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
                    if (task->cb_common.bComplete)
                        continue;

                    // Notify all possible tasks about the event
                    switch (task->type)
                    {
                        case X11ASYNC_CB_RECV:
                            if (task->cb_recv.hProperty == ev->atom)
                                task->result = handle_property_notify(&task->cb_recv, ev);
                            break;
                        case X11ASYNC_DND_RECV:
                            if ((task->dnd_recv.hProperty == ev->atom) &&
                                (task->dnd_recv.hTarget == ev->window))
                                task->result = handle_property_notify(&task->dnd_recv, ev);
                            break;
                        case X11ASYNC_CB_SEND:
                            if ((task->cb_send.hProperty == ev->atom) &&
                                (task->cb_send.hRequestor == ev->window))
                            {
                                status_t result = handle_property_notify(&task->cb_send, ev);
                                if (task->result == STATUS_OK)
                                    task->result    = result;
                            }
                            break;
                        default:
                            break;
                    }

                    if (task->result != STATUS_OK)
                        task->cb_common.bComplete   = true;
                }
            }

            void X11Display::complete_async_tasks()
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
                        case X11ASYNC_DND_RECV:
                            // Close and release data sink
                            if (task->dnd_recv.pSink != NULL)
                            {
                                task->dnd_recv.pSink->close(task->result);
                                task->dnd_recv.pSink->release();
                                task->dnd_recv.pSink = NULL;
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
                                    // Complete the INCR transfer: close and release the sink
                                    task->pSink->close(res);
                                    task->pSink->release();
                                    task->pSink     = NULL;
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

            status_t X11Display::handle_property_notify(dnd_recv_t *task, XPropertyEvent *ev)
            {
                status_t res    = STATUS_OK;
                uint8_t *data   = NULL;
                size_t bytes    = 0;
                Atom type       = None;

                switch (task->enState)
                {
                    case DND_RECV_INCR:
                    {
                        // Read incrementally property contents
                        if (ev->state == PropertyNewValue)
                        {
                            res = read_property(task->hTarget, task->hProperty, task->hType, &data, &bytes, &type);
                            if (res == STATUS_OK)
                            {
                                // Check property type
                                if (bytes <= 0) // End of transfer?
                                {
                                    // Complete the INCR transfer: close the sink and release it
                                    task->pSink->close(res);
                                    task->pSink->release();
                                    task->pSink     = NULL;

                                    complete_dnd_transfer(task, true);
                                    task->bComplete = true;
                                }
                                else if (type == task->hType)
                                {
                                    res = task->pSink->write(data, bytes); // Append data to the sink
                                    ::XDeleteProperty(pDisplay, hClipWnd, task->hProperty); // Request next chunk
                                    ::XFlush(pDisplay);
                                }
                                else
                                {
                                    complete_dnd_transfer(task, false);
                                    res     = STATUS_UNSUPPORTED_FORMAT;
                                }
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

                // Override error handler
                ::XSync(pDisplay, False);
                XErrorHandler old = ::XSetErrorHandler(x11_error_handler);

                // Read data from the stream
                ssize_t nread   = task->pStream->read_fully(pIOBuf, nIOBufSize);
                status_t res    = STATUS_OK;
                if (nread > 0)
                {
                    // Write the property to re requestor
                    lsp_trace("Sending %d bytes as incremental chunk to consumer", int(nread));
                    ::XChangeProperty(
                        pDisplay, task->hRequestor, task->hProperty,
                        task->hType, 8, PropModeReplace,
                        reinterpret_cast<unsigned char *>(pIOBuf), nread
                    );
                }
                else
                {
                    if ((nread < 0) && (nread != -STATUS_EOF))
                        res = -nread;
                    task->bComplete = true;

                    lsp_trace("Completing INCR transfer, result is %d", int(res));
                    ::XSelectInput(pDisplay, task->hRequestor, None);
                    ::XChangeProperty(
                        pDisplay, task->hRequestor, task->hProperty,
                        task->hType, 8, PropModeReplace, NULL, 0
                    );
                }

                ::XSync(pDisplay, False);
                ::XSetErrorHandler(old);

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

            status_t X11Display::handle_selection_notify(dnd_recv_t *task, XSelectionEvent *ev)
            {
                uint8_t *data   = NULL;
                size_t bytes    = 0;
                Atom type       = None;
                status_t res    = STATUS_OK;

                // Analyze state
                switch (task->enState)
                {
                    case DND_RECV_SIMPLE:
                    {
                        // We expect property of type INCR or of type task->hType
                        res = read_property(task->hTarget, task->hProperty, task->hType, &data, &bytes, &type);
                        if (res == STATUS_OK)
                        {
                            if (type == sAtoms.X11_INCR)
                            {
                                // Initiate INCR mode transfer
                                ::XDeleteProperty(pDisplay, task->hTarget, task->hProperty);
                                ::XFlush(pDisplay);
                                task->enState       = DND_RECV_INCR;
                            }
                            else if (type == task->hType)
                            {
                                ::XDeleteProperty(pDisplay, task->hTarget, task->hProperty); // Remove property
                                ::XFlush(pDisplay);

                                if (bytes > 0)
                                    res = task->pSink->write(data, bytes);

                                complete_dnd_transfer(task, true);
                                task->bComplete = true;
                            }
                            else
                            {
                                complete_dnd_transfer(task, false);
                                res     = STATUS_UNSUPPORTED_FORMAT;
                            }
                        }

                        break;
                    }

                    case DND_RECV_INCR:
                    {
                        // Read incrementally property contents
                        res = read_property(task->hTarget, task->hProperty, task->hType, &data, &bytes, &type);
                        if (res == STATUS_OK)
                        {
                            // Check property type
                            if (bytes <= 0) // End of transfer?
                            {
                                // Complete the INCR transfer
                                ::XDeleteProperty(pDisplay, task->hTarget, task->hProperty); // Delete the property
                                ::XFlush(pDisplay);

                                // Notify source about end of transfer
                                complete_dnd_transfer(task, true);
                                task->bComplete = true;
                            }
                            else if (type == task->hType)
                            {
                                ::XDeleteProperty(pDisplay, task->hTarget, task->hProperty); // Request next chunk
                                ::XFlush(pDisplay);
                                res     = task->pSink->write(data, bytes); // Append data to the sink
                            }
                            else
                            {
                                complete_dnd_transfer(task, false);
                                res     = STATUS_UNSUPPORTED_FORMAT;
                            }
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
                    if (task->cb_common.bComplete)
                        continue;

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

                    if (task->result != STATUS_OK)
                        task->cb_common.bComplete   = true;
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
                    if (task->result != STATUS_OK)
                        task->cb_common.bComplete   = true;
                }
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
                        *(dst++)    = sAtoms.X11_TARGETS;
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
                                avail = nIOBufSize << 1;

                            if (avail > ssize_t(nIOBufSize))
                            {
                                // Do incremental transfer
                                lsp_trace("Initiating incremental transfer");
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
                                // Fetch data from stream
                                avail   = in->read_fully(pIOBuf, avail);
                                if (avail == -STATUS_EOF)
                                    avail   = 0;

                                if (avail >= 0)
                                {
                                    lsp_trace("Read %ld bytes, transmitting directly to consumer", long(avail));

                                    // All is OK, set the property and complete transfer
                                    ::XChangeProperty(
                                        pDisplay, task->hRequestor, task->hProperty,
                                        task->hType, 8, PropModeReplace,
                                        reinterpret_cast<unsigned char *>(pIOBuf), avail
                                    );
                                    ::XFlush(pDisplay);
                                    ::XSendEvent(pDisplay, ev->requestor, True, NoEventMask, &response);
                                    ::XFlush(pDisplay);
                                    task->bComplete = true;
                                }
                                else
                                    res     = -avail;

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

                #if 0
                lsp_trace("Received event: %d (%s), serial = %ld, window = %x",
                    int(ev->type), event_name(ev->type), long(ev->xany.serial), int(ev->xany.window));


                if (ev->type == PropertyNotify)
                {
                    XPropertyEvent *pe = &ev->xproperty;
                    #ifdef LSP_TRACE
                    char *name = ::XGetAtomName(pDisplay, pe->atom);
                    lsp_trace("Received PropertyNotifyEvent: property=%s, state=%lx", name, (long)pe->state);
                    ::XFree(name);
                    #endif
                }
                #endif

                // Special case for buffers
                if (handle_clipboard_event(ev))
                {
                    complete_async_tasks();
                    return;
                }

                if (handle_drag_event(ev))
                {
                    complete_async_tasks();
                    return;
                }

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
//                    else if ((ev->type == ConfigureNotify) &&
//                            (wnd->x11parent() != None) &&
//                            (wnd->x11parent() == ev->xany.window))
//                    {
//                        lsp_trace("resize window: handle=%lx, width=%d, height=%d",
//                                long(wnd->x11handle()),
//                                int(ev->xconfigure.width),
//                                int(ev->xconfigure.height));
//                        ::XResizeWindow(pDisplay, wnd->x11handle(), ev->xconfigure.width, ev->xconfigure.height);
//                        ::XFlush(pDisplay);
//                        return;
//                    }
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
                    {
                        XClientMessageEvent *ce = &ev->xclient;
                        Atom type = ce->message_type;

                        if (type == sAtoms.X11_WM_PROTOCOLS)
                        {
                            if (ce->data.l[0] == long(sAtoms.X11_WM_DELETE_WINDOW))
                                ue.nType        = UIE_CLOSE;
                            else
                            {
                                #ifdef LSP_TRACE
                                char *name = ::XGetAtomName(pDisplay, ce->data.l[0]);
                                lsp_trace("received client WM_PROTOCOLS message with argument %s", name);
                                ::XFree(name);
                                #endif /* LSP_TRACE */
                            }
                        }
                        else
                        {
                            #ifdef LSP_TRACE
                            char *a_name = ::XGetAtomName(pDisplay, ev->xclient.message_type);
                            lsp_trace("received client message of type %s", a_name);
                            ::XFree(a_name);
                            #endif
                        }
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
                            // Check if there is grab enabled and obtain list of receivers
                            bool has_grab = false;
                            for (ssize_t i=__GRAB_TOTAL-1; i>=0; --i)
                            {
                                cvector<X11Window> &g = vGrab[i];
                                if (g.size() <= 0)
                                    continue;

                                // Add listeners from grabbing windows
                                for (size_t i=0; i<g.size();)
                                {
                                    X11Window *wnd = g.at(i);
                                    if ((wnd == NULL) || (vWindows.index_of(wnd) < 0))
                                    {
                                        g.remove(i, false);
                                        continue;
                                    }
                                    sTargets.add(wnd);
                                    ++i;
                                }

                                // Finally, break if there are target windows
                                if (sTargets.size() > 0)
                                {
                                    has_grab = true;
                                    break;
                                }
                            }

                            if (has_grab)
                            {
                                // Allow event replay
                                if ((se.nType == UIE_KEY_DOWN) || (se.nType == UIE_KEY_UP))
                                    ::XAllowEvents(pDisplay, ReplayKeyboard, CurrentTime);
                                else if (se.nType != UIE_CLOSE)
                                    ::XAllowEvents(pDisplay, ReplayPointer, CurrentTime);
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
                                    sTargets.set(i, redirect);
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
                        ::XTranslateCoordinates(pDisplay,
                            ev->xany.window, wnd->x11handle(),
                            ue.nLeft, ue.nTop,
                            &x, &y, &child);
                        se.nLeft    = x;
                        se.nTop     = y;

//                        lsp_trace("Sending event to target=%p", wnd);
                        wnd->handle_event(&se);
                    }
                }
            }

            X11Display::x11_async_t *X11Display::find_dnd_proxy_task(Window wnd)
            {
                for (size_t i=0, n=sAsync.size(); i<n; ++i)
                {
                    x11_async_t *task = sAsync.at(i);
                    if (task->cb_common.bComplete)
                        continue;
                    if ((task->type == X11ASYNC_DND_PROXY) &&
                        (task->dnd_proxy.hTarget = wnd))
                        return task;
                }

                return NULL;
            }

            bool X11Display::handle_drag_event(XEvent *ev)
            {
                // It SHOULD be a client message
                if (ev->type != ClientMessage)
                    return false;

                XClientMessageEvent *ce = &ev->xclient;
                Atom type = ce->message_type;

                if (type == sAtoms.X11_XdndEnter)
                {
                    lsp_trace("Received XdndEnter");

                    // Cancel all previous tasks
                    for (size_t i=0, n=sAsync.size(); i<n; ++i)
                    {
                        x11_async_t *task = sAsync.at(i);
                        if ((task->type == X11ASYNC_DND_RECV) && (!task->cb_common.bComplete))
                        {
                            task->result        = STATUS_CANCELLED;
                            task->cb_common.bComplete = true;
                        }
                    }

                    // Create new task
                    handle_drag_enter(ce);

                    return true;
                }
                else if (type == sAtoms.X11_XdndLeave)
                {
                    lsp_trace("Received XdndLeave");

                    // Handle proxy tasks first
                    x11_async_t *task = find_dnd_proxy_task(ce->window);
                    if (task == NULL)
                    {
                        for (size_t i=0, n=sAsync.size(); i<n; ++i)
                        {
                            x11_async_t *task = sAsync.at(i);
                            if ((task->type == X11ASYNC_DND_RECV) &&
                                (!task->cb_common.bComplete))
                            {
                                task->result        = handle_drag_leave(&task->dnd_recv, ce);
                                task->cb_common.bComplete = true;
                            }
                        }
                    }
                    else
                    {
                        task->cb_common.bComplete = true;
                        task->result        = proxy_drag_leave(&task->dnd_proxy, ce);
                    }

                    return true;
                }
                else if (type == sAtoms.X11_XdndPosition)
                {
                    lsp_trace("Received XdndPosition");

                    x11_async_t *task = find_dnd_proxy_task(ce->window);
                    if (task == NULL)
                    {
                        for (size_t i=0, n=sAsync.size(); i<n; ++i)
                        {
                            x11_async_t *task = sAsync.at(i);
                            if ((task->type == X11ASYNC_DND_RECV) && (!task->cb_common.bComplete))
                            {
                                task->result        = handle_drag_position(&task->dnd_recv, ce);
                                if (task->result != STATUS_OK)
                                    task->cb_common.bComplete   = true;
                            }
                        }
                    }
                    else
                    {
                        task->result        = proxy_drag_position(&task->dnd_proxy, ce);
                        if (task->result != STATUS_OK)
                            task->cb_common.bComplete   = true;
                    }

                    return true;
                }
                else if (type == sAtoms.X11_XdndDrop)
                {
                    lsp_trace("Received XdndDrop");
                    x11_async_t *task = find_dnd_proxy_task(ce->window);
                    if (task == NULL)
                    {
                        for (size_t i=0, n=sAsync.size(); i<n; ++i)
                        {
                            x11_async_t *task = sAsync.at(i);
                            if ((task->type == X11ASYNC_DND_RECV) && (!task->cb_common.bComplete))
                            {
                                task->result        = handle_drag_drop(&task->dnd_recv, ce);
                                if (task->result != STATUS_OK)
                                    task->cb_common.bComplete   = true;
                            }
                        }
                    }
                    else
                    {
                        task->cb_common.bComplete   = true;
                        task->result        = proxy_drag_drop(&task->dnd_proxy, ce);
                    }
                    return true;
                }

                return false;
            }

            X11Display::x11_async_t *X11Display::lookup_dnd_proxy_task()
            {
                // Lookup for DnD task
                for (size_t i=0, n=sAsync.size(); i<n; ++i)
                {
                    x11_async_t *task = sAsync.at(i);
                    if ((task->type == X11ASYNC_DND_PROXY) && (!task->cb_common.bComplete))
                        return task;
                }

                return NULL;
            }

            void X11Display::send_immediate(Window wnd, Bool propagate, long event_mask, XEvent *event)
            {
                X11Window *pwnd = find_window(wnd);
                if (pwnd == NULL)
                {
                    lsp_trace("Sending xevent to %lx", long(wnd));
                    ::XSendEvent(pDisplay, wnd, propagate, event_mask, event);
                    ::XFlush(pDisplay);
                }
                else
                {
                    lsp_trace("Handling xevent as for %lx", long(wnd));
                    handleEvent(event);
                }
            }

            status_t X11Display::proxy_drag_position(dnd_proxy_t *task, XClientMessageEvent *ev)
            {
                /**
                    data.l[0] contains the XID of the source window.
                    data.l[1] is reserved for future use (flags).
                    data.l[2] contains the coordinates of the mouse position relative to the root window.
                        data.l[2] = (x << 16) | y
                    data.l[3] contains the time stamp for retrieving the data. (new in version 1)
                    data.l[4] contains the action requested by the user. (new in version 2)
                 */
                int x           = (ev->data.l[2] >> 16) & 0xffff;
                int y           = ev->data.l[2] & 0xffff;
                Atom act        = ev->data.l[4];

                Window root     = None;
                Window parent   = None;
                Window child    = None;
                Window *children = NULL;
                unsigned int nchildren = 0;
                ::XQueryTree(pDisplay, task->hTarget, &root, &parent, &children, &nchildren);

                // Determine the target window to send data
                #ifdef LSP_TRACE
                {
                    lsp_trace("target=%lx, root = %lx, parent = %lx, children = %d",
                            long(task->hTarget), long(root), long(parent), int(nchildren));
                    for (size_t i=0; i<nchildren; ++i)
                    {
                        XWindowAttributes xwa;
                        Window wnd = children[i];
                        ::XGetWindowAttributes(pDisplay, wnd, &xwa);

                        lsp_trace("  child #%d = %lx located at x=%d, y=%d, w=%d, h=%d",
                                int(i), long(wnd),
                                int(xwa.x), int(xwa.y), int(xwa.width), int(xwa.height)
                        );
                    }
                }
                #endif

                // Release children
                if (children != NULL)
                    ::XFree(children);

                // Find child window that supports XDnD protocol
                int cx = -1, cy = -1;
                parent = task->hTarget;
                while (true)
                {
                    // Translate coordinates
                    ::XTranslateCoordinates(pDisplay, root, parent, x, y, &cx, &cy, &child);
                    lsp_trace(" found child %lx pointer location: x=%d, y=%d -> cx=%d, cy=%d",
                            long(child), x, y, cx, cy
                    );
                    if (child == None)
                        break;

                    // Test for XdndAware property
                    uint8_t *data       = NULL;
                    size_t size         = 0;
                    Atom xtype          = None;

                    // Test for support of XDnD protocol
                    status_t res    = read_property(child, sAtoms.X11_XdndAware, XA_ATOM, &data, &size, &xtype);
                    lsp_trace("xDndAware res=%d, xtype=%d, size=%d", int(res), int(xtype), int(size));
                    if ((res != STATUS_OK) || (xtype == None) || (size < 1) || (data[0] < 1))
                    {
                        lsp_trace("Window %lx does not support XDnD Protocol", long(child));
                        parent = child;
                        child = None;
                    }
                    if (data != NULL)
                        ::free(data);

                    // Found window that supports XDnd?
                    if (child != None)
                        break;
                }

                // Replace child with target if not found
                if (child == None)
                {
                    child = task->hTarget;
                    lsp_trace("Empty child window replaced with parent %lx", long(child));
                }

                if (task->hCurrent != child)
                {
                    // Need to send XDndLeave?
                    if (task->hCurrent != None)
                    {
                        XEvent xe;
                        XClientMessageEvent *xev = &xe.xclient;

                        xev->type           = ClientMessage;
                        xev->serial         = ev->serial;
                        xev->send_event     = True;
                        xev->display        = pDisplay;
                        xev->window         = task->hCurrent;
                        xev->message_type   = sAtoms.X11_XdndLeave;
                        xev->format         = 32;
                        xev->data.l[0]      = task->hSource;
                        xev->data.l[1]      = 0;
                        xev->data.l[2]      = 0;
                        xev->data.l[3]      = 0;
                        xev->data.l[4]      = 0;

                        lsp_trace("Sending XdndLeave to %lx", long(task->hCurrent));
                        send_immediate(task->hCurrent, True, NoEventMask, &xe);
                    }

                    // Update current window
                    task->hCurrent = child;

                    // Need to send XDndEnter?
                    if (task->hCurrent != None)
                    {
                        XEvent xe;
                        XClientMessageEvent *xev = &xe.xclient;

                        xev->type           = ClientMessage;
                        xev->serial         = ev->serial;
                        xev->send_event     = True;
                        xev->display        = pDisplay;
                        xev->window         = task->hCurrent;
                        xev->message_type   = sAtoms.X11_XdndEnter;
                        xev->format         = 32;
                        xev->data.l[0]      = task->hSource;
                        xev->data.l[1]      = task->enter[0];
                        xev->data.l[2]      = task->enter[1];
                        xev->data.l[3]      = task->enter[2];
                        xev->data.l[4]      = task->enter[3];

                        lsp_trace("Sending XdndEnter to %lx", long(task->hCurrent));
                        send_immediate(task->hCurrent, True, NoEventMask, &xe);
                    }
                }

                // Need to send XDndPosition now
                if (task->hCurrent != None)
                {
                    // For our windows, perform some additional stuff
                    X11Window *wnd = find_window(task->hCurrent);

                    dnd_recv_t *recv    = current_drag_task();
                    lsp_trace("Current drag task: %p", recv);

                    if ((wnd != NULL) && (recv != NULL))
                    {
                        // Our window - explore the drag
                        recv->enState       = DND_RECV_POSITION; // Allow specific state changes
                        recv->hProxy        = task->hTarget;

                        // Form the notification event
                        ws_event_t ue;
                        ue.nType            = UIE_DRAG_REQUEST;
                        ue.nLeft            = cx;
                        ue.nTop             = cy;
                        ue.nWidth           = 0;
                        ue.nHeight          = 0;
                        ue.nCode            = 0;
                        ue.nState           = DRAG_COPY;

                        // Decode action
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
                            recv->hAction       = None;

                        ue.nTime            = ev->data.l[3];

                        wnd->handle_event(&ue);

                        // Did the handler properly process the event?
                        if ((recv->enState != DND_RECV_ACCEPT) && (recv->enState != DND_RECV_REJECT))
                        {
                            lsp_trace("Forcing reject of proxied DnD transfer (task state=%d)", int(recv->enState));
                            reject_dnd_transfer(recv);
                        }

                        // Return state back
                        recv->enState   = DND_RECV_PENDING;
                        recv->hProxy    = None;
                    }
                    else
                    {
                        // For other windows, just proxy the message
                        XEvent xe;
                        XClientMessageEvent *xev = &xe.xclient;
                        xev->type           = ClientMessage;
                        xev->serial         = ev->serial;
                        xev->send_event     = True;
                        xev->display        = pDisplay;
                        xev->window         = task->hCurrent;
                        xev->message_type   = sAtoms.X11_XdndPosition;
                        xev->format         = 32;
                        xev->data.l[0]      = task->hSource;
                        xev->data.l[1]      = ev->data.l[1];
                        xev->data.l[2]      = ev->data.l[2];
                        xev->data.l[3]      = ev->data.l[3];
                        xev->data.l[4]      = ev->data.l[4];

                        lsp_trace("Sending XdndPosition to %lx", long(task->hCurrent));
                        ::XSendEvent(pDisplay, task->hCurrent, True, NoEventMask, &xe);
                        ::XFlush(pDisplay);
                    }
                }
                else // Need to send XDndStatus message with reject
                {
                    XEvent xe;
                    XClientMessageEvent *xev = &xe.xclient;

                    xev->type           = ClientMessage;
                    xev->serial         = ev->serial;
                    xev->send_event     = True;
                    xev->display        = pDisplay;
                    xev->window         = task->hSource;
                    xev->message_type   = sAtoms.X11_XdndStatus;
                    xev->format         = 32;
                    xev->data.l[0]      = task->hTarget;
                    xev->data.l[1]      = 0;
                    xev->data.l[2]      = 0;
                    xev->data.l[3]      = 0;
                    xev->data.l[4]      = None;

                    lsp_trace("Sending XdndReject to %lx", long(task->hSource));
                    send_immediate(task->hSource, True, NoEventMask, &xe);
                }

                return STATUS_OK;
            }

//            status_t X11Display::proxy_drag_enter(x11_async_t *task, XClientMessageEvent *ev)
//            {
//                dnd_proxy_t *dnd = &task->dnd_proxy;
//                Window root = None, parent = None, *children = NULL;
//                unsigned int nchildren;
//
//                // Determine the target window to send data
//                ::XQueryTree(pDisplay, dnd->hTarget, &root, &parent, &children, &nchildren);
//
//                lsp_trace("root = %lx, parent = %lx", long(root), long(parent));
//                for (size_t i=0; i<nchildren; ++i)
//                {
//                    lsp_trace("  child #%d = %lx", int(i), long(children[i]));
//                }
//
//                // Release children
//                if (children != NULL)
//                    ::XFree(children);
//
//                return STATUS_OK;
//            }

            status_t X11Display::handle_drag_enter(XClientMessageEvent *ev)
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

                lsp_trace("Received XdndEnter: src_wnd=0x%lx, dst_wnd=0x%lx, ext=%s",
                        long(ev->data.l[0]),
                        long(ev->window),
                        ((ev->data.l[1] & 1) ? "true" : "false")
                    );

                Atom type;
                size_t bytes;
                uint8_t *data = NULL;

                drop_mime_types(&vDndMimeTypes);

                // Find target window
                X11Window *tgt  = find_window(ev->window);
                if (tgt == NULL)
                {
                    lsp_trace("Target window 0x%lx not found, acting as a XDnD proxy", long(ev->window));
                    x11_async_t *task   = lookup_dnd_proxy_task();
                    if ((task != NULL) && (task->dnd_proxy.hTarget != Window(ev->window)))
                    {
                        lsp_trace("Target window does not match current proxy task, dropping task");
                        task->cb_common.bComplete   = true;
                        task                        = NULL;
                    }

                    if (task == NULL)
                    {
                        // Create task
                        if ((task = sAsync.add()) == NULL)
                        {
                            if (data != NULL)
                                ::free(data);
                            return STATUS_NO_MEM;
                        }

                        lsp_trace("Created new XDnD proxy task");

                        task->type          = X11ASYNC_DND_PROXY;
                        task->result        = STATUS_OK;
                        dnd_proxy_t *dnd    = &task->dnd_proxy;

                        dnd->bComplete      = false;
                        dnd->hProperty      = None;
                        dnd->hTarget        = ev->window;
                        dnd->hSource        = ev->data.l[0];
                        dnd->hCurrent       = None;
                        dnd->enter[0]       = ev->data.l[1];
                        dnd->enter[1]       = ev->data.l[2];
                        dnd->enter[2]       = ev->data.l[3];
                        dnd->enter[3]       = ev->data.l[4];

                        if (data != NULL)
                            ::free(data);
                    }

                    return STATUS_OK;
                }

                // There are more than 3 mime types?
                if (ev->data.l[1] & 1)
                {
                    // Fetch all MIME types as additional property
                    status_t res = read_property(ev->data.l[0],
                            sAtoms.X11_XdndTypeList, sAtoms.X11_XA_ATOM,
                            &data, &bytes, &type);
                    if (res != STATUS_OK)
                    {
                        lsp_trace("Could not read proprty XdndTypeList");
                        return res;
                    }
                    else if (type != sAtoms.X11_XA_ATOM)
                    {
                        lsp_trace("Could proprty XdndTypeList is not of XA_ATOM type");
                        return STATUS_BAD_TYPE;
                    }

                    // Decode MIME types
                    uint32_t *atoms = reinterpret_cast<uint32_t *>(data);
                    for (size_t i=0; i<bytes; i += sizeof(uint32_t), ++atoms)
                    {
                        char *a_name = ::XGetAtomName(pDisplay, *atoms);
                        if (a_name == NULL)
                            continue;

                        // Add atom name to list
                        char *xctype = ::strdup(a_name);
                        ::XFree(a_name);
                        if (xctype == NULL)
                        {
                            drop_mime_types(&vDndMimeTypes);
                            return STATUS_NO_MEM;
                        }
                        if (!vDndMimeTypes.add(xctype))
                        {
                            drop_mime_types(&vDndMimeTypes);
                            ::free(xctype);
                            return STATUS_NO_MEM;
                        }
                    }
                }
                else
                {
                    // Read MIME types from client message
                    for (size_t i=2; i<5; ++i)
                    {
                        if (ev->data.l[i] == None)
                            continue;
                        char *a_name = ::XGetAtomName(pDisplay, ev->data.l[i]);
                        if (a_name == NULL)
                            continue;

                        // Add atom name to list
                        char *xctype = ::strdup(a_name);
                        ::XFree(a_name);
                        if (xctype == NULL)
                        {
                            drop_mime_types(&vDndMimeTypes);
                            return STATUS_NO_MEM;
                        }
                        if (!vDndMimeTypes.add(xctype))
                        {
                            drop_mime_types(&vDndMimeTypes);
                            ::free(xctype);
                            return STATUS_NO_MEM;
                        }
                    }
                }

                // Add NULL-terminator
                if (!vDndMimeTypes.add(NULL))
                {
                    drop_mime_types(&vDndMimeTypes);
                    return STATUS_NO_MEM;
                }

                // Create async task
                x11_async_t *task   = sAsync.add();
                if (task == NULL)
                {
                    drop_mime_types(&vDndMimeTypes);
                    return STATUS_NO_MEM;
                }

                task->type          = X11ASYNC_DND_RECV;
                task->result        = STATUS_OK;
                dnd_recv_t *dnd     = &task->dnd_recv;

                dnd->bComplete      = false;
                dnd->hProperty      = None;
                dnd->hTarget        = ev->window;
                dnd->hSource        = ev->data.l[0];
                dnd->hSelection     = sAtoms.X11_XdndSelection;
                dnd->hType          = None;
                dnd->enState        = DND_RECV_PENDING;
                dnd->pSink          = NULL;
                dnd->hAction        = None;
                dnd->hProxy         = None;

                // Log all supported MIME types
                #ifdef LSP_TRACE
                for (size_t i=0, n=vDndMimeTypes.size()-1; i<n; ++i)
                {
                    char *mime = vDndMimeTypes.at(i);
                    lsp_trace("Supported MIME type: %s", mime);
                }
                #endif

                // Create DRAG_ENTER event
                ws_event_t ue;
                ue.nType        = UIE_DRAG_ENTER;
                ue.nLeft        = 0;
                ue.nTop         = 0;
                ue.nWidth       = 0;
                ue.nHeight      = 0;
                ue.nCode        = 0;
                ue.nState       = 0;
                ue.nTime        = 0;

                // Pass event to the target window
                return tgt->handle_event(&ue);
            }

            status_t X11Display::proxy_drag_leave(dnd_proxy_t *task, XClientMessageEvent *ev)
            {
                // Need to send XDndLeave?
                if (task->hCurrent == None)
                    return STATUS_OK;

                XEvent xe;
                XClientMessageEvent *xev = &xe.xclient;

                xev->type           = ClientMessage;
                xev->serial         = ev->serial;
                xev->send_event     = True;
                xev->display        = pDisplay;
                xev->window         = task->hCurrent;
                xev->message_type   = sAtoms.X11_XdndLeave;
                xev->format         = 32;
                xev->data.l[0]      = task->hSource;
                xev->data.l[1]      = 0;
                xev->data.l[2]      = 0;
                xev->data.l[3]      = 0;
                xev->data.l[4]      = 0;

                lsp_trace("Sending XdndLeave to %lx", long(task->hCurrent));
                send_immediate(task->hCurrent, True, NoEventMask, &xe);

                task->hCurrent      = None;

                return STATUS_OK;
            }

            status_t X11Display::handle_drag_leave(dnd_recv_t *task, XClientMessageEvent *ev)
            {
                /**
                Sent from source to target to cancel the drop.

                   data.l[0] contains the XID of the source window.
                   data.l[1] is reserved for future use (flags).
                */
                if ((task->hTarget != ev->window) && (long(task->hSource) != ev->data.l[0]))
                    return STATUS_PROTOCOL_ERROR;

                if (task->pSink != NULL)
                {
                    task->pSink->release();
                    task->pSink     = NULL;
                }

                // Find target window
                X11Window *tgt  = find_window(ev->window);
                if (tgt == NULL)
                    return STATUS_NOT_FOUND;

                ws_event_t ue;
                ue.nType        = UIE_DRAG_LEAVE;
                ue.nLeft        = 0;
                ue.nTop         = 0;
                ue.nWidth       = 0;
                ue.nHeight      = 0;
                ue.nCode        = 0;
                ue.nState       = 0;
                ue.nTime        = 0;

                return tgt->handle_event(&ue);
            }

            status_t X11Display::handle_drag_position(dnd_recv_t *task, XClientMessageEvent *ev)
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

                // Validate current state
                if ((task->hTarget != ev->window) || (long(task->hSource) != ev->data.l[0]))
                {
                    lsp_trace("Window IDs do not match");
                    return STATUS_PROTOCOL_ERROR;
                }
                if (task->enState != DND_RECV_PENDING)
                {
                    lsp_trace("Invalid recv task state, should be DND_RECV_PENDING");
                    return STATUS_PROTOCOL_ERROR;
                }

                // Decode the event
                int x = (ev->data.l[2] >> 16) & 0xffff, y = (ev->data.l[2] & 0xffff);
                Atom act            = ev->data.l[4];

                #ifdef LSP_TRACE
                char *a_name = ::XGetAtomName(pDisplay, ev->data.l[4]);
                lsp_trace("Received XdndPosition: wnd=0x%lx, flags=0x%lx, x=%d, y=%d, timestamp=%ld action=%ld (%s)",
                        ev->data.l[0], ev->data.l[1], x, y, ev->data.l[3], ev->data.l[4], a_name
                        );
                ::XFree(a_name);
                #endif

                // Find target window
                X11Window *tgt  = find_window(ev->window);
                if (tgt == NULL)
                {
                    lsp_trace("Could not find target window 0x%lx", long(ev->window));
                    return STATUS_NOT_FOUND;
                }

                Window child        = None;
                ::XSync(pDisplay, False);
                ::XTranslateCoordinates(pDisplay, hRootWnd, task->hTarget, x, y, &x, &y, &child);
                ::XSync(pDisplay, False);
                task->enState       = DND_RECV_POSITION; // Allow specific state changes

                // Form the notification event
                ws_event_t ue;
                ue.nType            = UIE_DRAG_REQUEST;
                ue.nLeft            = x;
                ue.nTop             = y;
                ue.nWidth           = 0;
                ue.nHeight          = 0;
                ue.nCode            = 0;
                ue.nState           = DRAG_COPY;

                // Decode action
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
                    task->hAction       = None;

                ue.nTime            = ev->data.l[3];

                status_t res        = tgt->handle_event(&ue);

                // Did the handler properly process the event?
                if ((task->enState != DND_RECV_ACCEPT) && (task->enState != DND_RECV_REJECT))
                {
                    lsp_trace("Forcing reject of DnD transfer (task state=%d)", int(task->enState));
                    reject_dnd_transfer(task);
                }

                // Return state back
                task->enState   = DND_RECV_PENDING;

                return res;
            }

            status_t X11Display::proxy_drag_drop(dnd_proxy_t *task, XClientMessageEvent *ev)
            {
                lsp_trace("Received proxied XdndDrop from %lx to wnd=0x%lx, ts=%ld", ev->data.l[0], ev->window, ev->data.l[2]);
                bool fail           = false;
                X11Window *tgt      = (task->hCurrent != None) ? find_window(task->hCurrent) : NULL;

                if (tgt != NULL)
                {
                    lsp_trace("Current window is %p, id=%lx", tgt, long(task->hCurrent));
                    dnd_recv_t *recv    = current_drag_task();

                    if (recv != NULL)
                    {
                        recv->hProxy        = task->hTarget;

                        XEvent xev;
                        XClientMessageEvent *xe = &xev.xclient;
                        xe->type            = ClientMessage;
                        xe->serial          = ev->serial;
                        xe->send_event      = True;
                        xe->display         = pDisplay;
                        xe->window          = task->hCurrent;
                        xe->message_type    = sAtoms.X11_XdndDrop;
                        xe->format          = 32;
                        xe->data.l[0]       = ev->data.l[0];
                        xe->data.l[1]       = ev->data.l[1];
                        xe->data.l[2]       = ev->data.l[2];
                        xe->data.l[3]       = ev->data.l[3];
                        xe->data.l[4]       = ev->data.l[4];

                        send_immediate(task->hCurrent, True, NoEventMask, &xev);
                        recv->hProxy        = None;
                    }
                    else
                        fail    = true;
                }
                else if (task->hCurrent != None)
                {
                    // Proxy DND Drop
                    lsp_trace("No internal window found, proxying event");

                    XEvent xev;
                    XClientMessageEvent *xe = &xev.xclient;
                    xe->type            = ClientMessage;
                    xe->serial          = 0;
                    xe->send_event      = True;
                    xe->display         = pDisplay;
                    xe->window          = task->hCurrent;
                    xe->message_type    = sAtoms.X11_XdndFinished;
                    xe->format          = 32;
                    xe->data.l[0]       = ev->data.l[0];
                    xe->data.l[1]       = ev->data.l[1];
                    xe->data.l[2]       = ev->data.l[2];
                    xe->data.l[3]       = ev->data.l[3];
                    xe->data.l[4]       = ev->data.l[4];

                    // Send the notification event
                    ::XSendEvent(pDisplay, task->hCurrent, True, NoEventMask, &xev);
                    ::XFlush(pDisplay);
                }
                else
                    fail    = true;

                // Need to fail DnD transfer?
                if (fail)
                {
                    lsp_trace("No proper internal window found, failing DnD event");

                    XEvent xev;
                    XClientMessageEvent *xe = &xev.xclient;
                    xe->type            = ClientMessage;
                    xe->serial          = 0;
                    xe->send_event      = True;
                    xe->display         = pDisplay;
                    xe->window          = task->hSource;
                    xe->message_type    = sAtoms.X11_XdndFinished;
                    xe->format          = 32;
                    xe->data.l[0]       = task->hTarget;
                    xe->data.l[1]       = 0;
                    xe->data.l[2]       = None;
                    xe->data.l[3]       = 0;
                    xe->data.l[4]       = 0;

                    // Send the notification event
                    ::XSendEvent(pDisplay, task->hSource, True, NoEventMask, &xev);
                    ::XFlush(pDisplay);
                }

                return STATUS_OK;
            }

            status_t X11Display::handle_drag_drop(dnd_recv_t *task, XClientMessageEvent *ev)
            {
                /**
                Sent from source to target to complete the drop.
                data.l[0] contains the XID of the source window.
                data.l[1] is reserved for future use (flags).
                data.l[2] contains the time stamp for retrieving the data. (new in version 1)
                */
                lsp_trace("Received XdndDrop wnd=0x%lx, ts=%ld", ev->data.l[0], ev->data.l[2]);

                // Validate state
                if ((task->hTarget != ev->window) || (long(task->hSource) != ev->data.l[0]))
                    return STATUS_PROTOCOL_ERROR;
                if (task->enState != DND_RECV_PENDING)
                    return STATUS_PROTOCOL_ERROR;
                if (task->pSink == NULL)
                {
                    complete_dnd_transfer(task, false);
                    return STATUS_UNSUPPORTED_FORMAT;
                }

                // Find target window
                X11Window *tgt  = find_window(task->hTarget);
                if (tgt == NULL)
                {
                    complete_dnd_transfer(task, false);
                    return STATUS_NOT_FOUND;
                }

                status_t res        = STATUS_OK;
                ssize_t index       = task->pSink->open(vDndMimeTypes.get_array());
                if (index >= 0)
                {
                    const char *mime = vDndMimeTypes.get(index);
                    if (mime != NULL)
                    {
                        // Update type of MIME
                        task->hType     = ::XInternAtom(pDisplay, mime, False);
                        lsp_trace("Selected MIME type: %s (%ld)", mime, task->hType);

                        // Generate property identifier
                        Atom prop_id = gen_selection_id();
                        if (prop_id != None)
                        {
                            // Request selection conversion and return
                            task->hProperty     = prop_id;
                            task->enState       = DND_RECV_SIMPLE;

                            ::XConvertSelection(pDisplay, task->hSelection,
                                    task->hType, prop_id, task->hTarget, CurrentTime);
                            ::XFlush(pDisplay);

                            return STATUS_OK;
                        }
                        else
                            res = STATUS_UNKNOWN_ERR;
                    }
                    else
                        res = STATUS_BAD_TYPE;

                    task->pSink->close(res);
                }
                else
                    res = -index;

                // Release sink and complete transfer
                task->pSink->release();
                task->pSink = NULL;
                complete_dnd_transfer(task, res == STATUS_OK);

                return res;
            }

            void X11Display::complete_dnd_transfer(dnd_recv_t *task, bool success)
            {
                /**
                XdndFinished (new in version 2)

                Sent from target to source to indicate that the source can toss the data
                because the target no longer needs access to it.

                    data.l[0] contains the XID of the target window.
                    data.l[1]:
                        Bit 0 is set if the current target accepted the drop and successfully
                        performed the accepted drop action. (new in version 5)
                        (If the version being used by the source is less than 5, then the program
                        should proceed as if the bit were set, regardless of its actual value.)
                        The rest of the bits are reserved for future use.
                    data.l[2] contains the action performed by the target. None should be sent if the
                        current target rejected the drop, i.e., when bit 0 of data.l[1]
                        is zero. (new in version 5) (Note: Performing an action other than the
                        one that was accepted with the last XdndStatus message is strongly discouraged
                        because the user expects the action to match the visual feedback that
                        was given based on the XdndStatus messages!)
                */

                // Send end of transfer and set status flag
                Window target  = (task->hProxy != None) ? task->hProxy : task->hTarget;
                lsp_trace("Sending XdndFinished from %lx to %lx", long(target), long(task->hSource));

                XEvent xev;
                XClientMessageEvent *xe = &xev.xclient;
                xe->type            = ClientMessage;
                xe->serial          = 0;
                xe->send_event      = True;
                xe->display         = pDisplay;
                xe->window          = task->hSource;
                xe->message_type    = sAtoms.X11_XdndFinished;
                xe->format          = 32;
                xe->data.l[0]       = target;
                xe->data.l[1]       = (success) ? 1 : 0;
                xe->data.l[2]       = (success) ? task->hAction : None;
                xe->data.l[3]       = 0;
                xe->data.l[4]       = 0;

                // Send the notification event
                ::XSendEvent(pDisplay, task->hSource, True, NoEventMask, &xev);
                ::XFlush(pDisplay);
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

            status_t X11Display::grab_events(X11Window *wnd, grab_t group)
            {
                // Check validity of argument
                if (group >= __GRAB_TOTAL)
                    return STATUS_BAD_ARGUMENTS;

                // Check that window does not belong to any active grab group
                for (size_t i=0; i<__GRAB_TOTAL; ++i)
                {
                    cvector<X11Window> &g = vGrab[i];
                    if (g.index_of(wnd) >= 0)
                    {
                        lsp_warn("Grab duplicated for window %p (id=%lx)", wnd, (long)wnd->hWindow);
                        return STATUS_DUPLICATED;
                    }
                }

                // Get the screen to obtain a grap
                x11_screen_t *s = vScreens.get(wnd->screen());
                if (s == NULL)
                {
                    lsp_warn("Invalid screen index");
                    return STATUS_BAD_STATE;
                }

                // Add a grab
                if (!vGrab[group].add(wnd))
                    return STATUS_NO_MEM;

                // Obtain a grab if necessary
                if (!(s->grabs++))
                {
                    lsp_trace("Setting grab for screen=%d", int(s->id));
                    Window root     = RootWindow(pDisplay, s->id);
                    ::XGrabPointer(pDisplay, root, True, PointerMotionMask | ButtonPressMask | ButtonReleaseMask, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
                    ::XGrabKeyboard(pDisplay, root, True, GrabModeAsync, GrabModeAsync, CurrentTime);

                    ::XFlush(pDisplay);
//                    ::XSync(pDisplay, False);
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
                bool found = false;

                // Obtain a screen object
                x11_screen_t *s = vScreens.get(wnd->screen());
                if (s == NULL)
                {
                    lsp_warn("No screen object found for window %p (%lx)", wnd, long(wnd->hWindow));
                    return STATUS_BAD_STATE;
                }

                // Check that window does belong to any active grab group
                for (size_t i=0; i<__GRAB_TOTAL; ++i)
                {
                    cvector<X11Window> &g = vGrab[i];
                    if (g.remove(wnd, false))
                    {
                        found = true;
                        break;
                    }
                }

                // Return error if not found
                if (!found)
                {
                    lsp_trace("No grab found for window %p (%lx)", wnd, long(wnd->hWindow));
                    return STATUS_NO_GRAB;
                }
                else if (s->grabs <= 0)
                {
                    lsp_trace("Grab for screen #%d has already been released", int(s->id));
                    return STATUS_BAD_STATE;
                }

                // Need to release X11 grab?
                if (!(--s->grabs))
                {
                    lsp_trace("Releasing grab for screen=%d", int(s->id));
                    ::XUngrabPointer(pDisplay, CurrentTime);
                    ::XUngrabKeyboard(pDisplay, CurrentTime);

                    ::XFlush(pDisplay);
//                    ::XSync(pDisplay, False);
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
                {
                    dst->release();
                    return STATUS_UNKNOWN_ERR;
                }

                // Create async task
                x11_async_t *task   = sAsync.add();
                if (task == NULL)
                {
                    dst->release();
                    return STATUS_NO_MEM;
                }

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

            void X11Display::handle_error(XErrorEvent *ev)
            {
#ifdef LSP_TRACE
                const char *error = "Unknown";

                switch (ev->error_code)
                {
                #define DE(name) case name: error = #name; break;
                    DE(BadRequest)
                    DE(BadValue)
                    DE(BadWindow)
                    DE(BadPixmap)
                    DE(BadAtom)
                    DE(BadCursor)
                    DE(BadFont)
                    DE(BadDrawable)
                    DE(BadAccess)
                    DE(BadAlloc)
                    DE(BadColor)
                    DE(BadGC)
                    DE(BadIDChoice)
                    DE(BadName)
                    DE(BadLength)
                    DE(BadImplementation)
                    default: break;
                #undef DE
                }
                lsp_trace("error: code=%d (%s), serial=%ld, request=%d, minor=%d",
                        int(ev->error_code), error, ev->serial, int(ev->request_code), int(ev->minor_code));
#endif
                if (ev->error_code == BadWindow)
                {
                    for (size_t i=0, n=sAsync.size(); i<n; ++i)
                    {
                        // Skip completed tasks
                        x11_async_t *task   = sAsync.at(i);
                        if (task->cb_common.bComplete)
                            continue;

                        switch (task->type)
                        {
                            case X11ASYNC_CB_SEND:
                                if (task->cb_send.hRequestor == ev->resourceid)
                                {
                                    task->cb_send.bComplete = true;
                                    task->result            = STATUS_PROTOCOL_ERROR;
                                }
                                break;
                            default:
                                break;
                        }
                    }
                }

            }

            X11Display::dnd_recv_t *X11Display::current_drag_task()
            {
                for (size_t i=0, n=sAsync.size(); i<n; ++i)
                {
                    x11_async_t *task = sAsync.at(i);
                    if ((task->type != X11ASYNC_DND_RECV) || (task->cb_common.bComplete))
                        continue;
                    return &task->dnd_recv;
                }
                return NULL;
            }

            const char * const *X11Display::getDragContentTypes()
            {
                dnd_recv_t *task = current_drag_task();
                return (task != NULL) ? vDndMimeTypes.get_array() : NULL;
            }

            void X11Display::reject_dnd_transfer(dnd_recv_t *task)
            {
                /**
                XdndStatus

                Sent from target to source to provide feedback on whether or not the drop will be accepted, and, if so, what action will be taken.

                    data.l[0] contains the XID of the target window. (This is required so XdndStatus
                        messages that arrive after XdndLeave is sent will be ignored.)
                    data.l[1]:
                        Bit 0 is set if the current target will accept the drop.
                        Bit 1 is set if the target wants XdndPosition messages while the mouse moves inside the
                            rectangle in data.l[2,3].
                        The rest of the bits are reserved for future use.
                    data.l[2,3] contains a rectangle in root coordinates that means "don't send another XdndPosition
                        message until the mouse moves out of here". It is legal to specify an empty rectangle.
                        This means "send another message when the mouse moves". Even if the rectangle is not empty,
                        it is legal for the source to send XdndPosition messages while in the rectangle. The rectangle
                        is stored in the standard Xlib format of (x,y,w,h):
                            data.l[2] = (x << 16) | y
                            data.l[3] = (w << 16) | h
                    data.l[4] contains the action accepted by the target. This is normally only allowed to be either
                        the action specified in the XdndPosition message, XdndActionCopy, or XdndActionPrivate. None
                        should be sent if the drop will not be accepted. (new in version 2)
                 */
                // Send end of transfer if status is bad
                Window target = (task->hProxy != None) ? task->hProxy : task->hTarget;
                lsp_trace("Sending XdndStatus (reject) from %lx to %lx", long(target), long(task->hSource));
                
//                XWindowAttributes xwa;
//                Window child = None;
//                int x = 0, y = 0;
//                ::XGetWindowAttributes(pDisplay, task->hTarget, &xwa);
//                ::XTranslateCoordinates(pDisplay, task->hTarget, hRootWnd, 0, 0, &x, &y, &child);

                XEvent xev;
                XClientMessageEvent *ev = &xev.xclient;
                ev->type            = ClientMessage;
                ev->serial          = 0;
                ev->send_event      = True;
                ev->display         = pDisplay;
                ev->window          = task->hSource;
                ev->message_type    = sAtoms.X11_XdndStatus;
                ev->format          = 32;
                ev->data.l[0]       = target;
                ev->data.l[1]       = 0;
                ev->data.l[2]       = 0;
                ev->data.l[3]       = 0;
//                ev->data.l[1]       = (1 << 1);
//                ev->data.l[2]       = ((x & 0xffff) << 16) | (y & 0xffff);
//                ev->data.l[3]       = ((xwa.width & 0xffff) << 16) | (xwa.height & 0xffff);
                ev->data.l[4]       = None;

                // Send the notification event
                ::XSendEvent(pDisplay, task->hSource, True, NoEventMask, &xev);
                ::XFlush(pDisplay);
            }

            status_t X11Display::rejectDrag()
            {
                // Check task state
                dnd_recv_t *task = current_drag_task();
                if ((task == NULL) || (task->enState != DND_RECV_POSITION))
                    return STATUS_BAD_STATE;

                // Release sink if present
                if (task->pSink != NULL)
                {
                    task->pSink->release();
                    task->pSink     = NULL;
                }
                task->enState       = DND_RECV_REJECT;

                // Send reject status to requestor
                lsp_trace("Handler rejected DnD transfer");
                reject_dnd_transfer(task);

                return STATUS_OK;
            }

            status_t X11Display::acceptDrag(IDataSink *sink, drag_t action, bool internal, const realize_t *r)
            {
                /**
                XdndStatus

                Sent from target to source to provide feedback on whether or not the drop will be accepted, and, if so, what action will be taken.

                    data.l[0] contains the XID of the target window. (This is required so XdndStatus
                        messages that arrive after XdndLeave is sent will be ignored.)
                    data.l[1]:
                        Bit 0 is set if the current target will accept the drop.
                        Bit 1 is set if the target wants XdndPosition messages while the mouse moves inside the
                            rectangle in data.l[2,3].
                        The rest of the bits are reserved for future use.
                    data.l[2,3] contains a rectangle in root coordinates that means "don't send another XdndPosition
                        message until the mouse moves out of here". It is legal to specify an empty rectangle.
                        This means "send another message when the mouse moves". Even if the rectangle is not empty,
                        it is legal for the source to send XdndPosition messages while in the rectangle. The rectangle
                        is stored in the standard Xlib format of (x,y,w,h):
                            data.l[2] = (x << 16) | y
                            data.l[3] = (w << 16) | h
                    data.l[4] contains the action accepted by the target. This is normally only allowed to be either
                        the action specified in the XdndPosition message, XdndActionCopy, or XdndActionPrivate. None
                        should be sent if the drop will not be accepted. (new in version 2)
                 */

                dnd_recv_t *task = current_drag_task();
                if ((task == NULL) || (task->enState != DND_RECV_POSITION))
                    return STATUS_BAD_STATE;

                Atom act            = None;
                switch (action)
                {
                    case DRAG_COPY: act = sAtoms.X11_XdndActionCopy; break;
                    case DRAG_PRIVATE: act = sAtoms.X11_XdndActionPrivate; break;

                    case DRAG_MOVE:
                        if ((act = sAtoms.X11_XdndActionMove) != task->hAction)
                            return STATUS_INVALID_VALUE;
                        break;
                    case DRAG_LINK:
                        if ((act = sAtoms.X11_XdndActionLink) != task->hAction)
                            return STATUS_INVALID_VALUE;
                        break;
                    case DRAG_ASK:
                        if ((act = sAtoms.X11_XdndActionLink) != task->hAction)
                            return STATUS_INVALID_VALUE;
                        break;
                    case DRAG_DIRECT_SAVE:
                        if ((act = sAtoms.X11_XdndActionDirectSave) != task->hAction)
                            return STATUS_INVALID_VALUE;
                        break;
                    default:
                        return STATUS_INVALID_VALUE;
                }

                if (r == NULL)
                    internal            = false;

                // Translate window coordinates
                int x, y;
                if (r != NULL)
                {
                    Window child = None;
                    if ((r->nWidth < 0) || (r->nWidth >= 0x10000) || (r->nHeight < 0) || (r->nHeight > 0x10000))
                        return STATUS_INVALID_VALUE;
                    ::XTranslateCoordinates(pDisplay, task->hTarget, hRootWnd, r->nLeft, r->nTop, &x, &y, &child);
                    ::XSync(pDisplay, False);
                    if ((x < 0) || (x >= 0x10000) || (y < 0) || (y >= 0x10000))
                        return STATUS_INVALID_VALUE;
                }

                // Form the message
                Window target       = (task->hProxy != None) ? task->hProxy : task->hTarget;
                lsp_trace("Sending XdndStatus (accept) from %lx to %lx", long(target), long(task->hSource));
                XEvent xev;
                XClientMessageEvent *ev = &xev.xclient;
                ev->type            = ClientMessage;
                ev->serial          = 0;
                ev->send_event      = True;
                ev->display         = pDisplay;
                ev->window          = task->hSource;
                ev->message_type    = sAtoms.X11_XdndStatus;
                ev->format          = 32;
                ev->data.l[0]       = target;
                ev->data.l[1]       = 1 | ((internal) ? (1 << 1) : 0);
                if (r != NULL)
                {
                    ev->data.l[2]       = (x << 16) | y;
                    ev->data.l[3]       = (r->nWidth << 16) | r->nHeight;
                }
                else
                {
                    ev->data.l[2]       = 0;
                    ev->data.l[3]       = 0;
                }
                ev->data.l[4]       = act;

                // Rewrite sink
                if (sink != NULL)
                    sink->acquire();
                if (task->pSink != NULL)
                    task->pSink->release();
                task->pSink     = sink;
                task->enState   = DND_RECV_ACCEPT;
                task->hAction   = act;

                // Send the response
                ::XSendEvent(pDisplay, task->hSource, True, NoEventMask, &xev);
                ::XFlush(pDisplay);

                return STATUS_OK;
            }

        } /* namespace x11 */
    } /* namespace ws */
} /* namespace lsp */

#endif /* USE_X11_DISPLAY */

