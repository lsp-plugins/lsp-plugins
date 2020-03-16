/*
 * LSPFraction.cpp
 *
 *  Created on: 29 мая 2018 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPFraction::metadata = { "LSPFraction", &LSPComplexWidget::metadata };
        
        LSPFraction::LSPFraction(LSPDisplay *dpy): LSPComplexWidget(dpy),
                sNumerator(dpy),
                sDenominator(dpy),
                sFont(this),
                sColor(this)
        {
            nMinWidth       = -1;
            nMinHeight      = -1;
            nTextBorder     = 2;
            fAngle          = 60.0f;

            sNum.nLeft      = -1;
            sNum.nTop       = -1;
            sNum.nWidth     = -1;
            sNum.nHeight    = -1;

            sDenom.nLeft    = -1;
            sDenom.nTop     = -1;
            sDenom.nWidth   = -1;
            sDenom.nHeight  = -1;

            nMFlags         = 0;
            nMState         = 0;
            pClass          = &metadata;
        }
        
        LSPFraction::~LSPFraction()
        {
            do_destroy();
        }

        void LSPFraction::do_destroy()
        {
            sNumerator.destroy();
            sDenominator.destroy();
        }

        status_t LSPFraction::init()
        {
            LSP_STATUS_ASSERT(LSPWidget::init());
            LSP_STATUS_ASSERT(sNumerator.init());
            LSP_STATUS_ASSERT(sDenominator.init());

            init_color(C_LABEL_TEXT, sFont.color());
            init_color(C_LABEL_TEXT, &sColor);

            sFont.init();
            sFont.set_bold(true);
            sFont.set_size(14.0f);

            sNumerator.set_parent(this);
            sDenominator.set_parent(this);

            // Bind slots
            ui_handler_id_t id = 0;
            id = sSlots.add(LSPSLOT_CHANGE, slot_on_change, self());
            if (id >= 0) id = sSlots.add(LSPSLOT_SUBMIT, slot_on_submit, self());
            if (id >= 0) id = sNumerator.slots()->bind(LSPSLOT_CHANGE, slot_on_list_change, self());
            if (id >= 0) id = sNumerator.slots()->bind(LSPSLOT_SUBMIT, slot_on_list_submit, self());
            if (id >= 0) id = sDenominator.slots()->bind(LSPSLOT_CHANGE, slot_on_list_change, self());
            if (id >= 0) id = sDenominator.slots()->bind(LSPSLOT_SUBMIT, slot_on_list_submit, self());

            return (id >= 0) ? STATUS_OK : -id;
        }

        void LSPFraction::destroy()
        {
            do_destroy();
            LSPComplexWidget::destroy();
        }

        status_t LSPFraction::slot_on_list_change(LSPWidget *sender, void *ptr, void *data)
        {
            LSPFraction *_this = widget_ptrcast<LSPFraction>(ptr);
            return (ptr != NULL) ? _this->on_list_change() : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPFraction::slot_on_list_submit(LSPWidget *sender, void *ptr, void *data)
        {
            LSPFraction *_this = widget_ptrcast<LSPFraction>(ptr);
            return (ptr != NULL) ? _this->on_list_submit() : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPFraction::slot_on_change(LSPWidget *sender, void *ptr, void *data)
        {
            LSPFraction *_this = widget_ptrcast<LSPFraction>(ptr);
            return (ptr != NULL) ? _this->on_change() : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPFraction::slot_on_submit(LSPWidget *sender, void *ptr, void *data)
        {
            LSPFraction *_this = widget_ptrcast<LSPFraction>(ptr);
            return (ptr != NULL) ? _this->on_submit() : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPFraction::on_list_change()
        {
            query_resize();
            return sSlots.execute(LSPSLOT_CHANGE, this);
        }

        status_t LSPFraction::on_list_submit()
        {
            if (!opened())
                return STATUS_OK;
            close();
            query_resize();
            return sSlots.execute(LSPSLOT_SUBMIT, this);
        }
    
        void LSPFraction::set_min_width(ssize_t value)
        {
            if (value == nMinWidth)
                return;
            nMinWidth   = value;
            query_resize();
        }

        void LSPFraction::set_min_height(ssize_t value)
        {
            if (value == nMinHeight)
                return;
            nMinHeight = value;
            query_resize();
        }

        status_t LSPFraction::set_num_selected(ssize_t value)
        {
            return sNumerator.set_selected(value);
        }

        status_t LSPFraction::set_num_opened(bool open)
        {
            if (open)
            {
                if (sDenominator.opened())
                    LSP_STATUS_ASSERT(sDenominator.close());
            }
            return sNumerator.set_opened(open);
        }

        status_t LSPFraction::set_num_closed(bool closed)
        {
            return set_num_opened(!closed);
        }

        status_t LSPFraction::set_denom_selected(ssize_t value)
        {
            return sDenominator.set_selected(value);
        }

        status_t LSPFraction::set_denom_opened(bool open)
        {
            if (open)
            {
                if (sNumerator.opened())
                    LSP_STATUS_ASSERT(sNumerator.close());
            }
            return sDenominator.set_opened(open);
        }

        status_t LSPFraction::set_denom_closed(bool closed)
        {
            return set_denom_opened(!closed);
        }

        status_t LSPFraction::open_num()
        {
            return set_num_opened(true);
        }

        status_t LSPFraction::toggle_num()
        {
            return set_num_opened(!sNumerator.opened());
        }

        status_t LSPFraction::close_num()
        {
            return set_num_opened(false);
        }

        status_t LSPFraction::open_denom()
        {
            return set_denom_opened(true);
        }

        status_t LSPFraction::toggle_denom()
        {
            return set_denom_opened(!sDenominator.opened());
        }

        status_t LSPFraction::close_denom()
        {
            return set_denom_opened(false);
        }

        status_t LSPFraction::close()
        {
            LSP_STATUS_ASSERT(set_num_opened(false));
            return set_denom_opened(false);
        }

        status_t LSPFraction::set_angle(float angle)
        {
            if (fAngle == angle)
                return STATUS_OK;
            fAngle = angle;
            query_resize();
            return STATUS_OK;
        }

        status_t LSPFraction::set_text_border(ssize_t border)
        {
            if (nTextBorder == border)
                return STATUS_OK;
            nTextBorder = border;
            query_resize();
            return STATUS_OK;
        }

        bool LSPFraction::check_mouse_over(const realize_t *r, ssize_t x, ssize_t y)
        {
            x              -= sSize.nLeft + r->nLeft;
            y              -= sSize.nTop + r->nTop;
            return ((x >= 0) && (x <= r->nWidth) && (y >= 0) && (y <= r->nHeight));
        }

        status_t LSPFraction::on_mouse_down(const ws_event_t *e)
        {
            take_focus();
            if (nMFlags == 0)
            {
                if (check_mouse_over(&sNum, e->nLeft, e->nTop))
                    nMState = NUM_CLICK;
                else if (check_mouse_over(&sDenom, e->nLeft, e->nTop))
                    nMState = DENOM_CLICK;
                else
                    nMState = NONE_CLICK;
            }
            nMFlags |= (1 << e->nCode);
            return STATUS_OK;
        }

        status_t LSPFraction::on_mouse_up(const ws_event_t *e)
        {
            nMFlags &= ~(1 << e->nCode);
            if ((nMFlags == 0) && (e->nCode == MCB_LEFT))
            {
                if ((nMState == NUM_CLICK) && check_mouse_over(&sNum, e->nLeft, e->nTop))
                    set_num_opened(true);
                else if ((nMState == DENOM_CLICK) && check_mouse_over(&sDenom, e->nLeft, e->nTop))
                    set_denom_opened(true);

                nMState = 0;
            }

            return STATUS_OK;
        }

        status_t LSPFraction::on_mouse_scroll(const ws_event_t *e)
        {
            if (nMFlags)
                return STATUS_OK;
            if (check_mouse_over(&sNum, e->nLeft, e->nTop))
                return sNumerator.on_mouse_scroll(e);
            else if (check_mouse_over(&sDenom, e->nLeft, e->nTop))
                return sDenominator.on_mouse_scroll(e);

            return STATUS_OK;
        }

        status_t LSPFraction::on_change()
        {
            return STATUS_OK;
        }

        status_t LSPFraction::on_submit()
        {
            return STATUS_OK;
        }

        void LSPFraction::size_request(size_request_t *r)
        {
            r->nMinWidth    = -1;
            r->nMinHeight   = -1;
            r->nMaxWidth    = -1;
            r->nMaxHeight   = -1;

            // Create surface to calculate font parameters
            ISurface *s = pDisplay->create_surface(1, 1);
            if (s == NULL)
                return;

            font_parameters_t fp;
            sFont.get_parameters(s, &fp);

            float lw    = sFont.size() * 0.1f;  // Fraction line width
            if (lw < 1.0f)
                lw          = 1.0f;

            realize_t t, b;
            t.nHeight   = fp.Height;
            t.nWidth    = estimate_max_size(&sNumerator, s) + (nTextBorder + lw)*2;
            b.nHeight   = fp.Height;
            b.nWidth    = estimate_max_size(&sDenominator, s) + (nTextBorder + lw)*2;

            // Fraction rotation
            float angle = fAngle * M_PI / 180.0f;
            float dx    = cosf(angle);
            float dy    = sinf(angle);

            ssize_t cx  = 0;    // Center of fraction (x)
            ssize_t cy  = 0;   // Center of fraction (y)

            t.nLeft     = cx - dy * t.nHeight * 0.5f;
            t.nTop      = cy - dx * t.nHeight * 0.5f;
            b.nLeft     = cx + dy * b.nHeight * 0.5f;
            b.nTop      = cy + dx * b.nHeight * 0.5f;

            ssize_t dx1 = (t.nLeft - t.nWidth) - (b.nLeft + b.nWidth);
            ssize_t dx2 = (t.nLeft + t.nWidth) - (b.nLeft - b.nWidth);
            ssize_t dy1 = (t.nTop - t.nHeight) - (b.nTop + b.nHeight);
            ssize_t dy2 = (t.nTop + t.nHeight) - (b.nTop - b.nHeight);

            if (dx1 < 0)
                dx1     = -dx1;
            if (dx2 < 0)
                dx2     = -dx2;

            if (dy1 < 0)
                dy1     = -dy1;
            if (dy2 < 0)
                dy2     = -dy2;
            if (dy1 < dy2)
                dy1 = dy2;

            // Output final size
            r->nMinWidth = (dx1 < dx2) ? dx2 : dx1;
            r->nMinHeight= (dy1 < dy2) ? dy2 : dy1;

            // Delete surface
            s->destroy();
            delete s;
        }

        ssize_t LSPFraction::estimate_max_size(LSPComboBox *cb, ISurface *s)
        {
            // Get font parameters
            text_parameters_t tp;

            LSPItemList *lst    = cb->items();
            ssize_t width       = 0;

            // Estimate the maximum width of the list box
            LSPString str;
            for (size_t i=0, n=lst->size(); i<n; ++i)
            {
                // Fetch item
                LSPItem *item = lst->get(i);
                if (item == NULL)
                    continue;
                item->text()->format(&str);
                if (str.is_empty())
                    continue;

                // Get text parameters
                sFont.get_text_parameters(s, &tp, &str);
                if (tp.Width > width)
                    width = tp.Width;
            }

            return width;
        }

        void LSPFraction::realize(const realize_t *r)
        {
            LSPString num, denom;
            font_parameters_t fp;
            text_parameters_t tp, bp;
            realize_t t, b;

            // Create surface to calculate font parameters
            ISurface *s = pDisplay->create_surface(1, 1);
            if (s == NULL) {
                LSPComplexWidget::realize(r);
                return;
            }

            // Get font parameters
            sFont.get_parameters(s, &fp);
            float lw    = sFont.size() * 0.1f;  // Fraction line width
            if (lw < 1.0f)
                lw          = 1.0f;
            if (sFont.bold())
                lw         *= 2;

            // Get numerator parameters
            num.set_native("-");
            ssize_t sel = sNumerator.selected();
            if (sel >= 0)
            {
                LSPItem *it = sNumerator.items()->get(sel);
                if (it != NULL)
                    it->text()->format(&num);
            }
            sFont.get_text_parameters(s, &tp, &num);

            // Get denominator parameters
            denom.set_native("-");
            sel = sDenominator.selected();
            if (sel >= 0)
            {
                LSPItem *it = sDenominator.items()->get(sel);
                if (it != NULL)
                    it->text()->format(&denom);
            }
            sFont.get_text_parameters(s, &bp, &denom);

            t.nHeight   = fp.Height;
            t.nWidth    = tp.Width + (nTextBorder + lw)*2;
            b.nHeight   = fp.Height;
            b.nWidth    = bp.Width + (nTextBorder + lw)*2;

            // Fraction rotation
            float angle = fAngle * M_PI / 180.0f;
            float dx    = cosf(angle);
            float dy    = sinf(angle);

            ssize_t cx  = sSize.nWidth >> 1;    // Center of fraction (x)
            ssize_t cy  = sSize.nHeight >> 1;   // Center of fraction (y)

            t.nLeft     = cx - dy * t.nHeight;
            t.nTop      = cy - dx * t.nHeight;
            b.nLeft     = cx + dy * b.nHeight;
            b.nTop      = cy + dx * b.nHeight;

            // Save coordinates of fraction parts
            realize_t rt, rb;
            size_request_t st, sb;

            sNumerator.size_request(&st);
            sDenominator.size_request(&sb);

            rt.nWidth       = (st.nMinWidth >= 0) ? st.nMinWidth : t.nWidth;
            rt.nHeight      = (st.nMinHeight >= 0) ? st.nMinHeight : t.nHeight;
            rt.nLeft        = sSize.nLeft + t.nLeft - (t.nWidth >> 1);
            rt.nTop         = sSize.nTop + t.nTop + (t.nHeight >> 1) - rt.nHeight;

            rb.nWidth       = (sb.nMinWidth >= 0) ? sb.nMinWidth : b.nWidth;
            rb.nHeight      = (sb.nMinHeight >= 0) ? sb.nMinHeight : b.nHeight;
            rb.nLeft        = sSize.nLeft + b.nLeft - (b.nWidth >> 1);
            rb.nTop         = sSize.nTop + b.nTop + (b.nHeight >> 1) - rb.nHeight;

            sNumerator.realize(&rt);
            sDenominator.realize(&rb);

            // Call parent method
            LSPComplexWidget::realize(r);

            // Delete surface
            s->destroy();
            delete s;
        }

        void LSPFraction::draw(ISurface *s)
        {
            LSPString num, denom;
            font_parameters_t fp;
            text_parameters_t tp, bp;
            realize_t t, b;

            // Prepare palette
            Color bg_color(sBgColor);
            Color color(sColor);
            Color font(sFont.raw_color());

            color.scale_lightness(brightness());
            font.scale_lightness(brightness());

            // Clear
            s->clear(bg_color);

            // Get font parameters
            sFont.get_parameters(s, &fp);
            float lw    = sFont.size() * 0.1f;  // Fraction line width
            if (lw < 1.0f)
                lw          = 1.0f;
            if (sFont.bold())
                lw         *= 2;

            // Get numerator parameters
            num.set_native("-");
            ssize_t sel = sNumerator.selected();
            if (sel >= 0)
            {
                LSPItem *it = sNumerator.items()->get(sel);
                if (it != NULL)
                    it->text()->format(&num);
            }
            sFont.get_text_parameters(s, &tp, &num);

            // Get denominator parameters
            denom.set_native("-");
            sel = sDenominator.selected();
            if (sel >= 0)
            {
                LSPItem *it = sDenominator.items()->get(sel);
                if (it != NULL)
                    it->text()->format(&denom);
            }
            sFont.get_text_parameters(s, &bp, &denom);

            t.nHeight   = fp.Height;
            t.nWidth    = tp.Width + (nTextBorder + lw)*2;
            b.nHeight   = fp.Height;
            b.nWidth    = bp.Width + (nTextBorder + lw)*2;

            // Fraction rotation
            float angle = fAngle * M_PI / 180.0f;
            float dx    = cosf(angle);
            float dy    = sinf(angle);

            ssize_t cx  = sSize.nWidth >> 1;    // Center of fraction (x)
            ssize_t cy  = sSize.nHeight >> 1;   // Center of fraction (y)

//            s->line(cx - 4, cy - 4, cx + 4, cy + 4, 1, c);
//            s->line(cx - 4, cy + 4, cx + 4, cy - 4, 1, c);

            t.nLeft     = cx - dy * t.nHeight;
            t.nTop      = cy - dx * t.nHeight;
            b.nLeft     = cx + dy * b.nHeight;
            b.nTop      = cy + dx * b.nHeight;

//            s->line(t.nLeft - 4, t.nTop - 4, t.nLeft + 4, t.nTop + 4, 1, sColor);
//            s->line(t.nLeft - 4, t.nTop + 4, t.nLeft + 4, t.nTop - 4, 1, sColor);
//            s->line(b.nLeft - 4, b.nTop - 4, b.nLeft + 4, b.nTop + 4, 1, sColor);
//            s->line(b.nLeft - 4, b.nTop + 4, b.nLeft + 4, b.nTop - 4, 1, sColor);

//            s->wire_rect(t.nLeft - (t.nWidth >> 1), t.nTop - (t.nHeight >> 1), t.nWidth, t.nHeight, 1, c);
//            s->wire_rect(b.nLeft - (b.nWidth >> 1), b.nTop - (b.nHeight >> 1), b.nWidth, b.nHeight, 1, c);

            // Save coordinates of fraction parts
            sNum.nLeft      = t.nLeft - (t.nWidth >> 1);
            sNum.nTop       = t.nTop - (t.nHeight >> 1);
            sNum.nWidth     = t.nWidth;
            sNum.nHeight    = t.nHeight;

            sDenom.nLeft    = b.nLeft - (b.nWidth >> 1);
            sDenom.nTop     = b.nTop - (b.nHeight >> 1);
            sDenom.nWidth   = b.nWidth;
            sDenom.nHeight  = b.nHeight;

            // Output numerator and denominator
            bool aa     = s->set_antialiasing(true);
            sFont.draw(s, t.nLeft - ( tp.Width)*0.5f, t.nTop - fp.Descent + fp.Height*0.5f, font, &num);
            sFont.draw(s, b.nLeft - ( bp.Width)*0.5f, b.nTop - fp.Descent + fp.Height*0.5f, font, &denom);

            // Draw line
            dx          = dx * t.nHeight;
            dy          = dy * t.nHeight;
            s->line(cx + dx, cy - dy, cx - dx, cy + dy, lw, color);
            s->set_antialiasing(aa);
        }

    } /* namespace tk */
} /* namespace lsp */
