/*
 *
 *  Created on: 21 окт. 2015 г.
 *      Author: sadko
 */

#include <ui/gtk2/ui.h>

#include <string.h>
#include <math.h>

#define LOG_BASE            1.0f
#define DB_BASE             0.025f

namespace lsp
{
    Gtk2Knob::Gtk2Knob(plugin_ui *ui): Gtk2CustomWidget(ui, W_KNOB)
    {
        sColor.set(pUI->theme(), C_KNOB_CAP);
//        sScaleColor.set(pUI->theme(), C_KNOB_SCALE);
        sBgColor.set(pUI->theme(), C_BACKGROUND);

        nSize       = 24;
        nBalance    = 0.0f;
        nButtons    = 0;

        fValue      = 0.5;
        fStep       = 0.01;
        fMin        = 0.0;
        fMax        = 1.0;

        nState      = 0;
        nLastY      = 0;
        pPort       = NULL;
//        COLOR_PORTS_INIT(Scale);
        sScaleColor.init(this, C_KNOB_SCALE, A_SCALE_COLOR, -1, -1, -1, A_SCALE_HUE_ID, A_SCALE_SAT_ID, A_SCALE_LIGHT_ID);
    }

    Gtk2Knob::~Gtk2Knob()
    {
    }

    void Gtk2Knob::set(widget_attribute_t att, const char *value)
    {
        switch (att)
        {
            case A_ID:
                pPort       = pUI->port(value);
                if (pPort != NULL)
                {
                    pPort->bind(this);
                    apply_metadata_params(pPort->metadata());
                    notify(pPort);
                }
                break;
            case A_COLOR:
                sColor.set(pUI->theme(), value);
                break;
            case A_SIZE:
                PARSE_INT(value, nSize = size_t(__));
                break;
            case A_BALANCE:
                PARSE_FLOAT(value, nBalance = __);
                if (nBalance < 0.0f)
                    nBalance = 0.0f;
                else if (nBalance > 1.0f)
                    nBalance = 1.0f;
                break;
            case A_BG_COLOR:
                sBgColor.set(pUI->theme(), value);
                break;
            default:
                if (sScaleColor.set(att, value))
                    break;
                Gtk2CustomWidget::set(att, value);
                break;
        }
    }

    void Gtk2Knob::notify(IUIPort *port)
    {
        Gtk2CustomWidget::notify(port);

        bool redraw = false;
        if (sScaleColor.notify(port))
            redraw = true;
        if (port == pPort)
        {
            fValue  = fMin;
            const port_t *mdata = pPort->metadata();
            if (mdata != NULL)
            {
                fValue      = pPort->getValue();

                // Check that value is in range
                float *min = (fMin < fMax) ? &fMin : &fMax;
                float *max = (fMin < fMax) ? &fMax : &fMin;

                if ((fValue < *min) || (fValue > *max))
                    fValue      = mdata->start;

                // Don't trust metadata
                if (fValue > *max)
                    fValue      = *max;
                else if (fValue < *min)
                    fValue      = *min;
            }
            redraw = true;
        }

        // Request for redraw
        if (redraw)
            markRedraw();
//        COLOR_PORTS_HANDLE(Scale, port, sScaleColor);
    }

    void Gtk2Knob::apply_metadata_params(const port_t *p)
    {
        if (p == NULL)
        {
            fStep       = 0.01;
            fMin        = 0.0;
            fMax        = 1.0;
            if (fValue < fMin)
                fValue      = fMin;
            else if (fValue > fMax)
                fValue      = fMax;
        }

        if (is_decibel_unit(p->unit)) // Decibels
        {
            fMin        = (p->flags & F_LOWER) ? p->min     : 0.0f;
            fMax        = (p->flags & F_UPPER) ? p->max     : 4.0f;
            fStep       = (p->flags & F_STEP)  ? p->step    : 0.1f;
        }
        else if (is_discrete_unit(p->unit)) // Integer type
        {
            fMin        = (p->flags & F_LOWER) ? p->min : 0.0f;
            fMax        = (p->flags & F_UPPER) ? p->max : 1.0f;
            if (p->unit == U_ENUM)
                fMax        = fMin + list_size(p->items) - 1.0f;

            // Get step, truncate to integer amd process value
            ssize_t step    = (p->flags & F_STEP) ? p->step : 1.0f;
            fStep           = (step == 0) ? 1.0 : step;
        }
        else // Float and other values
        {
            fMin        = (p->flags & F_LOWER) ? p->min     : 0.0f;
            fMax        = (p->flags & F_UPPER) ? p->max     : 1.0f;
            fStep       = (p->flags & F_STEP)  ? p->step    : 0.01f;
        }

        // Check that value is in range
        float *min = (fMin < fMax) ? &fMin : &fMax;
        float *max = (fMin < fMax) ? &fMax : &fMin;

        if ((fValue < *min) || (fValue > *max))
            fValue      = p->start;

        // Don't trust metadata
        if (fValue > *max)
            fValue      = *max;
        else if (fValue < *min)
            fValue      = *min;
    }

