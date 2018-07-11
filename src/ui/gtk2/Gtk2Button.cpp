/*
 * Gtk2Button.cpp
 *
 *  Created on: 21 окт. 2015 г.
 *      Author: sadko
 */

#include <ui/gtk2/ui.h>

#include <string.h>
#include <math.h>


namespace lsp
{
    Gtk2Button::Gtk2Button(plugin_ui *ui): Gtk2CustomWidget(ui, W_BUTTON)
    {
        sColor.set(pUI->theme(), C_BUTTON_FACE);
        sBgColor.set(pUI->theme(), C_BACKGROUND);

        nSize       = 18;
        nState      = 0;
        nBMask      = 0;
        pPort       = NULL;
        fValue      = 0;
    }

    Gtk2Button::~Gtk2Button()
    {
    }

    float Gtk2Button::next_value(bool down)
    {
        const port_t *mdata = (pPort != NULL) ? pPort->metadata() : NULL;
//        lsp_trace("mdata == %p", mdata);
        if (mdata == NULL)
            return (fValue >= 0.5) ? 0.0f : 1.0f;

//        lsp_trace("unit = %d, flags = %x", mdata->unit, mdata->flags);

        // Analyze event
        if (down)
        {
            if (mdata->unit == U_ENUM)
                return fValue;
            if (!IS_TRIGGER_PORT(mdata))
                return fValue;
        }

        // Get minimum and maximum
        float min   = (mdata->flags & F_LOWER) ? mdata->min : 0.0;
        float max   = (mdata->flags & F_UPPER) ? mdata->max : min + 1.0f;
        float step  = (mdata->flags & F_STEP) ? mdata->step : 1.0;
        if ((mdata->unit == U_ENUM) && (mdata->items != NULL))
            max     = mdata->min + list_size(mdata->items) - 1.0f;

//        lsp_trace("min = %f, max=%f, step=%f", min, max, step);

        float value = fValue + step;
        if (value > max)
            value = min;
        else if (value < min)
            value = max;

//        lsp_trace("value = %f", value);

        return value;
    }

    void Gtk2Button::set(widget_attribute_t att, const char *value)
    {
        switch (att)
        {
            case A_ID:
                {
                    pPort       = pUI->port(value);
                    if (pPort != NULL)
                    {
                        pPort->bind(this);
                        set_value(pPort->getValue());
                    }
                }
                break;
            case A_COLOR:
                sColor.set(pUI->theme(), value);
                break;
            case A_SIZE:
                PARSE_INT(value, nSize = size_t(__));
                break;
            case A_LED:
                PARSE_BOOL(value,
                    if (__)
                        nState  |= S_LED;
                    else
                        nState  &= ~S_LED;
                );
                break;
            case A_BG_COLOR:
                sBgColor.set(pUI->theme(), value);
                break;
            default:
                Gtk2CustomWidget::set(att, value);
                break;
        }
    }

    void Gtk2Button::render()
    {
        size_t pressed = nState;
//        lsp_trace("pressed = %d", int(pressed));

        // Get resource
        cairo_pattern_t *cp;
        cairo_t *cr = gdk_cairo_create(pWidget->window);
        cairo_save(cr);

        // Draw background
        cairo_set_source_rgba(cr, sBgColor.red(), sBgColor.green(), sBgColor.blue(), 1.0);
        cairo_rectangle(cr, 0, 0, nWidth, nHeight);
        cairo_fill(cr);

        // Move to center of the led
        cairo_translate(cr, ssize_t(nWidth >> 1), ssize_t(nHeight >> 1));

        float b_rad  = sqrtf(nWidth*nWidth + nHeight*nHeight);
        ssize_t b_r  = nSize >> 1;          // Button radius
        ssize_t b_rr = 2 + (nSize >> 4);    // Button rounding radius
        ssize_t h_r  = b_r + 1;             // Hole radius
        ssize_t h_rr = b_rr + 1;            // Hole rounding radius
        ssize_t l_rr = (nSize >> 2);

        // Draw hole
        cairo_set_line_width(cr, 1.0);
        cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);

