/*
 * Gtk2Switch.cpp
 *
 *  Created on: 21 окт. 2015 г.
 *      Author: sadko
 */

#include <ui/gtk2/ui.h>

#include <string.h>
#include <math.h>


namespace lsp
{
    static const float ANGLE = 15 * M_PI / 180.0;

    Gtk2Switch::Gtk2Switch(plugin_ui *ui): Gtk2CustomWidget(ui, W_SWITCH)
    {
        sColor.set(pUI->theme(), C_KNOB_CAP);
        sBgColor.set(pUI->theme(), C_BACKGROUND);
        sTextColor.set(pUI->theme(), C_LABEL_TEXT);
        sBorderColor.set(pUI->theme(), C_KNOB_CAP);

        nSize       = 24;
        nBorder     = 8;
        nState      = 0;
        nBMask      = 0;
        pPort       = NULL;
        nAngle      = 0;
        nAspect     = M_SQRT2;
    }

    Gtk2Switch::~Gtk2Switch()
    {
    }

    void Gtk2Switch::set(widget_attribute_t att, const char *value)
    {
        switch (att)
        {
            case A_ID:
                BIND_PORT(pUI, pPort, value);
                break;
            case A_COLOR:
                sColor.set(pUI->theme(), value);
                break;
            case A_BG_COLOR:
                sBgColor.set(pUI->theme(), value);
                break;
            case A_TEXT_COLOR:
                sTextColor.set(pUI->theme(), value);
                break;
            case A_SIZE:
                PARSE_INT(value, nSize = size_t(__));
                break;
            case A_BORDER:
                PARSE_INT(value, nBorder = size_t(__));
                break;
            case A_ANGLE:
                PARSE_INT(value, nAngle = size_t(__ % 4 ));
                break;
            case A_ASPECT:
                PARSE_FLOAT(value, nAspect = __);
                break;
            case A_INVERT:
                {
                    bool invert;
                    PARSE_BOOL(value, invert = __);
                    if (invert)
                        nState     |= S_INVERT;
                    else
                        nState     &= ~S_INVERT;
                }
                break;
            default:
                Gtk2CustomWidget::set(att, value);
                break;
        }
    }

