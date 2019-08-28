/*
 * backend.cpp
 *
 *  Created on: 24 апр. 2019 г.
 *      Author: sadko
 */

#include <core/types.h>

#ifdef PLATFORM_UNIX_COMPATIBLE

// Common libraries
#include <core/debug.h>
#include <dsp/dsp.h>
#include <core/stdlib/string.h>
#include <rendering/glx/backend.h>
#include <metadata/metadata.h>

static GLint rgba24x32[]    = { GLX_RGBA, GLX_RED_SIZE, 8, GLX_GREEN_SIZE, 8, GLX_BLUE_SIZE, 8, GLX_ALPHA_SIZE, 8, GLX_DEPTH_SIZE, 32, GLX_DOUBLEBUFFER, None };
static GLint rgba24x24[]    = { GLX_RGBA, GLX_RED_SIZE, 8, GLX_GREEN_SIZE, 8, GLX_BLUE_SIZE, 8, GLX_ALPHA_SIZE, 8, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
static GLint rgba16x24[]    = { GLX_RGBA, GLX_RED_SIZE, 5, GLX_GREEN_SIZE, 6, GLX_BLUE_SIZE, 5, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
static GLint rgba15x24[]    = { GLX_RGBA, GLX_RED_SIZE, 5, GLX_GREEN_SIZE, 5, GLX_BLUE_SIZE, 5, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
static GLint rgba16[]       = { GLX_RGBA, GLX_RED_SIZE, 5, GLX_GREEN_SIZE, 6, GLX_BLUE_SIZE, 5, GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None };
static GLint rgba15[]       = { GLX_RGBA, GLX_RED_SIZE, 5, GLX_GREEN_SIZE, 5, GLX_BLUE_SIZE, 5, GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None };
static GLint rgbax32[]      = { GLX_RGBA, GLX_DEPTH_SIZE, 32, GLX_DOUBLEBUFFER, None };
static GLint rgbax24[]      = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
static GLint rgbax16[]      = { GLX_RGBA, GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None };
static GLint rgba[]         = { GLX_RGBA, GLX_DOUBLEBUFFER, None };

static int pb_rgba24x32[]   = { GLX_RENDER_TYPE, GLX_RGBA_BIT, GLX_DRAWABLE_TYPE, GLX_PBUFFER_BIT, GLX_RED_SIZE, 8, GLX_GREEN_SIZE, 8, GLX_BLUE_SIZE, 8, GLX_ALPHA_SIZE, 8, GLX_DEPTH_SIZE, 32, None };
static int pb_rgba24x24[]   = { GLX_RENDER_TYPE, GLX_RGBA_BIT, GLX_DRAWABLE_TYPE, GLX_PBUFFER_BIT, GLX_RED_SIZE, 8, GLX_GREEN_SIZE, 8, GLX_BLUE_SIZE, 8, GLX_ALPHA_SIZE, 8, GLX_DEPTH_SIZE, 24, None };
static int pb_rgba16x24[]   = { GLX_RENDER_TYPE, GLX_RGBA_BIT, GLX_DRAWABLE_TYPE, GLX_PBUFFER_BIT, GLX_RED_SIZE, 5, GLX_GREEN_SIZE, 6, GLX_BLUE_SIZE, 5, GLX_DEPTH_SIZE, 24, None };
static int pb_rgba15x24[]   = { GLX_RENDER_TYPE, GLX_RGBA_BIT, GLX_DRAWABLE_TYPE, GLX_PBUFFER_BIT, GLX_RED_SIZE, 5, GLX_GREEN_SIZE, 5, GLX_BLUE_SIZE, 5, GLX_DEPTH_SIZE, 24, None };
static int pb_rgba16[]      = { GLX_RENDER_TYPE, GLX_RGBA_BIT, GLX_DRAWABLE_TYPE, GLX_PBUFFER_BIT, GLX_RED_SIZE, 5, GLX_GREEN_SIZE, 6, GLX_BLUE_SIZE, 5, GLX_DEPTH_SIZE, 16, None };
static int pb_rgba15[]      = { GLX_RENDER_TYPE, GLX_RGBA_BIT, GLX_DRAWABLE_TYPE, GLX_PBUFFER_BIT, GLX_RED_SIZE, 5, GLX_GREEN_SIZE, 5, GLX_BLUE_SIZE, 5, GLX_DEPTH_SIZE, 16, None };
static int pb_rgbax32[]     = { GLX_RENDER_TYPE, GLX_RGBA_BIT, GLX_DRAWABLE_TYPE, GLX_PBUFFER_BIT, GLX_DEPTH_SIZE, 32, None };
static int pb_rgbax24[]     = { GLX_RENDER_TYPE, GLX_RGBA_BIT, GLX_DRAWABLE_TYPE, GLX_PBUFFER_BIT, GLX_DEPTH_SIZE, 24, None };
static int pb_rgbax16[]     = { GLX_RENDER_TYPE, GLX_RGBA_BIT, GLX_DRAWABLE_TYPE, GLX_PBUFFER_BIT, GLX_DEPTH_SIZE, 16, None };
static int pb_rgba[]        = { GLX_RENDER_TYPE, GLX_RGBA_BIT, GLX_DRAWABLE_TYPE, GLX_PBUFFER_BIT, None };

static GLint *glx_visuals[] =
{
    rgba24x32, rgba24x24,
    rgba16x24, rgba16,
    rgba15x24, rgba15,
    rgbax32, rgbax24, rgbax16, rgba,
    NULL
};

static const int *glx_pb_config[] =
{
    pb_rgba24x32, pb_rgba24x24,
    pb_rgba16x24, pb_rgba16,
    pb_rgba15x24, pb_rgba15,
    pb_rgbax32, pb_rgbax24, pb_rgbax16, pb_rgba,
    NULL
};

//#define TRACK_GL_ERRORS { GLenum glErr; while ((glErr = glGetError()) != GL_NO_ERROR) lsp_error("GL ERROR code=%d", int(glErr)); }
#define TRACK_GL_ERRORS

namespace lsp
{
    void glx_backend_t::destroy(glx_backend_t *_this)
    {
        // Destroy pBuffer
        if (_this->hPBuffer != None)
        {
            ::glXDestroyPbuffer(_this->pDisplay, _this->hPBuffer);
            _this->hPBuffer     = None;
        }

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
            ::XSync(_this->pDisplay, False);
            ::XCloseDisplay(_this->pDisplay);
            _this->pDisplay    = NULL;
        }

        // Call parent structure for destroy
        r3d_base_backend_t::destroy(_this);
    }

    status_t glx_backend_t::init_window(glx_backend_t *_this, void **out_window)
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

        int screen      = DefaultScreen(_this->pDisplay);
        Window root     = RootWindow(_this->pDisplay, screen);

        // Choose GLX visual
        XVisualInfo *vi = NULL;
        for (GLint **visual = glx_visuals; *visual != NULL; ++visual)
        {
            if ((vi = ::glXChooseVisual(_this->pDisplay, screen, *visual)) != NULL)
                break;
        }

        lsp_trace("Choosed visual: 0x%lx, red=0x%lx, green=0x%lx, blue=0x%lx",
                long(vi->visualid), vi->red_mask, vi->green_mask, vi->blue_mask);

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
        ::XSync(_this->pDisplay, False);

        _this->bDrawing     = false;
        _this->bPBuffer     = false;

        // Return result
        if (out_window != NULL)
            *out_window     = reinterpret_cast<void *>(_this->hWnd);

        return STATUS_OK;
    }

    status_t glx_backend_t::init_offscreen(glx_backend_t *_this)
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

        // Choose FB config
        GLXFBConfig *fbc    = NULL;
        int screen          = DefaultScreen(_this->pDisplay);
        int nfbelements;
        for (const int **atts = glx_pb_config; *atts != NULL; ++atts)
        {
            if ((fbc = ::glXChooseFBConfig(_this->pDisplay, screen, *atts, &nfbelements)) != NULL)
                break;
        }

        // Success story?
        if ((fbc == NULL) || (nfbelements <= 0))
        {
            ::XCloseDisplay(_this->pDisplay);
            _this->pDisplay    = NULL;
            return STATUS_UNSUPPORTED_DEVICE;
        }

#ifdef LSP_TRACE
        int r, g, b, a, depth;
        glXGetFBConfigAttrib(_this->pDisplay, fbc[0], GLX_RED_SIZE, &r);
        glXGetFBConfigAttrib(_this->pDisplay, fbc[0], GLX_GREEN_SIZE, &g);
        glXGetFBConfigAttrib(_this->pDisplay, fbc[0], GLX_BLUE_SIZE, &b);
        glXGetFBConfigAttrib(_this->pDisplay, fbc[0], GLX_ALPHA_SIZE, &a);
        glXGetFBConfigAttrib(_this->pDisplay, fbc[0], GLX_DEPTH_SIZE, &depth);
        lsp_trace("Choosed FB config: r=%d, g=%d, b=%d, a=%d, depth=%d",
                r, g, b, a, depth);
#endif

        // Create context
        _this->hContext = ::glXCreateNewContext(_this->pDisplay, fbc[0], GLX_RGBA_TYPE, NULL, GL_TRUE);
        if (_this->hContext == NULL)
        {
            ::XFree(fbc);
            ::XCloseDisplay(_this->pDisplay);
            _this->pDisplay    = NULL;
            return STATUS_NO_DEVICE;
        }

        // Flush changes
        ::XFlush(_this->pDisplay);
        ::XSync(_this->pDisplay, False);

        _this->bDrawing     = false;
        _this->bPBuffer     = true;
        _this->pFBConfig    = fbc;

        return STATUS_OK;
    }

    status_t glx_backend_t::locate(glx_backend_t *_this, ssize_t left, ssize_t top, ssize_t width, ssize_t height)
    {
        if ((_this->pDisplay == NULL) || (_this->bDrawing))
            return STATUS_BAD_STATE;

        // Requested size does match current size?
        if (_this->bPBuffer)
        {
            if ((_this->viewWidth == width) &&
                (_this->viewHeight == height) &&
                (_this->hPBuffer != None))
            {
                // These attributes don't matter
                _this->viewLeft    = left;
                _this->viewTop     = top;
                return STATUS_OK;
            }

            // Destroy previously used pBuffer
            if (_this->hPBuffer != None)
            {
                ::glXDestroyPbuffer(_this->pDisplay, _this->hPBuffer);
                _this->hPBuffer     = None;
            }

            // Create new pBuffer
            int pbuffer_attributes[]={
                  GLX_PBUFFER_WIDTH, int(width),
                  GLX_PBUFFER_HEIGHT, int(height),
                  GLX_NONE
                };
            _this->hPBuffer    = ::glXCreatePbuffer(_this->pDisplay, _this->pFBConfig[0], pbuffer_attributes);
            if (_this->hPBuffer == None)
                return STATUS_NO_MEM;
        }
        else
        {
            if ((_this->viewLeft == left) &&
                (_this->viewTop == top) &&
                (_this->viewWidth == width) &&
                (_this->viewHeight == height))
                return STATUS_OK;

            if (!::XMoveResizeWindow(_this->pDisplay, _this->hWnd, left, top, width, height))
                return STATUS_UNKNOWN_ERR;
            ::XFlush(_this->pDisplay);
            ::XSync(_this->pDisplay, False);
        }

        // Update parameters
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

        // Setup current GLX context
        if (_this->bPBuffer)
        {
            ::glXMakeContextCurrent(_this->pDisplay, _this->hPBuffer, _this->hPBuffer, _this->hContext);
            ::glXWaitX();
            ::glDrawBuffer(GL_FRONT);
        }
        else
        {
            ::glXMakeCurrent(_this->pDisplay, _this->hWnd, _this->hContext);
            ::glXWaitX();
            ::glDrawBuffer(GL_BACK);
        }

        TRACK_GL_ERRORS
        ::glViewport(0, 0, _this->viewWidth, _this->viewHeight);

        // Enable depth test and culling
        ::glDepthFunc(GL_LEQUAL);
        ::glEnable(GL_DEPTH_TEST);
        TRACK_GL_ERRORS
        ::glEnable(GL_CULL_FACE);
        TRACK_GL_ERRORS
        ::glCullFace(GL_BACK);
        TRACK_GL_ERRORS
        ::glEnable(GL_COLOR_MATERIAL);
        TRACK_GL_ERRORS

        // Tune lighting
        ::glShadeModel(GL_SMOOTH);
        TRACK_GL_ERRORS
        ::glEnable(GL_RESCALE_NORMAL);
        TRACK_GL_ERRORS

        // Load matrices
        ::glMatrixMode(GL_PROJECTION);
        ::glLoadMatrixf(_this->matProjection.m);
        TRACK_GL_ERRORS

        ::glMatrixMode(GL_MODELVIEW);
        ::glLoadMatrixf(_this->matWorld.m);
        ::glMultMatrixf(_this->matView.m);
        TRACK_GL_ERRORS

        // Special tuning for non-poligonal primitives
        ::glPolygonOffset(1.0f, 2.0f);
        TRACK_GL_ERRORS
        ::glEnable(GL_POLYGON_OFFSET_POINT);
        ::glEnable(GL_POLYGON_OFFSET_FILL);
        ::glEnable(GL_POLYGON_OFFSET_LINE);
        TRACK_GL_ERRORS

        // Clear buffer
        ::glClearColor(_this->colBackground.r, _this->colBackground.g, _this->colBackground.b, _this->colBackground.a);
        TRACK_GL_ERRORS
        ::glClearDepth(1.0);
        TRACK_GL_ERRORS
        ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        TRACK_GL_ERRORS

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
            ::glLoadMatrixf(_this->matWorld.m);
            ::glMultMatrixf(_this->matView.m);
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
        if (buffer->flags & R3D_BUFFER_NO_CULLING)
            ::glDisable(GL_CULL_FACE);

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
        if (buffer->flags & R3D_BUFFER_NO_CULLING)
            ::glEnable(GL_CULL_FACE);

        return STATUS_OK;
    }

    status_t glx_backend_t::sync(glx_backend_t *_this)
    {
        if ((_this->pDisplay == NULL) || (!_this->bDrawing))
            return STATUS_BAD_STATE;

        ::glXWaitGL();
        TRACK_GL_ERRORS

        return STATUS_OK;
    }

    status_t glx_backend_t::read_pixels(glx_backend_t *_this, void *buf, size_t stride, r3d_pixel_format_t format)
    {
        if ((_this->pDisplay == NULL) || (!_this->bDrawing))
            return STATUS_BAD_STATE;

        size_t fmt;
        switch (format)
        {
            case R3D_PIXEL_RGBA: fmt    = GL_RGBA; break;
            case R3D_PIXEL_BGRA: fmt    = GL_BGRA; break;
            case R3D_PIXEL_RGB:  fmt    = GL_RGB;  break;
            case R3D_PIXEL_BGR:  fmt    = GL_BGR;  break;
            default:
                return STATUS_BAD_ARGUMENTS;
        }

        ::glReadBuffer(_this->bPBuffer ? GL_BACK : GL_FRONT);

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

        if (!_this->bPBuffer)
            ::glXSwapBuffers(_this->pDisplay, _this->hWnd);

        ::glXWaitGL();
        _this->bDrawing    = false;

        return STATUS_OK;
    }

    glx_backend_t::glx_backend_t()
    {
        pDisplay    = NULL;
        hWnd        = None;
        hPBuffer    = None;
        pFBConfig   = NULL;
        hContext    = NULL;
        bVisible    = false;
        bDrawing    = false;
        bPBuffer    = false;

        // Export virtual table
        #define R3D_GLX_BACKEND_EXP(func)   export_func(r3d_backend_t::func, &glx_backend_t::func);
        R3D_GLX_BACKEND_EXP(init_window);
        R3D_GLX_BACKEND_EXP(init_offscreen);
        R3D_GLX_BACKEND_EXP(destroy);
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

#endif