    void Gtk2Knob::update_value(float delta)
    {
        delta          *= fStep;
        double value    = fValue;

        // Get metadata
        const port_t *p = (pPort != NULL) ? pPort->metadata() : NULL;
        if (p == NULL)
        {
            value      += delta;
            if (value < fMin)
                fValue      = fMin;
            else if (value > fMax)
                fValue      = fMax;
            else
                fValue      = value;

            if (pPort != NULL)
            {
                pPort->setValue(fValue);
                pPort->notifyAll();
            }
            else
                markRedraw();
            return;
        }

        // Analyze metadata
        if (is_decibel_unit(p->unit)) // Decibels
        {
            // Translate value to decibels, update and translate back
            double mul  = (p->unit == U_GAIN_AMP) ? 20.0 : 10.0;
            if (fMax > 0.0)
            {
                value       = mul * log(value + DB_BASE) / M_LN10;
                value      += delta;
                value       = exp(value * M_LN10 / mul) - DB_BASE;
            }
            else
            {
                value       = mul * log(- value + DB_BASE) / M_LN10;
                value      += delta;
                value       = - exp(value * M_LN10 / mul) - DB_BASE;
            }
        }
        else if (is_discrete_unit(p->unit)) // Integer type
        {
            value       = truncf(value + delta);
        }
        else // Float and other values
        {
            if (p->flags & F_LOG)
            {
                if (fMax > 0.0)
                {
                    value       = log(value + LOG_BASE);
                    value      += delta;
                    value       = exp(value) - LOG_BASE;
                }
                else
                {
                    value       = log(- value + LOG_BASE);
                    value      += delta;
                    value       = - exp(value) - LOG_BASE;
                }
            }
            else
                value      += delta;
        }

        // Check that value is in range
        float *min = (fMin < fMax) ? &fMin : &fMax;
        float *max = (fMin < fMax) ? &fMax : &fMin;

        if (value > *max)
            value       = *max;
        else if (value < *min)
            value       = *min;

        // Update port's value
        pPort->setValue(float(value));
        pPort->notifyAll();
    }

    void Gtk2Knob::set_normalized_value(float value)
    {
        // Limit value
        if (value < 0.0f)
            value = 0.0f;
        else if (value > 1.0f)
            value = 1.0f;

        // Get metadata
        const port_t *p = (pPort != NULL) ? pPort->metadata() : NULL;
        if (p == NULL)
        {
            fValue      = fMin + (fMax - fMin) * value;
            markRedraw();
            return;
        }

        // Analyze metadata
        if (is_decibel_unit(p->unit) || (p->flags & F_LOG)) // Decibels
        {
            double min   = (fMax > 0.0)     ? log(fMin + DB_BASE)      : - log(-fMin + DB_BASE);
            double max   = (fMax > 0.0)     ? log(fMax + DB_BASE)      : - log(-fMax + DB_BASE);
            value        = min + (max - min) * value;

            if (fMax > 0.0f)
                value       = exp(value) - DB_BASE;
            else
                value       = - exp(value) - DB_BASE;
        }
        else if (is_discrete_unit(p->unit)) // Integer type
            value       = truncf(fMin + (fMax - fMin) * value);
        else // Float and other values
            value       = fMin + (fMax - fMin) * value;

        // Check that value is in range
        float *min = (fMin < fMax) ? &fMin : &fMax;
        float *max = (fMin < fMax) ? &fMax : &fMin;

        if (value > *max)
            value       = *max;
        else if (value < *min)
            value       = *min;

        // Update port's value
        fValue      = value;
        pPort->setValue(float(value));
        pPort->notifyAll();
    }

    float Gtk2Knob::get_normalized_value()
    {
        if (pPort == NULL)
            return fValue;
        const port_t *p = pPort->metadata();
        if (p == NULL)
            return fValue;

        // Analyze metadata
        if ((is_decibel_unit(p->unit)) || (p->flags & F_LOG)) // Decibels
        {
            // Translate value to decibels, update and translate back
            double min   = (fMax > 0.0)     ? log(fMin + DB_BASE)      : - log(-fMin + DB_BASE);
            double max   = (fMax > 0.0)     ? log(fMax + DB_BASE)      : - log(-fMax + DB_BASE);
            double value = (fMax > 0.0)     ? log(fValue + DB_BASE)    : - log(-fValue + DB_BASE);

            return (value - min) / (max - min);
        }
        else if (is_discrete_unit(p->unit)) // Integer type
            return (fValue - fMin) / (fMax - fMin);
        else // Float and other values
            return (fValue - fMin) / (fMax - fMin);
    }

