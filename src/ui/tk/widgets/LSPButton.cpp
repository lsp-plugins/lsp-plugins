/*
 * LSPButton.cpp
 *
 *  Created on: 21 июн. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPButton::metadata = { "LSPButton", &LSPWidget::metadata };

        LSPButton::LSPButton(LSPDisplay *dpy):
            LSPWidget(dpy),
            sColor(this),
            sFont(this),
            sTitle(this)
        {
            nWidth      = 18;
            nHeight     = 18;
            nMinWidth   = 18;
            nMinHeight  = 18;
            nState      = S_EDITABLE;
            nBMask      = 0;
            nChanges    = 0;

            pClass      = &metadata;
        }
        
        LSPButton::~LSPButton()
        {
        }

        status_t LSPButton::init()
        {
            status_t result = LSPWidget::init();
            if (result != STATUS_OK)
                return result;

            sFont.init();
            sFont.set_size(12.0f);

            init_color(C_BUTTON_FACE, &sColor);
            init_color(C_BUTTON_TEXT, sFont.color());
            sTitle.bind();

            ui_handler_id_t id = 0;
            id = sSlots.add(LSPSLOT_CHANGE, slot_on_change, self());
            if (id < 0) return -id;
            id = sSlots.add(LSPSLOT_SUBMIT, slot_on_submit, self());
            if (id < 0) return -id;

            return STATUS_OK;
        }

        status_t LSPButton::slot_on_change(LSPWidget *sender, void *ptr, void *data)
        {
            LSPButton *_this = widget_ptrcast<LSPButton>(ptr);
            return (_this != NULL) ? _this->on_change() : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPButton::slot_on_submit(LSPWidget *sender, void *ptr, void *data)
        {
            LSPButton *_this = widget_ptrcast<LSPButton>(ptr);
            return (_this != NULL) ? _this->on_submit() : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPButton::on_change()
        {
            return STATUS_OK;
        }

        status_t LSPButton::on_submit()
        {
            return STATUS_OK;
        }

        bool LSPButton::check_mouse_over(ssize_t x, ssize_t y)
        {
            x              -= sSize.nLeft;
            y              -= sSize.nTop;

            ssize_t left    = ssize_t(sSize.nWidth - nWidth) >> 1;
            ssize_t top     = ssize_t(sSize.nHeight - nHeight) >> 1;
            ssize_t right   = left + nWidth;
            ssize_t bottom  = top + nHeight;

            return ((x >= left) && (x <= right) && (y >= top) && (y <= bottom));
        }

        void LSPButton::set_trigger()
        {
            if (nState & S_TRIGGER)
                return;
            nState      = (nState & (~S_TOGGLE)) | S_TRIGGER;

            query_draw();
        }

        void LSPButton::set_toggle()
        {
            if (nState & S_TOGGLE)
                return;
            nState      = (nState & (~S_TRIGGER)) | S_TOGGLE;

            query_draw();
        }

        void LSPButton::set_normal()
        {
            if (!(nState & (S_TOGGLE | S_TRIGGER)))
                return;
            nState      = nState & (~(S_TRIGGER | S_TOGGLE));

            query_draw();
        }

        void LSPButton::set_down(bool value)
        {
            nState     &= ~(S_DOWN | S_PRESSED | S_TOGGLED);

            if (value)
                nState     |= S_DOWN | ((nState & S_TRIGGER) ? S_PRESSED : S_TOGGLED);

            query_draw();
        }

        void LSPButton::set_led(bool value)
        {
            size_t state = nState;
            if (value)
                nState     |= S_LED;
            else
                nState     &= ~S_LED;

            if (nState != state)
                query_draw();
        }

        void LSPButton::set_editable(bool value)
        {
            size_t state = nState;
            if (value)
                nState     |= S_EDITABLE;
            else
                nState     &= ~S_EDITABLE;

            if (nState != state)
                query_draw();
        }

        void LSPButton::set_min_width(size_t value)
        {
            if (nMinWidth == value)
                return;
            nMinWidth       = value;
            query_resize();
        }

        void LSPButton::set_min_height(size_t value)
        {
            if (nMinHeight == value)
                return;
            nMinHeight      = value;
            query_resize();
        }

        void LSPButton::set_min_size(size_t width, size_t height)
        {
            if ((nMinWidth == width) && (nMinHeight == height))
                return;
            nMinWidth       = width;
            nMinHeight      = height;
            query_resize();
        }

        void LSPButton::draw(ISurface *s)
        {
            IGradient *gr = NULL;
            size_t pressed = nState;

            // Prepare palette
            Color bg_color(sBgColor);
            Color color(sColor);
            color.scale_lightness(brightness());

            // Draw background
            s->fill_rect(0, 0, sSize.nWidth, sSize.nHeight, bg_color);

            // Calculate real boundaries
            ssize_t c_x     = (sSize.nWidth >> 1);
            ssize_t c_y     = (sSize.nHeight >> 1);

            // Calculate parameters
            Color hole(0.0f, 0.0f, 0.0f);
            float b_rad  = sqrtf(nWidth*nWidth + nHeight*nHeight);
            size_t bsize = (nWidth < nHeight) ? nWidth : nHeight;
            ssize_t b_w  = nWidth >> 1;
            ssize_t b_h  = nHeight >> 1;
            ssize_t b_r  = bsize >> 1;          // Button radius
            ssize_t b_rr = 2 + (bsize >> 4);    // Button rounding radius
            ssize_t l_rr = (bsize >> 2);

            // Draw hole
            bool aa = s->set_antialiasing(true);
            s->fill_round_rect(c_x - b_w - 1, c_y - b_h - 1, nWidth + 2, nHeight + 2, b_rr + 1, hole);

            // Change size if pressed
            ssize_t b_l = b_rr;
            if (pressed & S_PRESSED)
            {
                b_l ++;
                b_r --;
                b_w --;
                b_h --;
                b_rr --;
            }
            else if (pressed & S_TOGGLED)
            {
                b_r --;
                b_w --;
                b_h --;
            }
            else
                b_l ++;

            float lightness = color.lightness();
            if (pressed & S_LED)
            {
                // Draw light
//                size_t flag = (nState & S_TRIGGER) ? S_PRESSED : S_TOGGLED;

                if (pressed & S_DOWN)
                {
                    ssize_t x_rr = l_rr - 1;

                    gr  =  s->linear_gradient(c_x, c_y - b_h, c_x, c_y - b_h - x_rr);
                    gr->add_color(0.0, color, 0.5f);
                    gr->add_color(1.0, color, 1.0f);
                    s->fill_triangle(c_x - b_w - l_rr, c_y - b_h - l_rr, c_x + b_w + l_rr, c_y - b_h - l_rr, c_x, c_y, gr);
                    delete gr;

                    gr  =  s->linear_gradient(c_x, c_y + b_h, c_x, c_y + b_h + x_rr);
                    gr->add_color(0.0, color, 0.5f);
                    gr->add_color(1.0, color, 1.0f);
                    s->fill_triangle(c_x + b_w + l_rr, c_y + b_h + l_rr, c_x - b_w - l_rr, c_y + b_h + l_rr, c_x, c_y, gr);
                    delete gr;

                    gr  =  s->linear_gradient(c_x - b_w, c_y, c_x - b_w - x_rr, c_y);
                    gr->add_color(0.0, color, 0.5f);
                    gr->add_color(1.0, color, 1.0f);
                    s->fill_triangle(c_x - b_w - l_rr, c_y - b_h - l_rr, c_x - b_w - l_rr, c_y + b_h + l_rr, c_x, c_y, gr);
                    delete gr;

                    gr  =  s->linear_gradient(c_x + b_w, c_y, c_x + b_w + x_rr, c_y);
                    gr->add_color(0.0, color, 0.5f);
                    gr->add_color(1.0, color, 1.0f);
                    s->fill_triangle(c_x + b_w + l_rr, c_y + b_h + l_rr, c_x + b_w + l_rr, c_y - b_h - l_rr, c_x, c_y, gr);
                    delete gr;
                }
                else
                    lightness  *= 0.5f;
            }

            for (ssize_t i=0; (i++)<b_l; )
            {
                float bright = lightness * sqrtf(i * i) / b_l;

                if (pressed & S_PRESSED)
                    gr = s->radial_gradient(c_x - b_w, c_y + b_h, b_rad * 0.25f, c_x - b_w, c_y + b_h, b_rad * 3.0f);
                else if (pressed & S_TOGGLED)
                    gr = s->radial_gradient(c_x - b_w, c_y + b_h, b_rad * 0.25f, c_x - b_w, c_y + b_h, b_rad * 3.0f);
                else
                    gr = s->radial_gradient(c_x + b_w, c_y - b_h, b_rad * 0.25f, c_x + b_w, c_y - b_h, b_rad * 3.0f);

                Color cl(color);
                cl.lightness(bright);
                gr->add_color(0.0f, cl);
                cl.darken(0.9f);
                gr->add_color(1.0f, cl);

                s->fill_round_rect(c_x - b_w, c_y - b_h, b_w*2, b_h*2, b_rr, gr);
                delete gr; // Delete gradient!

                if ((--b_r) < 0)
                    b_r = 0;
                if ((--b_w) < 0)
                    b_w = 0;
                if ((--b_h) < 0)
                    b_h = 0;
            }

            if (pressed & S_LED)
            {
                Color cl(color);
                cl.lightness(lightness);

                gr = s->radial_gradient(c_x - b_w, c_y + b_h, b_rad * 0.25f, c_x, c_y, b_rad * 0.8f);
                gr->add_color(0.0, cl);
                gr->add_color(1.0, 1.0f, 1.0f, 1.0f);
                s->fill_round_rect(c_x - b_w, c_y - b_h, b_w * 2.0f, b_h * 2.0f, b_rr, gr);
                delete gr;
            }

            // Output text
            LSPString title;
            sTitle.format(&title);
            if (title.length() > 0)
            {
                text_parameters_t tp;
                font_parameters_t fp;

                Color font_color(sFont.raw_color());
                font_color.scale_lightness(brightness());

                sFont.get_parameters(s, &fp);
                sFont.get_text_parameters(s, &tp, &title);

                if (pressed & S_PRESSED)
                {
                    c_y++;
                    c_x++;
                }

                sFont.draw(s, c_x - (tp.XAdvance * 0.5f), c_y - (fp.Height * 0.5f) + fp.Ascent, font_color, &title);
            }

            s->set_antialiasing(aa);
        }

        void LSPButton::size_request(size_request_t *r)
        {
            r->nMaxWidth    = -1;
            r->nMaxHeight   = -1;
            r->nMinWidth    = nMinWidth;
            r->nMinHeight   = nMinHeight;

            LSPString title;
            sTitle.format(&title);

            if (title.length() > 0)
            {
                text_parameters_t tp;
                font_parameters_t fp;

                ISurface *s = pDisplay->create_surface(1, 1);

                if (s != NULL)
                {
                    sFont.get_parameters(s, &fp);
                    sFont.get_text_parameters(s, &tp, &title);
                    s->destroy();
                    delete s;

                    tp.Width       += 10;
                    fp.Height      += 10;

                    if (r->nMinWidth < tp.Width)
                        r->nMinWidth    = tp.Width;
                    if (r->nMinHeight < fp.Height)
                        r->nMinHeight   = fp.Height;
                }
            }

            size_t size     = (nWidth < nHeight) ? nWidth : nHeight;
            size_t delta    = (nState & S_LED) ? 2 + (size >> 2) : 2;

            r->nMinWidth   += delta;
            r->nMinHeight  += delta;
        }

        void LSPButton::realize(const realize_t *r)
        {
            LSPWidget::realize(r);

            nWidth      = nMinWidth;
            nHeight     = nMinHeight;

            LSPString title;
            sTitle.format(&title);
            if (title.length() <= 0)
                return;

            text_parameters_t tp;
            font_parameters_t fp;
            ISurface *s = pDisplay->create_surface(1, 1);
            if (s == NULL)
                return;

            sFont.get_parameters(s, &fp);
            sFont.get_text_parameters(s, &tp, &title);
            s->destroy();
            delete s;

            tp.Width       += 10;
            fp.Height      += 10;

            if (nWidth < tp.Width)
                nWidth      = tp.Width;
            if (nHeight < fp.Height)
                nHeight     = fp.Height;
        }

        status_t LSPButton::on_mouse_down(const ws_event_t *e)
        {
            if (!(nState & S_EDITABLE))
                return STATUS_OK;

            take_focus();

            bool m_over         = check_mouse_over(e->nLeft, e->nTop);
            size_t mask         = nBMask;
            nBMask             |= (1 << e->nCode);

            if (!mask)
            {
                if (!m_over)
                {
                    nState             |= S_OUT; // Mark that out of the button area
                    return STATUS_OK;
                }
                else
                    nChanges        = 0;
            }

            if (nState & S_OUT) // Mouse button was initially pressed out of the button area
                return STATUS_OK;

            // Update state according to mouse position and mouse button state
            size_t state        = nState;
            if ((nBMask == (1 << MCB_LEFT)) && (m_over))
                nState     |= S_PRESSED;
            else
                nState     &= ~S_PRESSED;

            // Special case for trigger button
            if ((nState & S_TRIGGER) && (state != nState))
            {
                if ((nState & S_PRESSED) && (!(nState & S_DOWN)))
                {
                    nState      |= S_DOWN;
                    nChanges    ++;
                    sSlots.execute(LSPSLOT_CHANGE, this);
                }
                else if ((!(nState & S_PRESSED)) && (nState & S_DOWN))
                {
                    nState      &= ~S_DOWN;
                    nChanges    ++;
                    sSlots.execute(LSPSLOT_CHANGE, this);
                }
            }

            // Query draw if state changed
            if (state != nState)
                query_draw();

            return STATUS_OK;
        }

        status_t LSPButton::on_mouse_up(const ws_event_t *e)
        {
            if (!(nState & S_EDITABLE))
                return STATUS_OK;

            size_t mask     = nBMask;
            nBMask         &= ~(1 << e->nCode);

            // Mouse button was initially pressed out of the button area, ignore this case
            if ((nBMask == 0) && (nState & S_OUT))
            {
                nState &= ~S_OUT;
                return STATUS_OK;
            }

            size_t state        = nState;
            bool m_over         = check_mouse_over(e->nLeft, e->nTop);

            if (nState & S_TRIGGER)
            {
                // Update state according to mouse position and mouse button state
                size_t state        = nState;
                if ((nBMask == (1 << MCB_LEFT)) && (m_over))
                    nState     |= S_PRESSED;
                else
                    nState     &= ~S_PRESSED;

                if (state != nState)
                {
                    if ((nState & S_PRESSED) && (!(nState & S_DOWN)))
                    {
                        nState      |= S_DOWN;
                        nChanges    ++;
                        sSlots.execute(LSPSLOT_CHANGE, this);
                    }
                    else if ((!(nState & S_PRESSED)) && (nState & S_DOWN))
                    {
                        nState      &= ~S_DOWN;
                        nChanges    ++;
                        sSlots.execute(LSPSLOT_CHANGE, this);
                    }
                }
            }
            else if (nState & S_TOGGLE)
            {
                if ((mask == (1 << MCB_LEFT)) && (e->nCode == MCB_LEFT) && (m_over))
                    nState ^= S_TOGGLED;

                if (state != nState)
                {
                    if ((nState & S_TOGGLED) && (!(nState & S_DOWN)))
                    {
                        nState      |= S_DOWN;
                        nChanges    ++;
                        sSlots.execute(LSPSLOT_CHANGE, this);
                    }
                    else if ((!(nState & S_TOGGLED)) && (nState & S_DOWN))
                    {
                        nState      &= ~S_DOWN;
                        nChanges    ++;
                        sSlots.execute(LSPSLOT_CHANGE, this);
                    }
                }
            }
            else
            {
                // Released left mouse button over the button widget?
                if ((mask == (1 << MCB_LEFT)) && (e->nCode == MCB_LEFT))
                {
                    nState &= ~(S_PRESSED | S_TOGGLED | S_DOWN);
                    if (m_over)
                    {
                        nChanges    ++;
                        sSlots.execute(LSPSLOT_CHANGE, this);
                    }
                }
            }

            if ((nBMask == (1 << MCB_LEFT)) && (m_over))
                nState     |= S_PRESSED;
            else
                nState     &= ~S_PRESSED;

            if ((mask == size_t(1 << e->nCode)) && (nChanges > 0))
            {
                sSlots.execute(LSPSLOT_SUBMIT, this);
                nChanges = 0;
            }

            // Query draw if state changed
            if (state != nState)
                query_draw();

            return STATUS_OK;
        }

        status_t LSPButton::on_mouse_move(const ws_event_t *e)
        {
            if (!(nState & S_EDITABLE))
                return STATUS_OK;

            // Mouse button was initially pressed out of the button area, ignore this case
            if (nState & S_OUT)
                return STATUS_OK;

            // Update state according to mouse position and mouse button state
            size_t state        = nState;
            if ((nBMask == (1 << MCB_LEFT)) && (check_mouse_over(e->nLeft, e->nTop)))
                nState     |= S_PRESSED;
            else
                nState     &= ~S_PRESSED;

            // Special case for trigger button
            if ((nState & S_TRIGGER) && (state != nState))
            {
                if ((nState & S_PRESSED) && (!(nState & S_DOWN)))
                {
                    nState      |= S_DOWN;
                    nChanges    ++;
                    sSlots.execute(LSPSLOT_CHANGE, this);
                }
                else if ((!(nState & S_PRESSED)) && (nState & S_DOWN))
                {
                    nState      &= ~S_DOWN;
                    nChanges    ++;
                    sSlots.execute(LSPSLOT_CHANGE, this);
                }
            }

            // Query draw if state changed
            if (state != nState)
                query_draw();

            return STATUS_OK;
        }

    } /* namespace tk */
} /* namespace lsp */
