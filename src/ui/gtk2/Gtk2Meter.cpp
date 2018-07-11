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

#define METER_ATT       0.1f
#define METER_REL       0.5f

namespace lsp
{
    Gtk2Meter::Gtk2Meter(plugin_ui *ui): Gtk2CustomWidget(ui, W_METER)
    {
        pPort[0]    = NULL;
        pPort[1]    = NULL;
        pActivity[0]= NULL;
        pActivity[1]= NULL;
        fMin        = 0.0f;
        fMax        = 10.0f;
        fValue[0]   = 0.0f;
        fValue[1]   = 0.0f;
        fRms[0]     = 0.0f;
        fRms[1]     = 0.0f;
        fReport[0]  = 0.0f;
        fReport[1]  = 0.0f;
        fBalance    = 0.0f;
        nAngle      = 0;
        nMWidth     = 20;
        nMHeight    = 192;
        nFlags      = MF_VALUE;
        nType       = MT_PEAK;
        nConversion = MC_NONE;
        nBorder     = 2;

        sIndColor.set(pUI->theme(), C_GLASS);
        sBgColor.set(pUI->theme(), C_BACKGROUND);
        sColor[0].init(this, C_GREEN, A_COLOR, -1, -1, -1, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);
        sColor[1].init(this, C_GREEN, A_COLOR2, -1, -1, -1, A_HUE2_ID, A_SAT2_ID, A_LIGHT2_ID);

        hFunction    = g_timeout_add (50, redraw_meter, this); // Schedule at 20 hz rate
    }

    Gtk2Meter::~Gtk2Meter()
    {
        if (hFunction > 0)
        {
            g_source_remove(hFunction);
            hFunction   = 0;
        }

        pPort[0]    = NULL;
        pPort[1]    = NULL;
    }

