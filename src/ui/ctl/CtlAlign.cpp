/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 17 июл. 2017 г.
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
        const ctl_class_t CtlAlign::metadata = { "CtlAlign", &CtlWidget::metadata };

        CtlAlign::CtlAlign(CtlRegistry *src, LSPAlign *widget): CtlWidget(src, widget)
        {
            pClass          = &metadata;
        }

        CtlAlign::~CtlAlign()
        {
        }

        void CtlAlign::init()
        {
            CtlWidget::init();

            if (pWidget == NULL)
                return;
        }

        void CtlAlign::set(widget_attribute_t att, const char *value)
        {
            LSPAlign *align     = widget_cast<LSPAlign>(pWidget);

            switch (att)
            {
                case A_VPOS:
                    if (align != NULL)
                        PARSE_FLOAT(value, align->set_vpos(__));
                    break;
                case A_HPOS:
                    if (align != NULL)
                        PARSE_FLOAT(value, align->set_hpos(__));
                    break;
                case A_VSCALE:
                    if (align != NULL)
                        PARSE_FLOAT(value, align->set_vscale(__));
                    break;
                case A_HSCALE:
                    if (align != NULL)
                        PARSE_FLOAT(value, align->set_hscale(__));
                    break;

                default:
                {
                    CtlWidget::set(att, value);
                    break;
                }
            }
        }

        status_t CtlAlign::add(CtlWidget *child)
        {
            LSPAlign *align     = widget_cast<LSPAlign>(pWidget);
            if (align == NULL)
                return STATUS_BAD_STATE;
            return align->add(child->widget());
        }
    } /* namespace ctl */
} /* namespace lsp */
