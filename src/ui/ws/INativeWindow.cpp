/*
 * INativeWindow.cpp
 *
 *  Created on: 12 дек. 2016 г.
 *      Author: sadko
 */

#include <ui/ws/ws.h>

namespace lsp
{
    namespace ws
    {
        INativeWindow::INativeWindow(IDisplay *dpy, IEventHandler *handler)
        {
            pDisplay    = dpy;
            pHandler    = handler;
        }

        INativeWindow::~INativeWindow()
        {
            pHandler    = NULL;
        }

        status_t INativeWindow::init()
        {
            return STATUS_OK;
        }

        void INativeWindow::destroy()
        {
            pDisplay    = NULL;
            pHandler    = NULL;
        }

        ISurface *INativeWindow::get_surface()
        {
            return NULL;
        }

        ssize_t INativeWindow::left()
        {
            realize_t r;
            status_t result = get_geometry(&r);
            return (result == STATUS_OK) ? r.nLeft : -1;
        }

        ssize_t INativeWindow::top()
        {
            realize_t r;
            status_t result = get_geometry(&r);
            return (result == STATUS_OK) ? r.nTop : -1;
        }

        ssize_t INativeWindow::width()
        {
            realize_t r;
            status_t result = get_geometry(&r);
            return (result == STATUS_OK) ? r.nWidth : -1;
        }

        ssize_t INativeWindow::height()
        {
            realize_t r;
            status_t result = get_geometry(&r);
            return (result == STATUS_OK) ? r.nHeight : -1;
        }

        bool INativeWindow::is_visible()
        {
            return false;
        }

        size_t INativeWindow::screen()
        {
            return 0;
        }

        void *INativeWindow::handle()
        {
            return NULL;
        }

        status_t INativeWindow::move(ssize_t left, ssize_t top)
        {
            realize_t r;
            status_t result = get_geometry(&r);
            if (result != STATUS_OK)
                return result;

            r.nLeft         = left;
            r.nTop          = top;
            return set_geometry(&r);
        }

        status_t INativeWindow::resize(ssize_t width, ssize_t height)
        {
            realize_t r;
            status_t result = get_geometry(&r);
            if (result != STATUS_OK)
                return result;

            r.nWidth        = width;
            r.nHeight       = height;
            return set_geometry(&r);
        }

        status_t INativeWindow::set_geometry(ssize_t left, ssize_t top, ssize_t width, ssize_t height)
        {
            realize_t r;

            r.nLeft     = left;
            r.nTop      = top;
            r.nWidth    = width;
            r.nHeight   = height;

            return set_geometry(&r);
        }

        status_t INativeWindow::set_geometry(const realize_t *realize)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t INativeWindow::set_border_style(border_style_t style)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t INativeWindow::get_border_style(border_style_t *style)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t INativeWindow::get_geometry(realize_t *realize)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t INativeWindow::get_absolute_geometry(realize_t *realize)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t INativeWindow::hide()
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t INativeWindow::show()
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t INativeWindow::show(INativeWindow *over)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t INativeWindow::set_left(ssize_t left)
        {
            realize_t r;
            status_t result = get_geometry(&r);
            if (result != STATUS_OK)
                return result;

            r.nLeft         = left;
            return set_geometry(&r);
        }

        status_t INativeWindow::set_top(ssize_t top)
        {
            realize_t r;
            status_t result = get_geometry(&r);
            if (result != STATUS_OK)
                return result;

            r.nTop          = top;
            return set_geometry(&r);
        }

        ssize_t INativeWindow::set_width(ssize_t width)
        {
            realize_t r;
            status_t result = get_geometry(&r);
            if (result != STATUS_OK)
                return result;

            r.nWidth        = width;
            return set_geometry(&r);
        }

        ssize_t INativeWindow::set_height(ssize_t height)
        {
            realize_t r;
            status_t result = get_geometry(&r);
            if (result != STATUS_OK)
                return result;

            r.nHeight       = height;
            return set_geometry(&r);
        }

