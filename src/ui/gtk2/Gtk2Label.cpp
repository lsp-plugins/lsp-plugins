/*
 * Gtk2Label.cpp
 *
 *  Created on: 21 окт. 2015 г.
 *      Author: sadko
 */

#include <ui/gtk2/ui.h>
#include <core/alloc.h>

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#define TMP_BUF_SIZE        128

namespace lsp
{
    Gtk2Label::Gtk2Label(plugin_ui *ui, label_type_t type): Gtk2Widget(ui, W_LABEL)
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
        bDetailed   = true;
        nFontSize   = -1;
        nUnits      = U_NONE - 1;
        nPrecision  = -1;
    }

    Gtk2Label::~Gtk2Label()
    {
        if (sText != NULL)
        {
            lsp_free(sText);
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
                        notify(pPort);
                    }
                }
                break;
            case A_TEXT:
                if (enType != LT_TEXT)
                    return;
                if (sText != NULL)
                {
                    lsp_free(sText);
                    sText = NULL;
                }
                sText = lsp_strdup(value);
                break;
            case A_UNITS:
                if (enType == LT_TEXT)
                    return;
                if (!strcmp(value, "default"))
                    nUnits      = U_NONE - 1;
                else
                    nUnits      = decode_unit(value);
                break;
            case A_FONT_SIZE:
                PARSE_FLOAT(value, nFontSize = __ * 1000);
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
            case A_DETAILED:
                PARSE_BOOL(value, bDetailed = __);
                break;
            case A_PRECISION:
                PARSE_INT(value, nPrecision = __);
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

    void Gtk2Label::updateText()
    {
        // Initial text
        char a_text[64];
        const char *text    = sText;
//        char *a_text        = NULL;

        if ((enType != LT_TEXT) && (pPort != NULL))
        {
            const port_t *mdata = pPort->metadata();
            if (mdata != NULL)
            {
                const char *u_name = NULL;
                if (nUnits != (U_NONE - 1))
                    u_name  = encode_unit(nUnits);
                else
                    u_name  = encode_unit((is_decibel_unit(mdata->unit)) ? U_DB : mdata->unit);

//                encode_unit((nUnits == (U_NONE - 1)) ? mdata->unit : nUnits);

                if (enType == LT_VALUE)
                {
                    char buf[TMP_BUF_SIZE];
                    format_value(buf, TMP_BUF_SIZE, mdata, fValue, nPrecision);

                    if (bDetailed)
                        snprintf(a_text, sizeof(a_text), "%s\n%s", buf, (u_name != NULL) ? u_name : "" );
                    else
                        snprintf(a_text, sizeof(a_text), "%s", buf);
                    text    = a_text;
                }
                else if (enType == LT_PARAM)
                {
                    text        = mdata->name;
                    if (u_name != NULL)
                    {
                        if (bDetailed)
                        {
                            if (text != NULL)
                                snprintf(a_text, sizeof(a_text), "%s (%s)", text, u_name);
                            else
                                snprintf(a_text, sizeof(a_text), "(%s)", u_name);
                        }
                        else if (text != NULL)
                            snprintf(a_text, sizeof(a_text), "%s", text);
                        text    = a_text;
                    }
                }
            }
        }

        //g_markup_printf_escaped
        gchar *out  = NULL;
        if (nFontSize > 0)
        {
            out = g_markup_printf_escaped(
                "<span foreground=\"#%06lx\" background=\"#%06lx\" size=\"%d\">%s</span>",
                 (unsigned long)(sColor.rgb24()),
                 (unsigned long)(sBgColor.rgb24()),
                 int(nFontSize),
                 (text != NULL) ? text : ""
                 );
        }
        else
            out = g_markup_printf_escaped(
                "<span foreground=\"#%06lx\" background=\"#%06lx\">%s</span>",
                 (unsigned long)(sColor.rgb24()),
                 (unsigned long)(sBgColor.rgb24()),
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
    }

    void Gtk2Label::notify(IUIPort *port)
    {
        Gtk2Widget::notify(port);

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
