/*
 * backend.cpp
 *
 *  Created on: 24 апр. 2019 г.
 *      Author: sadko
 */

// Common libraries
#include <dsp/dsp.h>
#include <core/stdlib/string.h>
#include <rendering/glx/backend.h>
#include <metadata/metadata.h>

static GLint rgba24[]       = { GLX_RGBA, GLX_RED_SIZE, 8, GLX_GREEN_SIZE, 8, GLX_BLUE_SIZE, 8, GLX_ALPHA_SIZE, 8, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
static GLint rgba16[]       = { GLX_RGBA, GLX_RED_SIZE, 5, GLX_GREEN_SIZE, 6, GLX_BLUE_SIZE, 5, GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None };
static GLint rgba15[]       = { GLX_RGBA, GLX_RED_SIZE, 5, GLX_GREEN_SIZE, 5, GLX_BLUE_SIZE, 5, GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None };
static GLint rgba[]         = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };

static GLint *glx_visuals[] =
{
    rgba24, rgba16, rgba15, rgba,
    NULL
};

namespace lsp
{
    void glx_backend_t::destroy(glx_backend_t *_this)
    {
        // Destroy GLX Context
        if (_this->hContext != NULL)
        {
            ::glXDestroyContext(_this->pDisplay, _this->hContext);
            _this->hContext    = NULL;
        }

        // Destroy the window
        if (_this->hWnd != None)
        {
            ::XDestroyWindow(_this->pDisplay, _this->hWnd);
            _this->hWnd        = None;
        }

        // Destroy X11 display
        if (_this->pDisplay != NULL)
        {
            ::XFlush(_this->pDisplay);
            ::XCloseDisplay(_this->pDisplay);
            _this->pDisplay    = NULL;
        }

        // Call parent structure for destroy
        r3d_base_backend_t::destroy(_this);
    }

    status_t glx_backend_t::init(glx_backend_t *_this, Window window, void **out_window)
    {
        // Check that already initialized
        if (_this->pDisplay != NULL)
            return STATUS_BAD_STATE;

        // Initialize parent structure
        status_t res = r3d_base_backend_t::init(_this);
        if (res != STATUS_OK)
            return res;


        // Open display
        _this->pDisplay = ::XOpenDisplay(NULL);
        if (_this->pDisplay == NULL)
            return STATUS_NO_DEVICE;

        // Save window
        _this->hParent         = window;

        // Check that window exists
        XWindowAttributes atts;
        if (!::XGetWindowAttributes(_this->pDisplay, _this->hParent, &atts))
        {
            ::XCloseDisplay(_this->pDisplay);
            _this->pDisplay    = NULL;
            return STATUS_BAD_ARGUMENTS;
        }

        int screen      = DefaultScreen(_this->pDisplay);
        Window root     = RootWindow(_this->pDisplay, screen);

        // Choose GLX visual
        XVisualInfo *vi = NULL;
        for (GLint **visual = glx_visuals; *visual != NULL; ++visual)
        {
            if ((vi = ::glXChooseVisual(_this->pDisplay, screen, *visual)) != NULL)
                break;
        }

        if (vi == NULL)
        {
            ::XCloseDisplay(_this->pDisplay);
            _this->pDisplay    = NULL;
            return STATUS_UNSUPPORTED_DEVICE;
        }

        // Create context
        _this->hContext = ::glXCreateContext(_this->pDisplay, vi, NULL, GL_TRUE);
        if (_this->hContext == NULL)
        {
            ::XCloseDisplay(_this->pDisplay);
            _this->pDisplay    = NULL;
            return STATUS_NO_DEVICE;
        }

        // Create child window
        Colormap cmap   = ::XCreateColormap(_this->pDisplay, root, vi->visual, AllocNone);
        XSetWindowAttributes    swa;

        swa.colormap = cmap;
    //        swa.event_mask = ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;

        // Create the child window
        _this->hWnd = ::XCreateWindow(_this->pDisplay, root, 0, 0, 1, 1, 0, vi->depth, InputOutput, vi->visual, CWColormap, &swa);
        if (_this->hWnd == None)
        {
            ::XCloseDisplay(_this->pDisplay);
            _this->pDisplay    = NULL;
            return STATUS_NO_DEVICE;
        }

        // Flush changes
        ::XFlush(_this->pDisplay);

        // Place window to the parent
//        if (_this->hParent != None)
//            ::XReparentWindow(_this->pDisplay, _this->hWnd, _this->hParent, 0, 0);
//        ::XFlush(_this->pDisplay);
//        ::XMapWindow(_this->pDisplay, _this->hWnd);
        ::XSync(_this->pDisplay, False);

        _this->bDrawing    = false;

        // Return result
        if (out_window != NULL)
            *out_window     = reinterpret_cast<void *>(_this->hWnd);

        return STATUS_OK;
    }

//    status_t glx_backend_t::show(glx_backend_t *_this)
//    {
//        if (_this->pDisplay == NULL)
//            return STATUS_BAD_STATE;
//        if (_this->bVisible)
//            return STATUS_OK;
//
//        ::XMapWindow(_this->pDisplay, _this->hWnd);
//        ::XFlush(_this->pDisplay);
//
//        _this->bVisible = true;
//        return STATUS_OK;
//    }
//
//    status_t glx_backend_t::hide(glx_backend_t *_this)
//    {
//        if (_this->pDisplay == NULL)
//            return STATUS_BAD_STATE;
//        if (!_this->bVisible)
//            return STATUS_OK;
//
//        ::XUnmapWindow(_this->pDisplay, _this->hWnd);
//        ::XFlush(_this->pDisplay);
//
//        _this->bVisible = false;
//        return STATUS_OK;
//    }

