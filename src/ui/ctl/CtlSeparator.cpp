/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 9 июл. 2017 г.
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
        const ctl_class_t CtlSeparator::metadata = { "CtlSeparator", &CtlWidget::metadata };
        
        CtlSeparator::CtlSeparator(CtlRegistry *src, LSPSeparator *widget, ssize_t orientation): CtlWidget(src, widget)
        {
            pClass          = &metadata;
            nOrientation    = orientation;
        }
        
        CtlSeparator::~CtlSeparator()
        {
        }

        void CtlSeparator::init()
        {
            CtlWidget::init();

            // Initialize color controllers
            LSPSeparator *sep = widget_cast<LSPSeparator>(pWidget);
            if (sep != NULL)
                sColor.init_hsl(pRegistry, sep, sep->color(), A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);
        }

        void CtlSeparator::set(widget_attribute_t att, const char *value)
        {
            LSPSeparator *sep = widget_cast<LSPSeparator>(pWidget);

            switch (att)
            {
                case A_SIZE:
                    if (sep != NULL)
                        PARSE_INT(value, sep->set_size(__));
                    break;
                case A_WIDTH:
                    if (sep != NULL)
                        PARSE_INT(value, sep->set_line_width(__));
                    break;
                case A_BORDER:
                    if (sep != NULL)
                        PARSE_INT(value, sep->set_border(__));
                    break;
                case A_PADDING:
                    if (sep != NULL)
                        PARSE_INT(value, sep->set_padding(__));
                    break;
                case A_HORIZONTAL:
                    if ((sep != NULL) && (nOrientation < 0))
                        PARSE_BOOL(value, sep->set_horizontal(__));
                    break;
                case A_VERTICAL:
                    if ((sep != NULL) && (nOrientation < 0))
                        PARSE_BOOL(value, sep->set_vertical(__));
                    break;
                default:
                {
                    sColor.set(att, value);
                    CtlWidget::set(att, value);
                    break;
                }
            }
        }

    } /* namespace ctl */
} /* namespace lsp */
