/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 27 июл. 2017 г.
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
        const ctl_class_t CtlCenter::metadata = { "CtlCenter", &CtlWidget::metadata };

        CtlCenter::CtlCenter(CtlRegistry *src, LSPCenter *cnt): CtlWidget(src, cnt)
        {
            pClass          = &metadata;
        }

        CtlCenter::~CtlCenter()
        {
        }

        void CtlCenter::set(widget_attribute_t att, const char *value)
        {
            LSPCenter *cnt = (pWidget != NULL) ? static_cast<LSPCenter *>(pWidget) : NULL;

            switch (att)
            {
                case A_VPOS:
                    if (cnt != NULL)
                        PARSE_FLOAT(value, cnt->set_canvas_top(__));
                    break;
                case A_HPOS:
                    if (cnt != NULL)
                        PARSE_FLOAT(value, cnt->set_canvas_left(__));
                    break;
                case A_SIZE:
                    if (cnt != NULL)
                        PARSE_FLOAT(value, cnt->set_radius(__));
                    break;
                default:
                {
                    bool set = sColor.set(att, value);
                    if (!set)
                        CtlWidget::set(att, value);
                    break;
                }
            }
        }

        void CtlCenter::init()
        {
            CtlWidget::init();
            if (pWidget == NULL)
                return;

            LSPCenter *cnt  = static_cast<LSPCenter *>(pWidget);

            // Initialize color controllers
            sColor.init_hsl(pRegistry, cnt, cnt->color(), A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);
        }
    } /* namespace ctl */
} /* namespace lsp */
