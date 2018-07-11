/*
 * Gtk2Meter.cpp
 *
 *  Created on: 30 мая 2016 г.
 *      Author: sadko
 */

#include <ui/gtk2/ui.h>

#define VU_RANGE0       1.58489319246e-05   /* -96 dB */
#define VU_RANGE1       0.00398107170553f   /* -48 dB */
#define VU_RANGE2       0.501187233627f     /* -6 dB */
#define VU_RANGE3       1.0f                /* 0 dB */

namespace lsp
{
    Gtk2Meter::Gtk2Meter(plugin_ui *ui): Gtk2CustomWidget(ui, W_METER)
    {
        pPort       = NULL;
        pActivity   = NULL;
        fMin        = 0.0f;
        fMax        = 10.0f;
        fValue      = 1.0f;
        fRms        = 0.0f;
        nAngle      = 0;
        nMWidth     = 20;
        nMHeight    = 192;
        nFlags      = MF_VALUE;
        nType       = MT_PEAK;
        nConversion = MC_NONE;
        nBorder     = 2;

        sIndColor.set(pUI->theme(), C_GLASS);
        sBgColor.set(pUI->theme(), C_BACKGROUND);
        sColor.init(this, C_GREEN, A_COLOR, -1, -1, -1, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);

        hFunction    = g_timeout_add (50, redraw_meter, this); // Schedule at 20 hz rate
    }

    Gtk2Meter::~Gtk2Meter()
    {
        if (hFunction > 0)
        {
            g_source_remove(hFunction);
            hFunction   = 0;
        }

        pPort       = NULL;
    }

    void Gtk2Meter::set(widget_attribute_t att, const char *value)
    {
        switch (att)
        {
            case A_ID:
                BIND_PORT(pUI, pPort, value);
                break;

            case A_ACTIVITY_ID:
                BIND_PORT(pUI, pActivity, value);
                break;

            case A_MIN:
                PARSE_FLOAT(value, fMin = __; nFlags |= MF_MIN);
                break;

            case A_MAX:
                PARSE_FLOAT(value, fMax = __; nFlags |= MF_MAX);
                break;

            case A_ANGLE:
                PARSE_INT(value, nAngle = (__ % 4));
                break;

            case A_WIDTH:
                PARSE_INT(value, nMWidth = __);
                break;

            case A_HEIGHT:
                PARSE_INT(value, nMHeight = __);
                break;

            case A_BORDER:
                PARSE_INT(value, nBorder = __);
                break;

            case A_TEXT:
                PARSE_BOOL(value,
                    if (__)
                        nFlags |= MF_VALUE;
                    else
                        nFlags &= ~MF_VALUE;
                );
                break;

            case A_LOGARITHMIC:
                PARSE_BOOL(value,
                    if (__)
                        nFlags |= MF_LOG | MF_LOG_SET;
                    else
                        nFlags = (nFlags & ~MF_LOG) | MF_LOG_SET;
                );
                break;

            case A_TYPE:
                if (!strcasecmp(value, "vu"))
                    nType   = MT_VU;
                else if (!strcasecmp(value, "peak"))
                    nType   = MT_PEAK;
                else if (!strcasecmp(value, "rms_peak"))
                    nType   = MT_RMS_PEAK;
                break;

            case A_REVERSIVE:
                PARSE_BOOL(value,
                    if (__)
                        nFlags |= MF_REV;
                    else
                        nFlags &= ~MF_REV;
                );
                break;

            default:
                if (sPadding.set(att, value))
                    return;
                else if (sColor.set(att, value))
                    return;

                Gtk2CustomWidget::set(att, value);
                break;
        }
    }

