/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 1 июл. 2017 г.
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

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlSwitch::metadata = { "CtlSwitch", &CtlWidget::metadata };
        
        CtlSwitch::CtlSwitch(CtlRegistry *src, LSPSwitch *widget): CtlWidget(src, widget)
        {
            pClass          = &metadata;
            fValue          = 0;
            pPort           = NULL;
            bInvert         = false;
        }
        
        CtlSwitch::~CtlSwitch()
        {
        }

        status_t CtlSwitch::slot_change(LSPWidget *sender, void *ptr, void *data)
        {
            CtlSwitch *_this    = static_cast<CtlSwitch *>(ptr);
            if (_this != NULL)
                _this->submit_value();
            return STATUS_OK;
        }

        void CtlSwitch::commit_value(float value)
        {
            LSPSwitch *sw   = widget_cast<LSPSwitch>(pWidget);
            if (sw == NULL)
                return;

            const port_t *p = (pPort != NULL) ? pPort->metadata() : NULL;
            float half = ((p != NULL) && (p->unit != U_BOOL)) ? (p->min + p->max) * 0.5f : 0.5f;
            sw->set_down((value >= half) ^ (bInvert));
        }

        void CtlSwitch::submit_value()
        {
            LSPSwitch *sw   = widget_cast<LSPSwitch>(pWidget);
            if (sw == NULL)
                return;

            bool down       = sw->is_down();
            lsp_trace("switch clicked down=%s", (down) ? "true" : "false");

            const port_t *p = (pPort != NULL) ? pPort->metadata() : NULL;
            float min       = ((p != NULL) && (p->unit != U_BOOL)) ? p->min : 0.0f;
            float max       = ((p != NULL) && (p->unit != U_BOOL)) ? p->max : 1.0f;
            float value     = (down ^ bInvert) ? max : min;

            if (pPort != NULL)
            {
                pPort->set_value(value);
                pPort->notify_all();
            }
        }

        void CtlSwitch::init()
        {
            CtlWidget::init();

            LSPSwitch *sw   = widget_cast<LSPSwitch>(pWidget);
            if (sw == NULL)
                return;

            // Initialize color controllers
            sColor.init_hsl(pRegistry, sw, sw->color(), A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);
            sBorderColor.init_basic(pRegistry, sw, sw->border_color(), A_BORDER_COLOR);
            sTextColor.init_basic(pRegistry, sw, sw->text_color(), A_TEXT_COLOR);

            // Bind slots
            sw->slots()->bind(LSPSLOT_CHANGE, slot_change, this);
        }

        void CtlSwitch::set(widget_attribute_t att, const char *value)
        {
            LSPSwitch *sw   = widget_cast<LSPSwitch>(pWidget);

            switch (att)
            {
                case A_ID:
                    BIND_PORT(pRegistry, pPort, value);
                    break;
                case A_SIZE:
                    if (sw != NULL)
                        PARSE_INT(value, sw->set_size(__));
                    break;
                case A_BORDER:
                    if (sw != NULL)
                        PARSE_INT(value, sw->set_border(__));
                    break;
                case A_ANGLE:
                    if (sw != NULL)
                        PARSE_INT(value, sw->set_angle(__ % 4 ));
                    break;
                case A_ASPECT:
                    if (sw != NULL)
                        PARSE_FLOAT(value, sw->set_aspect(__));
                    break;
                case A_INVERT:
                    PARSE_BOOL(value, bInvert = __);
                    break;
                default:
                {
                    sColor.set(att, value);
                    sTextColor.set(att, value);
                    sBorderColor.set(att, value);
                    CtlWidget::set(att, value);
                    break;
                }
            }
        }

        void CtlSwitch::notify(CtlPort *port)
        {
            CtlWidget::notify(port);

            if (port == pPort)
                commit_value(pPort->get_value());
        }

        void CtlSwitch::end()
        {
            if (pPort != NULL)
                commit_value(pPort->get_value());
            else
                commit_value(fValue);

            CtlWidget::end();
        }
    
    } /* namespace ctl */
} /* namespace lsp */
