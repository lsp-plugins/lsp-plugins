/*
 * CtlLabel.cpp
 *
 *  Created on: 7 июл. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

#define TMP_BUF_SIZE        128

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlLabel::metadata = { "CtlLabel", &CtlWidget::metadata };

        CtlLabel::CtlLabel(CtlRegistry *src, LSPLabel *widget, ctl_label_type_t type): CtlWidget(src, widget)
        {
            pClass          = &metadata;
            pPort           = NULL;
            enType          = type;
            fValue          = 0.0f;
            bDetailed       = true;
            bSameLine       = false;
            nUnits          = U_NONE - 1;
            nPrecision      = -1;
        }

        CtlLabel::~CtlLabel()
        {
        }

        void CtlLabel::init()
        {
            CtlWidget::init();

            if (pWidget == NULL)
                return;

            LSPLabel *lbl = static_cast<LSPLabel *>(pWidget);

            // Initialize color controllers
            sColor.init_hsl(pRegistry, lbl, lbl->font()->color(), A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);
            sBgColor.init_basic(pRegistry, lbl, lbl->bg_color(), A_BG_COLOR);
        }

        void CtlLabel::set(widget_attribute_t att, const char *value)
        {
            LSPLabel *lbl = (pWidget != NULL) ? static_cast<LSPLabel *>(pWidget) : NULL;

            switch (att)
            {
                case A_ID:
                    BIND_PORT(pRegistry, pPort, value);
                    break;
                case A_TEXT:
                    if (enType != CTL_LABEL_TEXT)
                        return;
                    if (lbl != NULL)
                        lbl->set_text(value);
                    break;
                case A_UNITS:
                    if (enType == CTL_LABEL_TEXT)
                        return;
                    if (!strcmp(value, "default"))
                        nUnits      = U_NONE - 1;
                    else
                        nUnits      = decode_unit(value);
                    break;
                case A_FONT_SIZE:
                    if (lbl != NULL)
                        PARSE_FLOAT(value, lbl->font()->set_size(__));
                    break;
                case A_VALIGN:
                    if (lbl != NULL)
                        PARSE_FLOAT(value, lbl->set_valign(__));
                    break;
                case A_HALIGN:
                    if (lbl != NULL)
                        PARSE_FLOAT(value, lbl->set_halign(__));
                    break;
                case A_DETAILED:
                    PARSE_BOOL(value, bDetailed = __);
                    break;
                case A_SAME_LINE:
                    PARSE_BOOL(value, bSameLine = __);
                    break;
                case A_PRECISION:
                    PARSE_INT(value, nPrecision = __);
                    break;
                case A_BORDER:
                    PARSE_INT(value, lbl->set_border(__));
                    break;
                default:
                {
                    bool set = sColor.set(att, value);
                    set |= sBgColor.set(att, value);

                    if (!set)
                        CtlWidget::set(att, value);
                    break;
                }
            }
        }

        void CtlLabel::notify(CtlPort *port)
        {
            CtlWidget::notify(port);

            if (pPort == port)
                commit_value();
        }

        void CtlLabel::commit_value()
        {
            // Get metadata and value
            if (pPort == NULL)
                return;
            const port_t *mdata = pPort->metadata();
            if (mdata == NULL)
                return;
            fValue      = pPort->get_value();

            // Get label widget
            if (pWidget == NULL)
                return;
            LSPLabel *lbl = static_cast<LSPLabel *>(pWidget);

            // Analyze type of the label
            char a_text[TMP_BUF_SIZE];
            a_text[0]           = '\0';
            bool detailed       = bDetailed;

            switch (enType)
            {
                case CTL_LABEL_TEXT:
                    if ((mdata != NULL) && (mdata->name != NULL))
                        lbl->set_text(mdata->name);
                    return;

                case CTL_LABEL_PARAM:
                {
                    // Encode units
                    const char *u_name = NULL;
                    if (nUnits != (U_NONE - 1))
                        u_name  = encode_unit(nUnits);
                    else
                        u_name  = encode_unit((is_decibel_unit(mdata->unit)) ? U_DB : mdata->unit);
                    if (mdata->unit == U_BOOL)
                        detailed = false;

                    // Form the final text
                    const char *text    = mdata->name;
                    if (u_name != NULL)
                    {
                        if (detailed)
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

                    // Update text
                    lbl->set_text(text);
                    break;
                }

                case CTL_LABEL_VALUE:
                {
                    // Encode units
                    const char *u_name = NULL;
                    if (nUnits != (U_NONE - 1))
                        u_name  = encode_unit(nUnits);
                    else
                        u_name  = encode_unit((is_decibel_unit(mdata->unit)) ? U_DB : mdata->unit);
                    if (mdata->unit == U_BOOL)
                        detailed = false;

                    // Form the final text
                    char buf[TMP_BUF_SIZE];
                    format_value(buf, TMP_BUF_SIZE, mdata, fValue, nPrecision);
                    if (detailed)
                        snprintf(a_text, sizeof(a_text), "%s%c%s", buf, (bSameLine) ? ' ' : '\n', (u_name != NULL) ? u_name : "" );
                    else
                        snprintf(a_text, sizeof(a_text), "%s", buf);

                    // Update text
                    lbl->set_text(a_text);
                    break;
                }

                case CTL_STATUS_CODE:
                {
                    status_t code = fValue;
                    const char *text = get_status(code);
                    if (status_is_success(code))
                        init_color(C_STATUS_OK, lbl->font()->color());
                    else if (status_is_preliminary(code))
                        init_color(C_STATUS_WARN, lbl->font()->color());
                    else
                        init_color(C_STATUS_ERROR, lbl->font()->color());
                    lbl->set_text(text);
                    break;
                }

                default:
                    break;
            }
        }

        void CtlLabel::end()
        {
            commit_value();
            CtlWidget::end();
        }

    } /* namespace ctl */
} /* namespace lsp */
