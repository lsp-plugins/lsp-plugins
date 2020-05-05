/*
 * LSPWindow.cpp
 *
 *  Created on: 16 июн. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPWindow::metadata = { "LSPWindow", &LSPWidgetContainer::metadata };

        void LSPWindow::Title::sync()
        {
            LSPWindow *window = widget_cast<LSPWindow>(pWidget);
            if ((window == NULL) || (window->pWindow == NULL))
                return;

            LSPString text;
            status_t res = window->sTitle.format(&text);
            if (res != STATUS_OK)
                return;

            char *ascii = text.clone_ascii();
            const char *caption = text.get_utf8();
            if (caption == NULL)
                caption = "";

            window->pWindow->set_caption((ascii != NULL) ? ascii : "", caption);
            if (ascii != NULL)
                ::free(ascii);
        }

        LSPWindow::LSPWindow(LSPDisplay *dpy, void *handle, ssize_t screen):
            LSPWidgetContainer(dpy),
            sActions(this),
            sBorder(this),
            sTitle(this)
        {
            lsp_trace("native_handle = %p", handle);

            pWindow         = NULL;
            pChild          = NULL;
            pNativeHandle   = handle;
            enStyle         = BS_SIZABLE;
            nScreen         = screen;
            pFocus          = NULL;
            pPointed        = NULL;
            bHasFocus       = false;
            bOverridePointer= false;
            bMapFlag        = false;
            bSizeRequest    = true;
            nVertPos        = 0.5f;
            nHorPos         = 0.5f;
            nVertScale      = 0.0f;
            nHorScale       = 0.0f;
            nBorder         = 0;

            sSize.nLeft     = -1;
            sSize.nTop      = -1;
            sSize.nWidth    = -1;
            sSize.nHeight   = -1;

            sConstraints.nMinWidth  = -1;
            sConstraints.nMinHeight = -1;
            sConstraints.nMaxHeight = -1;
            sConstraints.nMaxWidth  = -1;
            enPolicy        = WP_NORMAL;

            nFlags         &= ~F_VISIBLE;
            pClass          = &metadata;
        }

        LSPWindow::~LSPWindow()
        {
            do_destroy();
        }

        status_t LSPWindow::init()
        {
            // Initialize parent class
            status_t result = LSPWidgetContainer::init();
            if (result < 0)
                return result;

            // Init color
            init_color(C_LABEL_TEXT, &sBorder);

            // Add slot(s)
            ui_handler_id_t id = 0;
            id = sSlots.add(LSPSLOT_CLOSE, slot_window_close, self());
            if (id < 0)
                return - id;

            // Initialize redraw timer
            IDisplay *dpy   = pDisplay->display();
            if (dpy == NULL)
                return STATUS_BAD_STATE;

            sTitle.bind();
            sRedraw.bind(dpy);
            sRedraw.set_handler(tmr_redraw_request, self());

            // Create and initialize window
            pWindow     = (pNativeHandle != NULL) ? dpy->create_window(pNativeHandle) :
                          (nScreen >= 0) ? dpy->create_window(nScreen) :
                          dpy->create_window();
            if (pWindow == NULL)
                return STATUS_UNKNOWN_ERR;
            pWindow->set_handler(this);

            // Initialize
            result = pWindow->init();
            if (result != STATUS_SUCCESS)
            {
                destroy();
                return result;
            }

            // Initialize window geometry
            lsp_trace("Initializing window geometry, window id=%p", pWindow->handle());

            realize_t r;
            result = pWindow->set_border_style(enStyle);
            if (result != STATUS_SUCCESS)
            {
                destroy();
                return result;
            }

            result = pWindow->set_size_constraints(&sConstraints);
            if (result != STATUS_SUCCESS)
            {
                destroy();
                return result;
            }

            result = pWindow->get_geometry(&r);
            if (result != STATUS_SUCCESS)
            {
                destroy();
                return result;
            }

            result = sActions.init();
            if (result != STATUS_SUCCESS)
            {
                destroy();
                return result;
            }

            if (sSize.nLeft < 0)
                sSize.nLeft     = r.nLeft;
            if (sSize.nTop < 0)
                sSize.nTop      = r.nTop;
            if (sSize.nWidth < 0)
                sSize.nWidth    = r.nWidth;
            if (sSize.nHeight < 0)
                sSize.nHeight   = r.nHeight;

            lsp_trace("Window has been initialized");

            return STATUS_OK;
        }

        status_t LSPWindow::sync_size()
        {
            // Request size
            size_request_t sr;
            sr.nMinWidth    = -1;
            sr.nMinHeight   = -1;
            sr.nMaxWidth    = -1;
            sr.nMaxHeight   = -1;
            size_request(&sr);

            // Set window's size constraints and update geometry
            pWindow->set_size_constraints(&sr);
            realize_t r = sSize;
            if (enPolicy == WP_GREEDY)
            {
                if (sr.nMinWidth > 0)
                    r.nWidth        = sr.nMinWidth;
                if (sr.nMinHeight > 0)
                    r.nHeight       = sr.nMinHeight;
            }
            else
            {
                // Check whether window matches constraints
                if ((sr.nMaxWidth > 0) && (r.nWidth > sr.nMaxWidth))
                    r.nWidth        = sr.nMaxWidth;
                if ((sr.nMaxHeight > 0) && (r.nHeight > sr.nMaxHeight))
                    r.nHeight       = sr.nMaxHeight;

                if ((sr.nMinWidth > 0) && (r.nWidth < sr.nMinWidth))
                    r.nWidth        = sr.nMinWidth;
                if ((sr.nMinHeight > 0) && (r.nHeight < sr.nMinHeight))
                    r.nHeight       = sr.nMinHeight;
            }

            if ((sSize.nWidth != r.nWidth) && (sSize.nHeight != r.nHeight))
                pWindow->resize(r.nWidth, r.nHeight);

            return STATUS_OK;
        }

        void LSPWindow::do_destroy()
        {
            if (pChild != NULL)
            {
                unlink_widget(pChild);
                pChild = NULL;
            }

            if (pWindow != NULL)
            {
                pWindow->destroy();
                delete pWindow;
                pWindow = NULL;
            }
        }

        void LSPWindow::destroy()
        {
            do_destroy();

            LSPWidgetContainer::destroy();
        }

        status_t LSPWindow::tmr_redraw_request(timestamp_t ts, void *args)
        {
            if (args == NULL)
                return STATUS_BAD_ARGUMENTS;

            LSPWidget *widget = static_cast<LSPWidget *>(args);

            LSPWindow *_this   = static_cast<LSPWindow *>(widget);

            return (_this != NULL) ? _this->do_render() : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPWindow::slot_window_close(LSPWidget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            LSPWindow *_this   = widget_ptrcast<LSPWindow>(ptr);
            return (_this != NULL) ? _this->on_close(static_cast<ws_event_t *>(data)) : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPWindow::do_render()
        {
            if (pWindow == NULL)
                return STATUS_OK;

            if (bSizeRequest)
            {
                lsp_trace("Synchronizing size");
                sync_size();
                bSizeRequest    = false;
                query_draw(REDRAW_CHILD | REDRAW_SURFACE);
                realize(&sSize);
            }

            if (!redraw_pending())
                return STATUS_OK;

            // call rendering
            ISurface *s = pWindow->get_surface();
            if (s == NULL)
                return STATUS_OK;

            s->begin();
                render(s, nFlags & REDRAW_SURFACE);
                commit_redraw();
            s->end();

            // And also update pointer
            update_pointer();

            return STATUS_OK;
        }

        void LSPWindow::query_resize()
        {
            bSizeRequest = true;
        }

        status_t LSPWindow::get_absolute_geometry(realize_t *realize)
        {
            if (pWindow == NULL)
                return STATUS_BAD_STATE;
            return pWindow->get_absolute_geometry(realize);
        }

        void LSPWindow::render(ISurface *s, bool force)
        {
            Color bg_color(sBgColor);

            if (pChild == NULL)
            {
                s->clear(bg_color);
                return;
            }

            if ((force) || (pChild->redraw_pending()))
            {
                pChild->render(s, force);
                pChild->commit_redraw();
            }

            if (force)
            {
                s->fill_frame(
                    0, 0, sSize.nWidth, sSize.nHeight,
                    pChild->left(), pChild->top(), pChild->width(), pChild->height(),
                    bg_color);

                if (nBorder > 0)
                {
                    bool aa = s->set_antialiasing(true);
                    ssize_t bw = nBorder >> 1;

                    Color border(sBorder);
                    border.scale_lightness(brightness());

                    s->wire_round_rect(
                        bw + 0.5, bw + 0.5, sSize.nWidth - nBorder-1, sSize.nHeight - nBorder-1,
                        2, SURFMASK_ALL_CORNER, nBorder,
                        border
                    );
                    s->set_antialiasing(aa);
                }
            }
        }

        status_t LSPWindow::set_cursor(mouse_pointer_t mp)
        {
            LSPWidgetContainer::set_cursor(mp);
            return update_pointer();
        }

        status_t LSPWindow::override_pointer(bool override)
        {
            if (bOverridePointer == override)
                return STATUS_OK;
            bOverridePointer = override;
            return update_pointer();
        }

        status_t LSPWindow::update_pointer()
        {
            if (pWindow == NULL)
                return STATUS_OK;

            mouse_pointer_t mp  = enCursor;
            if ((!bOverridePointer) && (pPointed != NULL))
                mp      = pPointed->active_cursor();

            if (mp == pWindow->get_mouse_pointer())
                return STATUS_OK;

            return pWindow->set_mouse_pointer(mp);
        }

        status_t LSPWindow::point_child(LSPWidget *focus)
        {
            if (pPointed == focus)
                return STATUS_OK;
            pPointed    = (focus != this) ? focus : this;
            return update_pointer();
        }

        void LSPWindow::set_border(size_t border)
        {
            if (nBorder == border)
                return;
            nBorder     = border;
            query_resize();
        }

        status_t LSPWindow::grab_events(grab_t grab)
        {
            return (pWindow != NULL) ? pWindow->grab_events(grab) : STATUS_BAD_STATE;
        }

        status_t LSPWindow::ungrab_events()
        {
            return (pWindow != NULL) ? pWindow->ungrab_events() : STATUS_BAD_STATE;
        }

        void LSPWindow::set_policy(window_poilicy_t policy)
        {
            window_poilicy_t old = enPolicy;
            enPolicy = policy;
            if ((old == policy) || (!(nFlags & F_VISIBLE)))
                return;

            query_resize();
        }

        bool LSPWindow::hide()
        {
            sRedraw.cancel();
            if (pWindow != NULL)
                pWindow->hide();

            if (!(nFlags & F_VISIBLE))
                return false;
            nFlags &= ~F_VISIBLE;

            // Drop surface to not to eat memory
            if (pSurface != NULL)
            {
                pSurface->destroy();
                delete pSurface;
                pSurface = NULL;
            }

            // Query draw for parent widget
            if (pParent != NULL)
                pParent->query_resize();

            return true;
        }

        bool LSPWindow::show()
        {
            return show(NULL);
        }

        bool LSPWindow::show(LSPWidget *actor)
        {
            if (nFlags & F_VISIBLE)
                return false;

            nFlags |= F_VISIBLE;
            if (pParent != NULL)
                pParent->query_resize();

            if (pWindow == NULL)
            {
                sSlots.execute(LSPSLOT_SHOW, this);
                return true;
            }

            // Evaluate layering
            LSPWindow *wnd = (actor != NULL) ? widget_cast<LSPWindow>(actor->toplevel()) : NULL;

            // Update window parameters
            sync_size();
            update_pointer();

            // Launch redraw timer
            sRedraw.launch(-1, 40);
            query_draw();

            // Show window
            if (wnd != NULL)
            {
                // Correct window location
                switch (enStyle)
                {
                    case BS_DIALOG:
                    {
                        realize_t r, rw;
                        r.nLeft     = 0;
                        r.nTop      = 0;
                        r.nWidth    = 0;
                        r.nHeight   = 0;

                        rw.nLeft    = 0;
                        rw.nTop     = 0;
                        rw.nWidth   = 0;
                        rw.nHeight  = 0;

                        wnd->get_geometry(&r);
                        pWindow->get_geometry(&rw);

                        sSize.nLeft = r.nLeft + ((r.nWidth - rw.nWidth) >> 1);
                        sSize.nTop  = r.nTop  + ((r.nHeight - rw.nHeight) >> 1);
                        pWindow->move(sSize.nLeft, sSize.nTop);
                        break;
                    }
                    default:
                        break;
                }

                pWindow->show(wnd->pWindow);
            }
            else
                pWindow->show();

            return true;
        }

        status_t LSPWindow::add(LSPWidget *widget)
        {
            if (pChild != NULL)
                return STATUS_ALREADY_EXISTS;

            widget->set_parent(this);
            pChild = widget;

            query_resize();

            return STATUS_OK;
        }

        status_t LSPWindow::remove(LSPWidget *widget)
        {
            if (pChild != widget)
                return STATUS_NOT_FOUND;

            unlink_widget(pChild);
            pChild  = NULL;

            return STATUS_OK;
        }

        status_t LSPWindow::set_border_style(border_style_t style)
        {
            if (pWindow != NULL)
            {
                status_t result = pWindow->set_border_style(style);
                if (result != STATUS_OK)
                    return result;

                return pWindow->get_border_style(&enStyle);
            }
            else
                enStyle = style;

            return STATUS_OK;
        }

        status_t LSPWindow::handle_event(const ws_event_t *e)
        {
            status_t result = STATUS_OK;
            ws_event_t ev = *e;

            switch (e->nType)
            {
                case UIE_FOCUS_IN:
                    result = sSlots.execute(LSPSLOT_FOCUS_IN, this, &ev);
                    break;

                case UIE_FOCUS_OUT:
                    result = sSlots.execute(LSPSLOT_FOCUS_OUT, this, &ev);
                    break;

                case UIE_SHOW:
                    sRedraw.launch(-1, 40);
                    query_draw();
                    if (bMapFlag != bool(nFlags & F_VISIBLE))
                    {
                        lsp_trace("SHOW ptr=%p", this);
                        result      = sSlots.execute(LSPSLOT_SHOW, this, &ev);
                        bMapFlag    = nFlags & F_VISIBLE;
                    }
                    break;

                case UIE_HIDE:
                    sRedraw.cancel();
                    if (bMapFlag != bool(nFlags & F_VISIBLE))
                    {
                        lsp_trace("HIDE ptr=%p", this);
                        result      = sSlots.execute(LSPSLOT_HIDE, this, &ev);
                        bMapFlag    = nFlags & F_VISIBLE;
                    }
                    break;

                case UIE_REDRAW:
                    query_draw(REDRAW_SURFACE);
                    break;

                case UIE_CLOSE:
                    result = sSlots.execute(LSPSLOT_CLOSE, this, &ev);
                    break;

                case UIE_KEY_DOWN:
                case UIE_KEY_UP:
                    lsp_trace("key event received, focus = %p", pFocus);
                    result = (pFocus != NULL) ?
                        pFocus->handle_event(e) :
                        LSPWidget::handle_event(e);
                    break;

                case UIE_RESIZE:
                {
                    realize_t r;
//                    result = pWindow->get_geometry(&r);
                    lsp_trace("resize to: %d, %d, %d, %d", int(e->nLeft), int(e->nTop), int(e->nWidth), int(e->nHeight));
                    r.nLeft     = e->nLeft;
                    r.nTop      = e->nTop;
                    r.nWidth    = e->nWidth;
                    r.nHeight   = e->nHeight;
                    if (result == STATUS_OK)
                        this->realize(&r);
                    break;
                }

                default:
                    result      = LSPWidgetContainer::handle_event(e);
                    break;
            }

            // Update pointer
            update_pointer();

            return result;
        }

        LSPWidget *LSPWindow::find_widget(ssize_t x, ssize_t y)
        {
            if (pChild == NULL)
                return NULL;

            if ((x < pChild->left()) || (x >= pChild->right()))
                return NULL;

            if ((y < pChild->top()) || (y >= pChild->bottom()))
                return NULL;

            return pChild;
        }

        status_t LSPWindow::on_close(const ws_event_t *e)
        {
            return STATUS_OK;
        }

        status_t LSPWindow::on_focus_in(const ws_event_t *e)
        {
            bHasFocus = true;
            return LSPWidgetContainer::on_focus_in(e);
        }

        status_t LSPWindow::on_focus_out(const ws_event_t *e)
        {
            bHasFocus = false;
            return LSPWidgetContainer::on_focus_out(e);
        }

        status_t LSPWindow::set_width(ssize_t width)
        {
            if (pWindow != NULL)
            {
                status_t r = pWindow->set_width(width);
                if (r != STATUS_OK)
                    return r;

                sSize.nWidth    = pWindow->width();
            }
            else
                sSize.nWidth    = width;

            return STATUS_OK;
        }

        status_t LSPWindow::set_height(ssize_t height)
        {
            if (pWindow != NULL)
            {
                status_t r = pWindow->set_height(height);
                if (r != STATUS_OK)
                    return r;

                sSize.nHeight   = pWindow->width();
            }
            else
                sSize.nHeight   = height;

            return STATUS_OK;
        }

        status_t LSPWindow::resize(ssize_t width, ssize_t height)
        {
            lsp_trace("Resize: width=%d, height=%d", int(width), int(height));

            if (pWindow != NULL)
            {
                status_t r = pWindow->resize(width, height);
                if (r != STATUS_OK)
                    return r;

                return pWindow->get_geometry(&sSize);
            }

            sSize.nWidth        = width;
            sSize.nHeight       = height;
            return STATUS_OK;
        }

        status_t LSPWindow::set_left(ssize_t left)
        {
            if (pWindow != NULL)
            {
                status_t r = pWindow->set_left(left);
                if (r != STATUS_OK)
                    return r;

                sSize.nLeft     = pWindow->left();
            }
            else
                sSize.nLeft     = left;
            return STATUS_OK;
        }

        status_t LSPWindow::set_top(ssize_t top)
        {
            if (pWindow != NULL)
            {
                status_t r = pWindow->set_top(top);
                if (r != STATUS_OK)
                    return r;
                sSize.nTop      = pWindow->top();
            }
            else
                sSize.nTop      = top;
            return STATUS_OK;
        }

        status_t LSPWindow::move(ssize_t left, ssize_t top)
        {
            if (pWindow != NULL)
            {
                status_t r = pWindow->move(left, top);
                if (r != STATUS_OK)
                    return r;
                return pWindow->get_geometry(&sSize);
            }

            sSize.nLeft     = left;
            sSize.nTop      = top;
            return STATUS_OK;
        }

        status_t LSPWindow::set_geometry(ssize_t left, ssize_t top, ssize_t width, ssize_t height)
        {
            if (pWindow != NULL)
            {
                status_t r = pWindow->set_geometry(left, top, width, height);
                if (r != STATUS_OK)
                    return r;
            }

            sSize.nLeft     = left;
            sSize.nTop      = top;
            sSize.nWidth    = width;
            sSize.nHeight   = height;

            return STATUS_OK;
        }

        status_t LSPWindow::set_geometry(const realize_t *geometry)
        {
            lsp_trace("set_geometry: x=%d, y=%d, w=%d, h=%d",
                int(geometry->nLeft), int(geometry->nTop), int(geometry->nWidth), int(geometry->nHeight)
            );
            if (pWindow != NULL)
            {
                status_t r = pWindow->set_geometry(geometry);
                if (r != STATUS_OK)
                    return r;
            }
            sSize = *geometry;

            return STATUS_OK;
        }

        status_t LSPWindow::get_geometry(realize_t *geometry)
        {
            if (pWindow != NULL)
            {
                status_t r =  pWindow->get_geometry(&sSize);
                if (r != STATUS_OK)
                    return r;
            }
            *geometry = sSize;
            return STATUS_OK;
        }

        status_t LSPWindow::set_min_width(ssize_t width)
        {
            return set_min_size(width, sConstraints.nMinHeight);
        }

        status_t LSPWindow::set_min_height(ssize_t height)
        {
            return set_min_size(sConstraints.nMinWidth, height);
        }

        status_t LSPWindow::set_min_size(ssize_t width, ssize_t height)
        {
            sConstraints.nMinWidth  = ((width >= 0) && (width < ssize_t(sPadding.horizontal()))) ? sPadding.horizontal() : width;
            sConstraints.nMinHeight = ((height >= 0) && (height < ssize_t(sPadding.vertical()))) ? sPadding.vertical() : height;

            if (pWindow == NULL)
                return STATUS_OK;

            size_request_t sr;

            pWindow->get_size_constraints(&sr);
            if (sr.nMinWidth < width)
                sr.nMinWidth = width;
            if (sr.nMinHeight < height)
                sr.nMinHeight = height;

            return pWindow->set_size_constraints(&sr);
        }

        status_t LSPWindow::set_max_width(ssize_t width)
        {
            return set_max_size(width, sConstraints.nMaxHeight);
        }

        status_t LSPWindow::set_max_height(ssize_t height)
        {
            return set_max_size(sConstraints.nMaxWidth, height);
        }

        status_t LSPWindow::set_max_size(ssize_t width, ssize_t height)
        {
            sConstraints.nMaxWidth  = ((width >= 0) && (width < ssize_t(sPadding.horizontal()))) ? sPadding.horizontal() : width;
            sConstraints.nMaxHeight = ((height >= 0) && (height < ssize_t(sPadding.vertical()))) ? sPadding.vertical() : height;

            if (pWindow == NULL)
                return STATUS_OK;

            size_request_t sr;

            pWindow->get_size_constraints(&sr);
            if (sr.nMaxWidth < width)
                sr.nMaxWidth = width;
            if (sr.nMaxHeight < height)
                sr.nMaxHeight = height;

            return pWindow->set_size_constraints(&sr);
        }

        status_t LSPWindow::set_size_constraints(const size_request_t *c)
        {
            sConstraints.nMinWidth  = ((c->nMinWidth >= 0) && (c->nMinWidth < ssize_t(sPadding.horizontal()))) ? sPadding.horizontal() : c->nMinWidth;
            sConstraints.nMinHeight = ((c->nMinHeight >= 0) && (c->nMinHeight < ssize_t(sPadding.vertical()))) ? sPadding.vertical() : c->nMinHeight;
            sConstraints.nMaxWidth  = ((c->nMaxWidth >= 0) && (c->nMaxWidth < ssize_t(sPadding.horizontal()))) ? sPadding.horizontal() : c->nMaxWidth;
            sConstraints.nMaxHeight = ((c->nMaxHeight >= 0) && (c->nMaxHeight < ssize_t(sPadding.vertical()))) ? sPadding.vertical() : c->nMaxHeight;

            if (pWindow == NULL)
                return STATUS_OK;

            size_request_t sr;
            pWindow->get_size_constraints(&sr);

            if (sr.nMinWidth < c->nMinWidth)
                sr.nMinWidth = c->nMinWidth;
            if (sr.nMinHeight < c->nMinHeight)
                sr.nMinHeight = c->nMinHeight;
            if (sr.nMaxWidth < c->nMaxWidth)
                sr.nMaxWidth = c->nMaxWidth;
            if (sr.nMaxHeight < c->nMaxHeight)
                sr.nMaxHeight = c->nMaxHeight;

            return pWindow->set_size_constraints(&sr);
        }

        status_t LSPWindow::get_size_constraints(size_request_t *c)
        {
            if (pWindow != NULL)
            {
                status_t r = pWindow->get_size_constraints(&sConstraints);
                if (r != STATUS_OK)
                    return r;
            }

            *c      = sConstraints;
            return STATUS_OK;
        }

        status_t LSPWindow::set_size_constraints(ssize_t min_width, ssize_t min_height, ssize_t max_width, ssize_t max_height)
        {
            sConstraints.nMinWidth      = min_width;
            sConstraints.nMinHeight     = min_height;
            sConstraints.nMaxWidth      = max_width;
            sConstraints.nMaxHeight     = max_height;

            return set_size_constraints(&sConstraints);
        }

        status_t LSPWindow::focus_child(LSPWidget *focus)
        {
            if (pFocus == focus)
                return STATUS_OK;
            else if ((focus != NULL) && (focus->toplevel() != this))
                return STATUS_BAD_HIERARCHY;

            ws_event_t ev;
            ev.nLeft        = 0;
            ev.nTop         = 0;
            ev.nWidth       = 0;
            ev.nHeight      = 0;
            ev.nCode        = 0;
            ev.nState       = 0;
            ev.nTime        = 0;

            if (pFocus != NULL)
            {
                ev.nType        = UIE_FOCUS_OUT;
                LSPWidget *f    = pFocus;
                pFocus          = NULL;
                status_t status = f->handle_event(&ev);
                if (status != STATUS_OK)
                    return status;
            }

            if (focus != NULL)
            {
                ev.nType        = UIE_FOCUS_IN;
                pFocus          = focus;
                status_t status = focus->handle_event(&ev);
                if (status != STATUS_OK)
                    return status;
            }

            return STATUS_OK;
        }

        status_t LSPWindow::unfocus_child(LSPWidget *focus)
        {
            if (pPointed == focus)
                pPointed = NULL;
            if (focus != pFocus)
                return STATUS_OK;

            ws_event_t ev;
            ev.nType        = UIE_FOCUS_OUT;
            ev.nLeft        = 0;
            ev.nTop         = 0;
            ev.nWidth       = 0;
            ev.nHeight      = 0;
            ev.nCode        = 0;
            ev.nState       = 0;
            ev.nTime        = 0;

            status_t status = pFocus->handle_event(&ev);
            pFocus          = NULL;
            return status;
        }

        status_t LSPWindow::toggle_child_focus(LSPWidget *focus)
        {
            return (focus == pFocus) ? unfocus_child(focus) : focus_child(focus);
        }

        status_t LSPWindow::set_focus(bool focus)
        {
            if (!visible())
                return STATUS_OK;
            return (pWindow != NULL) ? pWindow->set_focus(focus) : STATUS_BAD_STATE;
        }

        status_t LSPWindow::toggle_focus()
        {
            if (!visible())
                return STATUS_OK;
            return (pWindow != NULL) ? pWindow->toggle_focus() : STATUS_BAD_STATE;
        }

        bool LSPWindow::has_focus() const
        {
            return (visible()) ? bHasFocus : false;
        }

        void LSPWindow::realize(const realize_t *r)
        {
            lsp_trace("width=%d, height=%d", int(r->nWidth), int(r->nHeight));
            LSPWidgetContainer::realize(r);
            bSizeRequest        = false;

            if (pChild == NULL)
                return;

            // Query for size
            size_request_t sr;
            sr.nMinWidth        = -1;
            sr.nMinHeight       = -1;
            sr.nMaxWidth        = -1;
            sr.nMaxHeight       = -1;
            pChild->size_request(&sr);

            // Calculate realize parameters
            realize_t rc;

            // Dimensions
            ssize_t xs          = r->nWidth  - sPadding.horizontal() - nBorder * 2;
            ssize_t ys          = r->nHeight - sPadding.vertical() - nBorder * 2;

            if ((sr.nMinWidth >= 0) && (sr.nMinWidth > xs))
            {
                rc.nLeft            = nBorder + sPadding.left();
                rc.nWidth           = sr.nMinWidth;
            }
            else if (sr.nMaxWidth < 0)
            {
                rc.nLeft            = nBorder + sPadding.left();
                rc.nWidth           = xs;
            }
            else
            {
                rc.nWidth           = (sr.nMinWidth >= 0)   ? sr.nMinWidth  + (xs - sr.nMinWidth)   * nHorScale     : xs * nHorScale;
                if (rc.nWidth > xs)
                    rc.nWidth           = xs;
                xs                 -= rc.nWidth;
                rc.nLeft            = nBorder + sPadding.left() + xs * nHorPos;
            }

            if ((sr.nMinHeight >= 0) && (sr.nMinHeight > ys))
            {
                rc.nTop             = nBorder + sPadding.top();
                rc.nHeight          = sr.nMinHeight;
            }
            else if (sr.nMaxHeight < 0)
            {
                rc.nTop             = nBorder + sPadding.top();
                rc.nHeight          = ys;
            }
            else
            {
                rc.nHeight          = (sr.nMinHeight >= 0)  ? sr.nMinHeight + (ys - sr.nMinHeight)  * nVertScale    : ys * nVertScale;
                if (rc.nHeight > ys)
                    rc.nHeight          = ys;
                ys                 -= rc.nHeight;
                rc.nTop             = nBorder + sPadding.top() + ys * nVertPos;
            }

            // Call for realize
            pChild->realize(&rc);
            pChild->query_draw();
        }

        void LSPWindow::size_request(size_request_t *r)
        {
            size_request_t cr;

            cr.nMinWidth        = -1;
            cr.nMinHeight       = -1;
            cr.nMaxWidth        = -1;
            cr.nMaxHeight       = -1;

            r->nMinWidth        = 0;
            r->nMinHeight       = 0;
            r->nMaxWidth        = -1;
            r->nMaxHeight       = -1;

            // Estimate minimum possible window dimensions
            r->nMinWidth        = (sConstraints.nMinWidth >= 0) ? sConstraints.nMinWidth : sPadding.horizontal();
            r->nMinHeight       = (sConstraints.nMinHeight >= 0) ? sConstraints.nMinHeight : sPadding.vertical();

            r->nMinWidth       += nBorder * 2;
            r->nMinHeight      += nBorder * 2;

            if (pChild != NULL)
            {
                pChild->size_request(&cr);

                if (cr.nMinWidth >= 0)
                {
                    cr.nMinWidth       += sPadding.horizontal();
                    if (r->nMinWidth < cr.nMinWidth)
                        r->nMinWidth    = cr.nMinWidth;
                }
                if (cr.nMinHeight >= 0)
                {
                    cr.nMinHeight      += sPadding.vertical();
                    if (r->nMinHeight < cr.nMinHeight)
                        r->nMinHeight   = cr.nMinHeight;
                }
            }

            // Estimate maximum possible window dimensions
            if (sConstraints.nMaxWidth >= 0)
                r->nMaxWidth        = sPadding.horizontal() + sConstraints.nMaxWidth;
            if (sConstraints.nMaxHeight >= 0)
                r->nMaxHeight       = sPadding.vertical()   + sConstraints.nMaxHeight;

            if ((r->nMaxWidth >= 0) && (r->nMinWidth >= 0) && (r->nMaxWidth < r->nMinWidth))
                r->nMinWidth        = r->nMaxWidth;
            if ((r->nMaxHeight >= 0) && (r->nMinHeight >= 0) && (r->nMaxHeight < r->nMinHeight))
                r->nMinHeight       = r->nMaxHeight;
        }

        status_t LSPWindow::set_icon(const void *bgra, size_t width, size_t height)
        {
            if (pWindow == NULL)
                return STATUS_BAD_STATE;

            return pWindow->set_icon(bgra, width, height);
        }

        status_t LSPWindow::set_class(const char *instance, const char *wclass)
        {
            if (pWindow == NULL)
                return STATUS_BAD_STATE;
            return pWindow->set_class(instance, wclass);
        }

        status_t LSPWindow::set_class(const LSPString *instance, const LSPString *wclass)
        {
            if ((instance == NULL) || (wclass == NULL))
                return STATUS_BAD_ARGUMENTS;
            if (pWindow == NULL)
                return STATUS_BAD_STATE;
            char *i = instance->clone_ascii();
            if (i == NULL)
                return STATUS_NO_MEM;
            const char *c = instance->get_utf8();

            status_t res = (c != NULL) ? set_class(i, c) : STATUS_NO_MEM;
            ::free(i);
            return res;
        }

        status_t LSPWindow::set_role(const char *role)
        {
            if (pWindow == NULL)
                return STATUS_BAD_STATE;
            return pWindow->set_role(role);
        }

        status_t LSPWindow::set_role(const LSPString *role)
        {
            return set_role(role->get_utf8());
        }

    } /* namespace tk */
} /* namespace lsp */