    void Gtk2Meter::end()
    {
        // Look at the type
        if (!(nFlags & MF_LOG_SET))
        {
            switch (nType)
            {
                case MT_VU:
                case MT_RMS_PEAK:
                    nFlags |= (MF_LOG_SET | MF_LOG);
                    break;
                default:
                    break;
            }
        }

        // Check that RMS and Peak meters are present
        if (nType == MT_RMS_PEAK)
            nFlags  |= MF_RMS;

        // Additional analyse
        if (pPort == NULL)
            return;

        const port_t *p = pPort->metadata();
        if (p == NULL)
            return;

        // Synchronize metadata parameters with meter
        if ((!(nFlags & MF_MIN)) && (p->flags & F_LOWER))
            fMin        = p->min;

        if ((!(nFlags & MF_MAX)) && (p->flags & F_UPPER))
            fMax        = p->max;

        fValue          = p->start;

        if (!(nFlags & MF_LOG_SET))
        {
            if ((p->flags & F_LOG) || (is_decibel_unit(p->unit)))
                nFlags     |= MF_LOG;
            else
                nFlags     &= ~MF_LOG;
        }

        if (nFlags & MF_LOG)
        {
            switch (p->unit)
            {
                case U_DB:
                case U_GAIN_AMP:
                    nConversion = MC_DB20;
                    break;

                case U_GAIN_POW:
                    nConversion = MC_DB10;
                    break;

                default:
                    nConversion = MC_LOG;
                    break;
            }
        }
    }

    float Gtk2Meter::normalized_value()
    {
        if (fValue < fMin)
            return 0.0f;
        else if (fValue > fMax)
            return 1.0f;

        if (nFlags & MF_LOG)
        {
            if (fMin <= 0.0f)
                fMin    = 1e-6f;
            if (fMax <= 0.0f)
                fMax    = 1e-6f;
            float v     = (fValue <= 0.0f) ? 1e-6f : 0.0f;
            return logf(v) / logf(fMax / fMin);
        }
        else
        {
            float delta     = fMax - fMin;
            return fValue / delta;
        }

        return fValue;
    }

    float Gtk2Meter::get_value(size_t seg, size_t nseg) const
    {
        // Limit value
        if (nseg < 1)
            nseg  = 1;

        if (nFlags & MF_LOG)
        {
            float min   = (fMin < 1e-6f) ? 1e-6f : fMin;
            float max   = (fMax < 1e-6f) ? 1e-6f : fMax;
            min         = logf(min);
            max         = logf(max);
            float delta = (max - min) / nseg;
            return expf(min + delta * seg);
        }
        else
        {
            float delta = (fMax - fMin) / nseg;
            return fMin + delta * seg;
        }

        return 0.0f;
    }

    void Gtk2Meter::get_color(float rs, float re, ColorHolder &cl)
    {
        if (nType == MT_PEAK)
        {
            cl.copy(sColor);
            return;
        }

        if (re <= VU_RANGE0)
        {
            cl.copy(sColor);
            cl.darken(0.5f);
        }
        else if (re <= VU_RANGE1)
        {
            cl.copy(sColor);
            cl.darken(0.25f);
        }
        else if (re <= VU_RANGE2)
            cl.copy(sColor);
        else if (re <= VU_RANGE3)
            cl.set(pUI->theme(), C_YELLOW);
        else
            cl.set(pUI->theme(), C_RED);
    }

    void Gtk2Meter::format_meter(float value, char *buf, size_t n) const
    {
        float avalue = (value < 0.0f) ? -value : value;

        switch (nConversion)
        {
            case MC_DB20:
            case MC_DB10:
            {
                if (avalue >= 1e-6)
                {
                    float k = (nConversion == MC_DB20) ? 20.0f / M_LN10 : 10.0f / M_LN10;
                    value   = k * logf(avalue);
                    avalue  = (value < 0) ? -value : value;
                }
                else
                {
                    strcpy(buf, "-inf");
                    return;
                }
                break;
            }
        }

        if (avalue < 10.0f)
            snprintf(buf, n, "%.2f", value);
        else if (avalue < 100.0f)
            snprintf(buf, n, "%.1f", value);
        else
            snprintf(buf, n, "%ld", long(value));
    }

