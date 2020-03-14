/*
 * LSPComboBox.cpp
 *
 *  Created on: 31 июл. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPComboBox::metadata = { "LSPComboBox", &LSPWidget::metadata };

        LSPComboBox::LSPComboList::LSPComboList(LSPDisplay *dpy, LSPComboBox *widget): LSPListBox(dpy)
        {
            pWidget = widget;
        }

        LSPComboBox::LSPComboList::~LSPComboList()
        {
        }

        LSPComboBox::LSPComboPopup::LSPComboPopup(LSPDisplay *dpy, LSPComboBox *widget, ssize_t screen):
            LSPWindow(dpy, NULL, screen)
        {
            pWidget = widget;
        }

        LSPComboBox::LSPComboPopup::~LSPComboPopup()
        {
        }

        status_t LSPComboBox::LSPComboPopup::handle_event(const ws_event_t *e)
        {
            switch (e->nType)
            {
                case UIE_KEY_DOWN:
                    pWidget->on_grab_key_down(e);
                    break;
            }
            return LSPWindow::handle_event(e);
        }

//        void LSPComboBox::LSPComboPopup::size_request(size_request_t *r)
//        {
//            LSPWindow::size_request(r);
//            size_request_t sr;
//            pWidget->sListBox.optimal_size_request(&sr);
//            if ((r->nMinWidth > 0) && (r->nMinWidth > sr.nMinWidth))
//                r->nMinWidth    = sr.nMinWidth;
//            if ((r->nMinHeight > 0) && (r->nMinHeight > sr.nMinHeight))
//                r->nMinHeight   = sr.nMinHeight;
//            if ((r->nMaxWidth > 0) && (r->nMaxWidth > sr.nMaxWidth))
//                r->nMaxWidth    = sr.nMaxWidth;
//            if ((r->nMaxHeight > 0) && (r->nMaxHeight > sr.nMaxHeight))
//                r->nMaxHeight   = sr.nMaxHeight;
//        }

        void LSPComboBox::LSPComboList::on_selection_change()
        {
            LSPListBox::on_selection_change();
            pWidget->on_selection_change();
        }

        void LSPComboBox::LSPComboList::on_item_change(ssize_t index, LSPItem *item)
        {
            LSPListBox::on_item_change(index, item);
            pWidget->on_item_change(index, item);
        }

        void LSPComboBox::LSPComboList::on_item_add(size_t index)
        {
            LSPListBox::on_item_add(index);
            pWidget->on_item_add(index);
        }

        void LSPComboBox::LSPComboList::on_item_remove(size_t index)
        {
            LSPListBox::on_item_remove(index);
            pWidget->on_item_remove(index);
        }

        void LSPComboBox::LSPComboList::on_item_swap(size_t idx1, size_t idx2)
        {
            LSPListBox::on_item_swap(idx1, idx2);
            pWidget->on_item_swap(idx1, idx2);
        }

        void LSPComboBox::LSPComboList::on_item_clear()
        {
            LSPListBox::on_item_clear();
            pWidget->on_item_clear();
        }

        LSPComboBox::LSPComboBox(LSPDisplay *dpy):
            LSPWidget(dpy),
            sListBox(dpy, this),
            sFont(dpy, this)
        {
            nCBFlags      = 0;
            nMinWidth   = -1;
            nMinHeight  = -1;
            nMFlags     = 0;
            pPopup      = NULL;
            pClass      = &metadata;
        }

        LSPComboBox::~LSPComboBox()
        {
            do_destroy();
        }

        void LSPComboBox::do_destroy()
        {
            if (pPopup != NULL)
            {
//                pDisplay->remove(pPopup);
                pPopup->destroy();
                delete pPopup;
                pPopup = NULL;
            }

            sListBox.destroy();
        }

        status_t LSPComboBox::slot_on_change(LSPWidget *sender, void *ptr, void *data)
        {
            LSPComboBox *_this = widget_ptrcast<LSPComboBox>(ptr);
            return (ptr != NULL) ? _this->on_change() : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPComboBox::slot_on_submit(LSPWidget *sender, void *ptr, void *data)
        {
            LSPComboBox *_this = widget_ptrcast<LSPComboBox>(ptr);
            return (ptr != NULL) ? _this->on_submit() : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPComboBox::slot_on_list_change(LSPWidget *sender, void *ptr, void *data)
        {
            LSPComboBox *_this = widget_ptrcast<LSPComboBox>(ptr);
            return (ptr != NULL) ? _this->on_list_change() : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPComboBox::slot_on_list_submit(LSPWidget *sender, void *ptr, void *data)
        {
            LSPComboBox *_this = widget_ptrcast<LSPComboBox>(ptr);
            return (ptr != NULL) ? _this->on_list_submit() : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPComboBox::slot_on_list_focus_out(LSPWidget *sender, void *ptr, void *data)
        {
            LSPComboBox *_this = widget_ptrcast<LSPComboBox>(ptr);
            return (ptr != NULL) ? _this->on_list_focus_out() : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPComboBox::slot_on_list_mouse_down(LSPWidget *sender, void *ptr, void *data)
        {
            LSPComboBox *_this = widget_ptrcast<LSPComboBox>(ptr);
            return (ptr != NULL) ? _this->on_grab_mouse_down(static_cast<ws_event_t *>(data)) : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPComboBox::slot_on_list_key_down(LSPWidget *sender, void *ptr, void *data)
        {
            LSPComboBox *_this = widget_ptrcast<LSPComboBox>(ptr);
            return (ptr != NULL) ? _this->on_grab_key_down(static_cast<ws_event_t *>(data)) : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPComboBox::slot_on_list_show(LSPWidget *sender, void *ptr, void *data)
        {
            LSPComboBox *_this = widget_ptrcast<LSPComboBox>(ptr);
            return (ptr != NULL) ? _this->on_list_show() : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPComboBox::on_list_change()
        {
            return sSlots.execute(LSPSLOT_CHANGE, this);
        }

        status_t LSPComboBox::on_list_submit()
        {
            if (!(nCBFlags & F_OPENED))
                return STATUS_OK;
            close();
            return sSlots.execute(LSPSLOT_SUBMIT, this);
        }

        status_t LSPComboBox::on_change()
        {
            return STATUS_OK;
        }

        status_t LSPComboBox::on_submit()
        {
            lsp_trace("on_submit");
            close();
            return STATUS_OK;
        }

        status_t LSPComboBox::init()
        {
            status_t result = LSPWidget::init();
            if (result != STATUS_OK)
                return result;

            result = sListBox.init();
            if (result != STATUS_OK)
                return result;

            sFont.init();
            sFont.set_size(12.0f);

            // Bind slots
            ui_handler_id_t id = 0;
            id = sSlots.add(LSPSLOT_CHANGE, slot_on_change, self());
            if (id >= 0) id = sSlots.add(LSPSLOT_SUBMIT, slot_on_submit, self());
            if (id >= 0) id = sListBox.slots()->bind(LSPSLOT_CHANGE, slot_on_list_change, self());
            if (id >= 0) id = sListBox.slots()->bind(LSPSLOT_SUBMIT, slot_on_list_submit, self());

            return (id >= 0) ? STATUS_OK : -id;
        }

        void LSPComboBox::destroy()
        {
            do_destroy();
            LSPWidget::destroy();
        }

        ssize_t LSPComboBox::selected() const
        {
            LSPComboList *lb = const_cast<LSPComboList *>(&sListBox);
            return lb->selection()->value();
        }

        status_t LSPComboBox::set_selected(ssize_t value)
        {
            return sListBox.selection()->set_value(value);
        }

        void LSPComboBox::set_min_width(ssize_t value)
        {
            if (value == nMinWidth)
                return;
            nMinWidth   = value;
            query_resize();
        }

        void LSPComboBox::set_min_height(ssize_t value)
        {
            if (value == nMinHeight)
                return;
            nMinHeight = value;
            query_resize();
        }

        void LSPComboBox::set_circular(bool circular)
        {
            if (circular)
                nCBFlags     |= F_CIRCULAR;
            else
                nCBFlags     &= ~F_CIRCULAR;
        }

        status_t LSPComboBox::set_opened(bool open)
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

            wr1.nLeft       = r.nLeft + sSize.nLeft;
            wr1.nTop        = r.nTop + sSize.nTop + sSize.nHeight;
            wr1.nWidth      = opr.nMaxWidth;
            wr1.nHeight     = sh - wr1.nTop;

            if (wr1.nLeft < 0)
                wr1.nLeft       = 0;
            if ((wr1.nWidth < sSize.nWidth) && (sSize.nWidth < sw))
                wr1.nWidth      = sSize.nWidth;
            if (wr1.nWidth > sw)
                wr1.nWidth      = sw;
            if (wr1.nHeight > sh)
                wr1.nHeight     = sh;
            if (wr1.nHeight > opr.nMaxHeight)
                wr1.nHeight     = opr.nMaxHeight;
            if ((wr1.nLeft + wr1.nWidth) >= sw)
            {
                wr1.nLeft      = sw - wr1.nWidth;
                if (wr1.nLeft < 0)
                    wr1.nLeft   = 0;
            }

            wr2.nLeft       = wr1.nLeft;
            wr2.nTop        = 0;
            wr2.nWidth      = wr1.nWidth;
            wr2.nHeight     = r.nTop + sSize.nTop - wr2.nTop;

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

        status_t LSPComboBox::on_mouse_down(const ws_event_t *e)
        {
            take_focus();
            nMFlags |= (1 << e->nCode);
            return STATUS_OK;
        }

        status_t LSPComboBox::on_mouse_up(const ws_event_t *e)
        {
            size_t flags = nMFlags;
            nMFlags &= ~(1 << e->nCode);
            if ((e->nCode == MCB_LEFT) && (flags == size_t(1 << e->nCode)))
            {
                if (inside(e->nLeft, e->nTop))
                    toggle();
            }

            return STATUS_OK;
        }

        status_t LSPComboBox::on_mouse_scroll(const ws_event_t *e)
        {
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

        ssize_t LSPComboBox::estimate_max_size(ISurface *s)
        {
            // Get font parameters
            text_parameters_t tp;

            LSPItemList *lst    = sListBox.items();
            ssize_t width       = 0;

            // Estimate the maximum width of the list box
            LSPString str;
            for (size_t i=0, n=lst->size(); i<n; ++i)
            {
                // Fetch item
                LSPItem *item = lst->get(i);
                if (item == NULL)
                    continue;

                // Perform text format
                item->text()->format(&str, this);
                if (str.is_empty())
                    continue;

                // Get text parameters
                sFont.get_text_parameters(s, &tp, &str);
                if (tp.Width > width)
                    width = tp.Width;
            }

            return width;
        }

        void LSPComboBox::size_request(size_request_t *r)
        {
            r->nMinWidth    = -1;
            r->nMinHeight   = -1;
            r->nMaxWidth    = -1;
            r->nMaxHeight   = -1;

            size_t padding  = 3;
            ssize_t bwidth  = 12;

            // Create surface to calculate font parameters
            ISurface *s = pDisplay->create_surface(1, 1);
            if (s == NULL)
                return;

            font_parameters_t fp;
            sFont.get_parameters(s, &fp);

            r->nMinHeight   = fp.Height + padding * 2;
            if (nMinWidth >= 0)
                r->nMinWidth    = nMinWidth;
            else
                r->nMinWidth    = estimate_max_size(s);

            if ((nMinWidth >= 0) && (nMinWidth > r->nMinWidth))
                r->nMinWidth        = nMinWidth;
            if ((nMinHeight >= 0) && (nMinHeight > r->nMinHeight))
                r->nMinHeight        = nMinHeight;

            r->nMinWidth   += bwidth + padding * 2;
            r->nMaxHeight   = r->nMinHeight;

            // Destroy surface
            s->destroy();
            delete s;
        }

        void LSPComboBox::draw(ISurface *s)
        {
            // Prepare palette
            Color bg(sListBox.bg_color()->color());
            Color col(sListBox.color()->color());

            col.scale_lightness(brightness());

            // Draw background
            s->clear(bg);

            // Draw body
            font_parameters_t fp;
            text_parameters_t tp;

            bool aa = s->set_antialiasing(true);
            s->fill_round_rect(0.5f, 0.5f, sSize.nWidth - 1, sSize.nHeight - 1, 4, SURFMASK_ALL_CORNER, col);

            // Get text to print
            LSPString text;
            text.set_ascii("----------------");
            ssize_t sel = sListBox.selection()->value();
            if (sel >= 0)
            {
                LSPItem *item = sListBox.items()->get(sel);
                if (item == NULL)
                    text.clear();
                else
                    item->text()->format(&text, this);
            }

            // Get text and font parameters
            sFont.get_parameters(s, &fp);
            sFont.get_text_parameters(s, &tp, &text);

            size_t padding = 3;
            s->set_antialiasing(aa);
            sFont.draw(s, padding, padding + (sSize.nHeight - padding * 2 - fp.Height)*0.5f + fp.Ascent, bg, &text);

            // Additionally wire around
            ssize_t bwidth = 12;
            s->set_antialiasing(true);
            float bleft = sSize.nWidth - bwidth;
            s->wire_round_rect(0.5f, 0.5f, sSize.nWidth - 1, sSize.nHeight - 1, 4, SURFMASK_ALL_CORNER, 1, col);
            s->fill_round_rect(bleft, 0.0f, 10, sSize.nHeight - 1, 4, SURFMASK_R_CORNER, col);
            s->set_antialiasing(false);
            s->line(bleft, 1, bleft, sSize.nHeight - 2, 1, bg);
            s->set_antialiasing(true);

            // Draw buttons
            size_t half = (sSize.nHeight >> 1);
            s->fill_triangle(
                    bleft + 2, half - 2,
                    sSize.nWidth - 2, half - 2,
                    (bleft + sSize.nWidth)*0.5f, half - 6,
                    bg);

            s->fill_triangle(
                    bleft + 2, half + 1,
                    sSize.nWidth - 2, half + 1,
                    (bleft + sSize.nWidth)*0.5f, half + 5,
                    bg);

            s->set_antialiasing(aa);
        }

        void LSPComboBox::on_selection_change()
        {
            query_draw();
        }

        void LSPComboBox::on_item_change(size_t index, LSPItem *item)
        {
            ssize_t sel = sListBox.selection()->value();
            if ((sel >= 0) && (index == size_t(sel)))
                query_draw();
        }

        void LSPComboBox::on_item_add(size_t index)
        {
        }

        void LSPComboBox::on_item_remove(size_t index)
        {
            ssize_t sel = sListBox.selection()->value();
            if ((sel >= 0) && (index == size_t(sel)))
                query_draw();
        }

        void LSPComboBox::on_item_swap(size_t idx1, size_t idx2)
        {
            ssize_t sel = sListBox.selection()->value();
            if (sel < 0)
                return;
            if ((idx1 == size_t(sel)) || (idx2 == size_t(sel)))
                query_draw();
        }

        void LSPComboBox::on_item_clear()
        {
            query_draw();
        }

        status_t LSPComboBox::on_list_focus_out()
        {
            lsp_trace("focus_out triggered");
            return STATUS_OK;
        }

        status_t LSPComboBox::on_list_show()
        {
//            if (pPopup != NULL)
//                pPopup->take_focus();
            return STATUS_OK;
        }

        status_t LSPComboBox::on_grab_mouse_down(const ws_event_t *e)
        {
            lsp_trace("mouse_down triggered left=%d, top=%d", int(e->nLeft), int(e->nTop));
            if ((e->nLeft < 0) || (e->nTop < 0) || (e->nLeft > pPopup->width()) || (e->nTop > pPopup->height()))
                close();

            return STATUS_OK;
        }

        status_t LSPComboBox::on_grab_key_down(const ws_event_t *e)
        {
            lsp_trace("key_down triggered left=%d, top=%d", int(e->nLeft), int(e->nTop));
            close();
            return STATUS_OK;
        }
    } /* namespace tk */
} /* namespace lsp */
