/*
 * IR3DBackend.cpp
 *
 *  Created on: 24 апр. 2019 г.
 *      Author: sadko
 */

#include <ui/ws/ws.h>

namespace lsp
{
    namespace ws
    {
        IR3DBackend::IR3DBackend(IDisplay *dpy, r3d_backend_t *backend, void *parent, void *window)
        {
            pBackend    = backend;
            hParent     = parent;
            hWindow     = window;
            pDisplay    = dpy;
        }
        
        IR3DBackend::~IR3DBackend()
        {
            pBackend    = NULL;
            hParent     = NULL;
            hWindow     = NULL;
        }

        status_t IR3DBackend::destroy()
        {
            if (pBackend != NULL)
            {
                pBackend->destroy(pBackend);
                pDisplay->deregister_backend(this);
            }

            pBackend    = NULL;
            hWindow     = NULL;
            hParent     = NULL;
            pDisplay    = NULL;

            return STATUS_OK;
        }

        void IR3DBackend::replace_backend(r3d_backend_t *backend, void *window)
        {
            if (pBackend != NULL)
            {
                // Replace matrices
                matrix3d_t tmp;
                if (pBackend->get_matrix(pBackend, R3D_MATRIX_PROJECTION, &tmp) == STATUS_OK)
                    backend->set_matrix(backend, R3D_MATRIX_PROJECTION, &tmp);
                if (pBackend->get_matrix(pBackend, R3D_MATRIX_VIEW, &tmp) == STATUS_OK)
                    backend->set_matrix(backend, R3D_MATRIX_VIEW, &tmp);
                if (pBackend->get_matrix(pBackend, R3D_MATRIX_WORLD, &tmp) == STATUS_OK)
                    backend->set_matrix(backend, R3D_MATRIX_WORLD, &tmp);

                // Copy location
                ssize_t l, t, w, h;
                if (pBackend->get_location(pBackend, &l, &t, &w, &h) == STATUS_OK)
                    backend->locate(backend, l, t, w, h);

                // Copy default color
                color3d_t c;
                if (pBackend->get_bg_color(pBackend, &c) == STATUS_OK)
                    backend->set_bg_color(backend, &c);

                // Destroy previous backend
                pBackend->destroy(pBackend);
            }

            pBackend    = backend;
            hWindow     = window;
        }
//
//        status_t IR3DBackend::show()
//        {
//            return (pBackend != NULL) ? pBackend->show(pBackend) : STATUS_BAD_STATE;
//        }
//
//        status_t IR3DBackend::hide()
//        {
//            return (pBackend != NULL) ? pBackend->hide(pBackend) : STATUS_BAD_STATE;
//        }

        status_t IR3DBackend::sync()
        {
            return (pBackend != NULL) ? pBackend->sync(pBackend) : STATUS_BAD_STATE;
        }

        status_t IR3DBackend::locate(ssize_t left, ssize_t top, ssize_t width, ssize_t height)
        {
            return (pBackend != NULL) ? pBackend->locate(pBackend, left, top, width, height) : STATUS_BAD_STATE;
        }

        status_t IR3DBackend::get_location(ssize_t *left, ssize_t *top, ssize_t *width, ssize_t *height)
        {
            return (pBackend != NULL) ? pBackend->get_location(pBackend, left, top, width, height) : STATUS_BAD_STATE;
        }

        status_t IR3DBackend::begin_draw()
        {
            return (pBackend != NULL) ? pBackend->start(pBackend) : STATUS_BAD_STATE;
        }

        status_t IR3DBackend::end_draw()
        {
            return (pBackend != NULL) ? pBackend->finish(pBackend) : STATUS_BAD_STATE;
        }

        status_t IR3DBackend::read_pixels(void *buf, size_t stride, r3d_pixel_format_t format)
        {
            return (pBackend != NULL) ? pBackend->read_pixels(pBackend, buf, stride, format) : STATUS_BAD_STATE;
        }

        status_t IR3DBackend::set_matrix(r3d_matrix_type_t type, const matrix3d_t *m)
        {
            return (pBackend != NULL) ? pBackend->set_matrix(pBackend, type, m) : STATUS_BAD_STATE;
        }

        status_t IR3DBackend::get_matrix(r3d_matrix_type_t type, matrix3d_t *m)
        {
            return (pBackend != NULL) ? pBackend->get_matrix(pBackend, type, m) : STATUS_BAD_STATE;
        }

        status_t IR3DBackend::set_lights(const r3d_light_t *lights, size_t count)
        {
            return (pBackend != NULL) ? pBackend->set_lights(pBackend, lights, count) : STATUS_BAD_STATE;
        }

        status_t IR3DBackend::draw_primitives(const r3d_buffer_t *buffer)
        {
            return (pBackend != NULL) ? pBackend->draw_primitives(pBackend, buffer) : STATUS_BAD_STATE;
        }

        status_t IR3DBackend::set_bg_color(const color3d_t *color)
        {
            return (pBackend != NULL) ? pBackend->set_bg_color(pBackend, color) : STATUS_BAD_STATE;
        }

    } /* namespace io */
} /* namespace lsp */
