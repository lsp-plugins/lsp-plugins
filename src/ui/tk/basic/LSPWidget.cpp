/*
 * LSPWidget.cpp
 *
 *  Created on: 15 июн. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPWidget::metadata = { "LSPWidget", NULL };

        LSPWidget::LSPWidget(LSPDisplay *dpy):
            sPadding(this),
            sBgColor(this),
            sBrightness(this)
        {
            pUID            = NULL;
            pDisplay        = dpy;
            pSurface        = NULL;
            pParent         = NULL;
            enCursor        = MP_DEFAULT;
            sSize.nLeft     = 0;
            sSize.nTop      = 0;
            sSize.nWidth    = 0;
            sSize.nHeight   = 0;
            nFlags          = REDRAW_SURFACE | F_VISIBLE | F_HFILL | F_VFILL;
            pClass          = &metadata;
        }

        LSPWidget::~LSPWidget()
        {
            do_destroy();
        }

        bool LSPWidget::instance_of(const w_class_t *wclass) const
        {
            const w_class_t *wc = pClass;
            while (wc != NULL)
            {
                if (wc == wclass)
                    return true;
                wc = wc->parent;
            }

            return false;
        }

        status_t LSPWidget::init()
        {
            // Initialize style
            status_t res = sStyle.init();
            if (res == STATUS_OK)
                res = sStyle.add_parent(pDisplay->theme()->root());
            if (res == STATUS_OK)
                res = sBgColor.bind("bg_color");
            if (res == STATUS_OK)
                res = sBrightness.bind("brightness");

            // Declare slots
            ui_handler_id_t id = 0;

            id = sSlots.add(LSPSLOT_FOCUS_IN, slot_focus_in, self());
            if (id >= 0) id = sSlots.add(LSPSLOT_FOCUS_OUT, slot_focus_out, self());
            if (id >= 0) id = sSlots.add(LSPSLOT_KEY_DOWN, slot_key_down, self());
            if (id >= 0) id = sSlots.add(LSPSLOT_KEY_UP, slot_key_up, self());
            if (id >= 0) id = sSlots.add(LSPSLOT_MOUSE_DOWN, slot_mouse_down, self());
            if (id >= 0) id = sSlots.add(LSPSLOT_MOUSE_UP, slot_mouse_up, self());
            if (id >= 0) id = sSlots.add(LSPSLOT_MOUSE_MOVE, slot_mouse_move, self());
            if (id >= 0) id = sSlots.add(LSPSLOT_MOUSE_SCROLL, slot_mouse_scroll, self());
            if (id >= 0) id = sSlots.add(LSPSLOT_MOUSE_DBL_CLICK, slot_mouse_dbl_click, self());
            if (id >= 0) id = sSlots.add(LSPSLOT_MOUSE_TRI_CLICK, slot_mouse_tri_click, self());
            if (id >= 0) id = sSlots.add(LSPSLOT_MOUSE_IN, slot_mouse_in, self());
            if (id >= 0) id = sSlots.add(LSPSLOT_MOUSE_OUT, slot_mouse_out, self());
            if (id >= 0) id = sSlots.add(LSPSLOT_HIDE, slot_hide, self());
            if (id >= 0) id = sSlots.add(LSPSLOT_SHOW, slot_show, self());
            if (id >= 0) id = sSlots.add(LSPSLOT_DESTROY, slot_destroy, self());
            if (id >= 0) id = sSlots.add(LSPSLOT_RESIZE, slot_resize, self());
            if (id >= 0) id = sSlots.add(LSPSLOT_RESIZE_PARENT, slot_resize_parent, self());
            if (id >= 0) id = sSlots.add(LSPSLOT_DRAG_REQUEST, slot_drag_request, self());

            return (id >= 0) ? STATUS_OK : -id;
        }

        void LSPWidget::do_destroy()
        {
            // Set parent widget to NULL
            set_parent(NULL);

            // Destroy surface
            if (pSurface != NULL)
            {
                pSurface->destroy();
                delete pSurface;
                pSurface = NULL;
            }

            // Execute slots and unbind all to prevent duplicate on_destroy calls
            sSlots.execute(LSPSLOT_DESTROY, this);
            sSlots.destroy();

            // Destroy widget identifier
            if (pUID != NULL)
                ::free(pUID);
            pUID = NULL;
        }

        void LSPWidget::unlink_widget(LSPWidget *w)
        {
            if (w == NULL)
                return;
            if (w->pParent == this)
                w->pParent  = NULL;
        }

        void LSPWidget::init_color(color_t value, LSPColor *color)
        {
            Color c;
            if (pDisplay != NULL)
            {
                LSPTheme *theme = pDisplay->theme();

                if (theme != NULL)
                    theme->get_color(value, c);
            }
            color->copy(&c);
        }

        status_t LSPWidget::slot_mouse_move(LSPWidget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            LSPWidget *_this  = static_cast<LSPWidget *>(ptr);
            ws_event_t *ev  = static_cast<ws_event_t *>(data);
            return _this->on_mouse_move(ev);
        }

        status_t LSPWidget::slot_mouse_down(LSPWidget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            LSPWidget *_this  = static_cast<LSPWidget *>(ptr);
            ws_event_t *ev  = static_cast<ws_event_t *>(data);
            return _this->on_mouse_down(ev);
        }

        status_t LSPWidget::slot_mouse_up(LSPWidget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            LSPWidget *_this  = static_cast<LSPWidget *>(ptr);
            ws_event_t *ev  = static_cast<ws_event_t *>(data);
            return _this->on_mouse_up(ev);
        }

        status_t LSPWidget::slot_mouse_dbl_click(LSPWidget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            LSPWidget *_this  = static_cast<LSPWidget *>(ptr);
            ws_event_t *ev  = static_cast<ws_event_t *>(data);
            return _this->on_mouse_dbl_click(ev);
        }

        status_t LSPWidget::slot_mouse_tri_click(LSPWidget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            LSPWidget *_this  = static_cast<LSPWidget *>(ptr);
            ws_event_t *ev  = static_cast<ws_event_t *>(data);
            return _this->on_mouse_tri_click(ev);
        }

        status_t LSPWidget::slot_mouse_scroll(LSPWidget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            LSPWidget *_this  = static_cast<LSPWidget *>(ptr);
            ws_event_t *ev  = static_cast<ws_event_t *>(data);
            return _this->on_mouse_scroll(ev);
        }

        status_t LSPWidget::slot_mouse_in(LSPWidget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            LSPWidget *_this  = static_cast<LSPWidget *>(ptr);
            ws_event_t *ev  = static_cast<ws_event_t *>(data);
            return _this->on_mouse_in(ev);
        }

        status_t LSPWidget::slot_mouse_out(LSPWidget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            LSPWidget *_this  = static_cast<LSPWidget *>(ptr);
            ws_event_t *ev  = static_cast<ws_event_t *>(data);
            return _this->on_mouse_out(ev);
        }

        status_t LSPWidget::slot_key_down(LSPWidget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            LSPWidget *_this  = static_cast<LSPWidget *>(ptr);
            ws_event_t *ev  = static_cast<ws_event_t *>(data);
            return _this->on_key_down(ev);
        }

        status_t LSPWidget::slot_key_up(LSPWidget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            LSPWidget *_this  = static_cast<LSPWidget *>(ptr);
            ws_event_t *ev  = static_cast<ws_event_t *>(data);
            return _this->on_key_up(ev);
        }

        status_t LSPWidget::slot_hide(LSPWidget *sender, void *ptr, void *data)
        {
            if (ptr == NULL)
                return STATUS_BAD_ARGUMENTS;

            LSPWidget *_this  = static_cast<LSPWidget *>(ptr);
            return _this->on_hide();
        }

        status_t LSPWidget::slot_show(LSPWidget *sender, void *ptr, void *data)
        {
            if (ptr == NULL)
                return STATUS_BAD_ARGUMENTS;

            LSPWidget *_this  = static_cast<LSPWidget *>(ptr);
            return _this->on_show();
        }

        status_t LSPWidget::slot_destroy(LSPWidget *sender, void *ptr, void *data)
        {
            if (ptr == NULL)
                return STATUS_BAD_ARGUMENTS;

            LSPWidget *_this  = static_cast<LSPWidget *>(ptr);
            return _this->on_destroy();
        }

        status_t LSPWidget::slot_resize(LSPWidget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            LSPWidget *_this  = static_cast<LSPWidget *>(ptr);
            realize_t *ev   = static_cast<realize_t *>(data);
            return _this->on_resize(ev);
        }

        status_t LSPWidget::slot_resize_parent(LSPWidget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            LSPWidget *_this  = static_cast<LSPWidget *>(ptr);
            realize_t *ev   = static_cast<realize_t *>(data);
            return _this->on_resize_parent(ev);
        }

        status_t LSPWidget::slot_focus_in(LSPWidget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            LSPWidget *_this  = static_cast<LSPWidget *>(ptr);
            ws_event_t *ev  = static_cast<ws_event_t *>(data);
            return _this->on_focus_in(ev);
        }

        status_t LSPWidget::slot_focus_out(LSPWidget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            LSPWidget *_this  = static_cast<LSPWidget *>(ptr);
            ws_event_t *ev  = static_cast<ws_event_t *>(data);
            return _this->on_focus_out(ev);
        }

        status_t LSPWidget::slot_drag_request(LSPWidget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            LSPWidget *_this  = static_cast<LSPWidget *>(ptr);
            ws_event_t *ev  = static_cast<ws_event_t *>(data);
            const char * const *ctype = _this->pDisplay->get_drag_mime_types();

            return _this->on_drag_request(ev, ctype);
        }

        ssize_t LSPWidget::relative_left() const
        {
            return sSize.nLeft - ((pParent != NULL) ? pParent->left() : 0);
        }

        ssize_t LSPWidget::relative_right() const
        {
            return sSize.nLeft - ((pParent != NULL) ? pParent->left() : 0) + sSize.nWidth;
        }

        ssize_t LSPWidget::relative_top() const
        {
            return sSize.nTop - ((pParent != NULL) ? pParent->top() : 0);
        }

        ssize_t LSPWidget::relative_bottom() const
        {
            return sSize.nTop - ((pParent != NULL) ? pParent->top() : 0) + sSize.nHeight;
        }

        bool LSPWidget::inside(ssize_t x, ssize_t y)
        {
            if (!(nFlags & F_VISIBLE))
                return false;
            else if (x < sSize.nLeft)
                return false;
            else if (x >= (sSize.nLeft + sSize.nWidth))
                return false;
            else if (y < sSize.nTop)
                return false;
            else if (y >= (sSize.nTop + sSize.nHeight))
                return false;

            return true;
        }

        mouse_pointer_t LSPWidget::active_cursor() const
        {
            return enCursor;
        }

        bool LSPWidget::hide()
        {
            if (!(nFlags & F_VISIBLE))
                return false;
            nFlags &= ~F_VISIBLE;
//            lsp_trace("class = %s, this=%p", get_class()->name, this);

            // Drop surface to not to eat memory
            if (pSurface != NULL)
            {
                pSurface->destroy();
                delete pSurface;
                pSurface = NULL;
            }

            // Execute slot
            sSlots.execute(LSPSLOT_HIDE, this);

            // Query draw for parent widget
            if (pParent != NULL)
                pParent->query_resize();

            return true;
        }

        bool LSPWidget::show()
        {
            if (nFlags & F_VISIBLE)
                return false;
//            lsp_trace("class = %s, this=%p", get_class()->name, this);

            nFlags |= F_VISIBLE;
            if (pParent != NULL)
                pParent->query_resize();
            query_draw(REDRAW_CHILD | REDRAW_SURFACE);
            sSlots.execute(LSPSLOT_SHOW, this);

            return true;
        }

        void LSPWidget::set_parent(LSPComplexWidget *parent)
        {
            if (pParent == parent)
                return;

            if (pParent != NULL)
            {
                LSPWindow *wnd = widget_cast<LSPWindow>(toplevel());
                if (wnd != NULL)
                    wnd->unfocus_child(this);
                sStyle.remove_parent(pParent->style()); // Unlink style

                LSPWidgetContainer *wc = widget_cast<LSPWidgetContainer>(pParent);
                if (wc != NULL)
                    wc->remove(this);
            }

            pParent = parent;
            if (parent != NULL) // Inherit the style of parent widget
                sStyle.add_parent(parent->style());
        }

        LSPWidget *LSPWidget::toplevel()
        {
            LSPWidget *p = this;
            while (p->pParent != NULL)
                p = p->pParent;

            return p;
        }

        void LSPWidget::query_draw(size_t flags)
        {
            if (!(nFlags & F_VISIBLE))
                return;
            nFlags     |= (flags & (REDRAW_CHILD | REDRAW_SURFACE));
            if (pParent != NULL)
                pParent->query_draw(REDRAW_CHILD);
        }

        void LSPWidget::commit_redraw()
        {
            nFlags &= ~(REDRAW_SURFACE | REDRAW_CHILD);
        }

        status_t LSPWidget::queue_destroy()
        {
            if (pDisplay == NULL)
                return STATUS_BAD_STATE;
            return pDisplay->queue_destroy(this);
        }

        void LSPWidget::query_resize()
        {
            LSPWidget *w = toplevel();
            if ((w != NULL) && (w != this))
                w->query_resize();
        }

        void LSPWidget::set_expand(bool value)
        {
            size_t flags = nFlags;
            if (value)
                nFlags  |= F_EXPAND;
            else
                nFlags  &= ~F_EXPAND;

            if (flags != nFlags)
                query_resize();
        }

        void LSPWidget::set_fill(bool value)
        {
            size_t flags = nFlags;
            if (value)
                nFlags  |= F_HFILL | F_VFILL;
            else
                nFlags  &= ~(F_HFILL | F_VFILL);

            if (flags != nFlags)
                query_resize();
        }

        void LSPWidget::set_hfill(bool value)
        {
            size_t flags = nFlags;
            if (value)
                nFlags  |= F_HFILL;
            else
                nFlags  &= ~F_HFILL;

            if (flags != nFlags)
                query_resize();
        }

        void LSPWidget::set_vfill(bool value)
        {
            size_t flags = nFlags;
            if (value)
                nFlags  |= F_VFILL;
            else
                nFlags  &= ~F_VFILL;

            if (flags != nFlags)
                query_resize();
        }

        void LSPWidget::set_visible(bool visible)
        {
            if (visible)
                show();
            else
                hide();
        }

        /** Set mouse pointer
         *
         * @param mp mouse pointer
         * @return mouse pointer
         */
        status_t LSPWidget::set_cursor(mouse_pointer_t mp)
        {
            enCursor       = mp;
            return STATUS_OK;
        }

        void LSPWidget::render(ISurface *s, bool force)
        {
            // Get surface of widget
            ISurface *src  = get_surface(s);
            if (src == NULL)
                return;

            // Render to the main surface
            s->draw(src, sSize.nLeft, sSize.nTop);
        }

        status_t LSPWidget::set_unique_id(const char *uid)
        {
            char *rep = NULL;
            if (uid != NULL)
            {
                if ((rep = strdup(uid)) == NULL)
                    return STATUS_NO_MEM;
            }

            if (pUID != NULL)
                free(pUID);
            pUID = rep;
            return STATUS_OK;
        }

        ISurface *LSPWidget::get_surface(ISurface *s)
        {
            return get_surface(s, sSize.nWidth, sSize.nHeight);
        }

        ISurface *LSPWidget::get_surface(ISurface *s, ssize_t width, ssize_t height)
        {
            // Check surface
            if (pSurface != NULL)
            {
                if ((width != ssize_t(pSurface->width())) || (height != ssize_t(pSurface->height())))
                {
                    pSurface->destroy();
                    delete pSurface;
                    pSurface    = NULL;
                }
            }

            // Create new surface if needed
            if (pSurface == NULL)
            {
                if (s == NULL)
                    return NULL;

                // Do not return surface if size is negative
                if ((width <= 0) || (height <= 0))
                    return NULL;

                pSurface        = s->create(width, height);
                if (pSurface == NULL)
                    return NULL;
                nFlags         |= REDRAW_SURFACE;
            }

            // Redraw surface if required
            if (nFlags & REDRAW_SURFACE)
            {
                draw(pSurface);
                nFlags         &= ~REDRAW_SURFACE;
            }

            return pSurface;
        }

        void LSPWidget::draw(ISurface *s)
        {
        }

        void LSPWidget::realize(const realize_t *r)
        {
            // Do not report size request on size change
            if ((sSize.nLeft == r->nLeft) &&
                (sSize.nTop  == r->nTop) &&
                (sSize.nWidth == r->nWidth) &&
                (sSize.nHeight == r->nHeight))
                return;

            // Update size and execute slot
            sSize       = *r;
            sSlots.execute(LSPSLOT_RESIZE, this, &sSize);
        }

        void LSPWidget::size_request(size_request_t *r)
        {
            r->nMinWidth    = -1;
            r->nMinHeight   = -1;
            r->nMaxWidth    = -1;
            r->nMaxHeight   = -1;
        }

        bool LSPWidget::has_focus() const
        {
            if (!(nFlags & F_VISIBLE))
                return false;

            LSPWidget *_this = const_cast<LSPWidget *>(this);
            LSPWindow *wnd = widget_cast<LSPWindow>(_this->toplevel());
            return (wnd != NULL) ? (wnd->focused_child() == this) : false;
        }

        status_t LSPWidget::set_focus(bool focus)
        {
            if (!(nFlags & F_VISIBLE))
                return STATUS_OK;

            LSPWindow *wnd = widget_cast<LSPWindow>(toplevel());
            if (wnd == NULL)
                return STATUS_BAD_HIERARCHY;
            return (focus) ? wnd->focus_child(this) : wnd->unfocus_child(this);
        }

        status_t LSPWidget::toggle_focus()
        {
            if (!(nFlags & F_VISIBLE))
                return STATUS_OK;

            LSPWindow *wnd = widget_cast<LSPWindow>(toplevel());
            return (wnd != NULL) ? wnd->toggle_child_focus(this) : STATUS_BAD_HIERARCHY;
        }

        status_t LSPWidget::mark_pointed()
        {
            LSPWindow *wnd = widget_cast<LSPWindow>(toplevel());
            if (wnd == NULL)
                return STATUS_SUCCESS;
            return wnd->point_child(this);
        }

        status_t LSPWidget::handle_event(const ws_event_t *e)
        {
            #define FWD_EVENT(ev, slot_id) \
                case ev: \
                { \
                    ws_event_t tmp = *e; \
                    sSlots.execute(slot_id, this, &tmp); \
                    break; \
                }

            switch (e->nType)
            {
                FWD_EVENT(UIE_KEY_DOWN, LSPSLOT_KEY_DOWN )
                FWD_EVENT(UIE_KEY_UP, LSPSLOT_KEY_UP )
                FWD_EVENT(UIE_MOUSE_DOWN, LSPSLOT_MOUSE_DOWN )
                FWD_EVENT(UIE_MOUSE_UP, LSPSLOT_MOUSE_UP )
                FWD_EVENT(UIE_MOUSE_IN, LSPSLOT_MOUSE_IN )
                FWD_EVENT(UIE_MOUSE_OUT, LSPSLOT_MOUSE_OUT )
                FWD_EVENT(UIE_MOUSE_MOVE, LSPSLOT_MOUSE_MOVE )
                FWD_EVENT(UIE_MOUSE_SCROLL, LSPSLOT_MOUSE_SCROLL )
                FWD_EVENT(UIE_MOUSE_DBL_CLICK, LSPSLOT_MOUSE_DBL_CLICK )
                FWD_EVENT(UIE_MOUSE_TRI_CLICK, LSPSLOT_MOUSE_TRI_CLICK )
                FWD_EVENT(UIE_FOCUS_IN, LSPSLOT_FOCUS_IN )
                FWD_EVENT(UIE_FOCUS_OUT, LSPSLOT_FOCUS_OUT )
                FWD_EVENT(UIE_DRAG_REQUEST, LSPSLOT_DRAG_REQUEST )

                default:
                    break;
            }
            #undef FWD_EVENT

            return STATUS_OK;
        }

        void LSPWidget::destroy()
        {
            do_destroy();
        }

        status_t LSPWidget::on_key_down(const ws_event_t *e)
        {
            return STATUS_OK;
        }

        status_t LSPWidget::on_key_up(const ws_event_t *e)
        {
            return STATUS_OK;
        }

        status_t LSPWidget::on_mouse_down(const ws_event_t *e)
        {
            return STATUS_OK;
        }

        status_t LSPWidget::on_mouse_up(const ws_event_t *e)
        {
            return STATUS_OK;
        }

        status_t LSPWidget::on_mouse_move(const ws_event_t *e)
        {
            return STATUS_OK;
        }

        status_t LSPWidget::on_mouse_in(const ws_event_t *e)
        {
            // Always mark widget pointed
            return mark_pointed();
        }

        status_t LSPWidget::on_mouse_out(const ws_event_t *e)
        {
            return STATUS_OK;
        }

        status_t LSPWidget::on_mouse_scroll(const ws_event_t *e)
        {
            return STATUS_OK;
        }

        status_t LSPWidget::on_mouse_dbl_click(const ws_event_t *e)
        {
            return STATUS_OK;
        }

        status_t LSPWidget::on_mouse_tri_click(const ws_event_t *e)
        {
            return STATUS_OK;
        }

        status_t LSPWidget::on_resize(const realize_t *r)
        {
            return STATUS_OK;
        }

        status_t LSPWidget::on_resize_parent(const realize_t *r)
        {
            return STATUS_OK;
        }

        status_t LSPWidget::on_hide()
        {
            return STATUS_OK;
        }

        status_t LSPWidget::on_show()
        {
            return STATUS_OK;
        }

        status_t LSPWidget::on_destroy()
        {
            return STATUS_OK;
        }

        status_t LSPWidget::on_focus_in(const ws_event_t *e)
        {
            return STATUS_OK;
        }

        status_t LSPWidget::on_focus_out(const ws_event_t *e)
        {
            return STATUS_OK;
        }

        status_t LSPWidget::on_drag_request(const ws_event_t *e, const char * const *ctype)
        {
            return STATUS_OK;
        }
    }

} /* namespace lsp */
