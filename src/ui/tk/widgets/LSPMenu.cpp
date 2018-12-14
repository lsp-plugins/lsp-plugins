/*
 * LSPMenu.cpp
 *
 *  Created on: 18 сент. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPMenu::metadata = { "LSPMenu", &LSPWidgetContainer::metadata };

        //-----------------------------------------------------------------------------
        // LSPMenu::LSPMenuWindow implementation
        LSPMenu::MenuWindow::MenuWindow(LSPDisplay *dpy, LSPMenu *menu, size_t screen):
            LSPWindow(dpy, NULL, screen)
        {
            pMenu       = menu;
        }

        LSPMenu::MenuWindow::~MenuWindow()
        {
        }

        LSPWidget *LSPMenu::MenuWindow::find_widget(ssize_t x, ssize_t y)
        {
            return (pMenu != NULL) ? pMenu->find_widget(x, y) : NULL;
        }

        void LSPMenu::MenuWindow::render(ISurface *s, bool force)
        {
            if (pMenu != NULL)
                pMenu->render(s, force);
            else
                LSPWindow::render(s, force);
        }

        status_t LSPMenu::MenuWindow::on_mouse_down(const ws_event_t *e)
        {
            return (pMenu != NULL) ? pMenu->on_mouse_down(e) : LSPWindow::on_mouse_down(e);
        }

        status_t LSPMenu::MenuWindow::on_mouse_up(const ws_event_t *e)
        {
            return (pMenu != NULL) ? pMenu->on_mouse_up(e) : LSPWindow::on_mouse_up(e);
        }

        status_t LSPMenu::MenuWindow::on_mouse_scroll(const ws_event_t *e)
        {
            return (pMenu != NULL) ? pMenu->on_mouse_scroll(e) : LSPWindow::on_mouse_scroll(e);
        }

        status_t LSPMenu::MenuWindow::on_mouse_move(const ws_event_t *e)
        {
            return (pMenu != NULL) ? pMenu->on_mouse_move(e) : LSPWindow::on_mouse_move(e);
        }

        void LSPMenu::MenuWindow::size_request(size_request_t *r)
        {
            if (pMenu != NULL)
                pMenu->size_request(r);

            // Limit the size of window with the screen size
            pDisplay->display()->screen_size(screen(), &r->nMaxWidth, &r->nMaxHeight);
            if ((r->nMinWidth > 0) && (r->nMinWidth > r->nMaxWidth))
                r->nMinWidth    = r->nMaxWidth;
            if ((r->nMinHeight > 0) && (r->nMinHeight > r->nMaxHeight))
                r->nMinHeight   = r->nMaxHeight;
        }

        //-----------------------------------------------------------------------------
        // LSPMenu implementation
        LSPMenu::LSPMenu(LSPDisplay *dpy):
            LSPWidgetContainer(dpy),
            sFont(dpy, this)
        {
            pWindow     = NULL;
            pActive     = NULL;
            nPopupLeft  = -1;
            nPopupTop   = -1;
            nSelected   = SEL_NONE;
            nScroll     = 0;
            nScrollMax  = 0;
            nBorder     = 1;
            nSpacing    = 6;
            nMBState    = 0;

            sPadding.set(16, 16, 0, 0);

            nFlags     &= ~F_VISIBLE;
            pClass      = &metadata;

            sScroll.bind(pDisplay);
            sScroll.set_handler(timer_handler, this);
        }
        
        LSPMenu::~LSPMenu()
        {
            do_destroy();
        }

        status_t LSPMenu::init()
        {
            status_t result = LSPWidget::init();
            if (result != STATUS_OK)
                return result;

            if (pDisplay != NULL)
            {
                // Get theme
                LSPTheme *theme = pDisplay->theme();
                if (theme != NULL)
                {
                    sFont.init(theme->font());
                    theme->get_color(C_BACKGROUND, sFont.color());
                    theme->get_color(C_BACKGROUND, &sBorderColor);
                    theme->get_color(C_LABEL_TEXT, &sColor);
                    theme->get_color(C_KNOB_SCALE, &sSelColor);
                }
            }

            return STATUS_OK;
        }

        void LSPMenu::destroy()
        {
            do_destroy();
            LSPWidgetContainer::destroy();
        }

        void LSPMenu::do_destroy()
        {
            size_t n            = vItems.size();
            for (size_t i=0; i<n; ++i)
            {
                LSPMenuItem *item   = vItems.at(i);
                if (item == NULL)
                    continue;

                unlink_widget(item);
            }

            vItems.flush();
        }

//        LSPWidget *LSPMenu::find_widget(ssize_t x, ssize_t y)
//        {
//            size_t items = vItems.size();
//            for (size_t i=0; i<items; ++i)
//            {
//                LSPMenuItem *w = vItems.at(i);
//                if ((w == NULL) || (w->hidden()))
//                    continue;
//                if (w->inside(x, y))
//                    return w;
//            }
//
//            return NULL;
//        }

        void LSPMenu::set_border(size_t value)
        {
            if (nBorder == value)
                return;
            nBorder = value;
            query_resize();
        }

        void LSPMenu::set_spacing(size_t value)
        {
            if (nSpacing == value)
                return;
            nSpacing = value;
            query_resize();
        }

        void LSPMenu::set_scroll(ssize_t scroll)
        {
            if (scroll < 0)
                scroll = 0;
            else if (scroll > nScrollMax)
                scroll = nScrollMax;

            if (nScroll == scroll)
                return;
            nScroll = scroll;

            query_draw();
            if (pWindow != NULL)
                pWindow->query_draw();
        }

        void LSPMenu::query_resize()
        {
            LSPWidgetContainer::query_resize();
            if (pWindow != NULL)
                pWindow->query_resize();
        }

        status_t LSPMenu::add(LSPWidget *child)
        {
            LSPMenuItem *item = widget_cast<LSPMenuItem>(child);
            if (child == NULL)
                return STATUS_BAD_ARGUMENTS;

            if (!vItems.add(item))
                return STATUS_NO_MEM;

            item->set_parent(this);

            query_resize();
            return STATUS_SUCCESS;
        }

        status_t LSPMenu::remove(LSPWidget *child)
        {
            size_t n            = vItems.size();
            for (size_t i=0; i<n; ++i)
            {
                LSPMenuItem *item   = vItems.at(i);
                if (item == child)
                {
                    query_resize();
                    return (vItems.remove(i)) ? STATUS_OK : STATUS_UNKNOWN_ERR;
                }
            }

            return STATUS_NOT_FOUND;
        }

        ssize_t LSPMenu::find_item(ssize_t mx, ssize_t my)
        {
            if ((mx < 0) || (mx >= sSize.nWidth))
                return SEL_NONE;
            if ((my < 0) || (my >= sSize.nHeight))
                return SEL_NONE;

            font_parameters_t fp;
            sFont.get_parameters(&fp);

            ssize_t separator = fp.Height * 0.5f + nSpacing;
            fp.Height      += nSpacing;

            if (nScrollMax > 0)
            {
                if ((nScroll > 0) && (my < ssize_t(nBorder + separator))) // Top button
                    return SEL_TOP_SCROLL;
                else if ((nScroll < nScrollMax) && (my > ssize_t(sSize.nHeight - nBorder - separator))) // Bottom button
                    return SEL_BOTTOM_SCROLL;
            }

            // Iterate over all menu items
            ssize_t y       = sPadding.top() + nBorder - nScroll;
            size_t n        = vItems.size();

            for (size_t i=0; i < n; ++i)
            {
                LSPMenuItem *item = vItems.get(i);
                if ((item == NULL) || (!item->visible()))
                    continue;

                if (item->is_separator())
                    y += separator;
                else
                {
                    if ((my >= y) && (my < (y + fp.Height)))
                        return i;

                    y += fp.Height;
                }
            }

            return SEL_NONE;
        }

        void LSPMenu::draw(ISurface *s)
        {
            s->clear(sColor);

            font_parameters_t fp;
            sFont.get_parameters(s, &fp);

            ssize_t separator = fp.Height * 0.5f + nSpacing;
            ssize_t sep_len = sSize.nWidth - (nBorder + nSpacing) * 2;
            ssize_t hspace  = nSpacing >> 1;

            fp.Height      += nSpacing;
            ssize_t y       = sPadding.top() + nBorder - nScroll;
            ssize_t x       = sPadding.left() + nBorder;
            size_t n        = vItems.size();

            for (size_t i=0; i < n; ++i)
            {
                LSPMenuItem *item = vItems.get(i);
                if ((item == NULL) || (!item->visible()))
                    continue;

//                lsp_trace("x,y = %d, %d", int(x), int(y));

                if (y >= sSize.nHeight)
                    break;

                if (item->is_separator())
                {
                    if (y > (-separator))
                    {
                        if (sep_len > 0)
                            s->fill_rect(x - sPadding.left() + nSpacing, y + (separator >> 1), sep_len, 1, sBorderColor);
                    }

                    y += separator;
                }
                else
                {
                    if (y > (-fp.Height))
                    {
                        const char *text = item->text();
                        if (nSelected == ssize_t(i))
                        {
                            s->fill_rect(nBorder, y, sSize.nWidth - nBorder*2, fp.Height, sSelColor);
                            if (text != NULL)
                                sFont.draw(s, x, y + fp.Ascent + hspace, sColor, text);
                        }
                        else if (text != NULL)
                            sFont.draw(s, x, y + fp.Ascent + hspace, text);
                    }

                    y += fp.Height;
                }
            }

            if (nScrollMax > 0)
            {
                float cx = sSize.nWidth * 0.5f;
                float aa = s->set_antialiasing(true);

                // Top button
                if (nScroll > 0)
                {
                    Color cl;

                    s->fill_rect(nBorder, nBorder, sSize.nWidth - nBorder * 2, separator, sColor);
                    if (nSelected == SEL_TOP_SCROLL)
                    {
                        cl.copy(sColor);
                        s->fill_rect(nBorder + 1, nBorder + 1, sSize.nWidth - (nBorder + 1)* 2, separator - 1, sBorderColor);
                    }
                    else
                        cl.copy(sFont.color());

                    // Draw arrow up
                    s->fill_triangle(
                        cx, nBorder + 3,
                        cx + separator, nBorder + separator - 2,
                        cx - separator, nBorder + separator - 2,
                        cl);
                }
                else if (sPadding.top() > 0)
                    s->fill_rect(nBorder, nBorder, sSize.nWidth - nBorder * 2, sPadding.top(), sColor);

                // Bottom button
                if (nScroll < nScrollMax)
                {
                    Color cl;
                    s->fill_rect(nBorder, sSize.nHeight - nBorder - separator,
                        sSize.nWidth - nBorder * 2, separator, sColor);

                    if (nSelected == SEL_BOTTOM_SCROLL)
                    {
                        cl.copy(sColor);
                        s->fill_rect(nBorder + 1, sSize.nHeight - nBorder - separator,
                            sSize.nWidth - (nBorder + 1) * 2, separator - 1, sBorderColor);
                    }
                    else
                        cl.copy(sFont.color());

                    // Draw arrow down
                    s->fill_triangle(
                        cx, sSize.nHeight - nBorder - 3,
                        cx + separator, sSize.nHeight - nBorder - separator + 2,
                        cx - separator, sSize.nHeight - nBorder - separator + 2,
                        cl);
                }
                else if (sPadding.bottom() > 0)
                    s->fill_rect(nBorder, sSize.nHeight - nBorder - sPadding.bottom(),
                        sSize.nWidth - nBorder * 2, sPadding.bottom(), sColor);

                // Restore anti-aliasing
                s->set_antialiasing(aa);
            }

            if (nBorder > 0)
                s->fill_frame(0, 0, sSize.nWidth, sSize.nHeight,
                    nBorder, nBorder, sSize.nWidth - nBorder * 2, sSize.nHeight - nBorder * 2, sBorderColor);
        }

        bool LSPMenu::hide()
        {
            if (!is_visible())
                return false;

            // Hide active submenu if present
            if (pActive != NULL)
            {
                pActive->hide();
                pActive = NULL;
            }

            // Hide window showing menu
            if (pWindow != NULL)
                pWindow->hide();

            return LSPWidgetContainer::hide();
        }

        bool LSPMenu::show()
        {
            if (is_visible())
                return false;

            size_t screen = pDisplay->display()->default_screen();
            LSPWindow *top = widget_cast<LSPWindow>(toplevel());
            if (top != NULL)
                screen = top->screen();

            return show(screen, nPopupLeft, nPopupTop);
        }

        bool LSPMenu::show(size_t screen)
        {
            return show(screen, nPopupLeft, nPopupTop);
        }

        bool LSPMenu::show(LSPWidget *w)
        {
            return show(w, nPopupLeft, nPopupTop);
        }

        bool LSPMenu::show(LSPWidget *w, ssize_t x, ssize_t y)
        {
            if (is_visible())
                return false;

            size_t screen = pDisplay->display()->default_screen();
            LSPWindow *top = widget_cast<LSPWindow>(toplevel());
            if (top != NULL)
                screen = top->screen();

            return show(screen, x, y);
        }

        bool LSPMenu::show(LSPWidget *w, const ws_event_t *ev)
        {
            if (ev == NULL)
                return show(w, nPopupLeft, nPopupTop);

            realize_t r;
            r.nLeft     = 0;
            r.nTop      = 0;
            r.nWidth    = 0;
            r.nHeight   = 0;

            LSPWindow *parent = widget_cast<LSPWindow>(w->toplevel());
            if (parent != NULL)
                parent->get_absolute_geometry(&r);

            return show(w, r.nLeft + ev->nLeft, r.nTop + ev->nTop);
        }

        bool LSPMenu::show(size_t screen, ssize_t left, ssize_t top)
        {
            if (is_visible())
                return false;

            // Determine what screen to use
            IDisplay *dpy = pDisplay->display();
            if (screen >= dpy->screens())
                screen = dpy->default_screen();

            // Now we are ready to create window
            if (pWindow == NULL)
            {
                // Create window
                pWindow = new MenuWindow(pDisplay, this, screen);
                if (pWindow == NULL)
                    return false;

                // Initialize window
                status_t result = pWindow->init();
                if (result != STATUS_OK)
                {
                    pWindow->destroy();
                    delete pWindow;
                    pWindow = NULL;
                    return false;
                }

                pWindow->set_border_style(BS_POPUP);
                pWindow->actions()->set_actions(WA_POPUP);
            }

            // Get initial window geometry
            realize_t wr;
            pWindow->get_geometry(&wr);
            if (left >= 0)
                wr.nLeft        = left;
            else if (wr.nLeft < 0)
                wr.nLeft        = 0;
            if (top >= 0)
                wr.nTop         = top;
            else if (wr.nTop < 0)
                wr.nTop         = 0;


            // Now request size and adjust location
            size_request_t sr;
            pWindow->size_request(&sr);

            ssize_t sw = 0, sh = 0;
            dpy->screen_size(pWindow->screen(), &sw, &sh);
            ssize_t xlast = wr.nLeft + sr.nMinWidth, ylast = wr.nTop + sr.nMinHeight;

            if (xlast >  sw)
                wr.nLeft   -= (xlast - sw);
            if (ylast > sh)
                wr.nTop    -= (ylast - sh);
            wr.nWidth       = sr.nMinWidth;
            wr.nHeight      = sr.nMinHeight;

            // Now we can set the geometry and show window
            pWindow->set_geometry(&wr);
            wr.nLeft        = 0;
            wr.nTop         = 0;
            realize(&wr);
            nSelected       = SEL_NONE;

            pWindow->show();

            return LSPWidgetContainer::show();
        }

        void LSPMenu::size_request(size_request_t *r)
        {
            r->nMinWidth    = 0;
            r->nMinHeight   = 0;
            r->nMaxWidth    = -1;
            r->nMaxHeight   = -1;

            // Create surface
            ISurface *s = pDisplay->create_surface(1, 1);
            if (s == NULL)
                return;

            // Estimate the size of menu
            font_parameters_t fp;
            text_parameters_t tp;
            sFont.get_parameters(s, &fp);
            size_t n = vItems.size();
            ssize_t separator = fp.Height * 0.5f;

            for (size_t i=0; i<n; ++i)
            {
                LSPMenuItem *mi = vItems.at(i);
                if ((mi == NULL) || (!mi->visible()))
                    continue;

                if (mi->is_separator())
                {
                    r->nMinHeight += separator + nSpacing;
                    if (r->nMinWidth < fp.Height)
                        r->nMinWidth = fp.Height;
                }
                else
                {
                    r->nMinHeight  += fp.Height + nSpacing;
                    ssize_t width   = (mi->submenu() != NULL) ? separator : 0;

                    const char *text = mi->text();
                    if (text != NULL)
                    {
                        sFont.get_text_parameters(s, &tp, text);
                        width          += tp.XAdvance;
                    }

                    if (r->nMinWidth < width)
                        r->nMinWidth        = width;
                }
            }

            r->nMinWidth    += nBorder * 2 + sPadding.horizontal();
            r->nMinHeight   += nBorder * 2 + sPadding.vertical();

            // Destroy surface
            s->destroy();
            delete s;
        }

        void LSPMenu::realize(const realize_t *r)
        {
            LSPWidgetContainer::realize(r);

            size_request_t sr;
            size_request(&sr);

            nScrollMax      = sr.nMinHeight - r->nHeight;
            set_scroll(nScroll);
//            lsp_trace("scroll_max = %d, scroll = %d", int(nScrollMax), int(nScroll));

            query_draw();
            if (pWindow != NULL)
                pWindow->query_draw();
        }

        status_t LSPMenu::on_mouse_down(const ws_event_t *e)
        {
            if (nMBState == 0)
            {
                if (!inside(e->nLeft, e->nTop))
                {
                    hide();
                    return STATUS_OK;
                }
            }

            nMBState |= (1 << e->nCode);
            return STATUS_OK;
        }

        status_t LSPMenu::on_mouse_up(const ws_event_t *e)
        {
            if ((nMBState == (1 << MCB_LEFT)) && (e->nCode == MCB_LEFT))
            {
                // Cleanup mouse button state flag
                nMBState &= ~ (1 << e->nCode);

                // Selection was found ?
                ssize_t sel = find_item(e->nLeft, e->nTop);
                LSPMenuItem *item = NULL;

                if (sel >= 0)
                {
                    item = vItems.get(sel);
                    if ((item != NULL) && (item->hidden()))
                        item = NULL;
                }

                // Hide only if scroll is not pressed
                if ((sel != SEL_TOP_SCROLL) && (sel != SEL_BOTTOM_SCROLL))
                    hide();

                if (item != NULL)
                {
                    ws_event_t ev = *e;
                    item->slots()->execute(LSPSLOT_SUBMIT, this, &ev);
                }
            }
            else
            {
                // Cleanup mouse button state flag
                nMBState &= ~ (1 << e->nCode);
                if (nMBState == 0)
                    hide();
            }

            return STATUS_OK;
        }

        status_t LSPMenu::on_mouse_scroll(const ws_event_t *e)
        {
            font_parameters_t fp;
            sFont.get_parameters(&fp);
            ssize_t amount = fp.Height + nSpacing;
            if (amount < 1)
                amount = 1;

            ssize_t scroll = nScroll;
            if (e->nCode == MCD_UP)
                set_scroll(nScroll - amount);
            else if (e->nCode == MCD_DOWN)
                set_scroll(nScroll + amount);

            if (scroll != nScroll)
            {
                ssize_t sel = nSelected;
                nSelected   = find_item(e->nLeft, e->nTop);

                if (sel != nSelected)
                {
                    // Query for draw
                    query_draw();
                    if (pWindow != NULL)
                        pWindow->query_draw();
                }
            }

            return STATUS_OK;
        }

        status_t LSPMenu::on_mouse_move(const ws_event_t *e)
        {
//            lsp_trace("x=%d, y=%d", int(e->nLeft), int(e->nTop));

            ssize_t sel = nSelected;
            nSelected   = find_item(e->nLeft, e->nTop);

            if (sel != nSelected)
            {
//                lsp_trace("selection changed. Was %d, now %d", int(sel), int(nSelected));
                // Update timer status
                if ((nSelected == SEL_TOP_SCROLL) || (nSelected == SEL_BOTTOM_SCROLL))
                    sScroll.launch(0, 25);
                else
                    sScroll.cancel();

                // Query for draw
                query_draw();
                if (pWindow != NULL)
                    pWindow->query_draw();
            }

            return STATUS_OK;
        }

        status_t LSPMenu::timer_handler(timestamp_t time, void *arg)
        {
            LSPMenu *_this = static_cast<LSPMenu *>(arg);
            if (_this == NULL)
                return STATUS_BAD_ARGUMENTS;
            _this->update_scroll();
            return STATUS_OK;
        }

        void LSPMenu::update_scroll()
        {
            font_parameters_t fp;
            sFont.get_parameters(&fp);
            ssize_t amount = fp.Height * 0.5f;
            if (amount < 1)
                amount = 1;

            switch (nSelected)
            {
                case SEL_TOP_SCROLL:
                    set_scroll(nScroll - amount);
                    if (nScroll <= 0)
                        sScroll.cancel();
                    break;

                case SEL_BOTTOM_SCROLL:
                    set_scroll(nScroll + amount);
                    if (nScroll >= nScrollMax)
                        sScroll.cancel();
                    break;

                default:
                    sScroll.cancel();
                    break;
            }
        }
    
    } /* namespace tk */
} /* namespace lsp */