    void Gtk2Meter::set(widget_attribute_t att, const char *value)
    {
        switch (att)
        {
            case A_ID:
                BIND_PORT(pUI, pPort[0], value);
                break;

            case A_ID2:
                BIND_PORT(pUI, pPort[1], value);
                break;

            case A_ACTIVITY_ID:
                BIND_PORT(pUI, pActivity[0], value);
                break;

            case A_ACTIVITY2_ID:
                BIND_PORT(pUI, pActivity[1], value);
                break;

            case A_MIN:
                PARSE_FLOAT(value, fMin = __; nFlags |= MF_MIN);
                break;

            case A_MAX:
                PARSE_FLOAT(value, fMax = __; nFlags |= MF_MAX);
                break;

            case A_BALANCE:
                PARSE_FLOAT(value,
                    fBalance = __;
                    fValue[0] = __;
                    fValue[1] = __;
                    fReport[0] = __;
                    fReport[1] = __;
                    nFlags |= MF_BALANCE
                );
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

            case A_STEREO:
                PARSE_BOOL(value,
                    if (__)
                        nFlags |= MF_STEREO;
                    else
                        nFlags &= ~MF_STEREO;
                );
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
                else if (sColor[0].set(att, value))
                    return;
                else if (sColor[1].set(att, value))
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
        if (pPort[0] == NULL)
            return;

        const port_t *p = pPort[0]->metadata();
        if (p == NULL)
            return;

        // Synchronize metadata parameters with meter
        if ((!(nFlags & MF_MIN)) && (p->flags & F_LOWER))
            fMin        = p->min;

        if ((!(nFlags & MF_MAX)) && (p->flags & F_UPPER))
            fMax        = p->max;

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

    void Gtk2Meter::get_color(size_t channel, float rs, float re, ColorHolder &cl)
    {
        if (nType == MT_PEAK)
        {
            if ((nFlags & MF_BALANCE) && (fBalance <= re) && (fBalance >= rs))
                cl.set(pUI->theme(), C_YELLOW);
            else
                cl.copy(sColor[channel]);
            return;
        }

        if (re <= VU_RANGE0)
        {
            cl.copy(sColor[channel]);
            cl.darken(0.5f);
        }
        else if (re <= VU_RANGE1)
        {
            cl.copy(sColor[channel]);
            cl.darken(0.25f);
        }
        else if (re <= VU_RANGE2)
            cl.copy(sColor[channel]);
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
                if (avalue >= 1e+6)
                {
                    strcpy(buf, "+inf");
                    return;
                }
                else if (avalue >= 1e-6)
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

    void Gtk2Meter::update_peaks()
    {
        size_t channels = (nFlags & MF_STEREO) ? 2 : 1;

        // Update values
        for (size_t i=0; i<channels; ++i)
        {
            float v         = fReport[i];
            float av        = fabs(v);

            // Peak value
            if (nFlags & MF_BALANCE)
            {
                if (v > fBalance)
                {
                    if (fValue[i] <= v)
                        fValue[i]       = v;
                    else
                        fValue[i]      += METER_REL * (v - fValue[i]);
                }
                else
                {
                    if (fValue[i] > v)
                        fValue[i]       = v;
                    else
                        fValue[i]      += METER_REL * (v - fValue[i]);
                }
            }
            else
            {
                if (fValue[i] < v)
                    fValue[i]      = v;
                else
                    fValue[i]     += METER_REL * (v - fValue[i]);
            }
            fRms[i]       += (av > fRms[i]) ? METER_ATT * (av - fRms[i]) :  METER_REL * (av - fRms[i]);

            // Limit RMS value
            if (fRms[i] < 0.0f)
                fRms[i]        = 0.0f;
        }
    }

    void Gtk2Meter::draw(cairo_t *cr)
    {
        // Variables
        char v_text[64];
        char *s_values[32];
        float value[2], peak[2];
        Color           t_col[2];
        cairo_text_extents_t extents;

        // Draw background
        cairo_set_source_rgb(cr, sBgColor.red(), sBgColor.green(), sBgColor.blue());
//        cairo_set_source_rgb(cr, 1, 1, 1);
        cairo_rectangle(cr, 0, 0, nWidth, nHeight);
        cairo_fill(cr);

        // Calculate parameters
        size_t channels = (nFlags & MF_STEREO) ? 2 : 1;
        ssize_t cx      = nWidth >> 1, cy = nHeight >> 1;
        ssize_t tsx     = 0;
        ssize_t tsy     = 0;
        ssize_t txt_w   = 0; // text field width
        ssize_t txt_h   = 0; // text field height
        ssize_t txt_cx  = 0;
        ssize_t txt_cy  = 0;

        // Store values
        for (size_t i=0; i<channels; ++i)
        {
            value[i]        = (nFlags & (MF_INACTIVE0 << i)) ? 0.0f : (nFlags & MF_RMS) ? fRms[i] : fValue[i];
            peak[i]         = (nFlags & (MF_INACTIVE0 << i)) ? 0.0f : fValue[i];
        }

        // Format text values
        if (nFlags & MF_VALUE)
        {
            cairo_text_extents_t extents;
            cairo_select_font_face(cr, "Sans",
                  CAIRO_FONT_SLANT_NORMAL,
                  CAIRO_FONT_WEIGHT_NORMAL);
            cairo_set_font_size(cr, 10);

            cairo_text_extents(cr, "-99.9", &extents);
            txt_w   = extents.width;
            txt_h   = extents.height;
            tsx     = txt_w + 2;
            tsy     = channels * (txt_h + 2) + 2;

            // Format text values
            for (size_t i=0; i<channels; ++i)
            {
                s_values[i]     = &v_text[i * 32];
                s_values[i][0]  = '\0';
                t_col[i].copy(sColor[i]);

                format_meter(value[i], s_values[i], 32);
            }
        }

        ColorHolder l_c;
        cairo_set_line_width(cr, 1);

        // Draw meter
        if (nAngle & 1)
        {
            // Vertical meter
            size_t segments     = nMHeight >> 2;
            ssize_t sx          = nMWidth;
            ssize_t sy          = (segments << 2) + 2;
            ssize_t left        = cx - (sx >> 1);
            ssize_t top         = cy - (sy >> 1);

            // Update top coordinate
            txt_cx          = cx;
            if (nAngle & 2)
            {
                txt_cy          = top;
                top            += (tsy >> 1);
            }
            else
            {
                txt_cy          = top + sy;
                top            -= (tsy >> 1);
            }

            // Draw glass
            cairo_set_source_rgb(cr, sIndColor.red(), sIndColor.green(), sIndColor.blue());
            cairo_rectangle(cr, ssize_t(left - nBorder), ssize_t(top - nBorder), sx + (nBorder << 1), sy + (nBorder << 1) + tsy);
            cairo_fill(cr);

            // Tune parameters
            tsx                 = cx;
            if (channels > 1)
                sx = (sx >> 1) - 3;
            else
                sx -= 4;

            // Top to bottom
            for (size_t i=0; i<segments; ++i)
            {
                size_t hpos = (nAngle & 2) ? segments - i - 1 : i;
                float lv    = get_value(i, segments);
                float rv    = get_value(i + 1, segments);

                for (size_t j=0; j<channels; ++j)
                {
                    get_color(j, lv, rv, l_c);

                    if (nFlags & (MF_INACTIVE0 << j))
                        l_c.blend(sIndColor, 0.05);
                    else if (nFlags & MF_BALANCE)
                    {
                        if (fBalance < value[j])
                        {
                            bool matched = ((lv < value[j]) && (lv >= fBalance)) ||
                                            ((fBalance <= rv) && (fBalance >= lv));

                            if (!((matched) ^ bool(nFlags & MF_REV)))
                                l_c.blend(sIndColor, 0.05);
                        }
                        else
                        {
                            bool matched = ((rv > value[j]) && (rv <= fBalance)) ||
                                            ((fBalance <= rv) && (fBalance >= lv));

                            if (!((matched) ^ bool(nFlags & MF_REV)))
                                l_c.blend(sIndColor, 0.05);
                        }
                    }
                    else
                    {
                        bool matched = (lv < value[j]);
                        if ((!matched) && (nFlags & MF_RMS))
                            matched = (peak[j] >= lv) && (peak[j] <= rv);

                        if (!((matched) ^ bool(nFlags & MF_REV)))
                            l_c.blend(sIndColor, 0.05);

                        if (lv < value[j])
                            t_col[j].copy(l_c);
                    }

                    cairo_set_source_rgb(cr, l_c.red(), l_c.green(), l_c.blue());
                    cairo_rectangle(cr, left + j*(sx+2) + 2, top + sy - ((hpos + 1) << 2), sx, 2);
                    cairo_stroke_preserve(cr);
                    cairo_fill(cr);
                }
            }
        }
        else
        {
            // Horizontal meter
            size_t segments = nMHeight >> 2;
            ssize_t sx = (segments << 2) + 2, sy = nMWidth;
            ssize_t left = cx - (sx >> 1), top = cy - (sy >> 1);

            // Update text sizes
            if (channels > 0)
                tsy            += txt_h + 2;

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

            if (channels > 1)
                sy = (sy >> 1) - 3;
            else
                sy -= 4;

            // Right to left
            for (size_t i=0; i<segments; ++i)
            {
                size_t vpos = (nAngle & 2) ? segments - i - 1 : i;
                float lv    = get_value(i, segments);
                float rv    = get_value(i + 1, segments);

                for (size_t j=0; j<channels; ++j)
                {
                    get_color(j, lv, rv, l_c);

                    if (nFlags & (MF_INACTIVE0 << j))
                        l_c.blend(sIndColor, 0.05);
                    else if (nFlags & MF_BALANCE)
                    {
                        if (fBalance < value[j])
                        {
                            bool matched = ((lv < value[j]) && (lv >= fBalance)) ||
                                            ((fBalance <= rv) && (fBalance >= lv));

                            if (!((matched) ^ bool(nFlags & MF_REV)))
                                l_c.blend(sIndColor, 0.05);
                        }
                        else
                        {
                            bool matched = ((rv > value[j]) && (rv <= fBalance)) ||
                                            ((fBalance <= rv) && (fBalance >= lv));

                            if (!((matched) ^ bool(nFlags & MF_REV)))
                                l_c.blend(sIndColor, 0.05);
                        }
                    }
                    else
                    {
                        bool matched = (lv < value[j]);
                        if ((!matched) && (nFlags & MF_RMS))
                            matched = (peak[j] >= lv) && (peak[j] <= rv);

                        if (!((matched) ^ bool(nFlags & MF_REV)))
                            l_c.blend(sIndColor, 0.05);

                        if (lv < value[j])
                            t_col[j].copy(l_c);
                    }

                    cairo_set_source_rgb(cr, l_c.red(), l_c.green(), l_c.blue());
                    cairo_rectangle(cr, left + j*(sy+2) + (vpos << 2) + 2, top + 2, 2, sy);
                    cairo_stroke_preserve(cr);
                    cairo_fill(cr);
                }
            }
        }

        // Output text
        if (nFlags & MF_VALUE)
        {
//            cairo_set_source_rgb(cr, sColor[0].red(), sColor[0].green(), sColor[0].blue());
//            cairo_set_line_width(cr, 1);
//            cairo_move_to(cr, txt_cx - 4, txt_cy);
//            cairo_line_to(cr, txt_cx + 4, txt_cy);
//            cairo_stroke(cr);
//            cairo_move_to(cr, txt_cx, txt_cy - 4);
//            cairo_line_to(cr, txt_cx, txt_cy + 4);
//            cairo_stroke(cr);

            for (size_t i=0; i<channels; ++i)
            {
                if (nFlags & (MF_INACTIVE0 << i))
                    continue;

                cairo_text_extents(cr, s_values[i], &extents);

                float r_w = extents.x_advance - extents.x_bearing;
                float r_h = extents.y_advance - extents.y_bearing;
                float fx = txt_cx - r_w * 0.5f;
                float fy = (channels > 1) ?
                            txt_cy + (r_h + 2) * (i - 0.5f) + 4 :
                            txt_cy + r_h * 0.5f;

                cairo_set_source_rgb(cr, t_col[i].red(), t_col[i].green(), t_col[i].blue());
                cairo_move_to(cr, fx, fy);
                cairo_show_text (cr, s_values[i]);
            }
        }
    }

    void Gtk2Meter::resize(size_t &w, size_t &h)
    {
        size_t width  = 0, height = 0;
        cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 1, 1);
        cairo_t *cr = cairo_create(surface);
        size_t channels = (nFlags & MF_STEREO) ? 2 : 1;

//        cairo_t *cr = gdk_cairo_create(pWidget->window);
        if (nFlags & MF_VALUE)
        {
            cairo_text_extents_t extents;
            cairo_select_font_face(cr, "Sans",
                  CAIRO_FONT_SLANT_NORMAL,
                  CAIRO_FONT_WEIGHT_NORMAL);
            cairo_set_font_size(cr, 10);

            cairo_text_extents(cr, "-99.9", &extents);

            ssize_t txt_w   = extents.width;
            ssize_t txt_h   = extents.height;
            if (nAngle & 1)
                height     += (txt_h * channels) + 2;
            else
                width      += txt_w + 4;
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
        _this->update_peaks();
        _this->markRedraw();
        return TRUE;
    }

    void Gtk2Meter::notify(IUIPort *port)
    {
        for (size_t i=0; i<2; ++i)
        {
            if (port == pPort[i])
            {
                fReport[i]      = port->getValue();
            }
            if (port == pActivity[i])
            {
                if (pActivity[i]->getValue() >= 0.5f)
                    nFlags &= ~(MF_INACTIVE0 << i);
                else
                    nFlags |= (MF_INACTIVE0 << i);
            }
        }


        Gtk2CustomWidget::notify(port);
    }

} /* namespace lsp */
