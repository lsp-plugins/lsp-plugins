/*
 * geometry3d.cpp
 *
 *  Created on: 29 авг. 2018 г.
 *      Author: sadko
 */

#include <test/mtest.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <math.h>
#include <errno.h>

#include <core/types.h>
#include <dsp/dsp.h>
#include <core/3d/RayTrace3D.h>
#include <core/3d/Object3D.h>
#include <core/3d/Scene3D.h>
#include <core/3d/RaySource3D.h>
#include <core/3d/TraceCapture3D.h>
#include <core/files/Model3DFile.h>
#include <data/cvector.h>

#include <stdlib.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysymdef.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include <sys/poll.h>

#define ADDM_BUF_SIZE  0x100000

namespace geometry3d_test
{
    using namespace lsp;
}

#include <test/mtest/3d/debug.h>
#include <test/mtest/3d/view.h>
#include <test/mtest/3d/task.h>

namespace geometry3d_test
{
    view_t                 view;

    bool on_key_press(view_t *s, KeySym key)
    {
        lsp_trace("Keycode = %x", int(key));

        switch (key)
        {
            case XK_Escape:
                return true;
            case ' ':
                s->bRotate = !s->bRotate;
                break;
            case 'w':
                s->bWireframe = !s->bWireframe;
                break;
            case 'l':
                s->bLight = !s->bLight;
                break;
            case 'c':
                s->bCullFace = !s->bCullFace;
                break;
            case 'i':
                s->bInvert = !s->bInvert;
                break;
            case 'r':
                s->bDrawRays    = ! s->bDrawRays;
                break;
            case 't':
                s->bDrawTriangles   = ! s->bDrawTriangles;
                break;
            case 'p':
                s->bDrawPoints  = !s->bDrawPoints;
                break;
            case 's':
                s->bDrawSegments  = !s->bDrawSegments;
                break;
            case 'g':
                s->bDrawSource  = !s->bDrawSource;
                break;
            case 'n':
                s->bDrawNormals = !s->bDrawNormals;
                break;
            case 'd':
                s->bDrawCapture = !s->bDrawCapture;
                break;
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
            {
                Object3D *obj = s->pScene->get_object(key - '0');
                if (obj != NULL)
                    obj->set_visible(!obj->is_visible());
                break;
            }
        }

        return false;
    }

    int test()
    {
        Display                 *dpy;
        Window                  root;
        GLint                   att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
        XVisualInfo             *vi;
        Colormap                cmap;
        XSetWindowAttributes    swa;
        Window                  win;
        GLXContext              glc;
        XWindowAttributes       gwa;

        Scene3D                 scene;
        build_scene(&scene);

        dpy = XOpenDisplay(NULL);
        if (dpy == NULL)
        {
            lsp_error("cannot connect to X server");
            return 0;
        }

        root = DefaultRootWindow(dpy);
        vi = glXChooseVisual(dpy, 0, att);
        if(vi == NULL)
        {
            lsp_error("no appropriate visual found");
            return 0;
        }
        else
            lsp_info("tvisual %p selected", (void *)vi->visualid); /* %p creates hexadecimal output like in glxinfo */

        cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);

        swa.colormap = cmap;
        swa.event_mask = ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;

        win = XCreateWindow(dpy, root, 0, 0, 800, 600, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
        XSelectInput(dpy, win, swa.event_mask);
        XMapWindow(dpy, win);
        XStoreName(dpy, win, "3D Viewer");
        glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
        glXMakeCurrent(dpy, win, glc);

        // Initialize view and build scene
        init_view(&view);
        view.pScene         = &scene;

        int x11_fd          = ConnectionNumber(dpy);
        struct pollfd x11_poll;
        struct timespec ts;
        struct timespec sLastRender;

        clock_gettime(CLOCK_REALTIME, &sLastRender);
        bool leave              = false;
        int bmask               = 0;
        int mouse_x = 0, mouse_y = 0;

        while (!leave)
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
                lsp_trace("Poll returned error: %d, code=%d", poll_res, err_code);
                if (err_code != EINTR)
                    return -1;
            }
            else if ((force) || ((poll_res > 0) && (x11_poll.revents > 0)))
            {
                if (force)
                {
                    XGetWindowAttributes(dpy, win, &gwa);
                    glViewport(0, 0, gwa.width, gwa.height);
                    render_view(&view, gwa.width, gwa.height);
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
                            leave = on_key_press(&view, XLookupKeysym(&xev.xkey, 0));
                            break;
                        case ButtonPress:
                            if (bmask == 0)
                            {
                                mouse_x = xev.xbutton.x;
                                mouse_y = xev.xbutton.y;
                            }
                            bmask |= (1 << xev.xbutton.button);
                            break;
                        case ButtonRelease:
                            bmask &= ~(1 << xev.xbutton.button);
                            if (bmask == 0)
                            {
                                view.fAngleX   += view.fAngleDX;
                                view.fAngleY   += view.fAngleDY;
                                view.fAngleZ   += view.fAngleDZ;
                                view.fScale    += view.fDeltaScale;

                                view.fAngleDX   = 0.0f;
                                view.fAngleDY   = 0.0f;
                                view.fAngleDZ   = 0.0f;
                                view.fDeltaScale= 0.0f;
                            }
                            break;
                        case MotionNotify:
                            if (bmask & 2)
                            {
                                view.fAngleDX  = ((xev.xbutton.y - mouse_y) * M_PI / 20.0f);
                                view.fAngleDZ  = ((xev.xbutton.x - mouse_x) * M_PI / 20.0f);
                            }
                            else if (bmask & 8)
                            {
                                view.fDeltaScale = (mouse_y - xev.xbutton.y) / 200.0f;
                            }
                            break;
                    }
                }
            }
        } // while

        destroy_view(&view);

        glXMakeCurrent(dpy, None, NULL);
        glXDestroyContext(dpy, glc);
        XDestroyWindow(dpy, win);
        XCloseDisplay(dpy);

        return 0;
    }

    #undef TOLERANCE3D
}

#undef ADDM_BUF_SIZE

MTEST_BEGIN("core", geometry3d)

    MTEST_MAIN
    {
        MTEST_ASSERT(geometry3d_test::test() == 0);
    }
MTEST_END