    void Gtk2Switch::draw(cairo_t *cr)
    {
        // Get resource
        cairo_pattern_t *cp;

        // Draw background
        cairo_set_source_rgb(cr, sBgColor.red(), sBgColor.green(), sBgColor.blue());
        cairo_rectangle(cr, 0, 0, nWidth, nHeight);
        cairo_fill(cr);

        // Get dimensions
        ssize_t w = 0, h = 0;
        dimensions(w, h);

        // Move to the left corner
        cairo_translate(cr, ssize_t((nWidth - w) >> 1), ssize_t((nHeight - h) >> 1));
        float delta = sqrtf(w*w + h*h);
        float radius    = nAspect * nSize * 0.5 / cosf(ANGLE);

        // Draw border if present
        if (nBorder > 0)
        {
            // Draw border
            ssize_t b_r = 3;

            for (ssize_t i=0; (i<b_r) && (i < ssize_t(nBorder)); ++i)
            {
                Color bc(sBorderColor);
                float bright = (i + 1.0) / b_r;
                bc.lightness(bc.lightness() + bright);

                cp = cairo_pattern_create_radial (w - (b_r << 1), (b_r << 1), delta * 0.5, w - (b_r << 1), (b_r << 1), delta * 2.0 / (bright + 1.0));
                cairo_pattern_add_color_stop_rgb(cp, 0.0, sBorderColor.red() * bright, sBorderColor.green() * bright, sBorderColor.blue() * bright);
                cairo_pattern_add_color_stop_rgb(cp, 1.0, 0.5 * sBorderColor.red(), 0.5 *  sBorderColor.green(), 0.5 * sBorderColor.blue());
                cairo_set_source (cr, cp);
                cairo_rectangle(cr, i, i, w - (i << 1), h - (i << 1));
                cairo_fill(cr);
                cairo_pattern_destroy(cp);
            }

            // Draw border
            if (ssize_t(nBorder) > b_r)
            {
                cp = cairo_pattern_create_radial (w - (b_r << 1), (b_r << 1), delta * 0.5, w - (b_r << 1), (b_r << 1), delta * 1.0);
                cairo_pattern_add_color_stop_rgb(cp, 0.0, sBorderColor.red(), sBorderColor.green(), sBorderColor.blue());
                cairo_pattern_add_color_stop_rgb(cp, 1.0, 0.5 * sBorderColor.red(), 0.5 *  sBorderColor.green(), 0.5 * sBorderColor.blue());
                cairo_set_source (cr, cp);
                cairo_rectangle(cr, b_r, b_r, w - (b_r << 1), h - (b_r << 1));
                cairo_fill(cr);
                cairo_pattern_destroy(cp);
            }

            // Draw hole
            Color hole(sBorderColor);
            hole.darken(0.75);

            cp = cairo_pattern_create_radial (w - (b_r << 1), (b_r << 1), 0, w - (b_r << 1), (b_r << 1), delta * 2.0);
            cairo_pattern_add_color_stop_rgb(cp, 0.0, hole.red(), hole.green(), hole.blue());
            cairo_pattern_add_color_stop_rgb(cp, 1.0, 0.5 * hole.red(), 0.5 *  hole.green(), 0.5 * hole.blue());
            cairo_set_source (cr, cp);
            cairo_rectangle(cr, nBorder, nBorder, w - (nBorder << 1), h - (nBorder << 1));
            cairo_fill(cr);
            cairo_pattern_destroy(cp);
        }
        else
        {
            // Draw hole
            ColorHolder hole(pUI->theme(), C_HOLE);
            cairo_set_source_rgb(cr, hole.red(), hole.green(), hole.blue());
            cairo_rectangle(cr, nBorder, nBorder, w - (nBorder << 1), h - (nBorder << 1));
            cairo_fill(cr);
        }

        size_t pos = (nState & S_PRESSED) ? 1 : (nState & S_TOGGLED) ? 2 : 0;
        if (nAngle & 2)
            pos     = 2 - pos;

        // Draw button
        Color bcl(sColor);
        size_t l    = nBorder + 1;
        size_t bw   = w - ((nBorder + 1) << 1);
        size_t t    = nBorder + 1;
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
            cairo_set_source_rgb(cr, bcl.red(), bcl.green(), bcl.blue());
            if (nAngle & 1)
                cairo_rectangle(cr, l, t + i, bw, dw1 - i);
            else
                cairo_rectangle(cr, l + i, t, dw1 - i, bh);
            cairo_fill(cr);
        }

        for (ssize_t i=0; i < dc1; ++i)
        {
            float bright = (b1 - bc) * (dc1 - i) / dc1 + bc;
            bcl.lightness(bright);
            cairo_set_source_rgb(cr, bcl.red(), bcl.green(), bcl.blue());
            if (nAngle & 1)
                cairo_rectangle(cr, l, t + dw1 + i, bw, dc1 - i);
            else
                cairo_rectangle(cr, l + dw1 + i, t, dc1 - i, bh);
            cairo_fill(cr);
        }

        for (ssize_t i=0; i < dw2; ++i)
        {
            float arg = i + 1.0;
            float bright = (b2 - b0) * arg / dw2 + b0;
            bcl.lightness(bright);
            cairo_set_source_rgb(cr, bcl.red(), bcl.green(), bcl.blue());
            if (nAngle & 1)
                cairo_rectangle(cr, l, t + bh - dw2, bw, dw2 - i);
            else
                cairo_rectangle(cr, l + bw - dw2, t, dw2 - i, bh);
            cairo_fill(cr);
        }

        for (ssize_t i=0; i < dc2; ++i)
        {
            float bright = (b2 - bc) * (dc2 - i) / dc2 + bc;
            bcl.lightness(bright);
            cairo_set_source_rgb(cr, bcl.red(), bcl.green(), bcl.blue());
            if (nAngle & 1)
                cairo_rectangle(cr, l, t + cx, bw, dc2 - i);
            else
                cairo_rectangle(cr, l + cx, t, dc2 - i, bh);
            cairo_fill(cr);
        }

        // Draw symbols
        bcl.copy(sTextColor);
        bc          = bcl.lightness();
        b1          = bc - ((2 - pos) * 0.1);
        b2          = bc - (pos * 0.1);
        wid        -= dw1 + dw2;
        float s1    = (wid >> 3);
        float s2    = (wid >> 3);

