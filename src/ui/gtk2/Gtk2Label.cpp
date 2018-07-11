/*
 * Gtk2Label.cpp
 *
 *  Created on: 21 окт. 2015 г.
 *      Author: sadko
 */

#include <ui/gtk2/ui.h>

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

namespace lsp
{
    Gtk2Label::Gtk2Label(plugin_ui *ui, label_type_t type): Gtk2Widget(ui)
    {
        sText       = NULL;
        pWidget     = gtk_label_new(NULL);
        sColor.set(pUI->theme(), C_LABEL_TEXT);
        sBgColor.set(pUI->theme(), C_BACKGROUND);
        pPort       = NULL;
        enType      = type;
        fValue      = 0.0f;
        fVAlign     = 0.5f;
        fHAlign     = 0.5f;
        nUnits      = U_NONE - 1;
    }

    Gtk2Label::~Gtk2Label()
    {
        if (sText != NULL)
        {
            free(sText);
            sText = NULL;
        }
    }

    void Gtk2Label::set(widget_attribute_t att, const char *value)
    {
        switch (att)
        {
            case A_ID:
                {
                    IUIPort *port = pUI->port(value);
                    if (port == NULL)
                        break;
                    if (enType == LT_TEXT)
                    {
                        const port_t *mdata = port->metadata();
                        if ((mdata != NULL) && (mdata->name != NULL))
                            set(A_TEXT, mdata->name);
                    }
                    else
                    {
                        pPort       = port;
                        pPort->bind(this);
                    }
                }
                break;
            case A_TEXT:
                if (enType != LT_TEXT)
                    return;
                if (sText != NULL)
                {
                    free(sText);
                    sText = NULL;
                }
                sText = strdup(value);
                break;
            case A_UNITS:
                if (enType == LT_TEXT)
                    return;
                if (!strcmp(value, "default"))
                    nUnits      = U_NONE - 1;
                else
                    nUnits      = decode_unit(value);
                break;
            case A_VALIGN:
                PARSE_FLOAT(value, fVAlign = __);
                break;
            case A_HALIGN:
                PARSE_FLOAT(value, fHAlign = __);
                break;
            case A_COLOR:
                sColor.set(pUI->theme(), value);
                break;
            case A_BG_COLOR:
                sBgColor.set(pUI->theme(), value);
                break;
            default:
                Gtk2Widget::set(att, value);
                break;
        }
    }

    void Gtk2Label::end()
    {
        updateText();
    }

    void Gtk2Label::format_float(char *buf, const port_t *meta)
    {
        float value         = (fValue < 0.0f) ? -fValue : fValue;
        size_t tolerance    = 0;

        // Determine tolerance
        if (value < 0.1)
            tolerance   = 4;
        else if (value < 1.0)
            tolerance   = 3;
        else if (value < 10.0)
            tolerance   = 2;
        else if (value < 100.0)
            tolerance   = 1;
        else
            tolerance   = 0;

        // Now determine normal tolerance
        if (meta->flags & F_STEP)
        {
            size_t max_tol = 0;
            float step      = (meta->step < 0.0f) ? - meta->step : meta->step;
            while ((max_tol < 4) && (truncf(step) <= 0))
            {
                step   *= 10;
                max_tol++;
            }

            if (tolerance > max_tol)
                tolerance = max_tol;
        }

        const char *fmt = "%.0f";
        switch (tolerance)
        {
            case 4:     fmt = "%.4f"; break;
            case 3:     fmt = "%.3f"; break;
            case 2:     fmt = "%.2f"; break;
            case 1:     fmt = "%.1f"; break;
            default:    fmt = "%.0f"; break;
        };

        sprintf(buf, fmt, fValue);
    }

    void Gtk2Label::format_decibels(char *buf, const port_t *meta)
    {
        double mul       = (meta->unit == U_GAIN_AMP) ? 20.0 : 10.0;
//        float thresh    = (meta->unit == U_GAIN_AMP) ? DECIBEL_INF_THRESH * DECIBEL_INF_THRESH : DECIBEL_INF_THRESH;
        double value    = fabs(fValue);
        value = mul * log(value) / M_LN10;
        if (value <= -75.0)
        {
            strcpy(buf, "-inf");
            return;
        }
        sprintf(buf, "%.2f", value);
    }