    void Gtk2Meter::draw(cairo_t *cr)
    {
        // Draw background
        cairo_set_source_rgb(cr, sBgColor.red(), sBgColor.green(), sBgColor.blue());
        cairo_rectangle(cr, 0, 0, nWidth, nHeight);
        cairo_fill(cr);

        float value     = (nFlags & MF_INACTIVE) ? 0.0f : (nFlags & MF_RMS) ? fRms : fValue;
        float peak      = (nFlags & MF_INACTIVE) ? 0.0f : fValue;
        fValue         *= 0.8f;
        fRms           *= 0.8f;

        ssize_t cx      = nWidth >> 1, cy = nHeight >> 1;
        cairo_text_extents_t extents;
        ssize_t tsx = 0, tsy = 0;
        Color           t_col(sColor);

        char v_text[32];
        v_text[0]       = '\0';

        if (nFlags & MF_VALUE)
        {
            cairo_text_extents_t extents;
            cairo_select_font_face(cr, "Sans",
                  CAIRO_FONT_SLANT_NORMAL,
                  CAIRO_FONT_WEIGHT_NORMAL);
            cairo_set_font_size(cr, 10);

            cairo_text_extents(cr, "-99.9", &extents);
            tsx     = extents.width  + 4;
            tsy     = extents.height + 4;

            format_meter(value, v_text, sizeof(v_text));
        }

        ColorHolder l_c;
        cairo_set_line_width(cr, 1);

        // Draw meter
        if (nAngle & 1)
        {
            // Vertical meter
            size_t segments = nMHeight >> 2;
            ssize_t sx = nMWidth, sy = (segments << 2) + 2;
            ssize_t left = cx - (sx >> 1), top = cy - (sy >> 1);

            // Update top coordinate
            if (nAngle & 2)
            {
                ssize_t tmp     = top;
                top            += (tsy >> 1);
                tsy             = tmp;
            }
            else
            {
                ssize_t tmp     = top;
                top            -= (tsy >> 1);
                tsy             = tmp + sy;
            }
            tsx = cx;

            // Draw glass
            cairo_set_source_rgb(cr, sIndColor.red(), sIndColor.green(), sIndColor.blue());
            cairo_rectangle(cr, left - nBorder, top - nBorder, sx + (nBorder << 1), sy + (nBorder << 1) + tsy);
            cairo_fill(cr);

            // Top to bottom
            for (size_t i=0; i<segments; ++i)
            {
                size_t hpos = (nAngle & 2) ? segments - i - 1 : i;
                float lv    = get_value(i, segments);
                float rv    = get_value(i + 1, segments);
                get_color(lv, rv, l_c);

                if (nFlags & MF_INACTIVE)
                    l_c.blend(sIndColor, 0.05);
                else
                {
                    bool matched = (lv < value);
                    if ((!matched) && (nFlags & MF_RMS))
                        matched = (peak >= lv) && (peak <= rv);

                    if (!((matched) ^ bool(nFlags & MF_REV)))
                        l_c.blend(sIndColor, 0.05);
                }
                if (lv < value)
                    t_col.copy(l_c);

                cairo_set_source_rgb(cr, l_c.red(), l_c.green(), l_c.blue());
                cairo_rectangle(cr, left + 2, top + sy - ((hpos + 1) << 2), sx - 4, 2);
                cairo_stroke_preserve(cr);
                cairo_fill(cr);
            }
        }
        else
        {
            // Horizontal meter
            size_t segments = nMHeight >> 2;
            ssize_t sx = (segments << 2) + 2, sy = nMWidth;
            ssize_t left = cx - (sx >> 1), top = cy - (sy >> 1);

            // Update left coordinate
            if (nAngle & 2)
            {
                ssize_t tmp     = left;
                left           += (tsx >> 1);
                tsx             = tmp;
            }
            else
            {
                ssize_t tmp     = left;
                left           -= (tsx >> 1);
                tsx             = tmp + sx;
            }
            tsy = cy;

            // Draw glass
            cairo_set_source_rgb(cr, sIndColor.red(), sIndColor.green(), sIndColor.blue());
            cairo_rectangle(cr, left - nBorder, top - nBorder, sx + (nBorder << 1) + tsx, sy + (nBorder << 1));
            cairo_fill(cr);

            // Right to left
            for (size_t i=0; i<segments; ++i)
            {
                size_t vpos = (nAngle & 2) ? segments - i - 1 : i;
                float lv    = get_value(i, segments);
                float rv    = get_value(i + 1, segments);
                get_color(lv, rv, l_c);

                if (nFlags & MF_INACTIVE)
                    l_c.blend(sIndColor, 0.05);
                else
                {
                    bool matched = (lv < value);
                    if ((!matched) && (nFlags & MF_RMS))
                        matched = (peak >= lv) && (peak <= rv);

                    if (!((matched) ^ bool(nFlags & MF_REV)))
                        l_c.blend(sIndColor, 0.05);
                }
                if (lv < value)
                    t_col.copy(l_c);

                cairo_set_source_rgb(cr, l_c.red(), l_c.green(), l_c.blue());
                cairo_rectangle(cr, left + 2 + (vpos << 2), top + 2, 2, sy - 4);
                cairo_stroke_preserve(cr);
                cairo_fill(cr);
            }
        }

        // Output text
        if ((!(nFlags & MF_INACTIVE)) && (nFlags & MF_VALUE))
        {
            // Calculate text dimensions
            cairo_text_extents(cr, v_text, &extents);

            // Debug
//            cairo_set_source_rgb(cr, sColor.color().red(), sColor.color().green(), sColor.color().blue());
//            cairo_set_line_width(cr, 1);
//            cairo_move_to(cr, tsx - 4, tsy);
//            cairo_line_to(cr, tsx + 4, tsy);
//            cairo_stroke(cr);
//            cairo_move_to(cr, tsx, tsy - 4);
//            cairo_line_to(cr, tsx, tsy + 4);
//            cairo_stroke(cr);

            float r_w = extents.x_advance - extents.x_bearing;
            float r_h = extents.y_advance - extents.y_bearing;
            float fx = tsx - r_w * 0.5f;
            float fy = tsy + r_h * 0.5f;

            cairo_set_source_rgb(cr, t_col.red(), t_col.green(), t_col.blue());
            cairo_move_to(cr, fx, fy);
            cairo_show_text (cr, v_text);
//
//            cairo_set_line_width(cr, 1);
//            cairo_set_source_rgb(cr, 1.0, 0, 0);
//
//            cairo_move_to(cr, fx + extents.x_bearing, fy - 10);
//            cairo_line_to(cr, fx + extents.x_bearing, fy + 10);
//            cairo_stroke(cr);
//
//            cairo_move_to(cr, fx + extents.x_advance, fy - 10);
//            cairo_line_to(cr, fx + extents.x_advance, fy + 10);
//            cairo_stroke(cr);
//
//            cairo_move_to(cr, fx - 10, fy + extents.y_bearing);
//            cairo_line_to(cr, fx + 10, fy + extents.y_bearing);
//            cairo_stroke(cr);
//
//            cairo_move_to(cr, fx - 10, fy + extents.y_advance);
//            cairo_line_to(cr, fx + 10, fy + extents.y_advance);
//            cairo_stroke(cr);
        }
    }

