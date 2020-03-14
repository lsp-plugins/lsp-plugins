/*
 * LSPListBox.cpp
 *
 *  Created on: 2 авг. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPListBox::metadata = { "LSPListBox", &LSPComplexWidget::metadata };

        //-----------------------------------------------------------------------------
        // LSPListBoxList implementation
        LSPListBox::LSPListBoxList::LSPListBoxList(LSPListBox *widget)
        {
            pWidget     = widget;
        }

        LSPListBox::LSPListBoxList::~LSPListBoxList()
        {
            pWidget     = NULL;
        }

        void LSPListBox::LSPListBoxList::on_item_change(LSPListItem *item)
        {
            ssize_t index = pWidget->items()->index_of(item);
            if (index >= 0)
                pWidget->on_item_change(index, item);
        }

        void LSPListBox::LSPListBoxList::on_item_add(size_t index)
        {
            pWidget->on_item_add(index);
        }

        void LSPListBox::LSPListBoxList::on_item_remove(size_t index)
        {
            pWidget->on_item_remove(index);
        }

        void LSPListBox::LSPListBoxList::on_item_swap(size_t idx1, size_t idx2)
        {
            pWidget->on_item_swap(idx1, idx2);
        }

        void LSPListBox::LSPListBoxList::on_item_clear()
        {
            pWidget->on_item_clear();
        }

        //-----------------------------------------------------------------------------
        // LSPListBoxSelection implementation

        LSPListBox::LSPListBoxSelection::LSPListBoxSelection(LSPListBox *widget)
        {
            pWidget     = widget;
        }

        LSPListBox::LSPListBoxSelection::~LSPListBoxSelection()
        {
            pWidget     = NULL;
        }

        void LSPListBox::LSPListBoxSelection::on_remove(ssize_t value)
        {
            float fh      = pWidget->sFont.height();
            ssize_t first = pWidget->sVBar.value() / fh;
            ssize_t last  = (pWidget->sVBar.value() + pWidget->sArea.nHeight + fh - 1) / fh;

            if ((value >= first) || (value <= last))
                pWidget->query_draw();

            pWidget->on_selection_change();
        }

        void LSPListBox::LSPListBoxSelection::on_add(ssize_t value)
        {
            float fh      = pWidget->sFont.height();
            ssize_t first = pWidget->sVBar.value() / fh;
            ssize_t last  = (pWidget->sVBar.value() + pWidget->sArea.nHeight + fh) / fh;

            if ((value >= first) || (value <= last))
                pWidget->query_draw();

            pWidget->on_selection_change();
        }

        bool LSPListBox::LSPListBoxSelection::validate(ssize_t value)
        {
            if (pWidget == NULL)
                return false;
            return (value >= 0) && (value < ssize_t(pWidget->sItems.size()));
        }

        void LSPListBox::LSPListBoxSelection::request_fill(ssize_t *first, ssize_t *last)
        {
            *first  = 0;
            *last   = (pWidget != NULL) ? pWidget->sItems.size() - 1 : -1;
        }

        void LSPListBox::LSPListBoxSelection::on_fill()
        {
            pWidget->query_draw();
            pWidget->on_selection_change();
        }

        void LSPListBox::LSPListBoxSelection::on_clear()
        {
            pWidget->query_draw();
            pWidget->on_selection_change();
        }

        //-----------------------------------------------------------------------------
        // LSPListBox implementation
        LSPListBox::LSPListBox(LSPDisplay *dpy):
            LSPComplexWidget(dpy),
            sItems(this),
            sSelection(this),
            sHBar(dpy, true),
            sVBar(dpy, false),
            sConstraints(this),
            sColor(this),
            sFont(this)
        {
            nFlags              = 0;
            nBMask              = 0;
            pArea               = NULL;

            pClass              = &metadata;
        }

        LSPListBox::~LSPListBox()
        {
            do_destroy();
        }

        bool LSPListBox::hide()
        {
            bool result = LSPComplexWidget::hide();

            if (result)
            {
                // Drop area to not to eat memory
                if (pArea != NULL)
                {
                    pArea->destroy();
                    delete pArea;
                    pArea   = NULL;
                }
            }
            return result;
        }

        void LSPListBox::do_destroy()
        {
            // Clear contents
            sItems.clear();
            sSelection.clear();
            sHBar.destroy();
            sVBar.destroy();

            // Drop area to not to eat memory
            if (pArea != NULL)
            {
                pArea->destroy();
                delete pArea;
                pArea   = NULL;
            }
        }

        status_t LSPListBox::init()
        {
            status_t result = LSPWidget::init();
            if (result != STATUS_OK)
                return result;

            init_color(C_LABEL_TEXT, &sColor);
            init_color(C_LABEL_TEXT, sFont.color());

            result = sHBar.init();
            if (result != STATUS_OK)
                return result;
            result = sVBar.init();
            if (result != STATUS_OK)
                return result;

            sVBar.set_parent(this);
            sHBar.set_parent(this);
            sVBar.hide();
            sHBar.hide();

            sFont.init();
            sFont.set_size(12);

            // Bind slots
            ui_handler_id_t id = 0;
            id = sSlots.add(LSPSLOT_CHANGE, slot_on_change, self());
            if (id >= 0) id = sSlots.add(LSPSLOT_SUBMIT, slot_on_submit, self());
            if (id >= 0) id = sSlots.add(LSPSLOT_HSCROLL, slot_on_hscroll, self());
            if (id >= 0) id = sSlots.add(LSPSLOT_VSCROLL, slot_on_vscroll, self());
            if (id >= 0) id = sVBar.slots()->bind(LSPSLOT_CHANGE, slot_on_sbar_vscroll, self());
            if (id >= 0) id = sHBar.slots()->bind(LSPSLOT_CHANGE, slot_on_sbar_hscroll, self());

            return (id >= 0) ? STATUS_OK : -id;
        }

        void LSPListBox::on_item_change(size_t index, LSPItem *item)
        {
            float fh      = sFont.height();
            ssize_t first = sVBar.value() / fh;
            ssize_t last  = (sVBar.value() + sArea.nHeight + fh - 1) / fh;

            if ((ssize_t(index) >= first) || (ssize_t(index) <= last))
                query_draw();
        }

        void LSPListBox::on_item_add(size_t index)
        {
            realize(&sSize);
            query_resize();
        }

        void LSPListBox::on_item_remove(size_t index)
        {
            realize(&sSize);
            query_resize();
        }

        void LSPListBox::on_item_swap(size_t idx1, size_t idx2)
        {
            float fh      = sFont.height();
            ssize_t first = sVBar.value() / fh;
            ssize_t last  = (sVBar.value() + sArea.nHeight + fh - 1) / fh;

            if ((ssize_t(idx1) >= first) || (ssize_t(idx1) <= last) || (ssize_t(idx2) >= first) || (ssize_t(idx2) <= last))
                query_draw();
        }

        void LSPListBox::on_item_clear()
        {
            realize(&sSize);
            query_resize();
        }

        void LSPListBox::on_selection_change()
        {
        }

        void LSPListBox::destroy()
        {
            do_destroy();
            LSPWidget::destroy();
        }

        status_t LSPListBox::slot_on_sbar_vscroll(LSPWidget *sender, void *ptr, void *data)
        {
            if (ptr == NULL)
                return STATUS_BAD_ARGUMENTS;

            LSPWidget *w    = static_cast<LSPWidget *>(ptr);
            return w->slots()->execute(LSPSLOT_VSCROLL, sender, data);
        }

        status_t LSPListBox::slot_on_sbar_hscroll(LSPWidget *sender, void *ptr, void *data)
        {
            if (ptr == NULL)
                return STATUS_BAD_ARGUMENTS;

            LSPWidget *w    = static_cast<LSPWidget *>(ptr);
            return w->slots()->execute(LSPSLOT_HSCROLL, sender, data);
        }

        status_t LSPListBox::slot_on_change(LSPWidget *sender, void *ptr, void *data)
        {
            LSPListBox *_this = widget_ptrcast<LSPListBox>(ptr);
            return (_this != NULL) ? _this->on_change() : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPListBox::slot_on_submit(LSPWidget *sender, void *ptr, void *data)
        {
            LSPListBox *_this = widget_ptrcast<LSPListBox>(ptr);
            return (_this != NULL) ? _this->on_submit() : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPListBox::slot_on_vscroll(LSPWidget *sender, void *ptr, void *data)
        {
            LSPListBox *_this = widget_ptrcast<LSPListBox>(ptr);
            return (_this != NULL) ? _this->on_vscroll() : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPListBox::slot_on_hscroll(LSPWidget *sender, void *ptr, void *data)
        {
            LSPListBox *_this = widget_ptrcast<LSPListBox>(ptr);
            return (_this != NULL) ? _this->on_hscroll() : STATUS_BAD_ARGUMENTS;
        }

        LSPWidget *LSPListBox::find_widget(ssize_t x, ssize_t y)
        {
            if (sHBar.visible() && sHBar.inside(x, y))
                return &sHBar;
            if (sVBar.visible() && sVBar.inside(x, y))
                return &sVBar;
            return NULL;
        }

        status_t LSPListBox::on_change()
        {
            return STATUS_OK;
        }

        status_t LSPListBox::on_submit()
        {
            return STATUS_OK;
        }

        void LSPListBox::on_click(ssize_t x, ssize_t y)
        {
            lsp_trace("x=%d, y=%d, area={%d, %d, %d, %d}",
                int(x), int(y), int(sArea.nLeft), int(sArea.nTop), int(sArea.nLeft + sArea.nWidth), int (sArea.nTop + sArea.nHeight));
            if ((x < sArea.nLeft) || (x >= (sArea.nLeft + sArea.nWidth)))
                return;
            else if ((y < sArea.nTop) || (y >= (sArea.nTop + sArea.nHeight)))
                return;

            x       = x - sArea.nLeft;
            y       = y - sArea.nTop + sVBar.value();

            float fh      = sFont.height();
            ssize_t item    = y / fh;
            lsp_trace("toggled item = %d", int(item));
            if (sSelection.multiple())
            {
                sSelection.toggle_value(item);
                sSlots.execute(LSPSLOT_CHANGE, this);
            }
            else
            {
                ssize_t old_value = sSelection.value();
                sSelection.set_value(item);
                if (old_value != item)
                    sSlots.execute(LSPSLOT_CHANGE, this);
            }

            nFlags |= F_SUBMIT;
        }

        status_t LSPListBox::on_mouse_down(const ws_event_t *e)
        {
            lsp_trace("x=%d, y=%d, code=%x, bmask=%lx", int(e->nLeft), int(e->nTop), int(e->nCode), long(nBMask));
            take_focus();
            size_t mask = nBMask;
            nBMask      = mask | (1 << e->nCode);

            if ((mask == 0) && (e->nCode == MCB_LEFT))
            {
                nFlags |= F_MDOWN;
                on_click(e->nLeft, e->nTop);
            }

            return STATUS_OK;
        }

        status_t LSPListBox::on_mouse_up(const ws_event_t *e)
        {
            lsp_trace("x=%d, y=%d, code=%x, bmask=%lx", int(e->nLeft), int(e->nTop), int(e->nCode), long(nBMask));
            nBMask      = nBMask & (~(1 << e->nCode));
            if (nBMask == 0)
                nFlags      &= ~F_MDOWN;

            if (nFlags & F_SUBMIT)
            {
                nFlags      &= ~F_SUBMIT;
                sSlots.execute(LSPSLOT_SUBMIT, this);
            }
            return STATUS_OK;
        }

        status_t LSPListBox::on_mouse_move(const ws_event_t *e)
        {
            if (sSelection.multiple())
                return STATUS_OK;

            if (nBMask == (1 << MCB_LEFT))
                on_click(e->nLeft, e->nTop);

            return STATUS_OK;
        }

        status_t LSPListBox::on_mouse_scroll(const ws_event_t *e)
        {
            if (e->nState & MCF_CONTROL)
            {
                ws_event_t xe = *e;
                xe.nState &= ~MCF_CONTROL;
                sHBar.handle_event(&xe);
            }
            else
                sVBar.handle_event(e);

            return STATUS_OK;
        }

        status_t LSPListBox::on_hscroll()
        {
            query_draw();
            return STATUS_OK;
        }

        status_t LSPListBox::on_vscroll()
        {
            query_draw();
            return STATUS_OK;
        }

        void LSPListBox::render(ISurface *s, bool force)
        {
            // Check dirty flag
            if (nFlags & REDRAW_SURFACE)
                force = true;

            // Draw list box
            ISurface *lst = get_surface(s, sArea.nWidth, sArea.nHeight);
            if (lst != NULL)
                s->draw(lst, sArea.nLeft, sArea.nTop);

            // Prepare palette
            Color bg_color(sBgColor);
            Color color(sColor);
            color.scale_lightness(brightness());

            // Draw the frame around
            size_t dx = (sVBar.visible()) ? 7 : 6;
            size_t dy = (sHBar.visible()) ? 7 : 6;

            s->fill_frame(sSize.nLeft, sSize.nTop, sArea.nWidth + dx, sArea.nHeight + dy,
                        sArea.nLeft, sArea.nTop, sArea.nWidth, sArea.nHeight,
                        bg_color);

            bool aa = s->set_antialiasing(true);
            s->wire_round_rect(sSize.nLeft + 0.5f, sSize.nTop + 0.5f, sArea.nWidth + 5, sArea.nHeight + 5, 2, SURFMASK_ALL_CORNER, 1, color);
            s->set_antialiasing(aa);

            // Finally, draw scroll bars
            if (sHBar.visible())
            {
                if ((sHBar.redraw_pending()) || (force))
                {
                    sHBar.render(s, false);
                    sHBar.commit_redraw();
                }
            }
            if (sVBar.visible())
            {
                if ((sVBar.redraw_pending()) || (force))
                {
                    sVBar.render(s, false);
                    sVBar.commit_redraw();
                }
            }
        }

        void LSPListBox::draw(ISurface *s)
        {
            // Prepare palette
            Color bg_color(sBgColor);
            Color color(sColor);
            Color font(sFont.raw_color());

            color.scale_lightness(brightness());
            font.scale_lightness(brightness());

            // Draw background
            s->clear(bg_color);

            // Draw
            font_parameters_t fp;
            sFont.get_parameters(s, &fp);

            ssize_t first = sVBar.value() / fp.Height;
            ssize_t last  = (sVBar.value() + sArea.nHeight + fp.Height - 1) / fp.Height;
            ssize_t y     = first * fp.Height - sVBar.value();

            LSPString text;
            for ( ; first <= last; first++, y += fp.Height)
            {
                LSPItem *item = sItems.get(first);
                if (item == NULL)
                    continue;

                item->text()->format(&text, this);
                if (sSelection.contains(first))
                {
                    s->fill_rect(0.0f, y, sArea.nWidth, fp.Height, font);
                    if (text.length() > 0)
                        sFont.draw(s, 1.0f, y + fp.Ascent, bg_color, &text);
                }
                else if (text.length() > 0)
                    sFont.draw(s, 1.0f, y + fp.Ascent, font, &text);
            }
        }

        void LSPListBox::size_request(size_request_t *r)
        {
            size_request_t hbar, vbar;
            hbar.nMinWidth   = -1;
            hbar.nMinHeight  = -1;
            hbar.nMaxWidth   = -1;
            hbar.nMaxHeight  = -1;
            vbar.nMinWidth   = -1;
            vbar.nMinHeight  = -1;
            vbar.nMaxWidth   = -1;
            vbar.nMaxHeight  = -1;

            sHBar.size_request(&hbar);
            sVBar.size_request(&vbar);

            // Estimate minimum size of bars
            ssize_t width = 0, height = 0;
            if (hbar.nMinWidth >= 0)
                width       += hbar.nMinWidth;
            if (vbar.nMinWidth >= 0)
                width       += vbar.nMinWidth;

            if (hbar.nMinHeight >= 0)
                height      += hbar.nMinHeight;
            if (vbar.nMinHeight >= 0)
                height      += vbar.nMinHeight;

            size_t padding  = 6;
            size_t n_items  = sItems.size();
            if (n_items <= 0)
                n_items ++;
            ssize_t i_height = sFont.height() * n_items + padding;
            if (height > i_height)
                height = i_height;

            // Fill final values
            r->nMinWidth    = width;
            r->nMinHeight   = height;
            r->nMaxWidth    = -1;
            r->nMaxHeight   = -1;

            // Apply constraints
            sConstraints.apply(r);
        }

        void LSPListBox::optimal_size_request(size_request_t *r)
        {
            r->nMinWidth    = 0;
            r->nMinHeight   = 0;
            r->nMaxWidth    = 0;
            r->nMaxHeight   = 0;

            font_parameters_t fp;
            text_parameters_t tp;

            ISurface *s = pDisplay->create_surface(1, 1);
            if (s == NULL)
                return;

            sFont.get_parameters(&fp);

            size_t padding  = 6;
            size_t n_items  = sItems.size();

            LSPString text;
            for (size_t i=0; i<n_items; ++i)
            {
                LSPItem *item = sItems.get(i);
                if (item == NULL)
                    continue;

                item->text()->format(&text, this);
                if (text.is_empty())
                    continue;

                sFont.get_text_parameters(s, &tp, &text);
                if (tp.Width > r->nMaxWidth)
                    r->nMaxWidth    = tp.Width;
            }

            r->nMaxHeight    = fp.Height * n_items + padding;

            size_request_t vbar;
            vbar.nMinWidth   = -1;
            vbar.nMinHeight  = -1;
            vbar.nMaxWidth   = -1;
            vbar.nMaxHeight  = -1;

            sVBar.size_request(&vbar);
            if (vbar.nMinWidth > 0)
                r->nMinWidth    =  vbar.nMinWidth * 2;
            if (n_items > 2)
                n_items         = 4;
            r->nMinHeight       = fp.Height * n_items + padding*2;
            if (r->nMaxWidth < r->nMinWidth)
                r->nMaxWidth = r->nMinWidth;
            if (r->nMaxHeight < r->nMinHeight)
                r->nMaxHeight = r->nMinHeight;

            s->destroy();
            delete s;
        }

        void LSPListBox::realize(const realize_t *r)
        {
            size_request_t hbar, vbar;
            hbar.nMinWidth   = -1;
            hbar.nMinHeight  = -1;
            hbar.nMaxWidth   = -1;
            hbar.nMaxHeight  = -1;
            vbar.nMinWidth   = -1;
            vbar.nMinHeight  = -1;
            vbar.nMaxWidth   = -1;
            vbar.nMaxHeight  = -1;

            sHBar.size_request(&hbar);
            sVBar.size_request(&vbar);

            size_t padding = 3;

            size_t n_items  = sItems.size();
            if (n_items <= 0)
                n_items ++;
            ssize_t i_height = sFont.height() * n_items + padding*2;

            bool vb     = ssize_t(r->nHeight) < i_height;
            bool hb     = false; // TODO

            realize_t   rh, rv;

            // Estimate size for vertical and horizontal scroll bars
            if (vb)
            {
                rv.nWidth   = (vbar.nMinWidth > 0) ? vbar.nMinWidth : 12;
                rv.nLeft    = r->nLeft + r->nWidth - rv.nWidth;
                rv.nTop     = r->nTop;
                rv.nHeight  = r->nHeight;
            }
            else
                rv.nWidth   = 0;

            if (hb)
            {
                rh.nHeight  = (hbar.nMinHeight > 0) ? hbar.nMinHeight : 12;
                rh.nLeft    = r->nLeft;
                rh.nTop     = r->nTop + r->nHeight - rh.nHeight;
                rh.nWidth   = r->nWidth;
            }
            else
                rh.nHeight      = 0;

            if (vb && hb)
            {
                rv.nHeight -= rh.nHeight;
                rh.nWidth  -= rv.nWidth;
            }

            if (vb)
            {
                // Realize and show
                sVBar.realize(&rv);
                sVBar.show();
                sVBar.query_draw();
            }
            else
            {
                sVBar.hide();
                sVBar.set_value(0.0f);
            }

            if (hb)
            {
                sHBar.realize(&rh);
                sHBar.show();
                sHBar.query_draw();
            }
            else
            {
                sHBar.hide();
                sHBar.set_value(0.0f);
            }

            // Remember drawing area parameters
            sArea.nLeft     = r->nLeft + padding;
            sArea.nTop      = r->nTop + padding;
            sArea.nWidth    = r->nWidth - rv.nWidth - padding*2;
            sArea.nHeight   = r->nHeight - rh.nHeight - padding*2;
            if (vb)
                sArea.nWidth --;
            if (hb)
                sArea.nHeight --;

            if (vb)
            {
                // Set scrolling parameters
                sVBar.set_min_value(0.0f);
                sVBar.set_max_value(i_height - r->nHeight + padding*2);
                sVBar.set_tiny_step(sFont.height());
                sVBar.set_step(sArea.nHeight - sArea.nHeight%ssize_t(sFont.height()));
            }
            else
            {
                sVBar.set_min_value(0.0f);
                sVBar.set_max_value(0.0f);
            }

            // Call parent method
            LSPComplexWidget::realize(r);
        }
    } /* namespace tk */
} /* namespace lsp */
