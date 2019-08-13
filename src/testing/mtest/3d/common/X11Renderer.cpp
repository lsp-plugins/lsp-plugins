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

#include <metadata/metadata.h>
#include <core/io/Path.h>
#include <core/io/Dir.h>
#include <rendering/factory.h>
#include <testing/mtest/3d/common/X11Renderer.h>

#ifdef LSP_IDE_DEBUG
    #ifdef PLATFORM_UNIX_COMPATIBLE
        #define USE_GLX_FACTORY

        #include <rendering/glx/factory.h>

        namespace lsp
        {
            extern glx_factory_t   glx_factory;
        }
    #endif
#endif /* LSP_IDE_DEBUG */

namespace mtest
{
    r3d_factory_t *probe_3d_backend(ipc::Library *dlib, const io::Path *path)
    {
        ipc::Library lib;

        lsp_trace("Probing file %s as 3D rendering backend...", path->as_native());

        // Open library
        status_t res = lib.open(path);
        if (res != STATUS_OK)
        {
            lsp_trace("Could not open library %s", path->as_native());
            return NULL;
        }

        // Lookup function
        lsp_r3d_factory_function_t func = reinterpret_cast<lsp_r3d_factory_function_t>(lib.import(R3D_FACTORY_FUNCTION_NAME));
        if (func == NULL)
        {
            lsp_trace("Could not lookup function %s", R3D_FACTORY_FUNCTION_NAME);
            lib.close();
            return NULL;
        }

        // Try to instantiate factory
        r3d_factory_t *factory  = func(LSP_MAIN_VERSION);
        if (factory == NULL)
        {
            lsp_trace("Could not obtain factory pointer");
            lib.close();
            return NULL;
        }

        lib.swap(dlib);
        return factory;
    }

    r3d_factory_t *lookup_factory(ipc::Library *dlib, const io::Path *path)
    {
        io::Dir dir;

        status_t res = dir.open(path);
        if (res != STATUS_OK)
            return NULL;

        io::Path child;
        LSPString item, prefix, postfix;
        if (!prefix.set_ascii(LSP_R3D_BACKEND_PREFIX))
            return NULL;

        io::fattr_t fattr;
        r3d_factory_t *factory;
        while ((res = dir.read(&item, false)) == STATUS_OK)
        {
            if (!item.starts_with(&prefix))
                continue;

            if ((res = child.set(path, &item)) != STATUS_OK)
                continue;
            if ((res = child.stat(&fattr)) != STATUS_OK)
                continue;

            switch (fattr.type)
            {
                case io::fattr_t::FT_DIRECTORY:
                case io::fattr_t::FT_BLOCK:
                case io::fattr_t::FT_CHARACTER:
                    continue;
                default:
                    if ((factory = probe_3d_backend(dlib, &child)) != NULL)
                        return factory;
                    break;
            }
        }

        return NULL;
    }

    r3d_factory_t *get_r3d_factory(ipc::Library *dlib)
    {
#ifdef USE_GLX_FACTORY
        return &glx_factory;
#else
        status_t res;
        io::Path path;
        r3d_factory_t *factory = NULL;

        res = ipc::Library::get_self_file(&path);
        if (res == STATUS_OK)
            res     = path.parent();
        if (res == STATUS_OK)
        {
            if ((factory = lookup_factory(dlib, &path)) != NULL)
                return factory;
        }

        return factory;
#endif
    }

