/*
 * LSPKnob.cpp
 *
 *  Created on: 10 июл. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

#define LOG_BASE            1.0f
#define DB_BASE             0.025f

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPKnob::metadata = { "LSPKnob", &LSPWidget::metadata };

        LSPKnob::LSPKnob(LSPDisplay *dpy): LSPWidget(dpy),
            sColor(this),
            sScaleColor(this),
            sHoleColor(this)
        {
            nSize       = 24;
            fBalance    = 0.0f;
            nButtons    = 0;

            fValue      = 0.5f;
            fDflValue   = 0.5f;
            fStep       = 0.01f;
            fTinyStep   = 0.001f;
            fMin        = 0.0f;
            fMax        = 1.0f;
            bCycling    = false;

            nState      = 0;
            nLastY      = 0;

            pClass      = &metadata;
        }

        LSPKnob::~LSPKnob()
        {
        }

        status_t LSPKnob::init()
        {
            status_t res = LSPWidget::init();
            if (res != STATUS_OK)
                return res;

            res = sHoleColor.bind("hole_color");
            if (res != STATUS_OK)
                return res;

            init_color(C_KNOB_CAP, &sColor);
            init_color(C_KNOB_SCALE, &sScaleColor);
            init_color(C_LABEL_TEXT, &sTipColor);

            if (!sSlots.add(LSPSLOT_CHANGE))
                return STATUS_NO_MEM;

            return STATUS_OK;
        }

        void LSPKnob::set_size(size_t value)
        {
            nSize       = value;
            query_resize();
        }

        float LSPKnob::limit_value(float value)
        {
            if (bCycling)
            {
                if (fMin < fMax)
                {
                    while (value >= fMax)
                        value  -= (fMax - fMin);
                    while (value < fMin)
                        value  += (fMax - fMin);
                }
                else
                {
                    while (value > fMin)
                        value  -= (fMin - fMax);
                    while (value <= fMax)
                        value  += (fMin - fMax);
                }
            }
            else
            {
                if (fMin < fMax)
                {
                    if (value < fMin)
                        return fMin;
                    else if (value > fMax)
                        return fMax;
                }
                else
                {
                    if (value < fMax)
                        return fMax;
                    else if (value > fMin)
                        return fMin;
                }
            }

            return value;
        }

        void LSPKnob::set_balance(float value)
        {
            if (value == fBalance)
                return;

            fBalance    = value;
            query_draw();
        }

        void LSPKnob::set_value(float value)
        {
            value       = limit_value(value);

            if (value == fValue)
                return;

            fValue      = value;
            query_draw();
        }

        void LSPKnob::set_default_value(float value)
        {
            value       = limit_value(value);
            if (fDflValue == value)
                return;

            fDflValue   = value;
        }

        void LSPKnob::set_step(float value)
        {
            fStep       = value;
        }

        void LSPKnob::set_tiny_step(float value)
        {
            fTinyStep   = value;
        }

        void LSPKnob::set_min_value(float value)
        {
            if (value == fMin)
                return;

            fMin        = value;
            query_draw();
            set_value(fValue);
        }

        void LSPKnob::set_max_value(float value)
        {
            if (value == fMax)
                return;

            fMax        = value;
            query_draw();
            set_value(fValue);
        }

        void LSPKnob::set_cycling(bool cycling)
        {
            if (cycling == bCycling)
                return;
            bCycling    = cycling;
            query_draw();
        }

        float LSPKnob::get_normalized_value(float value)
        {
            if (fMin < fMax)
            {
                if (value < fMin)
                    value = fMin;
                else if (value > fMax)
                    value = fMax;
            }
            else
            {
                if (value < fMax)
                    value = fMax;
                else if (value > fMin)
                    value = fMin;
            }

            // Float and other values
            return (value - fMin) / (fMax - fMin);
        }

        void LSPKnob::set_normalized_value(float value)
        {
            // Limit value
            if (value < 0.0f)
                value = 0.0f;
            else if (value > 1.0f)
                value = 1.0f;

            // Store new value
            set_value(fMin + (fMax - fMin) * value);
            sSlots.execute(LSPSLOT_CHANGE, this);
        }

        void LSPKnob::update_value(float delta)
        {
            lsp_trace("value=%f, delta=%f", fValue, delta);

            // Check that value is in range
            set_value(fValue + delta);
            sSlots.execute(LSPSLOT_CHANGE, this);
        }

        void LSPKnob::on_click(ssize_t x, ssize_t y)
        {
            x              -= sSize.nLeft;
            y              -= sSize.nTop;

            ssize_t cx      = ssize_t(sSize.nWidth) >> 1;
            ssize_t cy      = ssize_t(sSize.nHeight) >> 1;
            float dx        = x - cx;
            float dy        = cy - y;
            float d         = sqrtf(dx * dx + dy * dy);
            if (d <= 0.0f)
                return;

            float angle     = asinf(dy / d);
            if (bCycling)
            {
                if (dx < 0.0f)
                    angle           = M_PI - angle;
                // Angle is now between -PI/2 .. 3*PI/2
                if (angle < M_PI * 0.5f)
                    angle          += 1.5f * M_PI;
                else
                    angle          -= M_PI * 0.5f;

                set_normalized_value(1.0f - angle / (M_PI * 2.0f));
            }
            else
            {
                if (angle < (-M_PI / 3.0))
                {
                    set_normalized_value((dx > 0) ? 1.0f : 0.0f);
                    return;
                }
                if (dx < 0.0f)
                    angle           = M_PI - angle;

                angle          += M_PI / 3.0;

                // Update value
                set_normalized_value(1.0f - (angle / (5.0f * M_PI  / 3.0f)));
            }
        }

        size_t LSPKnob::check_mouse_over(ssize_t x, ssize_t y)
        {
            x              -= sSize.nLeft;
            y              -= sSize.nTop;

            ssize_t cx      = ssize_t(sSize.nWidth) >> 1;
            ssize_t cy      = ssize_t(sSize.nHeight) >> 1;
            ssize_t dx      = x - cx;
            ssize_t dy      = y - cy;
            ssize_t hole_r  = (nSize >> 1) + 1;

    //        lsp_trace("cx=%d, cy=%d, x=%d, y=%d, dx=%d, dy=%d, r=%d", int(cx), int(cy), int(x), int(y), int(dx), int(dy), int(r));
            ssize_t delta   = (dx * dx + dy * dy);

            if (delta <= ssize_t(hole_r * hole_r))
                return S_MOVING;

            ssize_t scale_in_r      = hole_r + 2;
            ssize_t scale_out_r     = scale_in_r + 5;

            if (delta >= ssize_t(scale_in_r * scale_in_r))
            {
                if (delta <= ssize_t(scale_out_r * scale_out_r))
                    return S_CLICK;
            }

            return S_NONE;
        }

        void LSPKnob::size_request(size_request_t *r)
        {
            r->nMinWidth    = nSize + (10 << 1);
            r->nMinHeight   = nSize + (10 << 1);
            r->nMaxWidth    = r->nMinWidth;
            r->nMaxHeight   = r->nMinHeight;
        }

        status_t LSPKnob::on_mouse_down(const ws_event_t *e)
        {
            take_focus();
//            lsp_trace("x=%d, y=%d, state=%x, code=%x", int(e->nLeft), int(e->nTop), int(e->nState), int(e->nCode));
            if ((nButtons == 0) && ((e->nCode == MCB_LEFT) || (e->nCode == MCB_RIGHT)))
            {
                size_t flags = check_mouse_over(e->nLeft, e->nTop);
                if (flags != 0)
                    nState      = flags;
            }

            nButtons   |= (1 << e->nCode);
            nLastY      = e->nTop;

            return STATUS_OK;
        }

        status_t LSPKnob::on_mouse_up(const ws_event_t *e)
        {
//            lsp_trace("x=%d, y=%d, state=%x, code=%x", int(e->nLeft), int(e->nTop), int(e->nState), int(e->nCode));
            nButtons &= ~(1 << e->nCode);
            nLastY = e->nTop;
            if (nButtons == 0)
            {
                if ((nState == S_CLICK) && (e->nCode == MCB_LEFT))
                    on_click(e->nLeft, e->nTop);
                nState      = 0;
            }

            return STATUS_OK;
        }

        status_t LSPKnob::on_mouse_move(const ws_event_t *e)
        {
//            lsp_trace("x=%d, y=%d, state=%x, code=%x", int(e->nLeft), int(e->nTop), int(e->nState), int(e->nCode));
            if (nState == S_MOVING)
            {
                if (!(nButtons & ((1 << MCB_LEFT) | (1 << MCB_RIGHT))))
                    return STATUS_OK;

                // Update value
                float step = (nButtons & (1 << MCB_RIGHT)) ? fTinyStep : fStep;
                update_value(step * (nLastY - e->nTop));
                nLastY = e->nTop;
            }
            else if (nState == S_CLICK)
            {
                if (!(nButtons & (1 << MCB_LEFT)))
                    return STATUS_OK;

                on_click(e->nLeft, e->nTop);
            }

            return STATUS_OK;
        }

        status_t LSPKnob::on_mouse_scroll(const ws_event_t *e)
        {
//            lsp_trace("x=%d, y=%d, state=%x, code=%x", int(e->nLeft), int(e->nTop), int(e->nState), int(e->nCode));
            float step = (e->nState & MCF_SHIFT) ? fTinyStep : fStep;

            // Update value
            float delta = 0.0;
            if (e->nCode == MCD_UP)
                delta   = step;
            else if (e->nCode == MCD_DOWN)
                delta   = -step;
            else
                return STATUS_OK;

            update_value(delta);

            return STATUS_OK;
        }

        status_t LSPKnob::on_mouse_dbl_click(const ws_event_t *e)
        {
//            lsp_trace("x=%d, y=%d, state=%x, code=%x", int(e->nLeft), int(e->nTop), int(e->nState), int(e->nCode));
            if (check_mouse_over(e->nLeft, e->nTop) == S_NONE)
                return STATUS_OK;

            set_value(fDflValue);
            sSlots.execute(LSPSLOT_CHANGE, this);

            return STATUS_OK;
        }

        void LSPKnob::draw(ISurface *s)
        {
            // Prepare the color palette
            float bright    = brightness();

            Color scol(sScaleColor);
            Color sdcol(sScaleColor);
            Color hole(sHoleColor);
            Color bg_color(sBgColor);
            Color cap(sColor);
            Color tip(sTipColor);

            sdcol.blend(bg_color, 0.75f);

            scol.scale_lightness(bright);
            sdcol.scale_lightness(bright);
            cap.scale_lightness(bright);
            tip.scale_lightness(bright);

            // Get actual values
            float value     = get_normalized_value(fValue);
            float balance   = get_normalized_value(fBalance);

            // Draw background
            s->fill_rect(0, 0, sSize.nWidth, sSize.nHeight, bg_color);

            // Calculate real boundaries
            ssize_t c_x     = (sSize.nWidth >> 1);
            ssize_t c_y     = (sSize.nHeight >> 1);

            // Draw scale background
            float knob_r        = (nSize >> 1);
            float hole_r        = (nSize >> 1) + 1;
            float scale_in_r    = hole_r + 2;
            float scale_out_r   = scale_in_r + 5;

            float delta, base, v_angle1, v_angle2;
            size_t nsectors;

            bool aa = s->set_antialiasing(true);

            if (!bCycling)
            {
                nsectors      = 20;
                base          = 2.0f * M_PI / 3.0f;
                delta         = 5.0f * M_PI / 3.0f;
                v_angle1      = base + value * delta;
                v_angle2      = base + balance * delta;

                s->fill_sector(c_x, c_y, scale_out_r, base, base + delta, sdcol);
                if (value < balance)
                    s->fill_sector(c_x, c_y, scale_out_r, v_angle1, v_angle2, scol);
                else
                    s->fill_sector(c_x, c_y, scale_out_r, v_angle2, v_angle1, scol);

                s->fill_circle(c_x, c_y, scale_in_r, bg_color);
                s->fill_circle(c_x, c_y, hole_r, hole);
            }
            else
            {
                nsectors      = 24;
                base          = 1.5f * M_PI;
                delta         = 2.0f * M_PI;
                v_angle1      = base + value * delta;
                v_angle2      = base + balance * delta * 0.5f;

                s->fill_circle(c_x, c_y, scale_out_r, sdcol);
                if (value < balance)
                    s->fill_sector(c_x, c_y, scale_out_r, v_angle1, v_angle2, scol);
                else
                    s->fill_sector(c_x, c_y, scale_out_r, v_angle2, v_angle1, scol);

                s->fill_circle(c_x, c_y, scale_in_r, bg_color);
                s->fill_circle(c_x, c_y, hole_r, hole);
            }

            // Draw scales: overall 10 segments separated by 2 sub-segments
            delta   = 0.25f * M_PI / 3.0f;

            for (size_t i=0; i<=nsectors; ++i)
            {
                float angle = base + delta * i;
                float r2    = scale_in_r + 3.0f * (i & 1);
                float f_sin = sinf(angle), f_cos = cosf(angle);

                s->line(c_x + (scale_out_r + 1) * f_cos,
                        c_y + (scale_out_r + 1) * f_sin,
                        c_x + r2 * f_cos,
                        c_y + r2 * f_sin,
                        1.0f, bg_color);
            }

            // Draw knob body
            ssize_t k_l = (nSize >> 3);
            if (k_l < 2)
                k_l = 2;
            float k_r = knob_r;
            float f_sin = sinf(v_angle1), f_cos = cosf(v_angle1);

            // Draw cap and tip
            for (ssize_t i=0; (i++)<k_l; )
            {
                float bright = sqrtf(i * i) / k_l;
                scol.blend(cap, hole, bright);
                sdcol.blend(scol, hole, 0.5f);

                // Draw cap
                IGradient *gr = s->radial_gradient(c_x + k_r, c_y - k_r, knob_r, c_x + k_r, c_y - k_r, knob_r * 4.0);
                gr->add_color(0.0f, scol);
                gr->add_color(1.0f, sdcol);
                s->fill_circle(c_x, c_y, k_r, gr);
                delete gr;

                // Draw tip
                scol.copy(tip);
                scol.blend(hole, bright);
                s->line(c_x + (knob_r * 0.25f) * f_cos, c_y + (knob_r * 0.25f) * f_sin,
                        c_x + k_r * f_cos, c_y + k_r * f_sin, 3.0f, scol);

                if ((--k_r) < 0.0f)
                    k_r = 0.0f;
            }

            s->set_antialiasing(aa);
        }

    } /* namespace tk */
} /* namespace lsp */