        cairo_move_to(cr, h_rr - h_r, -h_r);
        cairo_line_to(cr, h_r-h_rr, -h_r);
        cairo_curve_to(cr, h_r, -h_r, h_r, -h_r, h_r, h_rr - h_r);
        cairo_line_to(cr, h_r, h_r-h_rr);
        cairo_curve_to(cr, h_r, h_r, h_r, h_r, h_r-h_rr, h_r);
        cairo_line_to(cr, h_rr - h_r, h_r);
        cairo_curve_to(cr, -h_r, h_r, -h_r, h_r, -h_r, h_r-h_rr);
        cairo_line_to(cr, -h_r, h_rr - h_r);
        cairo_curve_to(cr, -h_r, -h_r, -h_r, -h_r, h_rr - h_r, -h_r);

        cairo_fill(cr);

        // Change size if pressed
        ssize_t b_l = b_rr;
        if (pressed & S_PRESSED)
        {
            b_l ++;
            b_r --;
            b_rr --;
        }
        else if (pressed & S_TOGGLED)
        {
            b_r --;
        }
        else
            b_l ++;

        bool is_trigger     = false;
        const port_t *mdata = (pPort != NULL) ? pPort->metadata() : NULL;
        if ((mdata != NULL) && IS_TRIGGER_PORT(mdata))
            is_trigger          = true;

        float lightness = 1.0f;
        if (nState & S_LED)
        {
            bool is_pressed =  (is_trigger) ? (pressed & S_PRESSED) : (pressed & S_TOGGLED);

            // Draw light
            if (is_pressed)
            {
                ssize_t x_rr = l_rr - 1;

                cp = cairo_pattern_create_linear(0, -b_r, 0, -b_r - x_rr);
                cairo_pattern_add_color_stop_rgba(cp, 0.0, sColor.red(), sColor.green(), sColor.blue(), 0.5f);
                cairo_pattern_add_color_stop_rgba(cp, 1.0, sColor.red(), sColor.green(), sColor.blue(), 0.0);
                cairo_set_source(cr, cp);
                cairo_move_to(cr, -b_r - l_rr, -b_r - l_rr);
                cairo_line_to(cr, b_r + l_rr, -b_r - l_rr);
                cairo_line_to(cr, 0, 0);
                cairo_close_path(cr);
                cairo_fill(cr);
                cairo_pattern_destroy(cp);

                cp = cairo_pattern_create_linear(0, b_r, 0, b_r + x_rr);
                cairo_pattern_add_color_stop_rgba(cp, 0.0, sColor.red(), sColor.green(), sColor.blue(), 0.5f);
                cairo_pattern_add_color_stop_rgba(cp, 1.0, sColor.red(), sColor.green(), sColor.blue(), 0.0);
                cairo_set_source(cr, cp);
                cairo_move_to(cr, b_r + l_rr, b_r + l_rr);
                cairo_line_to(cr, - b_r - l_rr, b_r + l_rr);
                cairo_line_to(cr, 0, 0);
                cairo_close_path(cr);
                cairo_fill(cr);
                cairo_pattern_destroy(cp);

                cp = cairo_pattern_create_linear(-b_r, 0, - b_r - x_rr, 0);
                cairo_pattern_add_color_stop_rgba(cp, 0.0, sColor.red(), sColor.green(), sColor.blue(), 0.5f);
                cairo_pattern_add_color_stop_rgba(cp, 1.0, sColor.red(), sColor.green(), sColor.blue(), 0.0);
                cairo_set_source(cr, cp);
                cairo_move_to(cr, -b_r - l_rr, -b_r - l_rr);
                cairo_line_to(cr, -b_r - l_rr, b_r + l_rr);
                cairo_line_to(cr, 0, 0);
                cairo_close_path(cr);
                cairo_fill(cr);
                cairo_pattern_destroy(cp);

                cp = cairo_pattern_create_linear(b_r, 0, b_r + x_rr, 0);
                cairo_pattern_add_color_stop_rgba(cp, 0.0, sColor.red(), sColor.green(), sColor.blue(), 0.5f);
                cairo_pattern_add_color_stop_rgba(cp, 1.0, sColor.red(), sColor.green(), sColor.blue(), 0.0);
                cairo_set_source(cr, cp);
                cairo_move_to(cr, b_r + l_rr, b_r + l_rr);
                cairo_line_to(cr, b_r + l_rr, - b_r - l_rr);
                cairo_line_to(cr, 0, 0);
                cairo_close_path(cr);
                cairo_fill(cr);
                cairo_pattern_destroy(cp);

                lightness   = 0.5f;
            }
            else
                lightness   = 0.25f;
        }

