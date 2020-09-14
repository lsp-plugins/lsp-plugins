/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 10 июл. 2017 г.
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
        const ctl_class_t CtlLed::metadata = { "CtlLed", &CtlWidget::metadata };

        CtlLed::CtlLed(CtlRegistry *src, LSPLed *widget): CtlWidget(src, widget)
        {
            pClass          = &metadata;
            fValue          = 0;
            pPort           = NULL;
            fKey            = 1;

            bActivitySet    = false;
            bInvert         = false;
        }

        CtlLed::~CtlLed()
        {
            destroy();
        }

        void CtlLed::init()
        {
            CtlWidget::init();

            if (pWidget == NULL)
                return;

            LSPLed *led = static_cast<LSPLed *>(pWidget);

            // Initialize color controllers
            sColor.init_hsl(pRegistry, led, led->color(), A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);

            // Initialize activity
            sActivity.init(pRegistry, NULL);
        }

        void CtlLed::set(widget_attribute_t att, const char *value)
        {
            LSPLed *led = (pWidget != NULL) ? static_cast<LSPLed *>(pWidget) : NULL;

            switch (att)
            {
                case A_ID:
                    BIND_PORT(pRegistry, pPort, value);
                    break;
                case A_KEY:
                    PARSE_FLOAT(value, fKey = __);
                    break;
                case A_VALUE:
                    PARSE_FLOAT(value, fValue = __);
                    break;
                case A_SIZE:
                    if (led != NULL)
                        PARSE_INT(value, led->set_size(size_t(__)));
                    break;
                case A_ACTIVITY:
                    BIND_EXPR(sVisibility, value);
                    bActivitySet = true;
                    break;
                case A_INVERT:
                    PARSE_BOOL(value, bInvert = __);
                    break;
                default:
                {
                    sColor.set(att, value);
                    CtlWidget::set(att, value);
                    break;
                }
            }
        }

        void CtlLed::update_value()
        {
            if (pWidget == NULL)
                return;

            bool on = false;
            if ((bActivitySet) && (sActivity.valid()))
            {
                float value = sActivity.evaluate();
                on = value >= 0.5f;
            }
            else if (pPort != NULL)
            {
                float value = pPort->get_value();
                on = value >= 0.5f;
            }
            else
                on = abs(fValue - fKey) <= CMP_TOLERANCE;

            // Update lighting
            LSPLed *led     = widget_cast<LSPLed>(pWidget);
            if (led != NULL)
                led->set_on(on ^ bInvert);
        }

        void CtlLed::notify(CtlPort *port)
        {
            CtlWidget::notify(port);

            if (sActivity.valid())
                sActivity.notify(port);

            update_value();
       }

        void CtlLed::end()
        {
            CtlWidget::end();
            update_value();
        }

        void CtlLed::destroy()
        {
            sActivity.destroy();
            CtlWidget::destroy();
        }

    } /* namespace ctl */
} /* namespace lsp */
