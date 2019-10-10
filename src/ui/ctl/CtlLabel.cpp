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

        //---------------------------------------------------------------------
        // PopupWindow implementation

        CtlLabel::PopupWindow::PopupWindow(CtlLabel *label, LSPDisplay *dpy): LSPWindow(dpy),
            sBox(dpy),
            sValue(dpy),
            sUnits(dpy),
            sApply(dpy),
            sCancel(dpy)
        {
            pLabel      = label;
        }

        CtlLabel::PopupWindow::~PopupWindow()
        {
            pLabel      = NULL;
        }

        status_t CtlLabel::PopupWindow::init()
        {
            // Initialize components
            status_t res = LSPWindow::init();
            if (res == STATUS_OK)
                res = sBox.init();
            if (res == STATUS_OK)
                res = sValue.init();
            if (res == STATUS_OK)
                res = sUnits.init();
            if (res == STATUS_OK)
                res = sApply.init();
            if (res == STATUS_OK)
                res = sCancel.init();

            if (res != STATUS_OK)
                return res;

            sBox.set_horizontal();
            sBox.set_spacing(2);
            sBox.add(&sValue);
            sBox.add(&sUnits);
            sBox.add(&sApply);
            sBox.add(&sCancel);

            this->slots()->bind(LSPSLOT_MOUSE_DOWN, CtlLabel::slot_mouse_button, pLabel);
            this->slots()->bind(LSPSLOT_MOUSE_UP, CtlLabel::slot_mouse_button, pLabel);

            sValue.slots()->bind(LSPSLOT_KEY_UP, CtlLabel::slot_key_up, pLabel);
            sValue.slots()->bind(LSPSLOT_CHANGE, CtlLabel::slot_change_value, pLabel);
            sValue.set_min_width(64);

            sUnits.padding()->set_left(4);

            sApply.set_title("Apply");
            sApply.slots()->bind(LSPSLOT_SUBMIT, CtlLabel::slot_submit_value, pLabel);

            sCancel.set_title("Cancel");
            sCancel.slots()->bind(LSPSLOT_SUBMIT, CtlLabel::slot_cancel_value, pLabel);

            this->add(&sBox);
            this->set_border(1);
            this->padding()->set(4, 2, 2, 2);

            return STATUS_OK;
        }

        void CtlLabel::PopupWindow::destroy()
        {
            sValue.destroy();
            sUnits.destroy();
            sApply.destroy();
            sBox.destroy();

            LSPWindow::destroy();
        }

        //---------------------------------------------------------------------
        // CtlLabel implementation

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
            pPopup          = NULL;
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
            lbl->slot(LSPSLOT_MOUSE_DBL_CLICK)->bind(slot_dbl_click, this);
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
                    sColor.set(att, value);
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
            LSPLabel *lbl = widget_cast<LSPLabel>(pWidget);
            if (lbl == NULL)
                return;

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

        bool CtlLabel::apply_value(const LSPString *value)
        {
            const port_t *meta = (pPort != NULL) ? pPort->metadata() : NULL;
            if ((meta == NULL) || (!IS_IN_PORT(meta)))
                return false;

            float fv;
            status_t res = parse_value(&fv, value->get_utf8(), meta);
            if (res != STATUS_OK)
                return false;

            pPort->set_value(fv);
            pPort->notify_all();
            return true;
        }

        void CtlLabel::end()
        {
            if (pPort != NULL)
                commit_value();
            CtlWidget::end();
        }

        status_t CtlLabel::slot_submit_value(LSPWidget *sender, void *ptr, void *data)
        {
            // Get control pointer
            CtlLabel *_this = static_cast<CtlLabel *>(ptr);
            if ((_this == NULL) || (_this->pPopup == NULL))
                return STATUS_OK;

            // Apply value
            PopupWindow *popup  = _this->pPopup;
            LSPString value;
            if (popup->sValue.get_text(&value) == STATUS_OK)
            {
                // The deploy should be always successful
                if (!_this->apply_value(&value))
                    return STATUS_OK;
            }

            // Hide the popup window
            if (popup != NULL)
            {
                popup->hide();
                if (popup->queue_destroy() == STATUS_OK)
                    _this->pPopup  = NULL;
            }

            return STATUS_OK;
        }

        status_t CtlLabel::slot_cancel_value(LSPWidget *sender, void *ptr, void *data)
        {
            // Get control pointer
            CtlLabel *_this = static_cast<CtlLabel *>(ptr);
            if ((_this == NULL) || (_this->pPopup == NULL))
                return STATUS_OK;

            // Hide the widget and queue for destroy
            PopupWindow *popup  = _this->pPopup;
            if (popup != NULL)
            {
                popup->hide();
                if (popup->queue_destroy() == STATUS_OK)
                    _this->pPopup  = NULL;
            }

            return STATUS_OK;
        }

        status_t CtlLabel::slot_dbl_click(LSPWidget *sender, void *ptr, void *data)
        {
            // Get control pointer
            CtlLabel *_this = static_cast<CtlLabel *>(ptr);
            if ((_this == NULL) || (_this->enType != CTL_LABEL_VALUE))
                return STATUS_OK;

            // Get port metadata
            const port_t *mdata = (_this->pPort != NULL) ? _this->pPort->metadata() : NULL;
            if ((mdata == NULL) || (!IS_IN_PORT(mdata)))
                return STATUS_OK;

            // Set-up units
            const char *u_name = NULL;
            if (_this->nUnits != (U_NONE - 1))
                u_name  = encode_unit(_this->nUnits);
            else
                u_name  = encode_unit((is_decibel_unit(mdata->unit)) ? U_DB : mdata->unit);
            if ((mdata->unit == U_BOOL) || (mdata->unit == U_ENUM))
                u_name  = NULL;

            // Get label widget
            LSPLabel *lbl = widget_cast<LSPLabel>(_this->pWidget);
            if (lbl == NULL)
                return STATUS_OK;

            // Create popup window if required
            PopupWindow *popup  = _this->pPopup;
            if (popup == NULL)
            {
                popup           = new PopupWindow(_this, lbl->display());
                status_t res    = popup->init();
                if (res != STATUS_OK)
                {
                    delete popup;
                    return res;
                }

                popup->set_border_style(BS_POPUP);
                popup->actions()->set_actions(WA_POPUP);

                _this->pPopup   = popup;
            }

            // Get location of popup window
            realize_t r;
            r.nLeft     = 0;
            r.nTop      = 0;
            r.nWidth    = 0;
            r.nHeight   = 0;

            LSPWindow *parent = widget_cast<LSPWindow>(lbl->toplevel());
            if (parent != NULL)
                parent->get_absolute_geometry(&r);

            // Set-up value
            char buf[TMP_BUF_SIZE];
            format_value(buf, TMP_BUF_SIZE, mdata, _this->fValue, _this->nPrecision);
            popup->sValue.set_text(buf);
            popup->sValue.selection()->set_all();

            if (u_name != NULL)
            {
                if (popup->sUnits.set_text(u_name) != STATUS_OK)
                    u_name = NULL;
            }

            popup->sUnits.set_visible(u_name != NULL);

            popup->move(r.nLeft + lbl->left(), r.nTop + lbl->top());
            popup->show(lbl);
            popup->grab_events();

            // Set focus
            popup->sValue.take_focus();

            return STATUS_OK;
        }

        status_t CtlLabel::slot_mouse_button(LSPWidget *sender, void *ptr, void *data)
        {
            // Get control pointer
            CtlLabel *_this = static_cast<CtlLabel *>(ptr);
            if ((_this == NULL) || (_this->pPopup == NULL))
                return STATUS_OK;

            // Get event
            ws_event_t *ev = reinterpret_cast<ws_event_t *>(data);
            if (ev == NULL)
                return STATUS_BAD_ARGUMENTS;

            // Hide popup window without any action
            PopupWindow *popup  = _this->pPopup;
            if (!popup->inside(ev->nLeft, ev->nTop))
            {
                popup->hide();
                if (popup->queue_destroy() == STATUS_OK)
                    _this->pPopup  = NULL;
            }

            return STATUS_OK;
        }

        status_t CtlLabel::slot_key_up(LSPWidget *sender, void *ptr, void *data)
        {
            // Get control pointer
            CtlLabel *_this = static_cast<CtlLabel *>(ptr);
            if ((_this == NULL) || (_this->pPopup == NULL))
                return STATUS_OK;

            // Should be keyboard event
            ws_event_t *ev = reinterpret_cast<ws_event_t *>(data);
            if ((ev == NULL) || (ev->nType != UIE_KEY_UP))
                return STATUS_BAD_ARGUMENTS;

            // Hide popup window
            ws_code_t key = LSPKeyboardHandler::translate_keypad(ev->nCode);

            PopupWindow *popup  = _this->pPopup;
            if (key == WSK_RETURN)
            {
                // Deploy new value
                LSPString value;
                if (popup->sValue.get_text(&value) == STATUS_OK)
                {
                    if (!_this->apply_value(&value))
                        return STATUS_OK;
                }
            }

            if ((key == WSK_RETURN) || (key == WSK_ESCAPE))
            {
                popup->hide();
                if (popup->queue_destroy() == STATUS_OK)
                    _this->pPopup  = NULL;
            }
            return STATUS_OK;
        }

        status_t CtlLabel::slot_change_value(LSPWidget *sender, void *ptr, void *data)
        {
            // Get control pointer
            CtlLabel *_this = static_cast<CtlLabel *>(ptr);
            if ((_this == NULL) || (_this->pPopup == NULL))
                return STATUS_OK;

            // Get port metadata
            const port_t *meta = (_this->pPort != NULL) ? _this->pPort->metadata() : NULL;
            if ((meta == NULL) || (!IS_IN_PORT(meta)))
                return false;

            // Get popup window
            PopupWindow *popup  = _this->pPopup;
            if (popup == NULL)
                return STATUS_OK;

            // Validate input
            LSPString value;
            color_t color = C_RED;
            if (popup->sValue.get_text(&value) == STATUS_OK)
            {
                if (parse_value(NULL, value.get_utf8(), meta) == STATUS_OK)
                    color   = C_BACKGROUND;
            }

            // Update color
            Color cl;
            popup->display()->theme()->get_color(color, &cl);
            popup->sValue.font()->color()->copy(&cl);

            return STATUS_OK;
        }

    } /* namespace ctl */
} /* namespace lsp */
