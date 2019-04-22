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
    bool        bDrawing;

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
        bDrawing    = false;

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
        if ((pDisplay == NULL) || (bDrawing))
            return STATUS_BAD_STATE;

        // Enable context
        ::glViewport(0, 0, viewWidth, viewHeight);
        ::glXMakeCurrent(pDisplay, hWnd, hContext);

        // Enable depth test and culling
        ::glEnable(GL_DEPTH_TEST);
        ::glEnable(GL_CULL_FACE);
        ::glCullFace(GL_BACK);
        ::glEnable(GL_COLOR_MATERIAL);

        // Tune lighting
        ::glShadeModel(GL_FLAT);
        ::glEnable(GL_RESCALE_NORMAL);

        // enable blending
        ::glEnable(GL_BLEND);
        ::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Load matrices
        ::glMatrixMode(GL_PROJECTION);
        ::glLoadMatrixf(matProjection.m);

        ::glMatrixMode(GL_MODELVIEW);
        matrix3d_t view;
        matrix_mul(&view, &matWorld, &matView);
        ::glLoadMatrixf(view.m);

        // Special tuning for non-poligonal primitives
        ::glPolygonOffset(-1, -1);
        ::glEnable(GL_POLYGON_OFFSET_POINT);

        // Clear buffer
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClearDepth(1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Mark as started
        bDrawing        = true;

        return STATUS_OK;
    }

    status_t set_lights(const r3d_light_t *lights, size_t count)
    {
        if ((pDisplay == NULL) || (!bDrawing))
            return STATUS_BAD_STATE;

        // Disable lighing?
        if ((count == 0) || (lights == NULL))
        {
            glDisable(GL_LIGHTING);
            return STATUS_OK;
        }

        // Enable all possible lights
        size_t light_id = GL_LIGHT0;

        for (size_t i=0; i<count; ++i)
        {
            // Skip disabled lights
            if (lights[i].type == R3D_LIGHT_NONE)
                continue;

            // Enable the light and set basic attributes
            vector3d_t position = lights[i].position;

            glEnable(light_id);
            glLightfv(light_id, GL_AMBIENT, &lights[i].ambient.r);
            glLightfv(light_id, GL_DIFFUSE, &lights[i].diffuse.r);
            glLightfv(light_id, GL_SPECULAR, &lights[i].specular.r);

            switch (lights[i].type)
            {
                case R3D_LIGHT_POINT:
                    position.dw     = 1.0f;
                    glLightfv(light_id, GL_POSITION, &position.dx);
                    glLighti(light_id, GL_SPOT_CUTOFF, 180);
                    break;
                case R3D_LIGHT_DIRECTIONAL:
                    position.dw     = 0.0f;
                    glLightfv(light_id, GL_POSITION, &position.dx);
                    glLighti(light_id, GL_SPOT_CUTOFF, 180);
                    break;
                case R3D_LIGHT_SPOT:
                    position.dw     = 1.0f;
                    glLightfv(light_id, GL_POSITION, &position.dx);
                    glLightfv(light_id, GL_SPOT_DIRECTION, &lights[i].direction.dx);
                    glLightf(light_id, GL_SPOT_CUTOFF, lights[i].cutoff);
                    glLightf(light_id, GL_CONSTANT_ATTENUATION, lights[i].constant);
                    glLightf(light_id, GL_LINEAR_ATTENUATION, lights[i].linear);
                    glLightf(light_id, GL_QUADRATIC_ATTENUATION, lights[i].quadratic);
                    break;
                default:
                    return STATUS_INVALID_VALUE;
            }

            // Ignore all lights that are out of 8 basic lights
            if (++light_id > GL_LIGHT7)
                break;
        }

        // Disable all other non-related lights
        while (light_id <= GL_LIGHT7)
            glDisable(light_id++);

        // Always enable lighting, even if there is nothing to shine
        glEnable(GL_LIGHTING);
    }

    status_t draw_primitives(const r3d_buffer_t *buffer)
    {
        if (buffer == NULL)
            return STATUS_BAD_ARGUMENTS;
        if ((pDisplay == NULL) || (!bDrawing))
            return STATUS_BAD_STATE;

        // Select the drawing mode
        GLenum mode;
        size_t count = buffer->count;

        switch (buffer->type)
        {
            case R3D_PRIMITIVE_TRIANGLES:
                mode    = GL_TRIANGLES;
                count   = (count << 1) + count; // count *= 3
                break;
            case R3D_PRIMITIVE_WIREFRAME_TRIANGLES:
                mode    = GL_LINE_LOOP;
                count   = (count << 1) + count; // count *= 3
                break;
            case R3D_PRIMITIVE_LINES:
                mode    = GL_LINES;
                count <<= 1;                    // count *= 2
                glLineWidth(buffer->size);
                break;
            case R3D_PRIMITIVE_POINTS:
                mode    = GL_POINTS;
                glPointSize(buffer->size);
                break;
            default:
                return STATUS_BAD_ARGUMENTS;
        }

        // Enable vertex pointer (if present)
        if (buffer->vertex.data != NULL)
        {
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT,
                (buffer->vertex.stride == 0) ? sizeof(point3d_t) : buffer->vertex.stride,
                buffer->vertex.data
            );
        }
        else
            glDisableClientState(GL_VERTEX_ARRAY);

        // Enable normal pointer
        if (buffer->normal.data != NULL)
        {
            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer(GL_FLOAT,
                (buffer->normal.stride == 0) ? sizeof(vector3d_t) : buffer->normal.stride,
                buffer->normal.data
            );
        }
        else
            glDisableClientState(GL_NORMAL_ARRAY);

        // Enable color pointer
        if (buffer->color.data != NULL)
        {
            glEnableClientState(GL_COLOR_ARRAY);
            glColorPointer(4, GL_FLOAT,
                (buffer->color.stride == 0) ? sizeof(color3d_t) : buffer->color.stride,
                buffer->color.data
            );
        }
        else
            glDisableClientState(GL_COLOR_ARRAY);

        // Draw the elements (or arrays, depending on configuration)
        if (buffer->type != R3D_PRIMITIVE_WIREFRAME_TRIANGLES)
        {
            if (buffer->index.data != NULL)
                glDrawElements(mode, count, GL_UNSIGNED_INT, buffer->index.data);
            else
                glDrawArrays(mode, 0, count);
        }
        else
        {
            if (buffer->index.data != NULL)
            {
                const uint32_t *ptr = buffer->index.data;
                for (size_t i=0; i<count; i += 3, ptr += 3)
                    glDrawElements(mode, 3, GL_UNSIGNED_INT, ptr);
            }
            else
            {
                for (size_t i=0; i<count; i += 3)
                    glDrawArrays(mode, i, 3);
            }
        }

        // Disable previous settings
        if (buffer->color.data != NULL)
            glDisableClientState(GL_COLOR_ARRAY);
        if (buffer->normal.data != NULL)
            glDisableClientState(GL_NORMAL_ARRAY);
        if (buffer->vertex.data != NULL)
            glDisableClientState(GL_VERTEX_ARRAY);

        return STATUS_OK;
    }

    status_t finish()
    {
        if ((pDisplay == NULL) || (!bDrawing))
            return STATUS_BAD_STATE;

        ::glXSwapBuffers(pDisplay, hWnd);
        ::glXMakeCurrent(pDisplay, hWnd, NULL);
        bDrawing    = false;

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
    R3D_GLX_BACKEND_EXP(set_lights);
    R3D_GLX_BACKEND_EXP(draw_primitives);
    R3D_GLX_BACKEND_EXP(finish);
}

#undef R3D_GLX_BACKEND_EXP



#endif /* RENDERING_GLX_BACKEND_H_ */