    X11Renderer::X11Renderer(View3D *view)
    {
        dpy                 = NULL;
        win                 = None;
        glwnd               = None;
        stopped             = true;
        nBMask              = 0;
        nMouseX             = 0;
        nMouseY             = 0;
        nWidth              = 0;
        nHeight             = 0;

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

        pBackend            = NULL;

        pView               = view;

        sAngles.fYaw        = 0.0f;
        sAngles.fPitch      = 0.0f;
        sAngles.fRoll       = 0.0f;
        dsp::init_point_xyz(&sPov, 0.0f, -6.0f, 0.0f);

        dsp::init_vector_dxyz(&sDir, 0.0f, -1.0f, 0.0f);
        dsp::init_vector_dxyz(&sTop, 0.0f, 0.0f, -1.0f);
        dsp::init_vector_dxyz(&sSide, -1.0f, 0.0f, 0.0f);

        // Article about yaw-pitch-roll
        // http://in2gpu.com/2016/02/26/opengl-fps-camera/
        // https://sites.google.com/site/csc8820/educational/move-a-camera
        dsp::init_matrix3d_identity(&sProjection);
        dsp::init_matrix3d_identity(&sDelta);
        dsp::init_matrix3d_identity(&sView);

        bViewChanged        = true;
        rotate_camera(0, 0, true);
    }
    
    X11Renderer::~X11Renderer()
    {
        destroy();
    }

    void X11Renderer::view_changed()
    {
    }

    status_t X11Renderer::init()
    {
        // Fetch factory
        r3d_factory_t *factory = get_r3d_factory(&sLibrary);
        if (factory == NULL)
        {
            lsp_error("Could not find proper 3D rendering backend");
            return STATUS_NOT_FOUND;
        }

        // Create backend
        pBackend        = factory->create(factory, 0);
        if (pBackend == NULL)
            return STATUS_UNKNOWN_ERR;

        // Connect to X11
        dpy = ::XOpenDisplay(NULL);
        if (dpy == NULL)
        {
            lsp_error("cannot connect to X server");
            return STATUS_NO_DEVICE;
        }

        Window root = DefaultRootWindow(dpy);

        nWidth          = 800;
        nHeight         = 600;

        XSetWindowAttributes    swa;
        swa.event_mask = ExposureMask | StructureNotifyMask |
                KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;
        win     = ::XCreateWindow(dpy, root, 0, 0, nWidth, nHeight, 0, CopyFromParent, InputOutput, CopyFromParent, CWEventMask, &swa);

        ::XSelectInput(dpy, win, swa.event_mask);

        Atom wm_delete = ::XInternAtom(dpy, "WM_DELETE_WINDOW", False);
        ::XSetWMProtocols(dpy, win, &wm_delete, 1);

        ::XMapWindow(dpy, win);
        ::XStoreName(dpy, win, "3D Viewer");

        void *hwnd = NULL;
        status_t res = pBackend->init_window(pBackend, &hwnd);
        if (res == STATUS_OK)
            res = pBackend->locate(pBackend, 0, 0, nWidth, nHeight);

        // Reparent window and show
        if (hwnd != NULL)
        {
            glwnd = reinterpret_cast<Window>(hwnd);

            ::XReparentWindow(dpy, glwnd, win, 0, 0);
            ::XMapWindow(dpy, glwnd);
            ::XSelectInput(dpy, glwnd,
                KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask
            );
            ::XSync(dpy, False);
        }

        return res;
    }

    void X11Renderer::move_camera(const vector3d_t *dir, float amount)
    {
        sPov.x     += dir->dx * amount * 0.1f;
        sPov.y     += dir->dy * amount * 0.1f;
        sPov.z     += dir->dz * amount * 0.1f;
        bViewChanged = true;
    }

    void X11Renderer::rotate_camera(ssize_t x, ssize_t y, bool commit)
    {
        matrix3d_t dm;  // Delta-matrix
        float yaw       = sAngles.fYaw - ((x - nMouseX) * M_PI / 1000.0f);
        float pitch     = sAngles.fPitch - ((y - nMouseY) * M_PI / 1000.0f);

        if (pitch >= (89.0f * M_PI / 360.0f))
            pitch       = (89.0f * M_PI / 360.0f);
        else if (pitch <= (-89.0f * M_PI / 360.0f))
            pitch       = (-89.0f * M_PI / 360.0f);

        dsp::init_matrix3d_rotate_z(&sDelta, yaw);
        dsp::init_matrix3d_rotate_x(&dm, pitch);
        dsp::apply_matrix3d_mm1(&sDelta, &dm);

        // Need to commit changes?
        if (commit)
        {
            sAngles.fYaw    = yaw;
            sAngles.fPitch  = pitch;
        }

        bViewChanged    = true;
    }