        for (ssize_t i=0; (i++)<b_l; )
        {
            float bright = lightness * sqrtf(i * i) / b_l;
            Color cl(sColor);
            cl.lightness(bright);
            float r = cl.red(), g = cl.green(), b = cl.blue();

            if (pressed & S_PRESSED)
                cp = cairo_pattern_create_radial (-b_r, b_r, b_r >> 1, -b_r, b_r, b_r << 3);
            else if (pressed & S_TOGGLED)
                cp = cairo_pattern_create_radial (-b_r, b_r, b_r >> 1, -b_r, b_r, (b_r << 3));
            else
                cp = cairo_pattern_create_radial (b_r, -b_r, b_r << 1, b_r, -b_r, (b_r << 3));

            cairo_pattern_add_color_stop_rgb(cp, 0.0, r, g, b);
            cairo_pattern_add_color_stop_rgb(cp, 1.0, r * 0.1, g * 0.1, b * 0.1);

            cairo_set_source (cr, cp);

            cairo_move_to(cr, b_rr - b_r, -b_r);
            cairo_line_to(cr, b_r-b_rr, -b_r);
            cairo_curve_to(cr, b_r, -b_r, b_r, -b_r, b_r, b_rr - b_r);
            cairo_line_to(cr, b_r, b_r-b_rr);
            cairo_curve_to(cr, b_r, b_r, b_r, b_r, b_r-b_rr, b_r);
            cairo_line_to(cr, b_rr - b_r, b_r);
            cairo_curve_to(cr, -b_r, b_r, -b_r, b_r, -b_r, b_r-b_rr);
            cairo_line_to(cr, -b_r, b_rr - b_r);
            cairo_curve_to(cr, -b_r, -b_r, -b_r, -b_r, b_rr - b_r, -b_r);

            cairo_fill(cr);
            cairo_pattern_destroy(cp);

            if ((--b_r) < 0)
                b_r = 0;
        }

        if (nState & S_LED)
        {
            Color cl(sColor);
            cl.lightness(lightness);

            cp = cairo_pattern_create_radial (-b_r, b_r, b_rad * 0.25f, 0, 0, b_rad * 0.8f);
            cairo_pattern_add_color_stop_rgb(cp, 0.0, cl.red(), cl.green(), cl.blue());
            cairo_pattern_add_color_stop_rgb(cp, 1.0, 1.0f, 1.0f, 1.0f);
            cairo_set_source (cr, cp);

            cairo_move_to(cr, b_rr - b_r, -b_r);
            cairo_line_to(cr, b_r-b_rr, -b_r);
            cairo_curve_to(cr, b_r, -b_r, b_r, -b_r, b_r, b_rr - b_r);
            cairo_line_to(cr, b_r, b_r-b_rr);
            cairo_curve_to(cr, b_r, b_r, b_r, b_r, b_r-b_rr, b_r);
            cairo_line_to(cr, b_rr - b_r, b_r);
            cairo_curve_to(cr, -b_r, b_r, -b_r, b_r, -b_r, b_r-b_rr);
            cairo_line_to(cr, -b_r, b_rr - b_r);
            cairo_curve_to(cr, -b_r, -b_r, -b_r, -b_r, b_rr - b_r, -b_r);

            cairo_fill(cr);
            cairo_pattern_destroy(cp);
        }

