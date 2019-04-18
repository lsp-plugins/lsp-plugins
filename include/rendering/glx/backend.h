/*
 * backend.h
 *
 *  Created on: 18 апр. 2019 г.
 *      Author: sadko
 */

#ifndef RENDERING_GLX_BACKEND_H_
#define RENDERING_GLX_BACKEND_H_

#define R3D_GLX_BACKEND_EXP(func)   export_func(r3d_backend_t::func, &r3d_base_backend_t::func);

static GLint rgba24[]       = { GLX_RGBA, GLX_RED_SIZE, 8, GLX_GREEN_SIZE, 8, GLX_BLUE_SIZE, 8, GLX_ALPHA_SIZE, 8, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
static GLint rgba16[]       = { GLX_RGBA, GLX_RED_SIZE, 5, GLX_GREEN_SIZE, 6, GLX_BLUE_SIZE, 5, GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None };
static GLint rgba15[]       = { GLX_RGBA, GLX_RED_SIZE, 5, GLX_GREEN_SIZE, 5, GLX_BLUE_SIZE, 5, GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None };
static GLint rgba[]         = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };

static GLint *glx_visuals[] =
{
    rgba24, rgba16, rgba15, rgba,
    NULL
};

typedef struct glx_backend_t: public r3d_base_backend_t
{
    Display    *pDisplay;
    Window      hParent;
    Window      hWnd;
    GLXContext  hContext;
    bool        bVisible;

    void build_vtable();

    void destroy()
    {
        // Destroy GLX Context
        if (hContext != NULL)
        {
            glXDestroyContext(pDisplay, hContext);
            hContext    = NULL;
        }

        // Destroy the window
        if (hWnd != None)
        {
            ::XDestroyWindow(pDisplay, hWnd);
            hWnd        = None;
        }

        // Destroy X11 display
        if (pDisplay != NULL)
        {
            ::XCloseDisplay(pDisplay);
            pDisplay    = NULL;
        }

        // Call parent structure for destroy
        r3d_base_backend_t::destroy();
    }

    status_t init(Window window)
    {
        // Check that already initialized
        if (pDisplay != NULL)
            return STATUS_BAD_STATE;

        // Initialize parent structure
        status_t res = r3d_base_backend_t::init();
        if (res != STATUS_OK)
            return res;

        // Init threading
        ::XInitThreads();

        // Open display
        pDisplay = ::XOpenDisplay(NULL);
        if (pDisplay == NULL)
            return STATUS_NO_DEVICE;

        // Save window
        hParent         = window;

        // Check that window exists
        XWindowAttributes atts;
        if (!::XGetWindowAttributes(pDisplay, hParent, &atts))
        {
            ::XCloseDisplay(pDisplay);
            pDisplay    = NULL;
            return STATUS_BAD_ARGUMENTS;
        }

        int screen      = DefaultScreen(pDisplay);
        Window root     = RootWindow(pDisplay, screen);

        // Choose GLX visual
        XVisualInfo *vi = NULL;
        for (GLint **visual = glx_visuals; *visual != NULL; ++visual)
        {
            if ((vi = ::glXChooseVisual(pDisplay, screen, *visual)) != NULL)
                break;
        }

        if (vi == NULL)
        {
            ::XCloseDisplay(pDisplay);
            pDisplay    = NULL;
            return STATUS_UNSUPPORTED_DEVICE;
        }

        // Create context
        hContext = glXCreateContext(pDisplay, vi, NULL, GL_TRUE);
        if (hContext == NULL)
        {
            ::XCloseDisplay(pDisplay);
            pDisplay    = NULL;
            return STATUS_NO_DEVICE;
        }

        // Create child window
        Colormap cmap   = XCreateColormap(pDisplay, root, vi->visual, AllocNone);
        XSetWindowAttributes    swa;

        swa.colormap = cmap;
        swa.event_mask = ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;

        // Create the child window
        hWnd = ::XCreateWindow(pDisplay, root, 0, 0, 0, 0, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
        if (hWnd == None)
        {
            ::XCloseDisplay(pDisplay);
            pDisplay    = NULL;
            return STATUS_NO_DEVICE;
        }

        // Place window to the parent
        ::XReparentWindow(pDisplay, hWnd, hParent, 0, 0);

        return STATUS_OK;
    }

    status_t show()
    {
        if (pDisplay == NULL)
            return STATUS_BAD_STATE;
        if (bVisible)
            return STATUS_OK;

        ::XMapWindow(pDisplay, hWnd);
        bVisible = true;
        return STATUS_OK;
    }

    status_t hide()
    {
        if (pDisplay == NULL)
            return STATUS_BAD_STATE;
        if (!bVisible)
            return STATUS_OK;

        ::XUnmapWindow(pDisplay, hWnd);
        bVisible = false;
        return STATUS_OK;
    }

    status_t locate(ssize_t left, ssize_t top, ssize_t width, ssize_t height)
    {
        if (pDisplay == NULL)
            return STATUS_BAD_STATE;

        if (!::XMoveResizeWindow(pDisplay, hWnd, left, top, width, height))
            return STATUS_UNKNOWN_ERR;

        viewLeft    = left;
        viewTop     = top;
        viewWidth   = width;
        viewHeight  = height;

        return STATUS_OK;
    }

    status_t start()
    {
        if (pDisplay == NULL)
            return STATUS_BAD_STATE;

        ::glXMakeCurrent(pDisplay, hWnd, hContext);
        return STATUS_OK;
    }

    status_t finish()
    {
        if (pDisplay == NULL)
            return STATUS_BAD_STATE;

        ::glViewport(0, 0, viewWidth, viewHeight);
        ::glXSwapBuffers(pDisplay, hWnd);
        ::glXMakeCurrent(pDisplay, hWnd, NULL);

        return STATUS_OK;
    }

} glx_backend_t;

void glx_backend_t::build_vtable()
{
    r3d_base_backend_t::build_vtable();

    R3D_GLX_BACKEND_EXP(init);
    R3D_GLX_BACKEND_EXP(destroy);
    R3D_GLX_BACKEND_EXP(show);
    R3D_GLX_BACKEND_EXP(hide);
    R3D_GLX_BACKEND_EXP(locate);
    R3D_GLX_BACKEND_EXP(start);
    R3D_GLX_BACKEND_EXP(finish);
}

#undef R3D_GLX_BACKEND_EXP



#endif /* RENDERING_GLX_BACKEND_H_ */