    void Gtk2Knob::render()
    {
        float value     = get_normalized_value();

        // Get resource
        cairo_pattern_t *cp;
        cairo_t *cr = gdk_cairo_create(pWidget->window);
        cairo_save(cr);

        // Draw background
        cairo_set_source_rgb(cr, sBgColor.red(), sBgColor.green(), sBgColor.blue());
        cairo_rectangle(cr, 0, 0, nWidth, nHeight);
        cairo_fill(cr);

        // Move to center of the knob
        cairo_translate(cr, ssize_t(nWidth >> 1), ssize_t(nHeight >> 1));

        // Draw scale background
        Color col(sScaleColor.color());
        Color dark(sScaleColor.color());
        dark.blend(0.0, 0.0, 0.0, 0.75);
//        light.lightness(light.lightness() * 1.5);
//        light.blend(0.0f, 0.0f, 0.0f, 0.5f);

        float base = 2.0 * M_PI / 3.0;
        float delta = 5.0 * M_PI / 3.0;
        float knob_r  = (nSize >> 1);
        float hole_r  = (nSize >> 1) + 1;
        float scale_in_r  = hole_r + 2;
        float scale_out_r = scale_in_r + 5;
        float v_angle1    = base + value * delta;
        float v_angle2    = base + nBalance * delta;

        cairo_set_source_rgb(cr, dark.red(), dark.green(), dark.blue());
        cairo_move_to(cr, 0, 0);
        cairo_arc(cr, 0, 0, scale_out_r, base, base + delta);
        cairo_close_path(cr);
        cairo_fill(cr);

        cairo_set_source_rgb(cr, col.red(), col.green(), col.blue());
        cairo_move_to(cr, 0, 0);
        if (value < nBalance)
            cairo_arc(cr, 0, 0, scale_out_r, v_angle1, v_angle2);
        else
            cairo_arc(cr, 0, 0, scale_out_r, v_angle2, v_angle1);
        cairo_close_path(cr);
        cairo_fill(cr);

        cairo_set_source_rgb(cr,sBgColor.red(), sBgColor.green(), sBgColor.blue());
        cairo_arc(cr, 0, 0, scale_in_r, 0, M_PI * 2);
        cairo_fill(cr);

        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_arc(cr, 0, 0, hole_r, 0, M_PI * 2);
        cairo_fill(cr);

        // Draw scales: overall 10 segments separated by 2 sub-segments
        cairo_set_source_rgb(cr, sBgColor.red(), sBgColor.green(), sBgColor.blue());
        cairo_set_line_width(cr, 1);
        delta /= 20.0;
        cairo_antialias_t anti_alias = cairo_get_antialias(cr);
//        cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);
        for (size_t i=0; i<=20; ++i)
        {
            float angle = base + delta * i;
            float r2    = scale_in_r + 3.0 * (i & 1);
            float f_sin = sinf(angle), f_cos = cosf(angle);

            cairo_move_to(cr, (scale_out_r + 1) * f_cos, (scale_out_r + 1) * f_sin);
            cairo_line_to(cr, r2 * f_cos, r2 * f_sin);
        }
        cairo_stroke(cr);
        cairo_set_antialias(cr, anti_alias);

        // Draw knob body
        ssize_t k_l = (nSize >> 3);
        if (k_l < 2)
            k_l = 2;
        float k_r = knob_r;

        float f_sin = sinf(v_angle1), f_cos = cosf(v_angle1);
        Color tip;
        pUI->theme().getColor(C_LABEL_TEXT, &tip);

        for (ssize_t i=0; (i++)<k_l; )
        {
            float bright = sqrtf(i * i) / k_l;
            float r = sColor.red() * bright, g = sColor.green() * bright, b = sColor.blue() * bright;

            cp = cairo_pattern_create_radial (k_r, -k_r, knob_r, k_r, -k_r, knob_r * 4.0);
            cairo_pattern_add_color_stop_rgb(cp, 0.0, r, g, b);
            cairo_pattern_add_color_stop_rgb(cp, 1.0, r * 0.05, g * 0.05, b * 0.05);

            cairo_set_source (cr, cp);
            cairo_arc(cr, 0, 0, k_r, 0, M_PI * 2);
            cairo_fill(cr);
            cairo_pattern_destroy(cp);

            // Draw line
            cairo_set_line_width(cr, 3);
            cairo_set_source_rgb(cr, tip.red() * bright, tip.green() * bright, tip.blue() * bright);
            cairo_move_to(cr, (knob_r * 0.25) * f_cos, (knob_r * 0.25) * f_sin);
            cairo_line_to(cr, k_r * f_cos, k_r * f_sin);
            cairo_stroke(cr);

            if ((--k_r) < 0.0)
                k_r = 0.0;
        }

        // Release resource
        cairo_restore(cr);
        cairo_destroy(cr);
    }