        cairo_set_line_width(cr, 2);
        bcl.lightness((nAngle & 2) ? b2 : b1);
        cairo_set_source_rgb(cr, bcl.red(), bcl.green(), bcl.blue());
        if (nAngle & 1)
            cairo_arc(cr, w >> 1, t + cx + (nAngle - 2) * (wid >> 2), s1, 0, M_PI * 2.0);
        else
            cairo_arc(cr, l + cx + (nAngle - 1) * (wid >> 2), h >> 1, s1, 0, M_PI * 2.0);
        cairo_stroke(cr);

        bcl.lightness((nAngle & 2) ? b1 : b2);
        cairo_set_source_rgb(cr, bcl.red(), bcl.green(), bcl.blue());
        if (nAngle & 1)
        {
            cairo_move_to(cr, w >> 1, t + cx - (nAngle - 2) * (wid >> 2) + s2);
            cairo_line_to(cr, w >> 1, l + cx - (nAngle - 2) * (wid >> 2) - s2);
        }
        else
        {
            cairo_move_to(cr, l + cx - (nAngle - 1) * (wid >> 2) + s2, h >> 1);
            cairo_line_to(cr, l + cx - (nAngle - 1) * (wid >> 2) - s2, h >> 1);
        }
        cairo_stroke(cr);
    }

    void Gtk2Switch::dimensions(ssize_t &w, ssize_t &h)
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

    void Gtk2Switch::resize(size_t &w, size_t &h)
    {
        ssize_t ww =w, hh = h;
        dimensions(ww, hh);
        w = ww;
        h = hh;
    }

    bool Gtk2Switch::check_mouse_over(ssize_t x, ssize_t y)
    {
        ssize_t w = 0, h = 0;
        dimensions(w, h);
        w -= (nBorder + 1) << 1;
        h -= (nBorder + 1) << 1;

        ssize_t left    = ssize_t(nWidth - w) >> 1;
        ssize_t top     = ssize_t(nHeight - h) >> 1;
        ssize_t right   = left + w;
        ssize_t bottom  = top + h;

//        lsp_trace("x=%d, y=%d, l=%d, r=%d, t=%d, b=%d", int(x), int(y), int(left), int(right), int(top), int(bottom));

        return ((x >= left) && (x <= right) && (y >= top) && (y <= bottom));
    }

    void Gtk2Switch::button_press(ssize_t x, ssize_t y, size_t state, size_t button)
    {
//        lsp_trace("x=%d, y=%d, state=%x, button=%x", int(x), int(y), int(state), int(button));
        nBMask         |= (1 << button);

        bool pressed    = (nBMask == (1 << 1)) && (check_mouse_over(x, y));
        bool is_pressed = nState & S_PRESSED;

        if (pressed != is_pressed)
        {
            if (pressed)
                nState     |= S_PRESSED;
            else
                nState     &= ~S_PRESSED;

            markRedraw();
        }
    }

    void Gtk2Switch::button_release(ssize_t x, ssize_t y, size_t state, size_t button)
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
            if (is_pressed)
                nState     ^= S_TOGGLED;
            if (pressed)
                nState     |= S_PRESSED;
            else
                nState     &= ~S_PRESSED;

            if (nBMask == 0)
                on_click(nState & S_TOGGLED);

            markRedraw();
        }
    }

    void Gtk2Switch::motion(ssize_t x, ssize_t y, size_t state)
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

            markRedraw();
        }
    }

    void Gtk2Switch::on_click(bool down)
    {
        lsp_trace("switch clicked=%d", int(down));
        bool invert = nState & S_INVERT;
        float value = (down ^ invert) ? 1.0 : 0.0;

        if (pPort != NULL)
        {
            pPort->setValue(value);
            pPort->notifyAll();
        }
    }

    void Gtk2Switch::notify(IUIPort *port)
    {
        Gtk2CustomWidget::notify(port);

        if (port == pPort)
        {
            const port_t *mdata = pPort->metadata();
            float value = 0;
            if (mdata != NULL)
                value       = pPort->getValue();

            if ((value > 0.5) ^ (nState & S_INVERT))
                nState |= S_TOGGLED;
            else
                nState &= ~S_TOGGLED;

            // Request for redraw
            markRedraw();
        }
    }

} /* namespace lsp */
