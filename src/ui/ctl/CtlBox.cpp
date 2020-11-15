/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 28 июн. 2017 г.
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
        const ctl_class_t CtlBox::metadata = { "CtlBox", &CtlWidget::metadata };

        CtlBox::CtlBox(CtlRegistry *src, LSPBox *widget, ssize_t orientation): CtlWidget(src, widget)
        {
            pClass          = &metadata;
            nOrientation    = orientation;
        }
        
        CtlBox::~CtlBox()
        {
        }

        void CtlBox::set(widget_attribute_t att, const char *value)
        {
            LSPBox *box     = widget_cast<LSPBox>(pWidget);

            switch (att)
            {
                case A_HORIZONTAL:
                    if ((box != NULL) && (nOrientation < 0))
                        PARSE_BOOL(value, box->set_horizontal(__));
                    break;
                case A_VERTICAL:
                    if ((box != NULL) && (nOrientation < 0))
                        PARSE_BOOL(value, box->set_vertical(__));
                    break;
                case A_SPACING:
                    if (box != NULL)
                        PARSE_INT(value, box->set_spacing(__));
                    break;
                default:
                    CtlWidget::set(att, value);
                    break;
            }
        }

        status_t CtlBox::add(CtlWidget *child)
        {
            LSPBox *box     = widget_cast<LSPBox>(pWidget);
            return (box != NULL) ? box->add(child->widget()) : STATUS_BAD_STATE;
        }

    } /* namespace ctl */
} /* namespace lsp */
