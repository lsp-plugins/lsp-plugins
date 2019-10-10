/*
 * LSPSwitch.cpp
 *
 *  Created on: 1 июл. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        static const float ANGLE = 15.0f * M_PI / 180.0f;
        
        const w_class_t LSPSwitch::metadata = { "LSPSwitch", &LSPWidget::metadata };

        LSPSwitch::LSPSwitch(LSPDisplay *dpy): LSPWidget(dpy),
            sColor(this),
            sTextColor(this),
            sBorderColor(this),
            sHoleColor(this)
        {
            nSize       = 24;
            nBorder     = 8;
            nState      = 0;
            nBMask      = 0;
            nAngle      = 0;
            nAspect     = M_SQRT2;
            pClass      = &metadata;
        }
        
        status_t LSPSwitch::init()
        {
            status_t result = LSPWidget::init();
            if (result != STATUS_OK)
                return result;

            init_color(C_KNOB_CAP, &sColor);
            init_color(C_LABEL_TEXT, &sTextColor);
            init_color(C_KNOB_CAP, &sBorderColor);

            sHoleColor.bind("hole_color");

            if (!sSlots.add(LSPSLOT_CHANGE))
                return STATUS_NO_MEM;

            return STATUS_OK;
        }

        LSPSwitch::~LSPSwitch()
        {
        }

        void LSPSwitch::draw(ISurface *s)
        {
            // Prepare palette
            Color bg_color(sBgColor);
            Color border(sBorderColor);
            Color bcl(sColor);
            Color font(sTextColor);

            border.scale_lightness(brightness());
            font.scale_lightness(brightness());
            bcl.scale_lightness(brightness());

            // Get resource
            IGradient *cp;

            bool aa     = s->set_antialiasing(true);

            // Draw background
            s->fill_rect(0, 0, sSize.nWidth, sSize.nHeight, bg_color);

            // Get dimensions
            ssize_t w = 0, h = 0;
            dimensions(w, h);
            ssize_t left = ssize_t((sSize.nWidth - w) >> 1), top = ssize_t((sSize.nHeight - h) >> 1);

            // Move to the left corner
            float delta = sqrtf(w*w + h*h);
            float radius    = nAspect * nSize * 0.5f / cosf(ANGLE);

            // Draw border if present
            if (nBorder > 0)
            {
                // Draw border
                ssize_t b_r = 3;

                for (ssize_t i=0; (i<b_r) && (i < ssize_t(nBorder)); ++i)
                {
                    Color bc(border);
                    float bright = (i + 1.0) / b_r;
                    bc.lightness(bc.lightness() + bright);

                    cp = s->radial_gradient(
                        left + w - (b_r << 1), top + (b_r << 1), delta * 0.5,
                        left + w - (b_r << 1), top + (b_r << 1), delta * 2.0 / (bright + 1.0)
                    );
                    cp->add_color(0.0, border.red() * bright, border.green() * bright, border.blue() * bright);
                    cp->add_color(1.0, 0.5 * border.red(), 0.5 *  border.green(), 0.5 * border.blue());
                    s->fill_rect(left + i, top + i, w - (i << 1), h - (i << 1), cp);
                    delete cp;
                }

                // Draw border
                if (ssize_t(nBorder) > b_r)
                {
                    cp = s->radial_gradient(
                        left + w - (b_r << 1), top + (b_r << 1), delta * 0.5,
                        left + w - (b_r << 1), top + (b_r << 1), delta * 1.0
                    );
                    cp->add_color(0.0, border);
                    cp->add_color(1.0, 0.5 * border.red(), 0.5 * border.green(), 0.5 * border.blue());
                    s->fill_rect(left + b_r, top + b_r, w - (b_r << 1), h - (b_r << 1), cp);
                    delete cp;
                }

                // Draw hole
                Color hole(border);
                hole.darken(0.75);

                cp = s->radial_gradient(
                    left + w - (b_r << 1), top + (b_r << 1), 0,
                    left + w - (b_r << 1), top + (b_r << 1), delta * 2.0
                );
                cp->add_color(0.0, hole);
                cp->add_color(1.0, 0.5 * hole.red(), 0.5 *  hole.green(), 0.5 * hole.blue());
                s->fill_rect(left + nBorder, top + nBorder, w - (nBorder << 1), h - (nBorder << 1), cp);
                delete cp;
            }
            else
            {
                // Draw hole
                Color hole;
                pDisplay->theme()->get_color(C_HOLE, &hole);
                s->fill_rect(left + nBorder, top + nBorder, w - (nBorder << 1), h - (nBorder << 1), hole);
            }

            size_t pos = (nState & S_PRESSED) ? 1 : (nState & S_TOGGLED) ? 2 : 0;
            if (nAngle & 2)
                pos     = 2 - pos;

            // Draw button
            size_t l    = left + nBorder + 1;
            size_t bw   = w - ((nBorder + 1) << 1);
            size_t t    = top + nBorder + 1;
            size_t bh   = h - ((nBorder + 1) << 1);

            ssize_t dw1 = radius * (sinf((pos + 1) * ANGLE) - sinf(ANGLE));
            ssize_t dw2 = radius * (sinf((3 - pos) * ANGLE) - sinf(ANGLE));
            float bc    = bcl.lightness();
            float b1    = (pos * 0.1) + bc;
            float b2    = ((2 - pos) * 0.1) + bc;
            float b0    = 0.1 * bc;
            size_t wid  = (nAngle & 1) ? bh : bw;
            ssize_t cx  = (wid + dw1 - dw2) >> 1;
            ssize_t dc1 = cx - dw1;
            ssize_t dc2 = wid - (cx + dw2);

            for (ssize_t i=0; i < dw1; ++i)
            {
                float arg = i + 1.0;
                float bright = (b1 - b0) * arg / dw1 + b0;
                bcl.lightness(bright);

                if (nAngle & 1)
                    s->fill_rect(l, t + i, bw, dw1 - i, bcl);
                else
                    s->fill_rect(l + i, t, dw1 - i, bh, bcl);
            }

            for (ssize_t i=0; i < dc1; ++i)
            {
                float bright = (b1 - bc) * (dc1 - i) / dc1 + bc;
                bcl.lightness(bright);

                if (nAngle & 1)
                    s->fill_rect(l, t + dw1 + i, bw, dc1 - i, bcl);
                else
                    s->fill_rect(l + dw1 + i, t, dc1 - i, bh, bcl);
            }

            for (ssize_t i=0; i < dw2; ++i)
            {
                float arg = i + 1.0;
                float bright = (b2 - b0) * arg / dw2 + b0;
                bcl.lightness(bright);

                if (nAngle & 1)
                    s->fill_rect(l, t + bh - dw2, bw, dw2 - i, bcl);
                else
                    s->fill_rect(l + bw - dw2, t, dw2 - i, bh, bcl);
            }

            for (ssize_t i=0; i < dc2; ++i)
            {
                float bright = (b2 - bc) * (dc2 - i) / dc2 + bc;
                bcl.lightness(bright);

                if (nAngle & 1)
                    s->fill_rect(l, t + cx, bw, dc2 - i, bcl);
                else
                    s->fill_rect(l + cx, t, dc2 - i, bh, bcl);
            }

            // Draw symbols
            bc          = font.lightness();
            b1          = bc - ((2 - pos) * 0.1);
            b2          = bc - (pos * 0.1);
            wid        -= dw1 + dw2;
            float s1    = (wid >> 3);
            float s2    = (wid >> 3);

            font.lightness((nAngle & 2) ? b2 : b1);
            if (nAngle & 1)
                s->wire_arc(left + (w >> 1), t + cx + (nAngle - 2) * (wid >> 2), s1, 0, M_PI * 2.0, 2.0f, font);
            else
                s->wire_arc(l + cx + (nAngle - 1) * (wid >> 2), top + (h >> 1), s1, 0, M_PI * 2.0, 2.0f, font);

            font.lightness((nAngle & 2) ? b1 : b2);
            if (nAngle & 1)
                s->line(
                    left + (w >> 1), t + cx - (nAngle - 2) * (wid >> 2) + s2,
                    left + (w >> 1), t + cx - (nAngle - 2) * (wid >> 2) - s2,
                    2.0f, font
                );
            else
                s->line(
                    l + cx - (nAngle - 1) * (wid >> 2) + s2, top + (h >> 1),
                    l + cx - (nAngle - 1) * (wid >> 2) - s2, top + (h >> 1),
                    2.0f, font
                );

            s->set_antialiasing(aa);
        }

        void LSPSwitch::on_click(bool down)
        {
            lsp_trace("switch clicked: down=%s", (down) ? "true" : "false");
            sSlots.execute(LSPSLOT_CHANGE, this);
        }

        void LSPSwitch::size_request(size_request_t *r)
        {
            dimensions(r->nMinWidth, r->nMinHeight);
            r->nMaxWidth        = r->nMinWidth;
            r->nMaxHeight       = r->nMinHeight;
        }

        bool LSPSwitch::check_mouse_over(ssize_t x, ssize_t y)
        {
            ssize_t w = 0, h = 0;
            dimensions(w, h);
            w -= (nBorder + 1) << 1;
            h -= (nBorder + 1) << 1;

            ssize_t left    = sSize.nLeft + ((sSize.nWidth - w) >> 1);
            ssize_t top     = sSize.nTop + ((sSize.nHeight - h) >> 1);
            ssize_t right   = left + w;
            ssize_t bottom  = top + h;

    //        lsp_trace("x=%d, y=%d, l=%d, r=%d, t=%d, b=%d", int(x), int(y), int(left), int(right), int(top), int(bottom));
            return ((x >= left) && (x <= right) && (y >= top) && (y <= bottom));
        }

        void LSPSwitch::dimensions(ssize_t &w, ssize_t &h)
        {
            size_t width = nSize + 2;
            size_t height = roundf(nSize * nAspect) + 2;

            if (nBorder > 0)
            {
                width   += (nBorder + 1) << 1;
                height  += (nBorder + 1) << 1;
            }

            // Round to be multiple of 2
            width   += width & 1;
            height  += height & 1;

            // Accept rotation
            if (nAngle & 1)
            {
                w       = width;
                h       = height;
            }
            else
            {
                w       = height;
                h       = width;
            }
        }

        status_t LSPSwitch::on_mouse_down(const ws_event_t *e)
        {
            take_focus();
            nBMask         |= (1 << e->nCode);

            bool pressed    = (nBMask == (1 << MCB_LEFT)) && (check_mouse_over(e->nLeft, e->nTop));
            bool is_pressed = nState & S_PRESSED;

            if (pressed != is_pressed)
            {
                if (pressed)
                    nState     |= S_PRESSED;
                else
                    nState     &= ~S_PRESSED;

                query_draw();
            }

            return STATUS_OK;
        }

        status_t LSPSwitch::on_mouse_up(const ws_event_t *e)
        {
            nBMask         &= ~(1 << e->nCode);
            bool pressed    = ((e->nCode == MCB_LEFT) && (nBMask == 0)) || ((e->nCode != MCB_LEFT) && (nBMask == (1 << MCB_LEFT)));
            if (pressed)
                pressed     = (check_mouse_over(e->nLeft, e->nTop));
            if (nBMask == 0)
                pressed     = false;

            bool is_pressed = nState & S_PRESSED;
            if (pressed != is_pressed)
            {
                if (is_pressed)
                    nState     ^= S_TOGGLED;
                if (pressed)
                    nState     |= S_PRESSED;
                else
                    nState     &= ~S_PRESSED;

                if (nBMask == 0)
                    on_click(nState & S_TOGGLED);

                query_draw();
            }
            return STATUS_OK;
        }

        status_t LSPSwitch::on_mouse_move(const ws_event_t *e)
        {
            bool pressed    = (nBMask == (1 << MCB_LEFT)) && (check_mouse_over(e->nLeft, e->nTop));
            bool is_pressed = nState & S_PRESSED;

            if (pressed != is_pressed)
            {
                if (pressed)
                    nState     |= S_PRESSED;
                else
                    nState     &= ~S_PRESSED;

                query_draw();
            }

            return STATUS_OK;
        }

        void LSPSwitch::set_down(bool down)
        {
            // Do not react if state does not change
            if (!(bool(nState & S_TOGGLED) ^ down))
                return;

            if (down)
                nState |= S_TOGGLED;
            else
                nState &= ~S_TOGGLED;

            // Request for redraw
            query_draw();
        }

        void LSPSwitch::set_up(bool up)
        {
            set_down(!up);
        }

        void LSPSwitch::set_size(ssize_t size)
        {
            nSize       = size;
            query_resize();
        }

        void LSPSwitch::set_border(size_t border)
        {
            nBorder     = border;
            query_resize();
        }

        void LSPSwitch::set_aspect(float aspect)
        {
            nAspect     = aspect;
            query_resize();
        }

        void LSPSwitch::set_angle(size_t angle)
        {
            nAngle      = angle;
            query_resize();
        }
    
    } /* namespace ctl */
} /* namespace lsp */
