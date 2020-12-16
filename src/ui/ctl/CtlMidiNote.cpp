/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 17 дек. 2020 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#include <ui/ctl/ctl.h>

#define TMP_BUF_SIZE        128

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlMidiNote::metadata = { "CtlMidiNote", &CtlWidget::metadata };

        //---------------------------------------------------------------------
        // PopupWindow implementation

        CtlMidiNote::PopupWindow::PopupWindow(CtlMidiNote *label, LSPDisplay *dpy): LSPWindow(dpy),
            sBox(dpy),
            sValue(dpy),
            sUnits(dpy),
            sApply(dpy),
            sCancel(dpy)
        {
            pNote       = label;
        }

        CtlMidiNote::PopupWindow::~PopupWindow()
        {
            pNote       = NULL;
        }

        status_t CtlMidiNote::PopupWindow::init()
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

            this->slots()->bind(LSPSLOT_MOUSE_DOWN, CtlMidiNote::slot_mouse_button, pNote);
            this->slots()->bind(LSPSLOT_MOUSE_UP, CtlMidiNote::slot_mouse_button, pNote);

            sValue.slots()->bind(LSPSLOT_KEY_UP, CtlMidiNote::slot_key_up, pNote);
            sValue.slots()->bind(LSPSLOT_CHANGE, CtlMidiNote::slot_change_value, pNote);
            sValue.set_min_width(64);

            sUnits.padding()->set_left(4);

            sApply.title()->set("actions.apply");
            sApply.slots()->bind(LSPSLOT_SUBMIT, CtlMidiNote::slot_submit_value, pNote);

            sCancel.title()->set("actions.cancel");
            sCancel.slots()->bind(LSPSLOT_SUBMIT, CtlMidiNote::slot_cancel_value, pNote);

            this->add(&sBox);
            this->set_border(1);
            this->padding()->set(4, 2, 2, 2);

            return STATUS_OK;
        }

        void CtlMidiNote::PopupWindow::destroy()
        {
            sValue.destroy();
            sUnits.destroy();
            sApply.destroy();
            sBox.destroy();

            LSPWindow::destroy();
        }

        //---------------------------------------------------------------------
        // CtlMidiNote implementation

        CtlMidiNote::CtlMidiNote(CtlRegistry *src, LSPIndicator *widget):
            CtlWidget(src, widget)
        {
            pClass          = &metadata;
            nNote           = 0;
            pNote           = NULL;
            pOctave         = NULL;
            pValue          = NULL;
            pPopup          = NULL;
        }

        CtlMidiNote::~CtlMidiNote()
        {
            do_destroy();
        }

        void CtlMidiNote::destroy()
        {
            do_destroy();
            CtlWidget::destroy();
        }

        void CtlMidiNote::do_destroy()
        {
            if (pPopup != NULL)
            {
                pPopup->destroy();
                delete pPopup;
                pPopup = NULL;
            }

            pWidget = NULL;
        }

        void CtlMidiNote::init()
        {
            CtlWidget::init();

            LSPIndicator *ind = widget_cast<LSPIndicator>(pWidget);
            if (ind == NULL)
                return;

            // Initialize color controllers
            sColor.init_hsl(pRegistry, ind, ind->color(), A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);
            sTextColor.init_basic(pRegistry, ind, ind->text_color(), A_TEXT_COLOR);
            ind->slot(LSPSLOT_MOUSE_DBL_CLICK)->bind(slot_dbl_click, this);
            ind->slot(LSPSLOT_MOUSE_SCROLL)->bind(slot_mouse_scroll, this);
        }

        void CtlMidiNote::end()
        {
            if (pValue != NULL)
                notify(pValue);
        }

        void CtlMidiNote::commit_value(float value)
        {
            LSPIndicator *ind = widget_cast<LSPIndicator>(pWidget);
            if (ind == NULL)
                return;

            nNote = value;
            ind->set_value(value);
        }

        bool CtlMidiNote::apply_value(const LSPString *value)
        {
            lsp_trace("Apply text value: %s", value->get_utf8());
            if (pValue == NULL)
                return false;

            const port_t *meta = pValue->metadata();
            if (meta == NULL)
                return false;

            float v;
            status_t res = parse_value(&v, value->get_utf8(), meta);
            if (res != STATUS_OK)
                return res;

            apply_value(v);
            return true;
        }

        void CtlMidiNote::apply_value(ssize_t value)
        {
            lsp_trace("Apply value: %d", int(value));

            value           = lsp_limit(value, 0, 127);
            size_t note     = value % 12;
            size_t octave   = value / 12;

            const port_t *meta;
            if (pNote != NULL)
            {
                if (((meta = pNote->metadata()) != NULL) && (meta->flags & F_LOWER))
                    pNote->set_value(meta->min + note);
                else
                    pNote->set_value(note);
            }

            if (pOctave != NULL)
            {
                if (((meta = pOctave->metadata()) != NULL) && (meta->flags & F_LOWER))
                    pOctave->set_value(meta->min + octave);
                else
                    pOctave->set_value(octave);
            }

            nNote           = value;
            if (pNote != NULL)
                pNote->notify_all();
            if (pOctave != NULL)
                pOctave->notify_all();
        }

        void CtlMidiNote::set(widget_attribute_t att, const char *value)
        {
            LSPIndicator *ind = widget_cast<LSPIndicator>(pWidget);

            switch (att)
            {
                case A_ID:
                    BIND_PORT(pRegistry, pValue, value);
                    break;
                case A_NOTE_ID:
                    BIND_PORT(pRegistry, pNote, value);
                    break;
                case A_OCTAVE_ID:
                    BIND_PORT(pRegistry, pOctave, value);
                    break;
                case A_FORMAT:
                    if (ind != NULL)
                        ind->set_format(value);
                    break;
                case A_VALUE:
                    PARSE_FLOAT(value, commit_value(__));
                    break;
                default:
                {
                    sColor.set(att, value);
                    sTextColor.set(att, value);
                    CtlWidget::set(att, value);
                    break;
                }
            }
        }

        void CtlMidiNote::notify(CtlPort *port)
        {
            CtlWidget::notify(port);
            if (pValue == port)
                commit_value(port->get_value());
        }

        status_t CtlMidiNote::slot_submit_value(LSPWidget *sender, void *ptr, void *data)
        {
            // Get control pointer
            CtlMidiNote *_this = static_cast<CtlMidiNote *>(ptr);
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

        status_t CtlMidiNote::slot_cancel_value(LSPWidget *sender, void *ptr, void *data)
        {
            // Get control pointer
            CtlMidiNote *_this = static_cast<CtlMidiNote *>(ptr);
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

        status_t CtlMidiNote::slot_mouse_scroll(LSPWidget *sender, void *ptr, void *data)
        {
            // Get control pointer
            CtlMidiNote *_this = static_cast<CtlMidiNote *>(ptr);
            if (_this == NULL)
                return STATUS_OK;

            // Should be keyboard event
            ws_event_t *ev = reinterpret_cast<ws_event_t *>(data);
            if ((ev == NULL) || (ev->nType != UIE_MOUSE_SCROLL))
                return STATUS_BAD_ARGUMENTS;

            ssize_t delta = (ev->nCode == MCD_UP) ? -1 : 1; // 1 semitone
            if (ev->nState & MCF_CONTROL)
                delta      *= 12; // 1 octave

            _this->apply_value(_this->nNote + delta);
            return STATUS_OK;
        }

        status_t CtlMidiNote::slot_dbl_click(LSPWidget *sender, void *ptr, void *data)
        {
            // Get control pointer
            CtlMidiNote *_this = static_cast<CtlMidiNote *>(ptr);
            if (_this == NULL)
                return STATUS_OK;

            // Get port metadata
            const port_t *mdata = (_this->pValue != NULL) ? _this->pValue->metadata() : NULL;
            if (mdata == NULL)
                return STATUS_OK;

            // Set-up units
            const char *u_key = unit_lc_key((is_decibel_unit(mdata->unit)) ? U_DB : mdata->unit);
            if ((mdata->unit == U_BOOL) || (mdata->unit == U_ENUM))
                u_key  = NULL;

            // Get label widget
            LSPIndicator *ind = widget_cast<LSPIndicator>(_this->pWidget);
            if (ind == NULL)
                return STATUS_OK;

            // Create popup window if required
            PopupWindow *popup  = _this->pPopup;
            if (popup == NULL)
            {
                popup           = new PopupWindow(_this, ind->display());
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

            LSPWindow *parent = widget_cast<LSPWindow>(ind->toplevel());
            if (parent != NULL)
                parent->get_absolute_geometry(&r);

            // Set-up value
            char buf[TMP_BUF_SIZE];
            format_value(buf, TMP_BUF_SIZE, mdata, _this->nNote, 0);
            popup->sValue.set_text(buf);
            popup->sValue.selection()->set_all();

            if (u_key != NULL)
            {
                if (popup->sUnits.text()->set(u_key) != STATUS_OK)
                    u_key = NULL;
            }

            popup->sUnits.set_visible(u_key != NULL);

            popup->move(r.nLeft + ind->left(), r.nTop + ind->top());
            popup->show(ind);
            popup->grab_events(GRAB_NORMAL);

            // Set focus
            popup->sValue.take_focus();

            return STATUS_OK;
        }

        status_t CtlMidiNote::slot_mouse_button(LSPWidget *sender, void *ptr, void *data)
        {
            // Get control pointer
            CtlMidiNote *_this = static_cast<CtlMidiNote *>(ptr);
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

        status_t CtlMidiNote::slot_key_up(LSPWidget *sender, void *ptr, void *data)
        {
            // Get control pointer
            CtlMidiNote *_this = static_cast<CtlMidiNote *>(ptr);
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

        status_t CtlMidiNote::slot_change_value(LSPWidget *sender, void *ptr, void *data)
        {
            // Get control pointer
            CtlMidiNote *_this = static_cast<CtlMidiNote *>(ptr);
            if ((_this == NULL) || (_this->pPopup == NULL))
                return STATUS_OK;

            // Get port metadata
            const port_t *meta = (_this->pValue != NULL) ? _this->pValue->metadata() : NULL;
            if (meta == NULL)
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
