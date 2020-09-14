/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 7 июл. 2017 г.
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
        const ctl_class_t CtlIndicator::metadata = { "CtlIndicator", &CtlWidget::metadata };
        
        CtlIndicator::CtlIndicator(CtlRegistry *src, LSPIndicator *widget): CtlWidget(src, widget)
        {
            pClass          = &metadata;
            fValue          = 0;
            pPort           = NULL;
        }
        
        CtlIndicator::~CtlIndicator()
        {
        }

        void CtlIndicator::init()
        {
            CtlWidget::init();

            if (pWidget == NULL)
                return;

            LSPIndicator *ind = static_cast<LSPIndicator *>(pWidget);

            // Initialize color controllers
            sColor.init_hsl(pRegistry, ind, ind->color(), A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);
            sTextColor.init_basic(pRegistry, ind, ind->text_color(), A_TEXT_COLOR);
        }

        void CtlIndicator::end()
        {
            if (pPort != NULL)
                notify(pPort);
        }

        void CtlIndicator::commit_value(float value)
        {
//            lsp_trace("commit value=%f", value);
            if (pWidget == NULL)
                return;
            LSPIndicator *ind = static_cast<LSPIndicator *>(pWidget);

            if (pPort != NULL)
            {
                const port_t *meta = pPort->metadata();
                if (meta != NULL)
                {
                    if (meta->unit == U_GAIN_AMP)
                        value = 20.0 * logf(value) / M_LN10;
                    else if (meta->unit == U_GAIN_POW)
                        value = 10.0 * logf(value) / M_LN10;
                }
            }

            ind->set_value(value);
        }

        void CtlIndicator::set(widget_attribute_t att, const char *value)
        {
            LSPIndicator *ind = (pWidget != NULL) ? static_cast<LSPIndicator *>(pWidget) : NULL;

            switch (att)
            {
                case A_ID:
                    BIND_PORT(pRegistry, pPort, value);
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

        void CtlIndicator::notify(CtlPort *port)
        {
            CtlWidget::notify(port);

            if (pPort == port)
                commit_value(pPort->get_value());
        }
    
    } /* namespace ctl */
} /* namespace lsp */
