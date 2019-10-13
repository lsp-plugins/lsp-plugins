/*
 * LSPScrollBar.cpp
 *
 *  Created on: 3 авг. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPScrollBar::metadata = { "LSPScrollBar", &LSPWidget::metadata };

        LSPScrollBar::LSPScrollBar(LSPDisplay *dpy, bool horizontal): LSPWidget(dpy),
            sColor(this),
            sSelColor(this)
        {
            fMin            = 0.0f;
            fMax            = 1.0f;
            fValue          = 0.5f;
            fStep           = 0.01f;
            fTinyStep       = 0.001f;
            nSize           = 12;
            nFlags          = 0;
            nButtons        = 0;
            nLastV          = 0;
            fLastValue      = 0.0f;
            fCurrValue      = 0.0f;
            enOrientation   = (horizontal) ? O_HORIZONTAL : O_VERTICAL;
            pClass          = &metadata;
            enActiveCursor  = enCursor;

            sTimer.bind(pDisplay);
            sTimer.set_handler(timer_handler, this);
        }

        LSPScrollBar::~LSPScrollBar()
        {
            do_destroy();
        }

        status_t LSPScrollBar::init()
        {
            status_t result = LSPWidget::init();
            if (result != STATUS_OK)
                return result;

            init_color(C_LABEL_TEXT, &sColor);
            init_color(C_KNOB_SCALE, &sSelColor);

            ui_handler_id_t id = 0;
            id = sSlots.add(LSPSLOT_CHANGE, slot_on_change, self());
            enActiveCursor  = cursor();

            return (id >= 0) ? STATUS_OK : -id;
        }

        status_t LSPScrollBar::set_cursor(mouse_pointer_t mp)
        {
            if (mp == enCursor)
                return STATUS_OK;
            if (enActiveCursor == enCursor)
                enActiveCursor = mp;
            return LSPWidget::set_cursor(mp);
        }

        mouse_pointer_t LSPScrollBar::active_cursor() const
        {
            return enActiveCursor;
        }

        void LSPScrollBar::destroy()
        {
            do_destroy();
            LSPWidget::destroy();
        }

        void LSPScrollBar::do_destroy()
        {
            sTimer.cancel();
        }

        status_t LSPScrollBar::timer_handler(timestamp_t time, void *arg)
        {
            LSPScrollBar *_this = static_cast<LSPScrollBar *>(arg);
            if (_this == NULL)
                return STATUS_BAD_ARGUMENTS;
            _this->update();
            return STATUS_OK;
        }

        float LSPScrollBar::limit_value(float value)
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

        status_t LSPScrollBar::slot_on_change(LSPWidget *sender, void *ptr, void *data)
        {
            LSPScrollBar *_this = widget_ptrcast<LSPScrollBar>(ptr);
            return (_this != NULL) ? _this->on_change() : STATUS_BAD_ARGUMENTS;
        }

        void LSPScrollBar::set_value(float value)
        {
            value       = limit_value(value);

            if (value == fValue)
                return;

            fValue      = value;
            sSlots.execute(LSPSLOT_CHANGE, this);
            query_draw();
        }

        void LSPScrollBar::set_step(float value)
        {
            fStep       = value;
        }

        void LSPScrollBar::set_tiny_step(float value)
        {
            fTinyStep   = value;
        }

        void LSPScrollBar::set_min_value(float value)
        {
            if (value == fMin)
                return;

            fMin        = value;
            query_draw();
            set_value(fValue);
        }

        void LSPScrollBar::set_max_value(float value)
        {
            if (value == fMax)
                return;

            fMax        = value;
            query_draw();
            set_value(fValue);
        }

        void LSPScrollBar::set_size(ssize_t value)
        {
            if (value < 8)
                value = 8;
            if (value == nSize)
                return;
            nSize       = value;
            query_resize();
        }

        void LSPScrollBar::set_orientation(orientation_t value)
        {
            if (enOrientation == value)
                return;

            enOrientation   = value;
            query_resize();
        }

        void LSPScrollBar::set_fill(bool value)
        {
            size_t flags = (value) ? nFlags | F_FILL : nFlags & (~F_FILL);
            if (flags == nFlags)
                return;

            nFlags  = flags;
            query_draw();
        }

        size_t LSPScrollBar::check_mouse_over(ssize_t x, ssize_t y)
        {
            float value     = get_normalized_value();

            realize_t r     = sSize;
            ssize_t wsize   = nSize + 1;

            if (enOrientation == O_VERTICAL) // Vertical
            {
                // Update dimensions
                if (!(nFlags & F_FILL))
                {
                    r.nLeft += (r.nWidth - nSize) >> 1;
                    r.nWidth = nSize;
                }
                r.nHeight   --;
            }
            else
            {
                // Update dimensions
                if (!(nFlags & F_FILL))
                {
                    r.nTop     += (r.nHeight - nSize) >> 1;
                    r.nHeight   = nSize;
                }
                r.nWidth    --;
            }

            // Check overall coordinates
            if ((x < r.nLeft) ||
                (x > (r.nLeft + r.nWidth)) ||
                (y < r.nTop) ||
                (y > (r.nTop + r.nHeight)))
                return 0;

            if (enOrientation == O_VERTICAL) // Vertical
            {
                y -= r.nTop;
                if (y < wsize)
                    return F_BTN_DOWN_ACTIVE;
                y -= wsize;

                ssize_t spare_space     = r.nHeight - (wsize << 1) - wsize;
                ssize_t spare_up_size   = spare_space * value;
                ssize_t spare_down_size = spare_space * (1.0f - value);

                if (y < spare_up_size)
                    return F_SPARE_DOWN_ACTIVE;
                y -= spare_up_size;

                if (y < wsize)
                    return F_SLIDER_ACTIVE;
                y -= wsize;

                return (y < spare_down_size) ? F_SPARE_UP_ACTIVE : F_BTN_UP_ACTIVE;
            }
            else
            {
                x -= r.nLeft;
                if (x < wsize)
                    return F_BTN_DOWN_ACTIVE;
                x -= wsize;

                ssize_t spare_space     = r.nWidth - (wsize << 1) - wsize;
                ssize_t spare_down_size = spare_space * value;
                ssize_t spare_up_size   = spare_space * (1.0f - value);

                if (x < spare_down_size)
                    return F_SPARE_DOWN_ACTIVE;
                x -= spare_down_size;

                if (x < wsize)
                    return F_SLIDER_ACTIVE;
                x -= wsize;

                return (x < spare_up_size) ? F_SPARE_UP_ACTIVE : F_BTN_UP_ACTIVE;
            }
        }

        status_t LSPScrollBar::on_change()
        {
            return STATUS_OK;
        }

        status_t LSPScrollBar::on_mouse_down(const ws_event_t *e)
        {
            take_focus();

//            lsp_trace("nButtons = %d, code = %d", int(nButtons), int(e->nCode));
            if (nButtons == 0)
            {
                // Update state of buttons
                nButtons   |= (1 << e->nCode);

                // Check that we first hit inside the bar
                size_t flags = check_mouse_over(e->nLeft, e->nTop);
                update_cursor_state(e->nLeft, e->nTop, true);

                if (flags == 0)
                {
                    nFlags         |= F_OUTSIDE;
                    return STATUS_OK;
                }

                // What button was pressed?
                if (e->nCode == MCB_LEFT)
                {
                    nFlags      = flags | (flags << F_ACTIVITY_BITS);

                    if (flags != F_SLIDER_ACTIVE)
                    {
                        lsp_trace("launch timer");
                        sTimer.launch(0, 100);
                    }
                }
                else if (e->nCode == MCB_RIGHT)
                {
                    // Only slider allows right button
                    if (flags != F_SLIDER_ACTIVE)
                    {
                        nFlags         |= F_OUTSIDE;
                        return STATUS_OK;
                    }

                    // Slider with precision option
                    nFlags      = flags | (flags << F_ACTIVITY_BITS) | F_PRECISION;
                }
                else
                {
                    nFlags         |= F_OUTSIDE;
                    return STATUS_OK;
                }

                // Save current value to edited value
                fLastValue      = fValue;
                fCurrValue      = fValue;
                nLastV          = (enOrientation == O_VERTICAL) ? e->nTop : e->nLeft;
            }
            else
            {
                nButtons   |= (1 << e->nCode);
                if (nFlags & F_OUTSIDE)
                    return STATUS_OK;

                float value = fValue;

                if (nFlags & F_TRG_SLIDER_ACTIVE) // Slider
                {
                    size_t mask = (nFlags & F_PRECISION) ? (1 << MCB_RIGHT) : (1 << MCB_LEFT);

                    if (nButtons == mask)
                    {
                        nFlags  = (nFlags & (~F_ACTIVITY_MASK)) | ((nFlags >> F_ACTIVITY_BITS) & F_ACTIVITY_MASK); // Restore activity state
                        value   = fCurrValue;
                    }
                    else
                    {
                        nFlags &= ~F_ACTIVITY_MASK; // Clear activity state
                        value   = fLastValue;
                    }
                }
                else // Not slider
                {
                    if (nButtons == (1 << MCB_LEFT))
                    {
                        nFlags  = (nFlags & (~F_ACTIVITY_MASK)) | ((nFlags >> F_ACTIVITY_BITS) & F_ACTIVITY_MASK); // Restore activity state
                        value   = fCurrValue;

                        lsp_trace("launch timer");
                        sTimer.launch(0, 100);
                    }
                    else
                    {
                        lsp_trace("cancel timer");
                        sTimer.cancel();

                        nFlags &= ~F_ACTIVITY_MASK; // Clear activity state
                        value   = fLastValue;
                    }
                }

                // Update value
                value   = limit_value(value);
                if (value != fValue)
                {
                    fValue      = value;
                    sSlots.execute(LSPSLOT_CHANGE, this);
                }
            }

            query_draw();

            return STATUS_OK;
        }

        void LSPScrollBar::update_cursor_state(ssize_t x, ssize_t y, bool set)
        {
            size_t flags = (set) ? check_mouse_over(x,y) : 0;
            if (flags & F_SLIDER_ACTIVE)
                enActiveCursor = (enOrientation == O_VERTICAL) ? MP_VSIZE : MP_HSIZE;
            else
                enActiveCursor = cursor();
        }

        status_t LSPScrollBar::on_mouse_up(const ws_event_t *e)
        {
//            lsp_trace("nButtons = %d, code = %d", int(nButtons), int(e->nCode));
            nButtons   &= ~(1 << e->nCode);
            if (nFlags & F_OUTSIDE)
            {
                if (nButtons == 0)
                    nFlags &= ~F_OUTSIDE;

                return STATUS_OK;
            }

            float value     = fValue;

            if (nFlags & F_TRG_SLIDER_ACTIVE)
            {
                size_t key  = (nFlags & F_PRECISION) ? MCB_RIGHT : MCB_LEFT;

                if (nButtons == 0) // All mouse buttons are released now
                {
                    nFlags  &= ~(F_ALL_ACTIVITY_MASK | F_PRECISION);
                    value = (e->nCode == key) ? fCurrValue : fLastValue;
                }
                else if (nButtons == size_t(1 << key)) // Currently pressed initially selected button
                {
                    nFlags  = (nFlags & (~F_ACTIVITY_MASK)) | ((nFlags >> F_ACTIVITY_BITS) & F_ACTIVITY_MASK); // Restore activity state
                    value = fCurrValue;
                }
                else
                {
                    nFlags &= ~F_ACTIVITY_MASK; // Clear activity state
                    value   = fLastValue;
                }
            }
            else
            {
                if (nButtons == 0)
                {
                    lsp_trace("cancel timer");
                    sTimer.cancel();

                    nFlags  &= ~F_ALL_ACTIVITY_MASK;
                    value = (e->nCode == MCB_LEFT) ? fCurrValue : fLastValue;
                }
                else if (nButtons == (1 << MCB_LEFT))
                {
                    size_t flags = check_mouse_over(e->nLeft, e->nTop);
                    size_t mask  = (nFlags >> F_ACTIVITY_BITS) & F_ACTIVITY_MASK;

                    if (mask == flags)
                    {
                        nFlags     |= flags;
                        value       = fCurrValue;

                        lsp_trace("launch timer");
                        sTimer.launch(0, 100);
                    }
                    else
                    {
                        nFlags     &= ~F_ACTIVITY_MASK;
                        lsp_trace("cancel timer");
                        sTimer.cancel();
                    }
                }
            }

            // Update value
            value   = limit_value(value);
            query_draw();

            if (nButtons == 0)
                update_cursor_state(e->nLeft, e->nTop, false);

            if (value != fValue)
            {
                fValue      = value;
                sSlots.execute(LSPSLOT_CHANGE, this);
            }

            return STATUS_OK;
        }

        status_t LSPScrollBar::on_mouse_move(const ws_event_t *e)
        {
            if (nFlags & F_OUTSIDE)
                return STATUS_OK;
            if (nButtons == 0)
            {
                update_cursor_state(e->nLeft, e->nTop, true);
                return STATUS_OK;
            }

            if (nFlags & F_TRG_SLIDER_ACTIVE)
            {
                size_t key = (nFlags & F_PRECISION) ? MCB_RIGHT : MCB_LEFT;
                if (nButtons != size_t(1 << key))
                    return STATUS_OK;

                // Different behaviour for slider
                ssize_t value = (enOrientation == O_VERTICAL) ? e->nTop : e->nLeft;
                float result  = fLastValue;
                if (value != nLastV)
                {
                    ssize_t range = (enOrientation == O_VERTICAL) ? sSize.nHeight : sSize.nWidth;
                    ssize_t spare_space = range - ((nSize + 1) << 1) - nSize - 2;

                    float delta   = (fMax - fMin) * float(value - nLastV) / float(spare_space);
                    if (nFlags & F_PRECISION)
                        delta       *= 0.1f;
                    result        = limit_value(result + delta);
                }

                if (fCurrValue != result)
                {
                    lsp_trace("set value to %f", fValue);
                    fCurrValue  = result;
                    fValue      = result;
                    query_draw();

                    sSlots.execute(LSPSLOT_CHANGE, this);
                }
            }
            else
            {
                size_t flags = check_mouse_over(e->nLeft, e->nTop);

                if (nFlags & (F_TRG_SPARE_UP_ACTIVE | F_TRG_SPARE_DOWN_ACTIVE))
                {
                    if (flags == 0)
                    {
                        if (nFlags & F_ACTIVITY_MASK)
                        {
                            nFlags &= ~F_ACTIVITY_MASK;
                            lsp_trace("cancel timer");
                            sTimer.cancel();
                        }
                    }
                    else
                    {
                        if ((nFlags & F_ACTIVITY_MASK) != ((nFlags >> F_ACTIVITY_BITS) & F_ACTIVITY_MASK))
                        {
                            nFlags = (nFlags & (~F_ACTIVITY_MASK)) | ((nFlags >> F_ACTIVITY_BITS) & F_ACTIVITY_MASK);
                            lsp_trace("launch timer");
                            sTimer.launch(0, 100);
                        }
                    }
                }
                else
                {
                    size_t k = ((nFlags >> F_ACTIVITY_BITS) & F_ACTIVITY_MASK);

                    if (k != flags)
                    {
                        if (nFlags & F_ACTIVITY_MASK)
                        {
                            nFlags &= (~F_ACTIVITY_MASK);
                            lsp_trace("cancel timer");
                            sTimer.cancel();
                        }
                    }
                    else
                    {
                        if ((nFlags & F_ACTIVITY_MASK) != ((nFlags >> F_ACTIVITY_BITS) & F_ACTIVITY_MASK))
                        {
                            nFlags = (nFlags & (~F_ACTIVITY_MASK)) | flags;
                            lsp_trace("launch timer");
                            sTimer.launch(0, 100);
                        }
                    }
                }

                query_draw();
            }

            return STATUS_OK;
        }

        status_t LSPScrollBar::on_mouse_scroll(const ws_event_t *e)
        {
            if (nFlags & F_ALL_ACTIVITY_MASK)
                return STATUS_OK;

            float step      = (e->nState & MCF_SHIFT) ? fTinyStep : fStep;
            float delta     = (e->nCode == MCD_UP) ? -step : step;
            float result    = limit_value(fValue + delta);

            if (result != fValue)
            {
                fValue          = result;
                query_draw();
                sSlots.execute(LSPSLOT_CHANGE, this);
            }

            return STATUS_OK;
        }

        float LSPScrollBar::get_normalized_value()
        {
            float delta = fMax - fMin;
            return (delta != 0.0f) ? (fValue - fMin) / (fMax - fMin) : 0.0f;
        }

        void LSPScrollBar::update()
        {
            float value = fCurrValue;

            switch (nFlags & F_ACTIVITY_MASK)
            {
                case F_BTN_UP_ACTIVE:
                    value   += fTinyStep;
                    break;
                case F_BTN_DOWN_ACTIVE:
                    value   -= fTinyStep;
                    break;
                case F_SPARE_UP_ACTIVE:
                    value   += fStep;
                    break;
                case F_SPARE_DOWN_ACTIVE:
                    value   -= fStep;
                    break;
            }

            value   = limit_value(value);
            if (value != fCurrValue)
            {
                lsp_trace("set value to %f", fValue);
                fCurrValue  = value;
                fValue      = value;
                query_draw();

                sSlots.execute(LSPSLOT_CHANGE, this);
            }
        }

        void LSPScrollBar::draw(ISurface *s)
        {
            // Prepare palette
            Color bg_color(sBgColor);
            Color color(sColor);
            Color quarter(sSelColor, 0.25f);
            Color half(sSelColor, 0.5f);

            color.scale_lightness(brightness());
            quarter.scale_lightness(brightness());
            half.scale_lightness(brightness());

            // Draw background
            s->fill_rect(0, 0, sSize.nWidth, sSize.nHeight, bg_color);

            float value     = get_normalized_value();
            float aa        = s->set_antialiasing(true);

            realize_t r     = sSize;
            r.nLeft         = 0;
            r.nTop          = 0;
            ssize_t size3   = nSize/3;
            ssize_t w       = nSize + 1;

            if (enOrientation == O_VERTICAL) // Vertical
            {
                // Update dimensions
                if (!(nFlags & F_FILL))
                {
                    r.nLeft += (r.nWidth - nSize) >> 1;
                    r.nWidth = nSize;
                }
                r.nHeight   --;

                // Draw button up
                if (nFlags & F_BTN_UP_ACTIVE)
                {
                    float top = r.nTop + r.nHeight - nSize + 0.5f;
                    s->fill_round_rect(r.nLeft, r.nTop + r.nHeight - nSize + 1, r.nWidth + 1, nSize - 1, 3.0f, SURFMASK_B_CORNER, half);
                    s->line(r.nLeft + 0.5f, top, r.nLeft + r.nWidth + 0.5f, top, 1.0f, color);
                    s->fill_triangle(
                            r.nLeft + 0.2f*w, r.nTop + r.nHeight + 1 - size3*2,
                            r.nLeft + 0.5f*w, r.nTop + r.nHeight + 1 - size3,
                            r.nLeft + 0.8f*w, r.nTop + r.nHeight + 1 - size3*2,
                            color);
                }
                else
                {
                    s->fill_round_rect(r.nLeft, r.nTop + r.nHeight - nSize, r.nWidth + 1, nSize, 3.0f, SURFMASK_B_CORNER, color);
                    s->fill_triangle(
                            r.nLeft + 0.2f*w, r.nTop + r.nHeight + 1 - size3*2,
                            r.nLeft + 0.5f*w, r.nTop + r.nHeight + 1 - size3,
                            r.nLeft + 0.8f*w, r.nTop + r.nHeight + 1 - size3*2,
                            bg_color);
                }

                // Draw button down
                if (nFlags & F_BTN_DOWN_ACTIVE)
                {
                    float top = r.nTop + nSize + 0.5f;
//                    s->fill_rect(r.nLeft + 2, 2, nSize - 3, nSize - 3, sColor);
                    s->fill_round_rect(r.nLeft, r.nTop+1, r.nWidth + 1, nSize-1, 3.0f, SURFMASK_T_CORNER, half);
                    s->line(r.nLeft + 0.5f, top, r.nLeft + r.nWidth + 0.5f, top, 1.0f, color);
                    s->fill_triangle(
                            r.nLeft + 0.2f*w, r.nTop + size3*2,
                            r.nLeft + 0.5f*w, r.nTop + size3,
                            r.nLeft + 0.8f*w, r.nTop + size3*2,
                            color);
                }
                else
                {
                    s->fill_round_rect(r.nLeft, r.nTop+1, r.nWidth + 1, nSize, 3.0f, SURFMASK_T_CORNER, color);
                    s->fill_triangle(
                            r.nLeft + 0.2f*w, r.nTop + size3*2,
                            r.nLeft + 0.5f*w, r.nTop + size3,
                            r.nLeft + 0.8f*w, r.nTop + size3*2,
                            bg_color);
                }

                ssize_t spare_space     = r.nHeight - ((nSize + 1) << 1) - nSize - 1;
                ssize_t spare_up_size   = spare_space * value;
                ssize_t spare_down_size = spare_space * (1.0f - value);

                // Draw slider
                if (nFlags & F_SLIDER_ACTIVE)
                {
                    s->wire_rect(r.nLeft + 2.5f, r.nTop + nSize + spare_up_size + 2.5f, r.nWidth - 4, nSize - 1, 1.0f, color);
                    s->fill_rect(r.nLeft + 3, r.nTop + nSize + spare_up_size + 3, r.nWidth - 5, nSize - 2, quarter);
                }
                else
                    s->fill_rect(r.nLeft + 2, r.nTop + nSize + spare_up_size + 2, r.nWidth - 3, nSize, color);

                // Draw spares
                if ((nFlags & F_SPARE_UP_ACTIVE) && (spare_down_size > 0))
                    s->fill_rect(r.nLeft + 2, r.nTop + spare_up_size + nSize*2 + 3, r.nWidth - 3, spare_down_size, quarter);

                if ((nFlags & F_SPARE_DOWN_ACTIVE) && (spare_up_size > 1))
                    s->fill_rect(r.nLeft + 2, r.nTop + nSize + 2, r.nWidth - 3, spare_up_size - 1, quarter);

                // Draw binding
                s->wire_round_rect(r.nLeft + 0.5f, r.nTop + 0.5f, r.nWidth, r.nHeight, 3, SURFMASK_ALL_CORNER, 1.0f, color);
            }
            else // Horizontal
            {
                // Update dimensions
                if (!(nFlags & F_FILL))
                {
                    r.nTop     += (r.nHeight - nSize) >> 1;
                    r.nHeight   = nSize;
                }
                r.nWidth    --;

                // Draw button up
                if (nFlags & F_BTN_UP_ACTIVE)
                {
                    float left = r.nLeft + r.nWidth - nSize + 0.5f;
                    s->fill_round_rect(r.nLeft + r.nWidth - nSize + 1, r.nTop+1, nSize, r.nHeight, 3.0f, SURFMASK_R_CORNER, half);
                    s->line(left, r.nTop + 0.5f, left, r.nTop + r.nWidth + 0.5f, 1.0f, color);
                    s->fill_triangle(
                            r.nLeft + r.nWidth + 1 - size3*2, r.nTop + 0.2f*w,
                            r.nLeft + r.nWidth + 1 - size3,   r.nTop + 0.5f*w,
                            r.nLeft + r.nWidth + 1 - size3*2, r.nTop + 0.8f*w,
                            color);
                }
                else
                {
                    s->fill_round_rect(r.nLeft + r.nWidth - nSize, r.nTop+1, nSize+1, r.nHeight, 3.0f, SURFMASK_R_CORNER, color);
                    s->fill_triangle(
                            r.nLeft + r.nWidth + 1 - size3*2, r.nTop + 0.2f*w,
                            r.nLeft + r.nWidth + 1 - size3,   r.nTop + 0.5f*w,
                            r.nLeft + r.nWidth + 1 - size3*2, r.nTop + 0.8f*w,
                            bg_color);
                }

                // Draw button down
                if (nFlags & F_BTN_DOWN_ACTIVE)
                {
                    float left = r.nLeft + nSize + 0.5f;
                    s->fill_round_rect(r.nLeft, r.nTop, nSize, r.nHeight, 3.0f, SURFMASK_L_CORNER, half);
                    s->line(left, r.nTop + 0.5f, left, r.nLeft + r.nHeight + 0.5f, 1.0f, color);
                    s->fill_triangle(
                            r.nLeft + size3*2, r.nTop + 0.2f*w,
                            r.nLeft + size3,   r.nTop + 0.5f*w,
                            r.nLeft + size3*2, r.nTop + 0.8f*w,
                            color);
                }
                else
                {
                    s->fill_round_rect(r.nLeft+1, r.nTop, nSize, r.nHeight, 3.0f, SURFMASK_L_CORNER, color);
                    s->fill_triangle(
                            r.nLeft + size3*2, r.nTop + 0.2f*w,
                            r.nLeft + size3,   r.nTop + 0.5f*w,
                            r.nLeft + size3*2, r.nTop + 0.8f*w,
                            bg_color);
                }

                ssize_t spare_space     = r.nWidth - ((nSize + 1) << 1) - nSize - 1;
                ssize_t spare_down_size = spare_space * value;
                ssize_t spare_up_size   = spare_space * (1.0f - value);

                // Draw slider
                if (nFlags & F_SLIDER_ACTIVE)
                {
                    s->wire_rect(r.nLeft + nSize + spare_down_size + 2.5f, r.nTop + 2.5f, nSize - 1, r.nHeight - 4, 1.0f, color);
                    s->fill_rect(r.nLeft + nSize + spare_down_size + 3, r.nTop + 3, nSize - 2, r.nHeight - 5, quarter);
                }
                else
                    s->fill_rect(r.nLeft + nSize + spare_down_size + 2, r.nTop + 2, nSize, r.nHeight - 3, color);

                // Draw spares
                if ((nFlags & F_SPARE_UP_ACTIVE) && (spare_up_size > 0))
                    s->fill_rect(r.nLeft + spare_down_size + nSize*2 + 3, r.nTop + 2, spare_up_size, r.nHeight - 3, quarter);

                if ((nFlags & F_SPARE_DOWN_ACTIVE) && (spare_down_size > 1))
                    s->fill_rect(r.nLeft + nSize + 2, r.nTop + 2, spare_down_size - 1, r.nHeight - 3, quarter);

                // Draw binding
                s->wire_round_rect(r.nLeft + 0.5f, r.nTop + 0.5f, r.nWidth, r.nHeight, 3, SURFMASK_ALL_CORNER, 1.0f, color);
            }

            s->set_antialiasing(aa);
        }

        void LSPScrollBar::size_request(size_request_t *r)
        {
            r->nMinWidth    = (enOrientation == O_VERTICAL) ? nSize + 1 : (nSize + 1) * 5;
            r->nMinHeight   = (enOrientation == O_VERTICAL) ? (nSize + 1) * 5 : nSize + 1;
            r->nMaxWidth    = ((enOrientation == O_HORIZONTAL) || (nFlags & F_FILL)) ? -1 : r->nMinWidth;
            r->nMaxHeight   = ((enOrientation == O_VERTICAL) || (nFlags & F_FILL)) ? -1 : r->nMinHeight;
        }
    } /* namespace tk */
} /* namespace lsp */
