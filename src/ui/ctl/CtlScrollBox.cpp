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
        const ctl_class_t CtlScrollBox::metadata = { "CtlScrollBox", &CtlWidget::metadata };

        CtlScrollBox::CtlScrollBox(CtlRegistry *src, LSPScrollBox *widget, ssize_t orientation): CtlWidget(src, widget)
        {
            pClass          = &metadata;
            nOrientation    = orientation;
        }
        
        CtlScrollBox::~CtlScrollBox()
        {
        }

        void CtlScrollBox::set(widget_attribute_t att, const char *value)
        {
            LSPScrollBox *box     = widget_cast<LSPScrollBox>(pWidget);

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
                case A_MIN_WIDTH:
                    if (box != NULL)
                        PARSE_INT(value, box->constraints()->set_min_width(__));
                    break;
                case A_MIN_HEIGHT:
                    if (box != NULL)
                        PARSE_INT(value, box->constraints()->set_min_height(__));
                    break;
                case A_MAX_WIDTH:
                    if (box != NULL)
                        PARSE_INT(value, box->constraints()->set_max_width(__));
                    break;
                case A_MAX_HEIGHT:
                    if (box != NULL)
                        PARSE_INT(value, box->constraints()->set_max_height(__));
                    break;
                case A_HSCROLL:
                    if (box != NULL)
                        box->set_hscroll(widget_scroll(value));
                    break;
                case A_VSCROLL:
                    if (box != NULL)
                        box->set_vscroll(widget_scroll(value));
                    break;
                default:
                    CtlWidget::set(att, value);
                    break;
            }
        }

        status_t CtlScrollBox::add(CtlWidget *child)
        {
            LSPScrollBox *box     = widget_cast<LSPScrollBox>(pWidget);
            return (box != NULL) ? box->add(child->widget()) : STATUS_BAD_STATE;
        }

    } /* namespace ctl */
} /* namespace lsp */