        // Release resource
        cairo_restore(cr);
        cairo_destroy(cr);
    }

    void Gtk2Button::resize(size_t &w, size_t &h)
    {
        size_t delta = (nState & S_LED) ? 2 + (nSize >> 2) : 2;

        w = nSize + delta;
        h = nSize + delta;
    }

    bool Gtk2Button::check_mouse_over(ssize_t x, ssize_t y)
    {
        ssize_t left    = ssize_t(nWidth - nSize) >> 1;
        ssize_t top     = ssize_t(nHeight - nSize) >> 1;
        ssize_t right   = left + nSize;
        ssize_t bottom  = top + nSize;

//        lsp_trace("x=%d, y=%d, l=%d, r=%d, t=%d, b=%d", int(x), int(y), int(left), int(right), int(top), int(bottom));

        return ((x >= left) && (x <= right) && (y >= top) && (y <= bottom));
    }

    void Gtk2Button::button_press(ssize_t x, ssize_t y, size_t state, size_t button)
    {
//        lsp_trace("x=%d, y=%d, state=%x, button=%x", int(x), int(y), int(state), int(button));
        nBMask         |= (1 << button);

        bool pressed    = (nBMask == (1 << 1)) && (check_mouse_over(x, y));
        bool is_pressed = nState & S_PRESSED;

        if (pressed != is_pressed)
        {
            nState      ^= S_PRESSED;

//            if (is_trigger())
            on_click(true);

            markRedraw();
        }
    }

    void Gtk2Button::button_release(ssize_t x, ssize_t y, size_t state, size_t button)
    {
//        lsp_trace("x=%d, y=%d, state=%x, button=%x", int(x), int(y), int(state), int(button));

        nBMask         &= ~(1 << button);
        bool pressed    = ((button == 1) && (nBMask == 0)) || ((button != 1) && (nBMask == (1 << 1)));
        if (pressed)
            pressed     = (check_mouse_over(x, y));
        if (nBMask == 0)
            pressed     = false;

        bool is_pressed = nState & S_PRESSED;
        if (pressed != is_pressed)
        {
            nState     ^= S_PRESSED;
            if ((is_pressed) && (!is_trigger()))
                nState     ^= S_TOGGLED;

            if (nBMask == 0)
                on_click(false);

            markRedraw();
        }
    }

    void Gtk2Button::motion(ssize_t x, ssize_t y, size_t state)
    {
//        lsp_trace("x=%d, y=%d, state=%x", int(x), int(y), int(state));
        bool pressed    = (nBMask == (1 << 1)) && (check_mouse_over(x, y));
        bool is_pressed = nState & S_PRESSED;

        if (pressed != is_pressed)
        {
            if (pressed)
                nState     |= S_PRESSED;
            else
                nState     &= ~S_PRESSED;

            if (is_trigger())
                on_click(pressed);

            markRedraw();
        }
    }

    void Gtk2Button::on_click(bool down)
    {
//        lsp_trace("button clicked down=%d", int(down));
        float value     = next_value(down);//(down) ? 1.0 : 0.0;
        if (value != fValue)
        {
            if (pPort != NULL)
            {
                pPort->setValue(value);
                pPort->notifyAll();
            }
        }
    }

    void Gtk2Button::notify(IUIPort *port)
    {
        Gtk2CustomWidget::notify(port);

        if (port == pPort)
        {
            set_value(pPort->getValue());

            // Request for redraw
            markRedraw();
        }
    }

    void Gtk2Button::set_value(float value)
    {
//        lsp_trace("value = %f", value);
        const port_t *mdata = (pPort != NULL) ? pPort->metadata() : NULL;

        if (mdata != NULL)
        {
            fValue      = value;

            float min   = (mdata->flags & F_LOWER) ? mdata->min : 0.0;
            float max   = (mdata->flags & F_UPPER) ? mdata->max : min + 1.0f;

            if (mdata->unit == U_ENUM)
                nState      &= ~S_TOGGLED;
            else if (mdata->flags & F_TRG)
                nState      &= ~S_TOGGLED;
            else if (fValue == max)
                nState      |= S_TOGGLED;
            else
                nState      &= ~S_TOGGLED;
        }
        else
        {
            fValue      = (value >= 0.5) ? 1.0f : 0.0f;
            if (fValue >= 0.5)
                nState      |= S_TOGGLED;
            else
                nState      &= ~S_TOGGLED;
        }
    }

    bool Gtk2Button::is_trigger()
    {
        const port_t *mdata = (pPort != NULL) ? pPort->metadata() : NULL;
        if (mdata == NULL)
            return false;
        if (mdata->unit == U_ENUM)
            return false;
        if (mdata->flags & F_TRG)
            return true;
        return false;
    }

} /* namespace lsp */
