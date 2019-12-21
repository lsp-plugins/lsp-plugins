/*
 * LSPFader.cpp
 *
 *  Created on: 19 нояб. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPFader::metadata = { "LSPFader", &LSPWidget::metadata };
        
        LSPFader::LSPFader(LSPDisplay *dpy):
            LSPWidget(dpy),
            sColor(this)
        {
            fMin            = 0.0f;
            fMax            = 1.0f;
            fValue          = 0.5f;
            fDefault        = 0.5f;
            fStep           = 0.01f;
            fTinyStep       = 0.001f;
            nMinSize        = 32;
            nAngle          = 0;
            nLastV          = 0;
            nButtons        = 0;
            nBtnLength      = 20;
            nBtnWidth       = 16;
            nXFlags         = 0;
            fCurrValue      = 0.0f;
            fLastValue      = 0.0f;
            pClass          = &metadata;
        }
        
        LSPFader::~LSPFader()
        {
            do_destroy();
        }

        status_t LSPFader::init()
        {
            status_t result = LSPWidget::init();
            if (result != STATUS_OK)
                return result;

            init_color(C_LABEL_TEXT, &sColor);

            ui_handler_id_t id = 0;
            id = sSlots.add(LSPSLOT_CHANGE, slot_on_change, self());

            return (id >= 0) ? STATUS_OK : -id;
        }

        void LSPFader::destroy()
        {
            do_destroy();
            LSPWidget::destroy();
        }

        void LSPFader::do_destroy()
        {
        }

        status_t LSPFader::slot_on_change(LSPWidget *sender, void *ptr, void *data)
        {
            LSPFader *_this = widget_ptrcast<LSPFader>(ptr);
            return (_this != NULL) ? _this->on_change() : STATUS_BAD_ARGUMENTS;
        }

        void LSPFader::set_value(float value)
        {
            value       = limit_value(value);

            if (value == fValue)
                return;

            fValue      = value;
            query_draw();
        }

        void LSPFader::set_default_value(float value)
        {
            fDefault    = value;
        }

        float LSPFader::limit_value(float value)
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

            return value;
        }

        void LSPFader::set_step(float value)
        {
            fStep       = value;
        }

        void LSPFader::set_tiny_step(float value)
        {
            fTinyStep   = value;
        }

        void LSPFader::set_min_value(float value)
        {
            if (value == fMin)
                return;

            fMin        = value;
            query_draw();
            set_value(fValue);
        }

        void LSPFader::set_max_value(float value)
        {
            if (value == fMax)
                return;

            fMax        = value;
            query_draw();
            set_value(fValue);
        }

        void LSPFader::set_min_size(ssize_t value)
        {
            if (value < 8)
                value = 8;
            if (value == nMinSize)
                return;
            nMinSize    = value;
            query_resize();
        }

        void LSPFader::set_angle(size_t value)
        {
            size_t old_angle = nAngle & 0x3;
            nAngle  = value;
            if ((nAngle & 0x03) == old_angle)
                return;
            query_resize();
        }

        void LSPFader::set_button_width(size_t value)
        {
            if (value < 8)
                value = 8;
            if (value == nBtnWidth)
                return;
            nBtnWidth   = value;
            query_resize();
        }

        void LSPFader::set_button_length(size_t value)
        {
            if (value < 8)
                value = 8;
            if (value == nBtnLength)
                return;
            nBtnLength  = value;
            query_resize();
        }

        void LSPFader::size_request(size_request_t *r)
        {
            size_t cap = (nBtnLength > 8) ? nBtnLength : 8;
            if (cap < nBtnLength)
                cap     = nBtnLength;

            if (nAngle & 1) // Vertical
            {
                r->nMinWidth        = (nBtnWidth > 8) ? nBtnWidth : 8;
                r->nMinHeight       = cap + nMinSize;
                r->nMaxWidth        = r->nMinWidth;
                r->nMaxHeight       = (nFlags & F_VFILL) ? -1 : r->nMaxHeight;
            }
            else // Horizontal
            {
                r->nMinWidth        = cap + nMinSize;
                r->nMinHeight       = (nBtnWidth > 8) ? nBtnWidth : 8;
                r->nMaxWidth        = (nFlags & F_HFILL) ? -1 : r->nMaxWidth;
                r->nMaxHeight       = r->nMinHeight;
            }
        }

        status_t LSPFader::on_change()
        {
            return STATUS_OK;
        }

        bool LSPFader::check_mouse_over(ssize_t x, ssize_t y)
        {
            float p         = get_normalized_value();

            ssize_t rw      = (nAngle & 1) ? nBtnWidth  : nBtnLength;
            ssize_t rh      = (nAngle & 1) ? nBtnLength : nBtnWidth;
            ssize_t bl      = (nAngle & 1) ? (sSize.nWidth - nBtnWidth) >> 1 : (sSize.nWidth  - nBtnLength) * p;
            ssize_t bt      = (nAngle & 1) ? (sSize.nHeight - nBtnLength) * p : (sSize.nHeight - nBtnWidth) >> 1;
            bl             += sSize.nLeft;
            bt             += sSize.nTop;

            return ((x >= bl) && (y >= bt) && (x < (bl + rw)) && (y < (bt + rh)));
        }

        status_t LSPFader::on_mouse_down(const ws_event_t *e)
        {
            if (nButtons == 0)
            {
                if (check_mouse_over(e->nLeft, e->nTop))
                {
                    if (e->nCode == MCB_RIGHT)
                        nXFlags        |= F_PRECISION | F_MOVER;
                    else if (e->nCode == MCB_LEFT)
                        nXFlags        |= F_MOVER;
                    else
                        nXFlags        |= F_IGNORE;
                }
                else
                    nXFlags        |= F_IGNORE;

                if (!(nXFlags & F_IGNORE))
                {
                    nLastV      = (nAngle & 1) ? e->nTop : e->nLeft;
                    fLastValue  = fValue;
                    fCurrValue  = fValue;
                }
            }

            nButtons       |= (1 << e->nCode);
            if (nXFlags & F_IGNORE)
                return STATUS_OK;

            size_t key      = (nXFlags & F_PRECISION) ? MCB_RIGHT : MCB_LEFT;

            // Update value
            float value     = (nButtons == size_t(1 << key)) ? fCurrValue : fLastValue;
            value           = limit_value(value);

            if (value != fValue)
            {
                fValue      = value;
                query_draw();
                sSlots.execute(LSPSLOT_CHANGE, this);
            }

            return STATUS_OK;
        }

        status_t LSPFader::on_mouse_up(const ws_event_t *e)
        {
            nButtons       &= ~(1 << e->nCode);
            if (nXFlags & F_IGNORE)
            {
                if (nButtons == 0)
                {
                    if (check_mouse_over(e->nLeft, e->nTop))
                        nXFlags     = F_MOVER;
                    else
                        nXFlags     = 0;
                }
                return STATUS_OK;
            }

            size_t key      = (nXFlags & F_PRECISION) ? MCB_RIGHT : MCB_LEFT;
            float value;

            if (nButtons == 0) // All mouse buttons are released now
            {
                nXFlags     = 0;
                value       = (e->nCode == key) ? fCurrValue : fLastValue;
            }
            else if (nButtons == size_t(1 << key)) // Currently pressed initially selected button
                value       = fCurrValue;
            else
                value       = fLastValue;

            // Update value
            value           = limit_value(value);

            if (value != fValue)
            {
                fValue      = value;
                query_draw();
                sSlots.execute(LSPSLOT_CHANGE, this);
            }

            return STATUS_OK;
        }

        float LSPFader::get_normalized_value()
        {
            size_t a        = nAngle & 3;
            float delta     = fMax - fMin;
            float v         = (delta == 0.0f) ? 0.5f : (fValue - fMin) / delta;
            return ((a == 1) || (a == 2)) ? 1.0f - v : v;
        }

        status_t LSPFader::on_mouse_move(const ws_event_t *e)
        {
            if (nXFlags & F_IGNORE)
                return STATUS_OK;

            size_t key = (nXFlags & F_PRECISION) ? MCB_RIGHT : MCB_LEFT;
            if (nButtons != size_t(1 << key))
            {
                if ((nButtons == 0) && (check_mouse_over(e->nLeft, e->nTop)))
                    nXFlags    |= F_MOVER;
                else
                    nXFlags    &= ~F_MOVER;
                return STATUS_OK;
            }

            // Different behaviour for slider
            nXFlags      |= F_MOVER;
            ssize_t value = (nAngle & 1) ? e->nTop : e->nLeft;
            float result  = fLastValue;
            if (value != nLastV)
            {
                ssize_t range = (nAngle & 1) ? sSize.nHeight - nBtnLength : sSize.nWidth - nBtnLength;
                float delta   = (fMax - fMin) * float(value - nLastV) / float(range);

                if (nXFlags & F_PRECISION)
                    delta       *= (fTinyStep / fStep);
                size_t a      = nAngle & 3;
                result        = limit_value(((a == 1) || (a == 2)) ? result - delta : result + delta);
            }

            if (fCurrValue != result)
            {
                fCurrValue  = result;
                fValue      = result;
                lsp_trace("set value to %f, min=%f, max=%f", fValue, fMin, fMax);
                query_draw();

                sSlots.execute(LSPSLOT_CHANGE, this);
            }

            return STATUS_OK;
        }

        status_t LSPFader::on_mouse_scroll(const ws_event_t *e)
        {
            float step      = (e->nState & MCF_SHIFT) ? fTinyStep : fStep;
            if (((nAngle & 3) == 0) || ((nAngle & 3) == 3))
                step            = - step;
            float delta     = (e->nCode == MCD_UP) ? step : -step;
            float result    = limit_value(fValue + delta);

            if (result != fValue)
            {
                fValue          = result;
                query_draw();
                sSlots.execute(LSPSLOT_CHANGE, this);
            }

            return STATUS_OK;
        }

        status_t LSPFader::on_mouse_dbl_click(const ws_event_t *e)
        {
            if (e->nCode != MCB_LEFT)
                return STATUS_OK;
            float value     = limit_value(fDefault);
            if (value == fValue)
                return STATUS_OK;

            fValue          = value;
            query_draw();
            return sSlots.execute(LSPSLOT_CHANGE, this);
        }

        void LSPFader::draw(ISurface *s)
        {
            // Prepare palette
            Color bg_color(sBgColor);
            Color color(sColor);
            color.scale_lightness(brightness());

            // Clear surface
            s->clear(bg_color);
            ssize_t l, t;

            if (nAngle & 1) // Vertical
            {
                l = (sSize.nWidth - 8) >> 1;
                t = (nBtnLength - 8) >> 1;
            }
            else // Horizontal
            {
                l = (nBtnLength - 8) >> 1;
                t = (sSize.nHeight - 8) >> 1;
            }

            // Draw the hole
            bool aa = s->set_antialiasing(true);
            Color hole(bg_color);
            float hlb = hole.lightness() + 0.5f;
            float hld = 0;
            float r = (nAngle & 1) ? sqrtf(sSize.nHeight*sSize.nHeight + 64) : sqrtf(sSize.nWidth*sSize.nWidth + 64);

            for (size_t i=0; i<3; ++i)
            {
                float bright = (hlb - hld) * (3 - i) / 3 + hld;
                hole.lightness(bright);

                if (nAngle & 1) // Vertical
                {
                    IGradient *gr = s->radial_gradient(l, sSize.nHeight - t, 1, l, sSize.nHeight - t, r);
                    gr->add_color(0.0, hole);
                    gr->add_color(1.0, 0.5 * hole.red(), 0.5 *  hole.green(), 0.5 * hole.blue());

                    s->fill_round_rect(l+i, t+i, 8-i*2, sSize.nHeight - nBtnLength + 8 - i*2, 4-i, SURFMASK_ALL_CORNER, gr);
                    delete gr;
                }
                else
                {
                    IGradient *gr = s->radial_gradient(l, t, 1, l, t, r);
                    gr->add_color(0.0, hole);
                    gr->add_color(1.0, 0.5 * hole.red(), 0.5 *  hole.green(), 0.5 * hole.blue());

                    s->fill_round_rect(l+i, t+i, sSize.nWidth - nBtnLength + 8 - i*2, 8-i*2, 4-i, SURFMASK_ALL_CORNER, gr);
                    delete gr;
                }
            }
            hole.set_rgb(0.0f, 0.0f, 0.0f);

            if (nAngle & 1) // Vertical
                s->fill_round_rect(l+3, t+3, 2, sSize.nHeight - nBtnLength + 2, 1, SURFMASK_ALL_CORNER, hole);
            else
                s->fill_round_rect(l+3, t+3, sSize.nWidth - nBtnLength + 2, 2, 1, SURFMASK_ALL_CORNER, hole);

            s->set_antialiasing(aa);

            // Draw the button
            float p         = get_normalized_value();
            ssize_t rw      = (nAngle & 1) ? nBtnWidth  : nBtnLength;
            ssize_t rh      = (nAngle & 1) ? nBtnLength : nBtnWidth;
            ssize_t bl      = (nAngle & 1) ? (sSize.nWidth - nBtnWidth) >> 1 : (sSize.nWidth  - nBtnLength) * p;
            ssize_t bt      = (nAngle & 1) ? (sSize.nHeight - nBtnLength) * p : (sSize.nHeight - nBtnWidth) >> 1;

            ssize_t b_l     = 4;
            ssize_t b_rr    = 2;
            float lightness = sColor.lightness();
            float b_rad     = sqrtf(nBtnWidth*nBtnWidth + nBtnLength*nBtnLength);

            for (ssize_t i=0; (i++)<b_l; )
            {
                float bright = lightness * sqrtf(i * i) / b_l;

                IGradient *gr = s->radial_gradient(bl, bt + rh, b_rad * 0.25f, bl, bt + rh, b_rad * 3.0f);

                Color cl(color);
                cl.lightness(bright);
                gr->add_color(0.0f, cl);
                cl.darken(0.9f);
                gr->add_color(1.0f, cl);

                s->fill_round_rect(bl + i, bt + i, rw - i*2, rh - i*2, b_rr, SURFMASK_ALL_CORNER, gr);
                delete gr; // Delete gradient!
            }
        }

        mouse_pointer_t LSPFader::active_cursor() const
        {
            return ((nXFlags & (F_MOVER | F_IGNORE)) == F_MOVER) ?
                    (nAngle & 1) ? MP_SIZE_NS : MP_SIZE_WE
                    : cursor();
        }
    
    } /* namespace tk */
} /* namespace lsp */