        status_t INativeWindow::set_visibility(bool visible)
        {
            return (visible) ? show() : hide();
        }

        status_t INativeWindow::set_size_constraints(const size_request_t *c)
        {
            return STATUS_OK;
        }

        status_t INativeWindow::set_size_constraints(ssize_t min_width, ssize_t min_height, ssize_t max_width, ssize_t max_height)
        {
            size_request_t sr;
            sr.nMinWidth        = min_width;
            sr.nMinHeight       = min_height;
            sr.nMaxWidth        = max_width;
            sr.nMaxHeight       = max_height;

            return set_size_constraints(&sr);
        }

        status_t INativeWindow::check_constraints()
        {
            return STATUS_OK;
        }

        status_t INativeWindow::get_size_constraints(size_request_t *c)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t INativeWindow::set_min_width(ssize_t value)
        {
            size_request_t sr;
            status_t result = get_size_constraints(&sr);
            if (result != STATUS_OK)
                return result;
            sr.nMinWidth    = value;
            return set_size_constraints(&sr);
        }

        status_t INativeWindow::set_min_height(ssize_t value)
        {
            size_request_t sr;
            status_t result = get_size_constraints(&sr);
            if (result != STATUS_OK)
                return result;
            sr.nMinHeight   = value;
            return set_size_constraints(&sr);
        }

        status_t INativeWindow::set_max_width(ssize_t value)
        {
            size_request_t sr;
            status_t result = get_size_constraints(&sr);
            if (result != STATUS_OK)
                return result;
            sr.nMaxWidth    = value;
            return set_size_constraints(&sr);
        }

        status_t INativeWindow::set_max_height(ssize_t value)
        {
            size_request_t sr;
            status_t result = get_size_constraints(&sr);
            if (result != STATUS_OK)
                return result;
            sr.nMaxHeight   = value;
            return set_size_constraints(&sr);
        }

        status_t INativeWindow::set_min_size(ssize_t width, ssize_t height)
        {
            size_request_t sr;
            status_t result = get_size_constraints(&sr);
            if (result != STATUS_OK)
                return result;
            sr.nMinWidth    = width;
            sr.nMinHeight   = height;
            return set_size_constraints(&sr);
        }

        status_t INativeWindow::set_max_size(ssize_t width, ssize_t height)
        {
            size_request_t sr;
            status_t result = get_size_constraints(&sr);
            if (result != STATUS_OK)
                return result;
            sr.nMaxWidth    = width;
            sr.nMaxHeight   = height;
            return set_size_constraints(&sr);
        }


        status_t INativeWindow::set_focus(bool focus)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t INativeWindow::toggle_focus()
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t INativeWindow::set_caption(const char *ascii, const char *utf8)
        {
            return STATUS_OK;
        }

        status_t INativeWindow::get_caption(char *text, size_t len)
        {
            if (len < 1)
                return STATUS_TOO_BIG;
            text[0] = '\0';
            return STATUS_OK;
        }

        status_t INativeWindow::set_icon(const void *bgra, size_t width, size_t height)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t INativeWindow::get_window_actions(size_t *actions)
        {
            if (actions != NULL)
                *actions = 0;
            return STATUS_OK;
        }

        status_t INativeWindow::set_window_actions(size_t actions)
        {
            return STATUS_OK;
        }

        status_t INativeWindow::set_mouse_pointer(mouse_pointer_t pointer)
        {
            return STATUS_OK;
        }

        mouse_pointer_t INativeWindow::get_mouse_pointer()
        {
            return MP_DEFAULT;
        }

        status_t INativeWindow::grab_events(grab_t grab)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t INativeWindow::ungrab_events()
        {
            return STATUS_NO_GRAB;
        }

        status_t INativeWindow::set_class(const char *instance, const char *wclass)
        {
            return STATUS_OK;
        }

        status_t INativeWindow::set_role(const char *wrole)
        {
            return STATUS_OK;
        }
    } /* namespace ws */
} /* namespace lsp */