    void Gtk2Knob::resize(size_t &w, size_t &h)
    {
        w = nSize + (10 << 1);
        h = nSize + (10 << 1);
    }

    size_t Gtk2Knob::check_mouse_over(ssize_t x, ssize_t y)
    {
        ssize_t cx      = ssize_t(nWidth) >> 1;
        ssize_t cy      = ssize_t(nHeight) >> 1;
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

    void Gtk2Knob::button_press(ssize_t x, ssize_t y, size_t state, size_t button)
    {
//        lsp_trace("x=%d, y=%d, state=%x, button=%x", int(x), int(y), int(state), int(button));
        if ((nButtons == 0) && ((button == 1) || (button == 3)))
        {
            size_t flags = check_mouse_over(x, y);
            if (flags != 0)
                nState      = flags;
        }

        nButtons |= (1 << button);
        nLastY = y;
    }

    void Gtk2Knob::button_release(ssize_t x, ssize_t y, size_t state, size_t button)
    {
//        lsp_trace("x=%d, y=%d, state=%x, button=%x", int(x), int(y), int(state), int(button));
        nButtons &= ~(1 << button);
        nLastY = y;
        if (nButtons == 0)
        {
            if ((nState == S_CLICK) && (button == 1))
                on_click(x, y);
            nState      = 0;
        }
    }

    void Gtk2Knob::on_click(ssize_t x, ssize_t y)
    {
        ssize_t cx      = ssize_t(nWidth) >> 1;
        ssize_t cy      = ssize_t(nHeight) >> 1;
        float dx        = x - cx;
        float dy        = cy - y;
        float d         = sqrtf(dx * dx + dy * dy);
        if (d <= 0.0f)
            return;

        float angle     = asinf(dy / d);
        if (angle < (-M_PI / 3.0))
        {
            set_normalized_value((dx > 0) ? 1.0f : 0.0f);
            return;
        }
        if (dx < 0.0f)
            angle           = M_PI - angle;

        angle          += M_PI / 3.0;

        // Update value
        set_normalized_value(1.0f - (angle / (5 * M_PI  / 3.0)));
    }

    void Gtk2Knob::motion(ssize_t x, ssize_t y, size_t state)
    {
//        lsp_trace("x=%d, y=%d, state=%x", int(x), int(y), int(state));
        if (nState == S_MOVING)
        {
            if (!(nButtons & ((1 << 1) | (1 << 3))))
                return;

            // Update value
            float step = calc_step(nButtons & (1 << 3));
            update_value(step * (nLastY - y));
            nLastY = y;
        }
        else if (nState == S_CLICK)
        {
            if (!(nButtons & (1 << 1)))
                return;

            on_click(x, y);
        }
    }

    void Gtk2Knob::button_double_press(ssize_t x, ssize_t y, size_t state, size_t button)
    {
        if (check_mouse_over(x, y) == S_NONE)
            return;

        const port_t *mdata = (pPort != NULL) ? pPort->metadata() : NULL;
        float value = (mdata != NULL) ? mdata->start : 0.0f;

        if (pPort != NULL)
        {
            pPort->setValue(value);
            pPort->notifyAll();
        }
        else
            markRedraw();
    }

    void Gtk2Knob::scroll(ssize_t x, ssize_t y, size_t state, size_t direction)
    {
        float step = calc_step(state & GDK_SHIFT_MASK);

        // Update value
        float delta = 0.0;
        if (direction == GDK_SCROLL_UP)
            delta   = step;
        else if (direction == GDK_SCROLL_DOWN)
            delta   = -step;
        else
            return;

        update_value(delta);
    }

    float Gtk2Knob::calc_step(bool tolerance)
    {
        float step = (tolerance) ? 1.0 : 10.0;

        // Reduce/increase step for several data types
        const port_t *p = (pPort != NULL) ? pPort->metadata() : NULL;
        if (p != NULL)
        {
            if (is_discrete_unit(p->unit) || (p->flags & F_INT))
            {
                float delta = fabs(fMax - fMin) / 100.0;
                if (step > delta)
                {
                    ssize_t d_step = ssize_t(delta);
                    if (d_step == 0)
                        d_step = 1;
                    step = (step > 0.0) ? d_step : - d_step;

                    if (tolerance)
                        step *= 0.25;
                }
            }
            else if ((is_decibel_unit(p->unit)) || (p->flags & F_LOG))
                step *= 0.1;
        }

        return step;
    }

} /* namespace lsp */