    status_t X11Renderer::run()
    {
        if (dpy == NULL)
            return STATUS_BAD_STATE;

        stopped                 = false;

        int x11_fd              = ConnectionNumber(dpy);
        struct pollfd x11_poll;
        struct timespec ts;
        struct timespec sLastRender;

        ::clock_gettime(CLOCK_REALTIME, &sLastRender);
        Atom wm_proto           = ::XInternAtom(dpy, "WM_PROTOCOLS", False);
        Atom wm_delete          = ::XInternAtom(dpy, "WM_DELETE_WINDOW", False);

        while (!stopped)
        {
            // Get current time
            ::clock_gettime(CLOCK_REALTIME, &ts);
            ssize_t dmsec   = (ts.tv_nsec - sLastRender.tv_nsec) / 1000000;
            ssize_t dsec    = (ts.tv_sec - sLastRender.tv_sec);
            dmsec          += dsec * 1000;
            bool force      = (dmsec >= 20); // each 20 msec render request

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
                    render();
                    sLastRender = ts;
                }

                XEvent xev;
                int pending = ::XPending(dpy);

                for (int i=0; i<pending; i++)
                {
                    ::XNextEvent(dpy, &xev);

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
                        case ConfigureNotify:
                            bViewChanged    = true;
                            nWidth          = xev.xconfigure.width;
                            nHeight         = xev.xconfigure.height;
                            XMoveResizeWindow(dpy, glwnd, 0, 0, nWidth, nHeight);
                            XFlush(dpy);
                            pBackend->locate(pBackend, 0, 0, nWidth, nHeight);
                            render();
                            sLastRender = ts;
                            break;
                        case ResizeRequest:
                            bViewChanged    = true;
                            nWidth          = xev.xresizerequest.width;
                            nHeight         = xev.xresizerequest.height;
                            XMoveResizeWindow(dpy, glwnd, 0, 0, nWidth, nHeight);
                            XFlush(dpy);
                            pBackend->locate(pBackend, 0, 0, nWidth, nHeight);
                            render();
                            sLastRender = ts;
                            break;
                        case Expose:
                            break;
                        case ClientMessage:
                            if (xev.xclient.message_type == wm_proto)
                            {
                                if (xev.xclient.data.l[0] == long(wm_delete))
                                    stopped = true;
                            }
                            break;
                    }
                }
            }
        } // while

        return STATUS_OK;
    }

    void X11Renderer::destroy()
    {
        if (pBackend != NULL)
        {
            pBackend->destroy(pBackend);
            pBackend    = NULL;
        }

        if (win != None)
        {
            ::XDestroyWindow(dpy, win);
            win = None;
        }

        if (dpy != NULL)
        {
            ::XCloseDisplay(dpy);
            dpy = NULL;
        }
    }

    void X11Renderer::stop()
    {
        stopped = true;
    }

    void X11Renderer::on_key_press(const XKeyEvent &ev, KeySym key)
    {
        vector3d_t dir, side;
        dsp::apply_matrix3d_mv2(&dir, &sDir, &sDelta);
        dsp::apply_matrix3d_mv2(&side, &sSide, &sDelta);

        switch (key)
        {
            case XK_Escape:
                stop();
                break;
            case XK_Left:       move_camera(&side, 1.0f); break;
            case XK_Right:      move_camera(&side,-1.0f); break;
            case XK_Up:         move_camera(&dir, -1.0f);  break;
            case XK_Down:       move_camera(&dir, 1.0f); break;
            case XK_Page_Up:    move_camera(&sTop, -1.0f);  break;
            case XK_Page_Down:  move_camera(&sTop, 1.0f); break;

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
            // Rotate camera
            rotate_camera(ev.x, ev.y, true);
        }
    }

    void X11Renderer::on_mouse_move(const XMotionEvent &ev)
    {
        if (nBMask & 2)
        {
            rotate_camera(ev.x, ev.y, false);
        }
        else if (nBMask & 8)
        {
//            fDeltaScale = (nMouseY - ev.y) / 200.0f;
            bViewChanged    = true;
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

    void X11Renderer::draw_normals(v_vertex3d_t *vv, size_t nvertex)
    {
        r3d_buffer_t buffer;
        point3d_t *np       = reinterpret_cast<point3d_t *>(::malloc(sizeof(point3d_t) * 2 * nvertex));
        if (np == NULL)
            return;

        buffer.type         = R3D_PRIMITIVE_LINES;
        buffer.width        = 1.0f;
        buffer.flags        = 0;
        buffer.count        = nvertex;

        // Fill primitive array
        point3d_t *dp       = np;
        v_vertex3d_t *sv    = vv;
        for (size_t i=0; i<nvertex; ++i, dp += 2, ++sv)
        {
            dp[0]   = sv->p;
            dp[1].x = sv->p.x + sv->n.dx;
            dp[1].y = sv->p.y + sv->n.dy;
            dp[1].z = sv->p.z + sv->n.dz;
            dp[1].w = 1.0f;
        }

        buffer.vertex.data      = np;
        buffer.vertex.stride    = sizeof(point3d_t);
        buffer.normal.data      = NULL;
        buffer.color.data       = NULL;
        buffer.index.data       = NULL;
        buffer.color.dfl.r      = 1.0f;
        buffer.color.dfl.g      = 1.0f;
        buffer.color.dfl.b      = 0.0f;
        buffer.color.dfl.a      = 0.0f;

        // Draw call
        pBackend->draw_primitives(pBackend, &buffer);

        ::free(np);
    }

    void X11Renderer::render()
    {
        // Changed view? Recompute matrices
        if (bViewChanged)
        {
            // Compute projection matrix
            float fovY = 90.0f;
            float aspect = float(nWidth)/float(nHeight);
            float zNear = 0.1f;
            float zFar = 100.0f;

            float fH = tan( fovY * M_PI / 360.0f) * zNear;
            float fW = fH * aspect;
            dsp::init_matrix3d_frustum(&sProjection, -fW, fW, -fH, fH, zNear, zFar);

            // Compute view matrix depending on camera's position
            vector3d_t dir;
            dsp::apply_matrix3d_mv2(&dir, &sDir, &sDelta);
            dsp::init_matrix3d_lookat_p1v2(&sView, &sPov, &dir, &sTop);

            // Make a callback that view matrix has changed
            lsp_trace("pov    = {%f, %f, %f}", sPov.x, sPov.y, sPov.z);
            lsp_trace("angles = {%f, %f, %f}", sAngles.fYaw, sAngles.fPitch, sAngles.fRoll);
            view_changed();

            // Reset 'changed view' flag
            bViewChanged = false;
        }

        // Light parameters
        r3d_light_t light;

        light.type          = R3D_LIGHT_POINT; //R3D_LIGHT_DIRECTIONAL;
        light.position      = sPov;
        light.direction.dx  = -sDir.dx;
        light.direction.dy  = -sDir.dy;
        light.direction.dz  = -sDir.dz;
        light.direction.dw  = 0.0f;

        light.ambient.r     = 0.0f;
        light.ambient.g     = 0.0f;
        light.ambient.b     = 0.0f;
        light.ambient.a     = 1.0f;

        light.diffuse.r     = 1.0f;
        light.diffuse.g     = 1.0f;
        light.diffuse.b     = 1.0f;
        light.diffuse.a     = 1.0f;

        light.specular.r    = 1.0f;
        light.specular.g    = 1.0f;
        light.specular.b    = 1.0f;
        light.specular.a    = 1.0f;

        light.constant      = 1.0f;
        light.linear        = 0.0f;
        light.quadratic     = 0.0f;
        light.cutoff        = 180.0f;

        r3d_buffer_t buffer;

        // Start rendering
        pBackend->start(pBackend);

            pBackend->set_matrix(pBackend, R3D_MATRIX_PROJECTION, &sProjection);
            pBackend->set_matrix(pBackend, R3D_MATRIX_VIEW, &sView);

            // Enable/disable lighting
            pBackend->set_lights(pBackend, &light, 1);

            // Draw non-transparent data
            if (bDrawTriangles)
            {
                v_vertex3d_t *vv    = pView->get_vertexes();
                size_t nvertex      = pView->num_vertexes();

                // Fill buffer
                buffer.type         = (bWireframe) ? R3D_PRIMITIVE_WIREFRAME_TRIANGLES : R3D_PRIMITIVE_TRIANGLES;
                buffer.width        = 1.0f;
                buffer.count        = nvertex / 3;
                buffer.flags        = 0;
                if (bLight)
                    buffer.flags       |= R3D_BUFFER_LIGHTING;
                if (!bCullFace)
                    buffer.flags       |= R3D_BUFFER_NO_CULLING;

                buffer.vertex.data  = &vv->p;
                buffer.vertex.stride= sizeof(v_vertex3d_t);
                buffer.normal.data  = &vv->n;
                buffer.normal.stride= sizeof(v_vertex3d_t);
                buffer.color.data   = &vv->c;
                buffer.color.stride = sizeof(v_vertex3d_t);
                buffer.index.data   = NULL;

                // Draw call
                pBackend->draw_primitives(pBackend, &buffer);

                // Draw normals?
                if (bDrawNormals)
                    draw_normals(vv, nvertex);
            }

            // Draw rays
            if (bDrawRays)
            {
                v_ray3d_t *rays = pView->get_rays();
                size_t nrays    = pView->num_rays();

                // Draw ray points
                buffer.type             = R3D_PRIMITIVE_POINTS;
                buffer.width            = 5.0f;
                buffer.count            = nrays;
                buffer.flags            = 0;

                buffer.vertex.data      = &rays->p;
                buffer.vertex.stride    = sizeof(v_ray3d_t);
                buffer.normal.data      = NULL;
                buffer.color.data       = &rays->c;
                buffer.color.stride     = sizeof(v_ray3d_t);
                buffer.index.data       = NULL;

                pBackend->draw_primitives(pBackend, &buffer);

                // Now draw segments
                v_point3d_t *tmp        = reinterpret_cast<v_point3d_t *>(::malloc(sizeof(v_point3d_t) * 2 * nrays));
                if (tmp != NULL)
                {
                    v_ray3d_t *sr   = rays;
                    v_point3d_t *dp = tmp;
                    for (size_t i=0; i<nrays; ++i, dp += 2, ++sr)
                    {
                        dp[0].p     = sr->p;
                        dp[0].c     = sr->c;
                        dp[1].p.x   = sr->p.x + sr->v.dx * 4.0f;
                        dp[1].p.y   = sr->p.y + sr->v.dy * 4.0f;
                        dp[1].p.z   = sr->p.z + sr->v.dz * 4.0f;
                        dp[1].p.w   = 1.0f;
                        dp[1].c     = sr->c;
                        dp[1].c.a   = 0.0f;
                    }

                    buffer.type             = R3D_PRIMITIVE_LINES;
                    buffer.width            = 1.0f;
                    buffer.flags            = R3D_BUFFER_BLENDING;
                    buffer.count            = nrays;

                    buffer.vertex.data      = &tmp->p;
                    buffer.vertex.stride    = sizeof(v_point3d_t);
                    buffer.normal.data      = NULL;
                    buffer.color.data       = &tmp->c;
                    buffer.color.stride     = sizeof(v_point3d_t);
                    buffer.index.data       = NULL;

                    // Draw call
                    pBackend->draw_primitives(pBackend, &buffer);

                    ::free(tmp);
                }
            }

            // Draw points
            if (bDrawPoints)
            {
                v_point3d_t *points     = pView->get_points();
                size_t npoints          = pView->num_points();

                buffer.type             = R3D_PRIMITIVE_POINTS;
                buffer.width            = 5.0f;
                buffer.count            = npoints;
                buffer.flags            = 0;

                buffer.vertex.data      = &points->p;
                buffer.vertex.stride    = sizeof(v_point3d_t);
                buffer.normal.data      = NULL;
                buffer.color.data       = &points->c;
                buffer.color.stride     = sizeof(v_point3d_t);
                buffer.index.data       = NULL;

                // Draw call
                pBackend->draw_primitives(pBackend, &buffer);
            }

            // Draw segments
            if (bDrawSegments)
            {
                v_segment3d_t *segments = pView->get_segments();
                size_t nsegments        = pView->num_segments();

                v_point3d_t *tmp        = reinterpret_cast<v_point3d_t *>(::malloc(sizeof(v_point3d_t) * 2 * nsegments));
                if (tmp != NULL)
                {
                    v_point3d_t *dp     = tmp;
                    v_segment3d_t *ss   = segments;

                    for (size_t i=0; i<nsegments; ++i, dp += 2, ++ss)
                    {
                        dp[0].p     = ss->p[0];
                        dp[0].c     = ss->c[0];
                        dp[1].p     = ss->p[1];
                        dp[1].c     = ss->c[1];
                    }

                    // Draw lines
                    buffer.type             = R3D_PRIMITIVE_LINES;
                    buffer.width            = 3.0f;
                    buffer.flags            = 0;
                    buffer.count            = nsegments;

                    buffer.vertex.data      = &tmp->p;
                    buffer.vertex.stride    = sizeof(v_point3d_t);
                    buffer.normal.data      = NULL;
                    buffer.color.data       = &tmp->c;
                    buffer.color.stride     = sizeof(v_point3d_t);
                    buffer.index.data       = NULL;

                    // Draw call
                    pBackend->draw_primitives(pBackend, &buffer);

                    // Draw points
                    buffer.type             = R3D_PRIMITIVE_POINTS;
                    buffer.width            = 5.0f;
                    buffer.count            = nsegments * 2;

                    // Draw call
                    pBackend->draw_primitives(pBackend, &buffer);

                    ::free(tmp);
                }
            }

            // Draw transparent data
            if (bDrawTriangles)
            {
                v_vertex3d_t *vv    = pView->get_vertexes2();
                size_t nvertex      = pView->num_vertexes2();

                // Fill buffer
                buffer.type         = (bWireframe) ? R3D_PRIMITIVE_WIREFRAME_TRIANGLES : R3D_PRIMITIVE_TRIANGLES;
                buffer.width        = 1.0f;
                buffer.count        = nvertex / 3;
                buffer.flags        = R3D_BUFFER_BLENDING;
                if (bLight)
                    buffer.flags       |= R3D_BUFFER_LIGHTING;

                buffer.vertex.data  = &vv->p;
                buffer.vertex.stride= sizeof(v_vertex3d_t);
                buffer.normal.data  = &vv->n;
                buffer.normal.stride= sizeof(v_vertex3d_t);
                buffer.color.data   = &vv->c;
                buffer.color.stride = sizeof(v_vertex3d_t);
                buffer.index.data   = NULL;

                // Draw call
                pBackend->draw_primitives(pBackend, &buffer);

                // Draw normals?
                if (bDrawNormals)
                    draw_normals(vv, nvertex);
            }

        pBackend->finish(pBackend);
    }
} /* namespace mtest */
