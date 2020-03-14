/*
 * LSPComboGroup.cpp
 *
 *  Created on: 29 апр. 2018 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        
        const w_class_t LSPComboGroup::metadata = { "LSPComboGroup", &LSPWidgetContainer::metadata };

        //---------------------------------------------------------------------
        LSPComboGroup::LSPComboList::LSPComboList(LSPDisplay *dpy, LSPComboGroup *widget): LSPListBox(dpy)
        {
            pWidget = widget;
        }

        LSPComboGroup::LSPComboList::~LSPComboList()
        {
        }

        void LSPComboGroup::LSPComboList::on_selection_change()
        {
            LSPListBox::on_selection_change();
            pWidget->on_selection_change();
        }

        void LSPComboGroup::LSPComboList::on_item_change(ssize_t index, LSPItem *item)
        {
            LSPListBox::on_item_change(index, item);
            pWidget->on_item_change(index, item);
        }

        void LSPComboGroup::LSPComboList::on_item_add(size_t index)
        {
            LSPListBox::on_item_add(index);
            pWidget->on_item_add(index);
        }

        void LSPComboGroup::LSPComboList::on_item_remove(size_t index)
        {
            LSPListBox::on_item_remove(index);
            pWidget->on_item_remove(index);
        }

        void LSPComboGroup::LSPComboList::on_item_swap(size_t idx1, size_t idx2)
        {
            LSPListBox::on_item_swap(idx1, idx2);
            pWidget->on_item_swap(idx1, idx2);
        }

        void LSPComboGroup::LSPComboList::on_item_clear()
        {
            LSPListBox::on_item_clear();
            pWidget->on_item_clear();
        }

        //---------------------------------------------------------------------
        LSPComboGroup::LSPComboPopup::LSPComboPopup(LSPDisplay *dpy, LSPComboGroup *widget, ssize_t screen):
            LSPWindow(dpy, NULL, screen)
        {
            pWidget = widget;
        }

        LSPComboGroup::LSPComboPopup::~LSPComboPopup()
        {
        }

        status_t LSPComboGroup::LSPComboPopup::handle_event(const ws_event_t *e)
        {
            switch (e->nType)
            {
                case UIE_KEY_DOWN:
                    pWidget->on_grab_key_down(e);
                    break;
            }
            return LSPWindow::handle_event(e);
        }

        //---------------------------------------------------------------------
        LSPComboGroup::LSPComboGroup(LSPDisplay *dpy):
            LSPWidgetContainer(dpy),
            sColor(this),
            sListBox(dpy, this),
            sFont(this)
        {
            nRadius     = 10;
            nBorder     = 0;
            nCBFlags    = 0;
            nMFlags     = 0;
            pPopup      = NULL;
            bEmbed      = false;

            sGroupHdr.nLeft     = 0;
            sGroupHdr.nTop      = 0;
            sGroupHdr.nWidth    = 0;
            sGroupHdr.nHeight   = 0;

            pClass      = &metadata;
        }

        LSPComboGroup::~LSPComboGroup()
        {
            do_destroy();
        }

        status_t LSPComboGroup::init()
        {
            status_t result = LSPWidgetContainer::init();
            if (result != STATUS_OK)
                return result;

            // Init list box
            result = sListBox.init();
            if (result != STATUS_OK)
                return result;

            sFont.init();
            sFont.set_size(12.0f);
            init_color(C_LABEL_TEXT, &sColor);
            init_color(C_BACKGROUND, sFont.color());

            // Bind slots
            ui_handler_id_t id = 0;
            id = sSlots.add(LSPSLOT_CHANGE, slot_on_change, self());
            if (id >= 0) id = sSlots.add(LSPSLOT_SUBMIT, slot_on_submit, self());
            if (id >= 0) id = sListBox.slots()->bind(LSPSLOT_CHANGE, slot_on_list_change, self());
            if (id >= 0) id = sListBox.slots()->bind(LSPSLOT_SUBMIT, slot_on_list_submit, self());

            return (id >= 0) ? STATUS_OK : -id;
        }

        void LSPComboGroup::destroy()
        {
            do_destroy();
            LSPWidgetContainer::destroy();
        }

        LSPWidget *LSPComboGroup::current_widget()
        {
            ssize_t idx = sListBox.selection()->value();
            if (idx >= ssize_t(vWidgets.size()))
                idx = vWidgets.size() - 1;
            LSPWidget *w = vWidgets.get(idx);
            return ((w == NULL) || (w->invisible())) ? NULL : w;
        }

        LSPWidget *LSPComboGroup::find_widget(ssize_t x, ssize_t y)
        {
            LSPWidget *curr = current_widget();
            return ((curr != NULL) && (curr->inside(x, y))) ? curr : NULL;
        }

        void LSPComboGroup::query_dimensions(dimensions_t *d)
        {
            size_t bw       = (bEmbed) ? 1 : ::round(nRadius * M_SQRT2 * 0.5) + 1;
            size_t dd       = bw + nBorder + 1;
            d->nGapLeft     = dd;
            d->nGapRight    = dd;
            d->nGapTop      = dd;
            d->nGapBottom   = dd;
            d->nMinWidth    = nBorder*2;
            d->nMinHeight   = nBorder*2;

            LSPString text;
            const LSPLocalString *lctext = this->text();
            if (lctext != NULL)
                lctext->format(&text, this);
            if (text.length() > 0)
            {
                // Create temporary surface
                ISurface *s = (pDisplay != NULL) ? pDisplay->create_surface(1, 1) : NULL;
                if (s == NULL)
                    return;

                font_parameters_t   fp;
                text_parameters_t   tp;

                sFont.get_parameters(s, &fp);
                sFont.get_text_parameters(s, &tp, &text);

                d->nMinWidth    += tp.Width + nRadius * 3;
                d->nMinHeight   += fp.Height + nRadius * 2;
                d->nGapTop      += fp.Height;

                // Destroy surface
                s->destroy();
                delete s;
            }
        }

        void LSPComboGroup::do_destroy()
        {
            size_t count = vWidgets.size();
            for (size_t i=0; i<count; ++i)
            {
                LSPWidget *w = vWidgets.get(i);
                if (w != NULL)
                    unlink_widget(w);
            }
            vWidgets.clear();
        }

        bool LSPComboGroup::check_mouse_over(ssize_t x, ssize_t y)
        {
            x              -= sGroupHdr.nLeft;
            y              -= sGroupHdr.nTop;

            return (x >= 0) && (y >= 0) && (x < sGroupHdr.nWidth) && (y < sGroupHdr.nHeight);
        }

        void LSPComboGroup::set_radius(size_t value)
        {
            if (nRadius == value)
                return;
            nRadius = value;
            query_resize();
        }

        void LSPComboGroup::set_border(size_t value)
        {
            if (nBorder == value)
                return;
            nBorder = value;
            query_resize();
        }

        void LSPComboGroup::render(ISurface *s, bool force)
        {
            if (nFlags & REDRAW_SURFACE)
                force = true;

            // Prepare palette
            Color bg_color(sBgColor);
            Color color(sColor);
            color.scale_lightness(brightness());

//            lsp_trace("Rendering this=%p, force=%d", this, int(force));
            LSPWidget *current = current_widget();

            // Draw child
            if (current != NULL)
            {
                if ((force) || (current->redraw_pending()))
                {
                    current->render(s, force);
                    current->commit_redraw();
                }
            }

            if (force)
            {
                // Get resource
                ssize_t cx  = sSize.nLeft + nBorder + 1;
                ssize_t cy  = sSize.nTop + nBorder + 1;
                ssize_t sx  = sSize.nWidth - (nBorder << 1) - 1;
                ssize_t sy  = sSize.nHeight - (nBorder << 1) - 1;
//                size_t bw   = round(nRadius * M_SQRT2 * 0.5f) + 1;

                // Draw background
                if (current == NULL)
                    s->fill_rect(sSize.nLeft, sSize.nTop, sSize.nWidth, sSize.nHeight, bg_color);
                else
                {
                    realize_t r;
                    current->get_dimensions(&r);

                    if ((bEmbed) && (nRadius > 1))
                        s->fill_round_frame(
                            sSize.nLeft, sSize.nTop, sSize.nWidth, sSize.nHeight,
                            r.nLeft, r.nTop, r.nWidth, r.nHeight,
                            nRadius-1, SURFMASK_B_CORNER,
                            bg_color
                        );
                    else
                        s->fill_frame(
                                sSize.nLeft, sSize.nTop, sSize.nWidth, sSize.nHeight,
                                r.nLeft, r.nTop, r.nWidth, r.nHeight,
                                bg_color
                            );
                }

                // Draw frame
                bool aa = s->set_antialiasing(true);
                s->wire_round_rect(cx, cy, sx-1, sy-1, nRadius, 0x0e, 2.0f, color);

                ssize_t bwidth      = 12;
                sGroupHdr.nLeft     = cx;
                sGroupHdr.nTop      = cy;
                sGroupHdr.nWidth    = nRadius;
                sGroupHdr.nHeight   = nRadius;

                // Draw text frame
                LSPString text;
                const LSPLocalString *lctext = this->text();
                if (lctext != NULL)
                    lctext->format(&text, this);
                if (text.length() > 0)
                {
                    // Draw text border
                    font_parameters_t   fp;
                    text_parameters_t   tp;
                    sFont.get_parameters(s, &fp);
                    sFont.get_text_parameters(s, &tp, &text);

                    sGroupHdr.nWidth    = 4 + nRadius + tp.Width + bwidth;
                    sGroupHdr.nHeight   = fp.Height + 4;

                    s->fill_round_rect(cx - 1, cy-1, sGroupHdr.nWidth, sGroupHdr.nHeight, nRadius, 0x04, color);

                    // Show text
                    Color font(sFont.raw_color());
                    font.scale_lightness(brightness());

                    sFont.draw(s, cx + bwidth + 4 , cy + fp.Ascent + nBorder, font, &text);

                    // Draw buttons
                    ssize_t half = sGroupHdr.nTop + (fp.Height * 0.5f);

                    s->fill_triangle(
                            cx + 2, half - 2,
                            cx + bwidth - 2, half - 2,
                            cx + bwidth*0.5f, half - 6,
                            font);

                    s->fill_triangle(
                            cx + 2, half + 1,
                            cx + bwidth - 2, half + 1,
                            cx + bwidth*0.5f, half + 5,
                            font);

                    s->set_antialiasing(false);
                    s->line(cx + bwidth + 2, cy + 1, cx + bwidth + 2, cy + fp.Height + 1, 1, font);
                }

                s->set_antialiasing(aa);
            }
        }

        status_t LSPComboGroup::add(LSPWidget *widget)
        {
            widget->set_parent(this);
            vWidgets.add(widget);
            query_resize();
            return STATUS_OK;
        }

        status_t LSPComboGroup::remove(LSPWidget *widget)
        {
            if (vWidgets.remove(widget, false))
                return STATUS_NOT_FOUND;

            unlink_widget(widget);
            return STATUS_OK;
        }

        void LSPComboGroup::size_request(size_request_t *r)
        {
            LSPWidget *w = current_widget();
            if (w != NULL)
                w->size_request(r);

            if (r->nMinWidth < 0)
                r->nMinWidth    = 0;
            if (r->nMinHeight < 0)
                r->nMinHeight   = 0;

            if (w != NULL)
            {
                r->nMinWidth   += w->padding()->horizontal();
                r->nMinHeight  += w->padding()->vertical();
            }

            dimensions_t d;
            query_dimensions(&d);

            if (r->nMinWidth >= 0)
            {
                size_t  n = r->nMinWidth + d.nGapLeft + d.nGapRight;
                if (n < d.nMinWidth)
                    r->nMinWidth    = d.nMinWidth;
                else
                    r->nMinWidth    = n;
            }
            if (r->nMinHeight >= 0)
            {
                size_t  n = r->nMinHeight + d.nGapTop + d.nGapBottom;
                if (n < d.nMinHeight)
                    r->nMinHeight   = d.nMinHeight;
                else
                    r->nMinHeight   = n;
            }

            // Align to 8-pixel grid
//            r->nMinWidth    = ((r->nMinWidth  + 7) >> 3) << 3;
//            r->nMinHeight   = ((r->nMinHeight + 7) >> 3) << 3;

            if ((r->nMaxWidth >= 0) && (r->nMaxWidth < r->nMinWidth))
                r->nMaxWidth    = r->nMinWidth;
            if ((r->nMaxHeight >= 0) && (r->nMaxHeight < r->nMinHeight))
                r->nMaxHeight   = r->nMinHeight;
        }

        void LSPComboGroup::realize(const realize_t *r)
        {
            LSPWidgetContainer::realize(r);
            LSPWidget *w = current_widget();
            if (w == NULL)
                return;

            dimensions_t d;
            query_dimensions(&d);

            size_request_t sr;
            w->size_request(&sr);

            realize_t rc;
            rc.nLeft    = r->nLeft   + d.nGapLeft  + w->padding()->left();
            rc.nTop     = r->nTop    + d.nGapTop   + w->padding()->top();
            rc.nWidth   = r->nWidth  - d.nGapLeft  - d.nGapRight   - w->padding()->horizontal();
            rc.nHeight  = r->nHeight - d.nGapTop   - d.nGapBottom  - w->padding()->vertical();

            if ((sr.nMaxWidth > 0) && (sr.nMaxWidth < rc.nWidth))
            {
                rc.nLeft   += (rc.nWidth - sr.nMaxWidth) >> 1;
                rc.nWidth   = sr.nMaxWidth;
            }

            if ((sr.nMaxHeight > 0) && (sr.nMaxHeight < rc.nHeight))
            {
                rc.nTop    += (rc.nHeight - sr.nMaxHeight) >> 1;
                rc.nHeight  = sr.nMaxHeight;
            }

            w->realize(&rc);
        }
    
        status_t LSPComboGroup::on_mouse_down(const ws_event_t *e)
        {
            if (!check_mouse_over(e->nLeft, e->nTop))
            {
                nCBFlags |= F_MOUSE_OUT;
                return STATUS_OK;
            }

            take_focus();
            nMFlags |= (1 << e->nCode);
            return STATUS_OK;
        }

        status_t LSPComboGroup::on_mouse_up(const ws_event_t *e)
        {
            size_t flags = nMFlags;
            nMFlags &= ~(1 << e->nCode);

            if (nCBFlags & F_MOUSE_OUT)
            {
                if (!nMFlags)
                    nCBFlags &= ~F_MOUSE_OUT;
                return STATUS_OK;
            }

            if ((e->nCode == MCB_LEFT) && (flags == size_t(1 << e->nCode)))
            {
                if (inside(e->nLeft, e->nTop))
                    toggle();
            }

            return STATUS_OK;
        }

        status_t LSPComboGroup::on_mouse_scroll(const ws_event_t *e)
        {
            if (!check_mouse_over(e->nLeft, e->nTop))
                return STATUS_OK;

            ssize_t selection = sListBox.selection()->value();
            ssize_t old  = selection;
            ssize_t last = sListBox.items()->size() - 1;

            if (e->nCode == MCD_UP)
            {
                if (selection > 0)
                    selection --;
                else if (selection == 0)
                {
                    if (!(nCBFlags & F_CIRCULAR))
                        return STATUS_OK;
                    selection = last;
                }
                else
                    selection = sListBox.items()->size() - 1;
            }
            else if (e->nCode == MCD_DOWN)
            {
                if (selection >= 0)
                {
                    if (selection < last)
                        selection ++;
                    else if (!(nCBFlags & F_CIRCULAR))
                        return STATUS_OK;
                    else
                        selection = 0;
                }
                else
                    selection = 0;
            }
            else
                return STATUS_OK;

            sListBox.selection()->set_value(selection);
            if (sListBox.selection()->value() == old)
                return STATUS_OK;

            sSlots.execute(LSPSLOT_CHANGE, this);
            return sSlots.execute(LSPSLOT_SUBMIT, this);
        }

        status_t LSPComboGroup::on_change()
        {
            return STATUS_OK;
        }

        status_t LSPComboGroup::on_submit()
        {
            lsp_trace("on_submit");
            close();
            return STATUS_OK;
        }

        const LSPLocalString *LSPComboGroup::text() const
        {
            LSPComboList *lb = const_cast<LSPComboList *>(&sListBox);
            ssize_t idx = lb->selection()->value();
            LSPItem *itm = lb->items()->get(idx);
            return (itm != NULL) ? itm->text() : NULL;
        }

        ssize_t LSPComboGroup::selected() const
        {
            LSPComboList *lb = const_cast<LSPComboList *>(&sListBox);
            return lb->selection()->value();
        }

        status_t LSPComboGroup::set_selected(ssize_t value)
        {
            return sListBox.selection()->set_value(value);
        }

        status_t LSPComboGroup::set_opened(bool open)
        {
            if (open == bool(nCBFlags & F_OPENED))
                return STATUS_OK;

            // Check if we need to close combo box
            if (!open)
            {
                if (pPopup != NULL)
                    pPopup->hide();
                sListBox.hide();

                nCBFlags &= ~F_OPENED;
                return STATUS_OK;
            }

            LSPWindow *parent = widget_cast<LSPWindow>(toplevel());

            // Now we need to open combo box
            // Create popup window
            if (pPopup == NULL)
            {
                if (parent != NULL)
                    pPopup  = new LSPComboPopup(pDisplay, this, parent->screen());
                else
                    pPopup  = new LSPComboPopup(pDisplay, this);
                if (pPopup == NULL)
                    return STATUS_NO_MEM;

                status_t result = pPopup->init();
                if (result != STATUS_OK)
                {
                    pPopup->destroy();
                    delete pPopup;
                    pPopup = NULL;
                    return result;
                }

                pPopup->set_border_style(BS_COMBO);
                pPopup->actions()->set_actions(WA_COMBO);
                pPopup->add(&sListBox);
                pPopup->slots()->bind(LSPSLOT_MOUSE_DOWN, slot_on_list_mouse_down, self());
                pPopup->slots()->intercept(LSPSLOT_KEY_DOWN, slot_on_list_key_down, self());
                pPopup->slots()->bind(LSPSLOT_SHOW, slot_on_list_show, self());
            }

            // Calculate popup window size and location
            realize_t r;
            r.nLeft     = 0;
            r.nTop      = 0;
            r.nWidth    = 0;
            r.nHeight   = 0;
            if (parent != NULL)
                parent->get_absolute_geometry(&r);

            // Get the screen size
            ssize_t sw, sh;
            size_t screen = pDisplay->display()->default_screen();
            LSPWindow *top = widget_cast<LSPWindow>(toplevel());
            if (top != NULL)
                screen = top->screen();
            pDisplay->screen_size(screen, &sw, &sh);

            // Get initial geometry of the window
            size_request_t opr;
            realize_t wr1, wr2;

            sListBox.optimal_size_request(&opr);

            wr1.nLeft       = r.nLeft + sGroupHdr.nLeft;
            if (wr1.nLeft < 0)
                wr1.nLeft       = 0;
            wr1.nWidth      = opr.nMaxWidth;

            if ((wr1.nWidth < sGroupHdr.nWidth) && (sGroupHdr.nHeight < sw))
                wr1.nWidth      = sGroupHdr.nWidth;
            if (wr1.nWidth > sw)
                wr1.nWidth      = sw;
            if (wr1.nHeight > sh)
                wr1.nHeight     = sh;
            if ((wr1.nLeft + wr1.nWidth) >= sw)
            {
                wr1.nLeft      = sw - wr1.nWidth;
                if (wr1.nLeft < 0)
                    wr1.nLeft   = 0;
            }
            wr2.nLeft       = wr1.nLeft;
            wr2.nWidth      = wr1.nWidth;

            wr1.nTop        = r.nTop + sGroupHdr.nTop + sGroupHdr.nHeight;
            wr2.nTop        = 0;

            wr1.nHeight     = sh - wr1.nTop;
            wr2.nHeight     = r.nTop + sGroupHdr.nTop - wr2.nTop;

            if (wr1.nHeight > opr.nMaxHeight)
                wr1.nHeight     = opr.nMaxHeight;
            if (wr2.nHeight > opr.nMaxHeight)
            {
                wr2.nTop       += wr2.nHeight - opr.nMaxHeight;
                wr2.nHeight     = opr.nMaxHeight;
            }

            if ((wr1.nHeight < opr.nMinHeight) && (wr2.nHeight >= opr.nMinHeight))
                pPopup->set_geometry(&wr2);
            else
                pPopup->set_geometry(&wr1);

            // Finally, show the popup window
            sListBox.show();
            sListBox.set_focus();
            pPopup->show(this);
            pPopup->grab_events(GRAB_DROPDOWN);
            nCBFlags |= F_OPENED;

            return STATUS_OK;
        }

        void LSPComboGroup::set_circular(bool circular)
        {
            if (circular)
                nCBFlags     |= F_CIRCULAR;
            else
                nCBFlags     &= ~F_CIRCULAR;
        }

        void LSPComboGroup::set_embed(bool embed)
        {
            if (bEmbed == embed)
                return;
            bEmbed = embed;
            query_resize();
        }

        status_t LSPComboGroup::on_list_change()
        {
            return sSlots.execute(LSPSLOT_CHANGE, this);
        }

        status_t LSPComboGroup::on_list_submit()
        {
            if (!(nCBFlags & F_OPENED))
                return STATUS_OK;
            close();
            return sSlots.execute(LSPSLOT_SUBMIT, this);
        }


        status_t LSPComboGroup::slot_on_change(LSPWidget *sender, void *ptr, void *data)
        {
            LSPComboGroup *_this = widget_ptrcast<LSPComboGroup>(ptr);
            return (ptr != NULL) ? _this->on_change() : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPComboGroup::slot_on_submit(LSPWidget *sender, void *ptr, void *data)
        {
            LSPComboGroup *_this = widget_ptrcast<LSPComboGroup>(ptr);
            return (ptr != NULL) ? _this->on_submit() : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPComboGroup::slot_on_list_change(LSPWidget *sender, void *ptr, void *data)
        {
            LSPComboGroup *_this = widget_ptrcast<LSPComboGroup>(ptr);
            return (ptr != NULL) ? _this->on_list_change() : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPComboGroup::slot_on_list_submit(LSPWidget *sender, void *ptr, void *data)
        {
            LSPComboGroup *_this = widget_ptrcast<LSPComboGroup>(ptr);
            return (ptr != NULL) ? _this->on_list_submit() : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPComboGroup::slot_on_list_focus_out(LSPWidget *sender, void *ptr, void *data)
        {
            LSPComboGroup *_this = widget_ptrcast<LSPComboGroup>(ptr);
            return (ptr != NULL) ? _this->on_list_focus_out() : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPComboGroup::slot_on_list_mouse_down(LSPWidget *sender, void *ptr, void *data)
        {
            LSPComboGroup *_this = widget_ptrcast<LSPComboGroup>(ptr);
            return (ptr != NULL) ? _this->on_grab_mouse_down(static_cast<ws_event_t *>(data)) : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPComboGroup::slot_on_list_key_down(LSPWidget *sender, void *ptr, void *data)
        {
            LSPComboGroup *_this = widget_ptrcast<LSPComboGroup>(ptr);
            return (ptr != NULL) ? _this->on_grab_key_down(static_cast<ws_event_t *>(data)) : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPComboGroup::slot_on_list_show(LSPWidget *sender, void *ptr, void *data)
        {
            LSPComboGroup *_this = widget_ptrcast<LSPComboGroup>(ptr);
            return (ptr != NULL) ? _this->on_list_show() : STATUS_BAD_ARGUMENTS;
        }

        void LSPComboGroup::on_selection_change()
        {
            query_resize();
//            query_draw();
        }

        void LSPComboGroup::on_item_change(size_t index, LSPItem *item)
        {
            ssize_t sel = sListBox.selection()->value();
            if ((sel >= 0) && (index == size_t(sel)))
                query_draw();
        }

        void LSPComboGroup::on_item_add(size_t index)
        {
        }

        void LSPComboGroup::on_item_remove(size_t index)
        {
            ssize_t sel = sListBox.selection()->value();
            if ((sel >= 0) && (index == size_t(sel)))
                query_draw();
        }

        void LSPComboGroup::on_item_swap(size_t idx1, size_t idx2)
        {
            ssize_t sel = sListBox.selection()->value();
            if (sel < 0)
                return;
            if ((idx1 == size_t(sel)) || (idx2 == size_t(sel)))
                query_draw();
        }

        void LSPComboGroup::on_item_clear()
        {
            query_draw();
        }

        status_t LSPComboGroup::on_list_focus_out()
        {
            lsp_trace("focus_out triggered");
            return STATUS_OK;
        }

        status_t LSPComboGroup::on_list_show()
        {
//            if (pPopup != NULL)
//                pPopup->take_focus();
            return STATUS_OK;
        }

        status_t LSPComboGroup::on_grab_mouse_down(const ws_event_t *e)
        {
            lsp_trace("mouse_down triggered left=%d, top=%d", int(e->nLeft), int(e->nTop));
            if ((e->nLeft < 0) || (e->nTop < 0) || (e->nLeft > pPopup->width()) || (e->nTop > pPopup->height()))
                close();

            return STATUS_OK;
        }

        status_t LSPComboGroup::on_grab_key_down(const ws_event_t *e)
        {
            lsp_trace("key_down triggered left=%d, top=%d", int(e->nLeft), int(e->nTop));
            close();
            return STATUS_OK;
        }
    } /* namespace tk */
} /* namespace lsp */
