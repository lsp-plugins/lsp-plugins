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
    X11Renderer::X11Renderer(View3D *view)
    {
        dpy                 = NULL;
        win                 = None;
        glc                 = NULL;
        stopped             = true;
        nBMask              = 0;
        nMouseX             = 0;
        nMouseY             = 0;

        bWireframe          = false;
        bRotate             = false;
        bLight              = false;
        bInvert             = false;
        bCullFace           = true;

        bDrawRays           = true;
        bDrawTriangles      = true;
        bDrawPoints         = true;
        bDrawNormals        = true;
        bDrawSegments       = true;
        bDrawCapture        = true;
        bDrawSource         = true;

        fAngleX             = 0.0f;
        fAngleY             = 0.0f;
        fAngleZ             = 0.0f;
        fScale              = 1.0f;

        fAngleDX            = 0.0f;
        fAngleDY            = 0.0f;
        fAngleDZ            = 0.0f;
        fDeltaScale         = 0.0f;
        pView               = view;
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

        // Get extensions
        const char* extensions = (const char *)glGetString(GL_EXTENSIONS);
        if (extensions != NULL)
        {
            lsp_trace("OpenGL extension list: %s", extensions);
            lsp_trace("GL_ARB_arrays_of_arrays supported: %s", is_supported(extensions, "gl_arb_arrays_of_arrays") ? "true" : "false");
            lsp_trace("GL_SUN_slice_accum supported: %s", is_supported(extensions, "gl_sun_slice_accum") ? "true" : "false");
            lsp_trace("INVALID_EXTENSION supported: %s", is_supported(extensions, "invalid_extension") ? "true" : "false");
            // TODO: detect several set of extensions
        }

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
                    render(gwa.width, gwa.height);
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
        switch (key)
        {
            case XK_Escape:
                stop();
                break;
            case ' ':
                bRotate = !bRotate;
                break;
            case 'w':
                bWireframe = !bWireframe;
                break;
            case 'l':
                bLight = !bLight;
                break;
            case 'c':
                bCullFace = !bCullFace;
                break;
            case 'i':
                bInvert = !bInvert;
                break;
            case 'r':
                bDrawRays    = ! bDrawRays;
                break;
            case 't':
                bDrawTriangles   = ! bDrawTriangles;
                break;
            case 'p':
                bDrawPoints  = !bDrawPoints;
                break;
            case 's':
                bDrawSegments  = !bDrawSegments;
                break;
            case 'g':
                bDrawSource  = !bDrawSource;
                break;
            case 'n':
                bDrawNormals = !bDrawNormals;
                break;
            case 'd':
                bDrawCapture = !bDrawCapture;
                break;
//            case '0': case '1': case '2': case '3': case '4':
//            case '5': case '6': case '7': case '8': case '9':
//            {
//                Object3D *obj = pScene->get_object(key - '0');
//                if (obj != NULL)
//                    obj->set_visible(!obj->is_visible());
//                break;
//            }
        }
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
            fAngleX    += fAngleDX;
            fAngleY    += fAngleDY;
            fAngleZ    += fAngleDZ;
            fScale     += fDeltaScale;

            fAngleDX    = 0.0f;
            fAngleDY    = 0.0f;
            fAngleDZ    = 0.0f;
            fDeltaScale = 0.0f;
        }
    }

    void X11Renderer::on_mouse_move(const XMotionEvent &ev)
    {
        if (nBMask & 2)
        {
            fAngleDX    = ((ev.y - nMouseY) * M_PI / 20.0f);
            fAngleDZ    = ((ev.x - nMouseX) * M_PI / 20.0f);
        }
        else if (nBMask & 8)
        {
            fDeltaScale = (nMouseY - ev.y) / 200.0f;
        }
    }

    bool X11Renderer::is_supported(const char *set, const char *ext)
    {
        size_t ck_len = strlen(ext);

        while (set != NULL)
        {
            char *sep = strchr(const_cast<char *>(set), ' ');
            if (sep == NULL)
                sep = strchr(const_cast<char *>(set), '\0');

            size_t len = sep - set;
            if (len == ck_len)
            {
                if (!strncasecmp(set, ext, len))
                    return true;
            }

            set = (*sep == '\0') ? NULL : sep + 1;
        }

        return false;
    }

    void X11Renderer::perspectiveGL(double fovY, double aspect, double zNear, double zFar)
    {
        GLdouble fW, fH;

        fH = tan( fovY * M_PI / 360.0f) * zNear;
        fW = fH * aspect;

        glFrustum( -fW, fW, -fH, fH, zNear, zFar );
    }

    void X11Renderer::render(size_t width, size_t height)
    {
        static const float light_pos[] = { 0.0, 0.0, 3.0f };

        glEnable(GL_DEPTH_TEST);
        if (bCullFace)
        {
            glEnable(GL_CULL_FACE);
            glCullFace((bInvert) ? GL_FRONT : GL_BACK);
        }
        glEnable(GL_COLOR_MATERIAL);

        glShadeModel(GL_SMOOTH);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        perspectiveGL(90.0f, float(width)/float(height), 0.1f, 100.0f);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClearDepth(1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glTranslatef(0.0f, 0.0f, -6.0f);

        float scale = fScale + fDeltaScale;
        glScalef(scale, scale, scale); // Scale

        if (bLight)
        {
            glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
            glEnable(GL_LIGHT0);
            glEnable(GL_LIGHTING);
            glEnable(GL_RESCALE_NORMAL);
        }

        glRotatef(fAngleX + fAngleDX, 1.0f, 0.0f, 0.0f);
        glRotatef(fAngleY + fAngleDY, 0.0f, 1.0f, 0.0f);
        glRotatef(fAngleZ + fAngleDZ, 0.0f, 0.0f, 1.0f);

        glPolygonOffset(-1, -1);
        glEnable(GL_POLYGON_OFFSET_POINT);
        glPointSize(5.0f);

        if (bDrawTriangles)
        {
            size_t n = pView->num_vertexes();
            v_vertex3d_t *vv = pView->get_vertexes();

            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_NORMAL_ARRAY);
            glEnableClientState(GL_COLOR_ARRAY);

            glVertexPointer(3, GL_FLOAT, sizeof(v_vertex3d_t), &vv->p);
            glNormalPointer(GL_FLOAT, sizeof(v_vertex3d_t), &vv->n);
            glColorPointer(3, GL_FLOAT, sizeof(v_vertex3d_t), &vv->c);

            if (bWireframe)
            {
                for (size_t i=0; i<n; i += 3)
                    glDrawArrays(GL_LINE_LOOP, i, 3);
            }
            else
                glDrawArrays(GL_TRIANGLES, 0, n);

            glDisableClientState(GL_COLOR_ARRAY);
            glDisableClientState(GL_NORMAL_ARRAY);
            glDisableClientState(GL_VERTEX_ARRAY);

            if (bDrawNormals)
            {
                if (bLight)
                    glDisable(GL_LIGHTING);

                glColor3f(1.0f, 1.0f, 0.0f);
                glBegin(GL_LINES);

                for (size_t i=0; i < n; ++i)
                {
                    v_vertex3d_t *v = &vv[i];
                    glVertex3fv(&v[0].p.x);
                    glVertex3f(v[0].p.x + v[0].n.dx, v[0].p.y + v[0].n.dy, v[0].p.z + v[0].n.dz);
                }
                glEnd();

                if (bLight)
                    glEnable(GL_LIGHTING);
            }
        }

        if (bLight)
        {
            glDisable(GL_RESCALE_NORMAL);
            glDisable(GL_LIGHTING);
        }

        // Draw segments
        if (bDrawSegments)
        {
            v_segment3d_t *s = pView->get_segments();
            size_t n = pView->num_segments();

            for (size_t i=0; i<n; ++i, ++s)
            {
                glColor3fv(&s->c.r);
                glBegin(GL_POINTS);
                    glVertex3fv(&s->p[0].x);
                    glVertex3fv(&s->p[1].x);
                glEnd();

                glBegin(GL_LINES);
                    glVertex3fv(&s->p[0].x);
                    glVertex3fv(&s->p[1].x);
                glEnd();
            }
        }

        // Draw rays
        if (bDrawRays)
        {
            glEnable (GL_BLEND);
            glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            v_ray3d_t *r    = pView->get_rays();
            size_t n        = pView->num_rays();

            glPointSize(5.0f);

            for (size_t i=0; i<n; ++i, ++r)
            {
                if (r->v.dw < 0.0f)
                    continue;

                glColor4f(r->c.r, r->c.g, r->c.b, 1.0f);
                glBegin(GL_POINTS);
                    glVertex3fv(&r->p.x);
                glEnd();

                glBegin(GL_LINES);
                    glVertex3fv(&r->p.x);
                    glColor4f(r->c.r, r->c.g, r->c.b, 0.0f);
                    glVertex3f(r->p.x + r->v.dx*4.0f, r->p.y + r->v.dy*4.0f, r->p.z + r->v.dz*4.0f);
                glEnd();
            }

            glDisable(GL_BLEND);
        }

        // Draw points
        if (bDrawPoints)
        {
            v_point3d_t *p = pView->get_points();
            size_t n = pView->num_points();

            glColor3f(0.0f, 1.0f, 1.0f);
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_COLOR_ARRAY);

            glVertexPointer(3, GL_FLOAT, sizeof(v_point3d_t), &p->p.x);
            glColorPointer(3, GL_FLOAT, sizeof(v_vertex3d_t), &p->c.r);

            glDrawArrays(GL_POINTS, 0, n);

            glDisableClientState(GL_COLOR_ARRAY);
            glDisableClientState(GL_VERTEX_ARRAY);
        }

        if (bLight)
            glEnable(GL_LIGHTING);

        glDisable(GL_POLYGON_OFFSET_POINT);

        if (bLight)
            glDisable(GL_LIGHTING);

        // Draw axis coordinates
        glDisable(GL_DEPTH_TEST);

        glLoadIdentity();
        glTranslatef(-0.8f * float(width)/float(height), -0.8f /* float(height)/float(width) */, -0.9f);
        glScalef(0.2, 0.2, 0.2); // Scale

        glRotatef(fAngleX + fAngleDX, 1.0f, 0.0f, 0.0f);
        glRotatef(fAngleY + fAngleDY, 0.0f, 1.0f, 0.0f);
        glRotatef(fAngleZ + fAngleDZ, 0.0f, 0.0f, 1.0f);

        glBegin(GL_LINES);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(1.0f, 0.0f, 0.0f);
            glVertex3f(1.0f, 0.0f, 0.0f);
            glVertex3f(0.8f, 0.05f, 0.0f);
            glVertex3f(1.0f, 0.0f, 0.0f);
            glVertex3f(0.8f, -0.05f, 0.0f);
            glVertex3f(1.0f, 0.0f, 0.0f);
            glVertex3f(0.8f, 0.0f, 0.05f);
            glVertex3f(1.0f, 0.0f, 0.0f);
            glVertex3f(0.8f, 0.0f, -0.05f);

            glColor3f(0.0f, 1.0f, 0.0f);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, 1.0f, 0.0f);
            glVertex3f(0.0f, 1.0f, 0.0f);
            glVertex3f(0.0f, 0.8f, 0.05f);
            glVertex3f(0.0f, 1.0f, 0.0f);
            glVertex3f(0.0f, 0.8f, -0.05f);
            glVertex3f(0.0f, 1.0f, 0.0f);
            glVertex3f(0.05f, 0.8f, 0.0f);
            glVertex3f(0.0f, 1.0f, 0.0f);
            glVertex3f(-0.05f, 0.8f, 0.0f);

            glColor3f(0.0f, 0.0f, 1.0f);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, 0.0f, 1.0f);

            glVertex3f(0.0f, 0.0f, 1.0f);
            glVertex3f(0.05f, 0.0f, 0.8f);
            glVertex3f(0.0f, 0.0f, 1.0f);
            glVertex3f(-0.05f, 0.0f, 0.8f);
            glVertex3f(0.0f, 0.0f, 1.0f);
            glVertex3f(0.0f, 0.05f, 0.8f);
            glVertex3f(0.0f, 0.0f, 1.0f);
            glVertex3f(0.0f, -0.05f, 0.8f);
        glEnd();

        if (bCullFace)
            glDisable(GL_CULL_FACE);

        // Rotate scene if it is possible
        if (bRotate)
        {
            fAngleX -= 0.3f;
            fAngleZ -= 0.4f;
        }
    }
} /* namespace mtest */
