/*
 * backend.h
 *
 *  Created on: 18 апр. 2019 г.
 *      Author: sadko
 */

#ifndef RENDERING_GLX_BACKEND_H_
#define RENDERING_GLX_BACKEND_H_

#define R3D_GLX_BACKEND_EXP(func)   export_func(r3d_backend_t::func, &r3d_base_backend_t::func);

typedef struct glx_backend_t: public r3d_base_backend_t
{
    Display    *pDisplay;
    Window      hWnd;

    void build_vtable();

    void destroy()
    {
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
        hWnd        = window;

        // Get window attributes
        XWindowAttributes atts;
        if (!::XGetWindowAttributes(pDisplay, hWnd, &atts))
        {
            ::XCloseDisplay(pDisplay);
            pDisplay    = NULL;
            return STATUS_NO_DEVICE;
        }


        return STATUS_OK;
    }

    status_t start(size_t x, size_t y, size_t width, size_t height)
    {
        return STATUS_OK;
    }

    status_t finish()
    {
        return STATUS_OK;
    }

} glx_backend_t;

void glx_backend_t::build_vtable()
{
    r3d_base_backend_t::build_vtable();

    R3D_GLX_BACKEND_EXP(init);
    R3D_GLX_BACKEND_EXP(destroy);
    R3D_GLX_BACKEND_EXP(start);
    R3D_GLX_BACKEND_EXP(finish);
}

#undef R3D_GLX_BACKEND_EXP



#endif /* RENDERING_GLX_BACKEND_H_ */