    status_t glx_backend_t::locate(glx_backend_t *_this, ssize_t left, ssize_t top, ssize_t width, ssize_t height)
    {
        if (_this->pDisplay == NULL)
            return STATUS_BAD_STATE;

        if (!::XMoveResizeWindow(_this->pDisplay, _this->hWnd, left, top, width, height))
            return STATUS_UNKNOWN_ERR;
        ::XFlush(_this->pDisplay);

        _this->viewLeft    = left;
        _this->viewTop     = top;
        _this->viewWidth   = width;
        _this->viewHeight  = height;

        return STATUS_OK;
    }

    status_t glx_backend_t::start(glx_backend_t *_this)
    {
        if ((_this->pDisplay == NULL) || (_this->bDrawing))
            return STATUS_BAD_STATE;

        // Enable context
        ::glXMakeCurrent(_this->pDisplay, _this->hWnd, _this->hContext);
        ::glViewport(0, 0, _this->viewWidth, _this->viewHeight);

        // Enable depth test and culling
        ::glEnable(GL_DEPTH_TEST);
        ::glEnable(GL_CULL_FACE);
        ::glCullFace(GL_BACK);
        ::glEnable(GL_COLOR_MATERIAL);

        // Tune lighting
        ::glShadeModel(GL_SMOOTH);
        ::glEnable(GL_RESCALE_NORMAL);

        // Load matrices
        ::glMatrixMode(GL_PROJECTION);
        ::glLoadMatrixf(_this->matProjection.m);

        ::glMatrixMode(GL_MODELVIEW);
        matrix3d_t view;
        matrix_mul(&view, &_this->matWorld, &_this->matView);
        ::glLoadMatrixf(view.m);

        // Special tuning for non-poligonal primitives
        ::glPolygonOffset(-1, -1);
        ::glEnable(GL_POLYGON_OFFSET_POINT);

        // Clear buffer
        glClearColor(_this->colBackground.r, _this->colBackground.g, _this->colBackground.b, _this->colBackground.a);
        glClearDepth(1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Mark as started
        _this->bDrawing        = true;

        return STATUS_OK;
    }

    status_t glx_backend_t::set_matrix(glx_backend_t *_this, r3d_matrix_type_t type, const matrix3d_t *m)
    {
        status_t res = r3d_base_backend_t::set_matrix(_this, type, m);

        // Need to immediately update matrices?
        if ((res == STATUS_OK) && (_this->bDrawing))
        {
            // Load matrices
            ::glMatrixMode(GL_PROJECTION);
            ::glLoadMatrixf(_this->matProjection.m);

            ::glMatrixMode(GL_MODELVIEW);
            matrix3d_t view;
            matrix_mul(&view, &_this->matWorld, &_this->matView);
            ::glLoadMatrixf(view.m);
        }

        return res;
    }

    status_t glx_backend_t::set_lights(glx_backend_t *_this, const r3d_light_t *lights, size_t count)
    {
        if ((_this->pDisplay == NULL) || (!_this->bDrawing))
            return STATUS_BAD_STATE;

        // Enable all possible lights
        size_t light_id = GL_LIGHT0;

        for (size_t i=0; i<count; ++i)
        {
            // Skip disabled lights
            if (lights[i].type == R3D_LIGHT_NONE)
                continue;

            // Enable the light and set basic attributes
            vector3d_t position;

            ::glEnable(light_id);
            ::glLightfv(light_id, GL_AMBIENT, &lights[i].ambient.r);
            ::glLightfv(light_id, GL_DIFFUSE, &lights[i].diffuse.r);
            ::glLightfv(light_id, GL_SPECULAR, &lights[i].specular.r);

            switch (lights[i].type)
            {
                case R3D_LIGHT_POINT:
                    position.dx     = lights[i].position.x;
                    position.dy     = lights[i].position.y;
                    position.dz     = lights[i].position.z;
                    position.dw     = 1.0f;
                    ::glLightfv(light_id, GL_POSITION, &position.dx);
                    ::glLighti(light_id, GL_SPOT_CUTOFF, 180);
                    break;
                case R3D_LIGHT_DIRECTIONAL:
                    position.dx     = lights[i].direction.dx;
                    position.dy     = lights[i].direction.dy;
                    position.dz     = lights[i].direction.dz;
                    position.dw     = 0.0f;
                    ::glLightfv(light_id, GL_POSITION, &position.dx);
                    ::glLighti(light_id, GL_SPOT_CUTOFF, 180);
                    break;
                case R3D_LIGHT_SPOT:
                    position.dx     = lights[i].position.x;
                    position.dy     = lights[i].position.y;
                    position.dz     = lights[i].position.z;
                    position.dw     = 1.0f;
                    ::glLightfv(light_id, GL_POSITION, &position.dx);
                    ::glLightfv(light_id, GL_SPOT_DIRECTION, &lights[i].direction.dx);
                    ::glLightf(light_id, GL_SPOT_CUTOFF, lights[i].cutoff);
                    ::glLightf(light_id, GL_CONSTANT_ATTENUATION, lights[i].constant);
                    ::glLightf(light_id, GL_LINEAR_ATTENUATION, lights[i].linear);
                    ::glLightf(light_id, GL_QUADRATIC_ATTENUATION, lights[i].quadratic);
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
            ::glDisable(light_id++);

        return STATUS_OK;
    }

    status_t glx_backend_t::draw_primitives(glx_backend_t *_this, const r3d_buffer_t *buffer)
    {
        if (buffer == NULL)
            return STATUS_BAD_ARGUMENTS;
        if ((_this->pDisplay == NULL) || (!_this->bDrawing))
            return STATUS_BAD_STATE;

        // Is there any data to draw?
        if (buffer->count <= 0)
            return STATUS_OK;

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
                ::glLineWidth(buffer->width);
                break;
            case R3D_PRIMITIVE_LINES:
                mode    = GL_LINES;
                count <<= 1;                    // count *= 2
                ::glLineWidth(buffer->width);
                break;
            case R3D_PRIMITIVE_POINTS:
                mode    = GL_POINTS;
                ::glPointSize(buffer->width);
                break;
            default:
                return STATUS_BAD_ARGUMENTS;
        }

        // enable blending
        if (buffer->flags & R3D_BUFFER_BLENDING)
        {
            ::glEnable(GL_BLEND);
            ::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        if (buffer->flags & R3D_BUFFER_LIGHTING)
            ::glEnable(GL_LIGHTING);

        // Enable vertex pointer (if present)
        if (buffer->vertex.data != NULL)
        {
            ::glEnableClientState(GL_VERTEX_ARRAY);
            ::glVertexPointer(3, GL_FLOAT,
                (buffer->vertex.stride == 0) ? sizeof(point3d_t) : buffer->vertex.stride,
                buffer->vertex.data
            );
        }
        else
            ::glDisableClientState(GL_VERTEX_ARRAY);

        // Enable normal pointer
        if (buffer->normal.data != NULL)
        {
            ::glEnableClientState(GL_NORMAL_ARRAY);
            ::glNormalPointer(GL_FLOAT,
                (buffer->normal.stride == 0) ? sizeof(vector3d_t) : buffer->normal.stride,
                buffer->normal.data
            );
        }
        else
            ::glDisableClientState(GL_NORMAL_ARRAY);

        // Enable color pointer
        if (buffer->color.data != NULL)
        {
            ::glEnableClientState(GL_COLOR_ARRAY);
            ::glColorPointer(4, GL_FLOAT,
                (buffer->color.stride == 0) ? sizeof(color3d_t) : buffer->color.stride,
                buffer->color.data
            );
        }
        else
        {
            ::glColor4fv(&buffer->color.dfl.r);         // Set-up default color
            ::glDisableClientState(GL_COLOR_ARRAY);
        }

        // Draw the elements (or arrays, depending on configuration)
        if (buffer->type != R3D_PRIMITIVE_WIREFRAME_TRIANGLES)
        {
            if (buffer->index.data != NULL)
                ::glDrawElements(mode, count, GL_UNSIGNED_INT, buffer->index.data);
            else
                ::glDrawArrays(mode, 0, count);
        }
        else
        {
            if (buffer->index.data != NULL)
            {
                const uint32_t *ptr = buffer->index.data;
                for (size_t i=0; i<count; i += 3, ptr += 3)
                    ::glDrawElements(mode, 3, GL_UNSIGNED_INT, ptr);
            }
            else
            {
                for (size_t i=0; i<count; i += 3)
                    ::glDrawArrays(mode, i, 3);
            }
        }

        // Disable previous settings
        if (buffer->color.data != NULL)
            ::glDisableClientState(GL_COLOR_ARRAY);
        if (buffer->normal.data != NULL)
            ::glDisableClientState(GL_NORMAL_ARRAY);
        if (buffer->vertex.data != NULL)
            ::glDisableClientState(GL_VERTEX_ARRAY);

        if (buffer->flags & R3D_BUFFER_BLENDING)
            ::glDisable(GL_BLEND);
        if (buffer->flags & R3D_BUFFER_LIGHTING)
            ::glDisable(GL_LIGHTING);

        return STATUS_OK;
    }

    status_t glx_backend_t::sync(glx_backend_t *_this)
    {
        if ((_this->pDisplay == NULL) || (!_this->bDrawing))
            return STATUS_BAD_STATE;

        ::glFinish();
        ::glFlush();

        return STATUS_OK;
    }

    status_t glx_backend_t::read_pixels(glx_backend_t *_this, void *buf, size_t stride, r3d_pixel_format_t format)
    {
        if ((_this->pDisplay == NULL) || (!_this->bDrawing))
            return STATUS_BAD_STATE;

//        size_t rowsize = _this->viewWidth * sizeof(uint32_t);
        size_t fmt = (format == R3D_PIXEL_RGBA) ? GL_RGBA : GL_BGRA;
//        if (rowsize == stride) // Read once
//        {
//            ::glReadPixels(0, 0, _this->viewWidth, _this->viewHeight, fmt, GL_UNSIGNED_INT_8_8_8_8, buf);
//        }
//        else // Read row-by row
//        {
//            uint8_t *ptr = reinterpret_cast<uint8_t *>(buf);
//            for (ssize_t i=0; i<_this->viewHeight; ++i)
//            {
//                ::glReadPixels(0, i, _this->viewWidth, i, fmt, GL_UNSIGNED_INT_8_8_8_8, ptr);
//                ptr     += stride;
//            }
//        }
        uint8_t *ptr = reinterpret_cast<uint8_t *>(buf);
        for (ssize_t i=0; i<_this->viewHeight; ++i)
        {
            ssize_t row  = _this->viewHeight - i - 1;
            ::glReadPixels(0, row, _this->viewWidth, 1, fmt, GL_UNSIGNED_INT_8_8_8_8, ptr);
            ptr     += stride;
        }

        return STATUS_OK;
    }

    status_t glx_backend_t::finish(glx_backend_t *_this)
    {
        if ((_this->pDisplay == NULL) || (!_this->bDrawing))
            return STATUS_BAD_STATE;

        ::glFinish();
        ::glFlush();
        ::glXSwapBuffers(_this->pDisplay, _this->hWnd);
        _this->bDrawing    = false;

        return STATUS_OK;
    }

    glx_backend_t::glx_backend_t()
    {
        pDisplay    = NULL;
        hParent     = None;
        hWnd        = None;
        hContext    = NULL;
        bVisible    = false;
        bDrawing    = false;

        // Export virtual table
        #define R3D_GLX_BACKEND_EXP(func)   export_func(r3d_backend_t::func, &glx_backend_t::func);
        R3D_GLX_BACKEND_EXP(init);
        R3D_GLX_BACKEND_EXP(destroy);
//        R3D_GLX_BACKEND_EXP(show);
//        R3D_GLX_BACKEND_EXP(hide);
        R3D_GLX_BACKEND_EXP(locate);

        R3D_GLX_BACKEND_EXP(start);
        R3D_GLX_BACKEND_EXP(sync);
        R3D_GLX_BACKEND_EXP(read_pixels);
        R3D_GLX_BACKEND_EXP(finish);

        R3D_GLX_BACKEND_EXP(set_matrix);
        R3D_GLX_BACKEND_EXP(set_lights);
        R3D_GLX_BACKEND_EXP(draw_primitives);

        #undef R3D_GLX_BACKEND_EXP
    }
}
