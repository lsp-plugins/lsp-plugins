/*
 * Renderer.cpp
 *
 *  Created on: 24 дек. 2018 г.
 *      Author: sadko
 */

#include <core/debug.h>

#include <stdlib.h>
#include <errno.h>
#include <time.h>

#include <test/mtest/3d/common/X11Renderer.h>

namespace mtest
{
    X11Renderer::X11Renderer()
    {
        dpy         = NULL;
        win         = None;
        glc         = NULL;
        stopped     = true;
        nBMask      = 0;
        nMouseX     = 0;
        nMouseY     = 0;
    }
    
    X11Renderer::~X11Renderer()
    {
        destroy();
    }

    status_t X11Renderer::init()
    {
        Window                  root;
        GLint                   att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
        XVisualInfo             *vi;
        Colormap                cmap;
        XSetWindowAttributes    swa;

        dpy = XOpenDisplay(NULL);
        if (dpy == NULL)
        {
            lsp_error("cannot connect to X server");
            return STATUS_NO_DEVICE;
        }

        root = DefaultRootWindow(dpy);
        vi = glXChooseVisual(dpy, 0, att);
        if (vi == NULL)
        {
            lsp_error("no appropriate visual found");
            return STATUS_UNSUPPORTED_FORMAT;
        }
        lsp_info("tvisual %p selected", (void *)vi->visualid); /* %p creates hexadecimal output like in glxinfo */

        cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);

        swa.colormap = cmap;
        swa.event_mask = ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;

        win = XCreateWindow(dpy, root, 0, 0, 800, 600, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
        if (win == None)
        {
            lsp_error("error creating window");
            return STATUS_NO_DEVICE;
        }

        XSelectInput(dpy, win, swa.event_mask);
        XMapWindow(dpy, win);
        XStoreName(dpy, win, "3D Viewer");
        glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
        if (glc == NULL)
        {
            lsp_error("error creating GLX context");
            return STATUS_NO_DEVICE;
        }
        glXMakeCurrent(dpy, win, glc);

        return STATUS_OK;
    }

    status_t X11Renderer::run()
    {
        if (dpy == NULL)
            return STATUS_BAD_STATE;

        XWindowAttributes       gwa;

        stopped                 = false;
        int x11_fd              = ConnectionNumber(dpy);
        struct pollfd x11_poll;
        struct timespec ts;
        struct timespec sLastRender;

        clock_gettime(CLOCK_REALTIME, &sLastRender);

        while (!stopped)
        {
            // Get current time
            clock_gettime(CLOCK_REALTIME, &ts);
            ssize_t dmsec   = (ts.tv_nsec - sLastRender.tv_nsec) / 1000000;
            ssize_t dsec    = (ts.tv_sec - sLastRender.tv_sec);
            dmsec          += dsec * 1000;
            bool force      = dmsec >= 20; // each 20 msec render request

            // Try to poll input data for a 100 msec period
            x11_poll.fd         = x11_fd;
            x11_poll.events     = POLLIN | POLLPRI | POLLHUP;
            x11_poll.revents    = 0;

            errno               = 0;
            int poll_res        = (dmsec < 20) ? poll(&x11_poll, 1, 20 - dmsec) : 0;

            if (poll_res < 0)
            {
                int err_code = errno;
                if (err_code != EINTR)
                {
                    lsp_error("Poll returned error: %d, code=%d", poll_res, err_code);
                    return STATUS_IO_ERROR;
                }
            }
            else if ((force) || ((poll_res > 0) && (x11_poll.revents > 0)))
            {
                if (force)
                {
                    XGetWindowAttributes(dpy, win, &gwa);
                    glViewport(0, 0, gwa.width, gwa.height);
//                    render_view(&view, gwa.width, gwa.height);
                    glXSwapBuffers(dpy, win);
                    sLastRender = ts;
                }

                XEvent xev;
                int pending = XPending(dpy);

                for (int i=0; i<pending; i++)
                {
                    XNextEvent(dpy, &xev);

                    switch (xev.type)
                    {
                        case KeyPress:
                        {
                            int key = XLookupKeysym(&xev.xkey, 0);
                            lsp_trace("KeyPress: x=%d, y=%d, keycode = %x", xev.xkey.x, xev.xkey.y, int(key));
                            on_key_press(xev.xkey, key);
                            break;
                        }
                        case ButtonPress:
                            lsp_trace("ButtonPress: x=%d, y=%d, button = %x", xev.xbutton.x, xev.xbutton.y, int(xev.xbutton.button));
                            on_mouse_down(xev.xbutton);
                            break;
                        case ButtonRelease:
                            lsp_trace("ButtonRelease: x=%d, y=%d, button = %x", xev.xbutton.x, xev.xbutton.y, int(xev.xbutton.button));
                            on_mouse_up(xev.xbutton);
                            break;
                        case MotionNotify:
                            on_mouse_move(xev.xmotion);
                            break;
                    }
                }
            }
        } // while

//        destroy_view(&view);

        return STATUS_OK;
    }

    void X11Renderer::destroy()
    {
        if (glc != NULL)
        {
            if (dpy != NULL)
            {
                glXMakeCurrent(dpy, None, NULL);
                glXDestroyContext(dpy, glc);
            }
            glc = NULL;
        }

        if (win != None)
        {
            XDestroyWindow(dpy, win);
            win = None;
        }

        if (dpy != NULL)
        {
            XCloseDisplay(dpy);
            dpy = NULL;
        }
    }

    void X11Renderer::stop()
    {
        stopped = true;
    }

    void X11Renderer::on_key_press(const XKeyEvent &ev, KeySym key)
    {
    }

    void X11Renderer::on_mouse_down(const XButtonEvent &ev)
    {
        if (nBMask == 0)
        {
            nMouseX     = ev.x;
            nMouseY     = ev.y;
        }
        nBMask |= (1 << ev.button);
    }

    void X11Renderer::on_mouse_up(const XButtonEvent &ev)
    {
        nBMask &= ~(1 << ev.button);
        if (nBMask == 0)
        {
//            view.fAngleX   += view.fAngleDX;
//            view.fAngleY   += view.fAngleDY;
//            view.fAngleZ   += view.fAngleDZ;
//            view.fScale    += view.fDeltaScale;
//
//            view.fAngleDX   = 0.0f;
//            view.fAngleDY   = 0.0f;
//            view.fAngleDZ   = 0.0f;
//            view.fDeltaScale= 0.0f;
        }
    }

    void X11Renderer::on_mouse_move(const XMotionEvent &ev)
    {
        if (nBMask & 2)
        {
//            view.fAngleDX  = ((xev.xbutton.y - mouse_y) * M_PI / 20.0f);
//            view.fAngleDZ  = ((xev.xbutton.x - mouse_x) * M_PI / 20.0f);
        }
        else if (nBMask & 8)
        {
//            view.fDeltaScale = (mouse_y - xev.xbutton.y) / 200.0f;
        }
    }

} /* namespace mtest */
