/*
 * LSPMountStud.cpp
 *
 *  Created on: 7 нояб. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

#define CURVE           12
#define STUD_H          16
#define SCREW_SIZE      40

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPMountStud::metadata = { "LSPMountStud", &LSPWidget::metadata };

        LSPMountStud::LSPMountStud(LSPDisplay *dpy):
            LSPWidget(dpy),
            sFont(this)
        {
            pClass      = &metadata;
            nAngle      = 0;
            nButtons    = 0;
            bPressed    = false;
        }
        
        LSPMountStud::~LSPMountStud()
        {
        }

        status_t LSPMountStud::init()
        {
            status_t result = LSPWidget::init();
            if (result != STATUS_OK)
                return result;

            sFont.init();
            sFont.set_name("Arial");
            sFont.set_size(16);
            sFont.set_bold(true);

            init_color(C_LOGO_FACE, &sColor);
            init_color(C_LOGO_TEXT, sFont.color());

            ui_handler_id_t id = 0;
            id = sSlots.add(LSPSLOT_SUBMIT, slot_on_submit, self());
            if (id < 0) return -id;

            return STATUS_OK;
        }

        void LSPMountStud::destroy()
        {
            LSPWidget::destroy();
        }

        status_t LSPMountStud::slot_on_submit(LSPWidget *sender, void *ptr, void *data)
        {
            LSPMountStud *_this = widget_ptrcast<LSPMountStud>(ptr);
            return (_this != NULL) ? _this->on_submit() : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPMountStud::set_text(const char *text)
        {
            if (!sText.set_native(text))
                return STATUS_NO_MEM;
            query_resize();
            return STATUS_OK;
        }

        status_t LSPMountStud::set_text(const LSPString *src)
        {
            if (!sText.set(src))
                return STATUS_NO_MEM;
            query_resize();
            return STATUS_OK;
        }

        void LSPMountStud::set_angle(size_t value)
        {
            size_t old_value = nAngle;
            nAngle  = value;
            if ((value & 0x03) == (old_value & 0x03))
                return;

            query_resize();
        }

        void LSPMountStud::draw_screw(ISurface *s, ssize_t x, ssize_t y, float angle)
        {
            size_t h_s = 8;
            size_t h_rr = 3;

            // Draw hole
            Color hole(sBgColor);
            float hlb = hole.lightness() + 0.5f;
            float hld = 0;

            for (size_t i=0; i<=h_rr; ++i)
            {
                float bright = (hlb - hld) * (h_rr - i) / h_rr + hld;
                hole.lightness(bright);

                IGradient *gr = s->radial_gradient(x - h_s, y + h_s, h_s >> 2, x - h_s, y + h_s, h_s << 1);
                gr->add_color(0.0, hole);
                gr->add_color(1.0, 0.5 * hole.red(), 0.5 *  hole.green(), 0.5 * hole.blue());

                s->fill_round_rect(x + i - (h_s * 1.75), y + i - h_s, h_s * 3.5 - (i<<1), (h_s - i) << 1, (h_s - i), SURFMASK_ALL_CORNER, gr);

                delete gr;
            }

            // Draw mounting stud body
            Color stud(0, 0, 0);
            size_t f_rr     = h_s - 3;
            float slb       = stud.lightness();
            float sle       = slb + 0.2;

            for (size_t i=0; i<=f_rr; ++i)
            {
                Color c(0, 0, 0);
                c.blend(stud, float(f_rr - i) / f_rr);
                float bright = (sle - slb) * sinf(M_PI * i / f_rr) + slb;

                c.lightness(bright * i / f_rr);

                IGradient *gr = s->radial_gradient(x + (h_s >> 1), y - (h_s >> 1), 0, x + (h_s >> 1), y - (h_s >> 1), h_s * 1.5);
                gr->add_color(0.0, 1.0, 1.0, 1.0);
                gr->add_color(1.0, 0.5 * c.red(), 0.5 * c.green(), 0.5 * c.blue());
                s->fill_circle(x, y, h_s + 2 - i, gr);
                delete gr;
            }

            // Draw cross
            size_t c_rr     = 3;

            surf_line_cap_t cap = s->set_line_cap(SURFLCAP_ROUND);

            for (size_t i=0; i < c_rr; ++i)
            {
                float a_cos = (h_s - i) * cosf(angle), a_sin = (h_s - i) * sinf(angle);
                float bright = float(i) / c_rr;
                Color c(1, 1, 1);
                c.blend(0.5, 0.5, 0.5, bright);

                IGradient *gr = s->radial_gradient(x - (h_s >> 1), y + (h_s >> 1), 0, x - (h_s >> 1), y + (h_s >> 1), h_s * 1.5);
                gr->add_color(0.0, c);
                gr->add_color(1.0, 0, 0, 0);

                s->line(x + a_cos, y + a_sin, x - a_cos, y - a_sin, c_rr - i, gr);
                s->line(x - a_sin, y + a_cos, x + a_sin, y - a_cos, c_rr - i, gr);

                delete gr;
            }

            s->set_line_cap(cap);
        }

        void LSPMountStud::draw(ISurface *s)
        {
            font_parameters_t fp;
            text_parameters_t tp;

            // Prepare palette
            Color bg_color(sBgColor);
            Color logo(sColor);
            Color font(sFont.raw_color());

            logo.scale_lightness(brightness());
            font.scale_lightness(brightness());

            // Draw background
            s->clear(bg_color);

            bool pressed    = bPressed;
            float aa        = s->set_antialiasing(true);

            sFont.get_parameters(s, &fp);
            sFont.get_text_parameters(s, &tp, &sText);

            // Draw screws
            if (nAngle & 0x02)
            {
                size_t screw    = (SCREW_SIZE - 4) >> 1;
                size_t stud     = STUD_H + 4;
                draw_screw(s, screw, stud * 0.5, M_PI * 1.0 / 8.0 + M_PI / 16.0);
                draw_screw(s, sSize.nWidth - screw, stud * 0.5, M_PI * 3.0 / 8.0 + M_PI / 16.0);
            }
            else
            {
                float angle  = (nAngle & 1);
                size_t screw = (nAngle & 1) ? (SCREW_SIZE >> 1) : sSize.nWidth - (SCREW_SIZE >> 1);

                draw_screw(s, screw, STUD_H * 0.75, M_PI * (angle + 1) / 8 + M_PI / 16);
                draw_screw(s, screw, sSize.nHeight - STUD_H * 0.75, M_PI * (angle + 3) / 8 + M_PI / 16);
            }

            // Draw logo bar
            float logo_l    = logo.lightness();
            float l_rr      = 3;
            ssize_t l_x     = sLogo.nLeft - sSize.nLeft;
            ssize_t l_y     = sLogo.nTop - sSize.nTop;
            ssize_t lw      = tp.Width + 10;
            ssize_t lh      = tp.Height + 10;

            for (size_t i=0; i<=l_rr; ++i)
            {
                float bright = logo_l * (i + 1) / (l_rr + 1);

                IGradient *gr = (pressed) ?
                        s->radial_gradient(l_x - lw, l_y + lh, lw >> 2, l_x - lw, l_y + lh , lw) :
                        s->radial_gradient(l_x + lw, l_y, lw >> 2, l_x + lw, l_y , lw);

                logo.lightness(bright * 1.5f);
                gr->add_color(0.0f, logo);
                logo.lightness(bright);
                gr->add_color(1.0f, logo);

                s->fill_round_rect(l_x + i, l_y + i, sLogo.nWidth - i*2, sLogo.nHeight - i*2, 5 - i, SURFMASK_ALL_CORNER, gr);
                delete gr;
            }

            // Draw logo text
//            if (pressed)
//                font.darken(0.5f);

            // Output text
            sFont.draw(s,
                    l_x + ((sLogo.nWidth - tp.Width) * 0.5f),
                    l_y + ((sLogo.nHeight - fp.Height) * 0.5f) + fp.Ascent,
                    font,
                    &sText);
            s->set_antialiasing(aa);
        }

        void LSPMountStud::size_request(size_request_t *r)
        {
            // Create surface
            ISurface *s = pDisplay->create_surface(1, 1);
            if (s == NULL)
                return;

            font_parameters_t fp;
            text_parameters_t tp1, tp2;

            sFont.get_parameters(s, &fp);
            sFont.get_text_parameters(s, &tp1, &sText);
            sFont.get_text_parameters(s, &tp2, "WWW0");

            s->destroy();
            delete s;

            r->nMinWidth    = (tp1.Width < tp2.Width) ? tp2.Width : tp1.Width;
            r->nMinHeight   = fp.Height;

            if (!(nAngle & 0x2))
            {
                r->nMinWidth   += STUD_H * 1.5;
                r->nMinHeight  += CURVE * 2 + STUD_H*4;

                r->nMaxWidth    = r->nMinWidth;
                r->nMaxHeight   = -1;
            }
            else
            {
                r->nMinWidth   += 12 + 32 + STUD_H*4;
                r->nMinHeight  += 8;

                r->nMaxWidth    = -1;
                r->nMaxHeight   = r->nMinHeight;
            }

//            if (nAngle & 0x2)
//            {
//                r->nMinWidth   += 12 + 32 + ((STUD_H + 4) << 1);
//                r->nMinHeight  += 8;
//                if (r->nMinHeight < (STUD_H + 4))
//                    r->nMinHeight   = STUD_H + 4;
//
//
//            }
//            else
//            {
//                r->nMinWidth   += (8 + 3) * 2;
//                r->nMinHeight  += (STUD_H + CURVE)* 2;
//
//
//            }
        }

        void LSPMountStud::realize(const realize_t *r)
        {
            LSPWidget::realize(r);

            // Create surface
            ISurface *s = pDisplay->create_surface(1, 1);
            if (s == NULL)
                return;

            font_parameters_t fp;
            text_parameters_t tp1, tp2;

            sFont.get_parameters(s, &fp);
            sFont.get_text_parameters(s, &tp1, &sText);
            sFont.get_text_parameters(s, &tp2, "WWW0");

            s->destroy();
            delete s;

            sLogo.nWidth    = (tp1.Width < tp2.Width) ? tp2.Width : tp1.Width;
            sLogo.nHeight   = fp.Height;

            if (!(nAngle & 0x2))
            {
                sLogo.nWidth   += 6;
                sLogo.nHeight  += CURVE * 2;
                sLogo.nLeft     = sSize.nLeft + ((r->nWidth  - sLogo.nWidth) >> 1);
                sLogo.nTop      = sSize.nTop  + ((r->nHeight - sLogo.nHeight) >> 1);
            }
            else
            {
                sLogo.nWidth   += 12 + 32;
                sLogo.nHeight  += 8;
                sLogo.nLeft     = sSize.nLeft + ((r->nWidth  - sLogo.nWidth) >> 1);
                sLogo.nTop      = sSize.nTop  + ((r->nHeight - sLogo.nHeight) >> 1);
            }
        }

        bool LSPMountStud::mouse_over_logo(ssize_t x, ssize_t y)
        {
            lsp_trace("x=%d, y=%d, logo = {%d, %d, %d, %d}, size = {%d, %d, %d, %d}",
                    int(x), int(y),
                    int(sLogo.nLeft), int(sLogo.nTop), int(sLogo.nWidth), int(sLogo.nHeight),
                    int(sSize.nLeft), int(sSize.nTop), int(sSize.nWidth), int(sSize.nHeight)
            );

            return (x >= sLogo.nLeft) &&
                    (x < (sLogo.nLeft + sLogo.nWidth)) &&
                    (y >= sLogo.nTop) &&
                    (y < (sLogo.nTop + sLogo.nHeight));
        }

        status_t LSPMountStud::on_mouse_down(const ws_event_t *e)
        {
            lsp_trace("button=%d", int(e->nCode));
            nButtons |= 1 << e->nCode;

            bool pressed = ((nButtons == (1 << MCB_LEFT)) && (mouse_over_logo(e->nLeft, e->nTop)));
            if (pressed != bPressed)
            {
                bPressed        = pressed;
                query_draw();
            }
            return STATUS_OK;
        }

        status_t LSPMountStud::on_mouse_up(const ws_event_t *e)
        {
            lsp_trace("button=%d", int(e->nCode));
            nButtons &= ~(1 << e->nCode);

            bool over    = mouse_over_logo(e->nLeft, e->nTop);
            bool pressed = ((nButtons == (1 << MCB_LEFT)) && (over));
            if (pressed != bPressed)
            {
                bPressed        = pressed;
                query_draw();
            }

            if ((nButtons == 0) && (e->nCode == MCB_LEFT))
            {
                if (over)
                {
                    ws_event_t ev = *e;
                    sSlots.execute(LSPSLOT_SUBMIT, this, &ev);
                }
            }
            return STATUS_OK;
        }

        status_t LSPMountStud::on_mouse_move(const ws_event_t *e)
        {
            bool pressed = ((nButtons == (1 << MCB_LEFT)) && (mouse_over_logo(e->nLeft, e->nTop)));
            if (pressed != bPressed)
            {
                bPressed        = pressed;
                query_draw();
            }
            return STATUS_OK;
        }

        status_t LSPMountStud::on_submit()
        {
            return STATUS_OK;
        }
    
    } /* namespace tk */
} /* namespace lsp */