    void Gtk2Meter::resize(size_t &w, size_t &h)
    {
        size_t width  = 0, height = 0;
        cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 1, 1);
        cairo_t *cr = cairo_create(surface);

//        cairo_t *cr = gdk_cairo_create(pWidget->window);
        if (nFlags & MF_VALUE)
        {
            cairo_text_extents_t extents;
            cairo_select_font_face(cr, "Sans",
                  CAIRO_FONT_SLANT_NORMAL,
                  CAIRO_FONT_WEIGHT_NORMAL);
            cairo_set_font_size(cr, 10);

            cairo_text_extents(cr, "-99.9", &extents);

            if (nAngle & 1)
                height     += extents.height + 4;
            else
                width      += extents.width + 4;
        }

        if (nAngle & 1)
        {
            width      += sPadding.left() + sPadding.right() + nMWidth + nBorder * 2 + 2;
            height     += sPadding.top() + sPadding.bottom() + nMHeight + nBorder * 2 + 2 + 3;
        }
        else
        {
            width      += sPadding.left() + sPadding.right() + nMHeight + nBorder * 2 + 2 + 3;
            height     += sPadding.top() + sPadding.bottom() + nMWidth  + nBorder * 2 + 2;
        }

        if (w < width)
            w = width;
        if (h < height)
            h = height;

        cairo_destroy(cr);
        cairo_surface_destroy(surface);
    }

    gboolean Gtk2Meter::redraw_meter(gpointer ptr)
    {
        Gtk2Meter *_this = reinterpret_cast<Gtk2Meter *>(ptr);
        _this->markRedraw();
        return TRUE;
    }

    void Gtk2Meter::notify(IUIPort *port)
    {
        if (port == pPort)
        {
            float value = pPort->getValue();
            if (fValue < value)
                fValue      = value;
            fRms       = 0.95f*fRms + 0.05f*fabs(value);
            if (fRms < 0.0f)
                fRms        = 0.0f;
        }
        else if (port == pActivity)
        {
            if (pActivity->getValue() >= 0.5f)
                nFlags &= ~MF_INACTIVE;
            else
                nFlags |= MF_INACTIVE;
        }
        Gtk2CustomWidget::notify(port);
    }

} /* namespace lsp */