    void Gtk2Label::format_int(char *buf, const port_t *meta)
    {
        sprintf(buf, "%ld", long(fValue));
    }

    void Gtk2Label::format_enum(char *buf, const port_t *meta)
    {
        float min   = (meta->flags & F_LOWER) ? meta->min: 0;
        float max   = meta->min + list_size(meta->items) - 1.0f;
        float step  = (meta->flags & F_STEP) ? meta->step : 1.0;

        for (const char **p = meta->items; (p != NULL) && (*p != NULL) && (min < max); ++p)
        {
            if (min >= fValue)
            {
                strncpy(buf, *p, 128);
                return;
            }
            min    += step;
        }
        strcpy(buf, "");
    }

    void Gtk2Label::updateText()
    {
        // Initial text
        const char *text    = sText;
        char *a_text        = NULL;

        if ((enType != LT_TEXT) && (pPort != NULL))
        {
            const port_t *mdata = pPort->metadata();
            if (mdata != NULL)
            {
                const char *u_name = NULL;
                if (nUnits != (U_NONE - 1))
                    u_name  = encode_unit(nUnits);
                else
                {
                    switch (mdata->unit)
                    {
                        case U_GAIN_AMP:
                        case U_GAIN_POW:
                            u_name  = encode_unit(U_DB);
                            break;
                        default:
                            u_name  = encode_unit(mdata->unit);
                            break;
                    }
                }

                encode_unit((nUnits == (U_NONE - 1)) ? mdata->unit : nUnits);

                if (enType == LT_VALUE)
                {
                    char buf[128];

                    if (mdata->unit == U_ENUM)
                        format_enum(buf, mdata);
                    else
                    {
                        if ((mdata->unit == U_GAIN_AMP) || (mdata->unit == U_GAIN_POW))
                            format_decibels(buf, mdata);
                        else if (mdata->flags & F_INT)
                            format_int(buf, mdata);
                        else
                            format_float(buf, mdata);
                    }

                    asprintf(&a_text, "%s\n%s", buf, (u_name != NULL) ? u_name : "" );
                    text    = a_text;
                }
                else if (enType == LT_PARAM)
                {
                    text        = mdata->name;
                    if (u_name != NULL)
                    {
                        if (text != NULL)
                            asprintf(&a_text, "%s (%s)", text, u_name);
                        else
                            asprintf(&a_text, "(%s)", u_name);
                        text    = a_text;
                    }
                }
            }
        }

        //g_markup_printf_escaped
        gchar *out = g_markup_printf_escaped(
            "<span foreground=\"#%06lx\" background=\"#%06lx\">%s</span>",
             (unsigned long)(sColor.getColor()),
             (unsigned long)(sBgColor.getColor()),
             (text != NULL) ? text : ""
             );

        gtk_label_set_markup(GTK_LABEL(pWidget), out);

        if (fHAlign < 0.0f)
            gtk_label_set_justify(GTK_LABEL(pWidget), GTK_JUSTIFY_FILL);
        else if (fHAlign < 0.3f)
            gtk_label_set_justify(GTK_LABEL(pWidget), GTK_JUSTIFY_LEFT);
        else if (fHAlign > 0.6f)
            gtk_label_set_justify(GTK_LABEL(pWidget), GTK_JUSTIFY_RIGHT);
        else
            gtk_label_set_justify(GTK_LABEL(pWidget), GTK_JUSTIFY_CENTER);

        gtk_misc_set_alignment(GTK_MISC(pWidget), fHAlign, fVAlign);

        g_free(out);

        if (a_text != NULL)
            free(a_text);
    }

    void Gtk2Label::notify(IUIPort *port)
    {
        if (enType != LT_VALUE)
            return;

        if (port == pPort)
        {
            const port_t *mdata = pPort->metadata();
            if (mdata != NULL)
            {
                fValue      = pPort->getValue();
                updateText();
            }
        }
    }
} /* namespace lsp */
