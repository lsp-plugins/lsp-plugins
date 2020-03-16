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

            sApply.title()->set("actions.apply");
            sApply.slots()->bind(LSPSLOT_SUBMIT, CtlLabel::slot_submit_value, pLabel);

            sCancel.title()->set("actions.cancel");
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

        void CtlLabel::Listener::notify(ui_atom_t property)
        {
            if (pLabel == NULL)
                return;
            if (property == pLabel->nAtomID)
                pLabel->commit_value();
        }

        //---------------------------------------------------------------------
        // CtlLabel implementation

        CtlLabel::CtlLabel(CtlRegistry *src, LSPLabel *widget, ctl_label_type_t type):
            CtlWidget(src, widget),
            sListener(this)
        {
            pClass          = &metadata;

            pPort           = NULL;
            enType          = type;
            fValue          = 0.0f;
            bDetailed       = true;
            bSameLine       = false;
            nUnits          = U_NONE - 1;
            nPrecision      = -1;
            nAtomID         = -1;
            pPopup          = NULL;
        }

        CtlLabel::~CtlLabel()
        {
            do_destroy();
        }

        void CtlLabel::destroy()
        {
            do_destroy();
        }

        void CtlLabel::do_destroy()
        {
            sListener.pLabel = NULL;

            LSPLabel *lbl = widget_cast<LSPLabel>(pWidget);
            if (lbl == NULL)
                return;

            if (nAtomID >= 0)
            {
                LSPStyle *style = lbl->style();
                style->unbind(nAtomID, &sListener);
                nAtomID = -1;
            }

            pWidget = NULL;
        }

        void CtlLabel::init()
        {
            CtlWidget::init();

            if (pWidget == NULL)
                return;

            LSPLabel *lbl = widget_cast<LSPLabel>(pWidget);
            if (lbl == NULL)
                return;

            LSPStyle *style = lbl->style();
            LSPDisplay *dpy = lbl->display();
            nAtomID = dpy->atom_id("language");
            if (nAtomID >= 0)
                style->bind(nAtomID, PT_STRING, &sListener);

            // Initialize color controllers
            sColor.init_hsl(pRegistry, lbl, lbl->font()->color(), A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);
            lbl->slot(LSPSLOT_MOUSE_DBL_CLICK)->bind(slot_dbl_click, this);
        }

        void CtlLabel::set(const char *name, const char *value)
        {
            LSPLabel *lbl = widget_cast<LSPLabel>(pWidget);
            if ((lbl != NULL) && (enType == CTL_LABEL_TEXT))
                set_lc_attr(A_TEXT, lbl->text(), name, value);

            CtlWidget::set(name, value);
        }

        void CtlLabel::set(widget_attribute_t att, const char *value)
        {
            LSPLabel *lbl = widget_cast<LSPLabel>(pWidget);

            switch (att)
            {
                case A_ID:
                    BIND_PORT(pRegistry, pPort, value);
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
            bool detailed       = bDetailed;

            switch (enType)
            {
                case CTL_LABEL_TEXT:
                    if ((mdata != NULL) && (mdata->name != NULL))
                        lbl->text()->set_raw(mdata->name);
                    return;

                case CTL_LABEL_PARAM:
                {
                    // Encode units
                    LSPLocalString sunit;
                    if (nUnits != (U_NONE - 1))
                        sunit.set(unit_lc_key(nUnits));
                    else
                        sunit.set(unit_lc_key((is_decibel_unit(mdata->unit)) ? U_DB : mdata->unit));
                    if (mdata->unit == U_BOOL)
                        detailed = false;

                    // Form the final text
                    LSPString text, funit;
                    calc::Parameters params;

                    if (mdata->name != NULL)
                        text.set_utf8(mdata->name);
                    sunit.format(&funit, lbl);


                    if ((detailed) && (funit.length() > 0))
                    {
                        if (text.length() > 0)
                            text.append_ascii(" (");
                        else
                            text.append('(');
                        text.append(&funit);
                        text.append(')');
                    }

                    // Update text
                    const char *key = "labels.values.desc_name";
                    if ((detailed) && (funit.length() > 0))
                    {
                        if (text.length() > 0)
                            key = (bSameLine) ? "labels.values.desc_single_line" : "labels.values.desc_multi_line";
                        else
                            key = "labels.values.desc_unit";
                    }

                    params.add_string("name", &text);
                    params.add_string("unit", &funit);

                    lbl->text()->set(key, &params);
                    break;
                }

                case CTL_LABEL_VALUE:
                {
                    // Encode units
                    LSPLocalString sunit;
                    if (nUnits != (U_NONE - 1))
                        sunit.set(unit_lc_key(nUnits));
                    else
                        sunit.set(unit_lc_key((is_decibel_unit(mdata->unit)) ? U_DB : mdata->unit));

                    // Format the value
                    char buf[TMP_BUF_SIZE];
                    calc::Parameters params;
                    LSPString text, funit;

                    format_value(buf, TMP_BUF_SIZE, mdata, fValue, nPrecision);
                    text.set_ascii(buf);
                    sunit.format(&funit, lbl);
                    if (mdata->unit == U_BOOL)
                    {
                        text.prepend_ascii("labels.bool.");
                        sunit.set(&text);
                        sunit.format(&text, lbl);
                        detailed = false;
                    }

                    // Update text
                    const char *key = "labels.values.fmt_value";
                    if ((detailed) && (funit.length() > 0))
                        key = (bSameLine) ? "labels.values.fmt_single_line" : "labels.values.fmt_multi_line";

                    params.add_string("value", &text);
                    params.add_string("unit", &funit);

                    lbl->text()->set(key, &params);
                    break;
                }

                case CTL_STATUS_CODE:
                {
                    status_t code = fValue;
                    const char *text = get_status_lc_key(code);
                    if (status_is_success(code))
                        init_color(C_STATUS_OK, lbl->font()->color());
                    else if (status_is_preliminary(code))
                        init_color(C_STATUS_WARN, lbl->font()->color());
                    else
                        init_color(C_STATUS_ERROR, lbl->font()->color());
                    lbl->text()->set(text);
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
            const char *u_key = NULL;
            if (_this->nUnits != (U_NONE - 1))
                u_key  = unit_lc_key(_this->nUnits);
            else
                u_key  = unit_lc_key((is_decibel_unit(mdata->unit)) ? U_DB : mdata->unit);
            if ((mdata->unit == U_BOOL) || (mdata->unit == U_ENUM))
                u_key  = NULL;

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

            if (u_key != NULL)
            {
                if (popup->sUnits.text()->set(u_key) != STATUS_OK)
                    u_key = NULL;
            }

            popup->sUnits.set_visible(u_key != NULL);

            popup->move(r.nLeft + lbl->left(), r.nTop + lbl->top());
            popup->show(lbl);
            popup->grab_events(GRAB_NORMAL);

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
