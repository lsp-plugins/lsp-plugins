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
        IR3DBackend::IR3DBackend(IDisplay *dpy, r3d_backend_t *backend, void *window)
        {
            pBackend    = backend;
            hWindow     = window;
            pDisplay    = dpy;
        }
        
        IR3DBackend::~IR3DBackend()
        {
            pBackend    = NULL;
            hWindow     = NULL;
        }

        status_t IR3DBackend::close()
        {
            if (pBackend != NULL)
                pDisplay->destroy_backend(pBackend);
            pBackend    = NULL;
            pDisplay    = NULL;
            hWindow     = NULL;

            return STATUS_OK;
        }
    
        status_t IR3DBackend::show()
        {
            return (pBackend != NULL) ? pBackend->show(pBackend) : STATUS_BAD_STATE;
        }

        status_t IR3DBackend::hide()
        {
            return (pBackend != NULL) ? pBackend->hide(pBackend) : STATUS_BAD_STATE;
        }

        status_t IR3DBackend::locate(ssize_t left, ssize_t top, ssize_t width, ssize_t height)
        {
            return (pBackend != NULL) ? pBackend->locate(pBackend, left, top, width, height) : STATUS_BAD_STATE;
        }

        status_t IR3DBackend::begin_draw()
        {
            return (pBackend != NULL) ? pBackend->start(pBackend) : STATUS_BAD_STATE;
        }

        status_t IR3DBackend::end_draw()
        {
            return (pBackend != NULL) ? pBackend->finish(pBackend) : STATUS_BAD_STATE;
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
