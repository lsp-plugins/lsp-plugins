/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 11 апр. 2020 г.
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
        const ctl_class_t CtlVoid::metadata = { "CtlVoid", &CtlWidget::metadata };
        
        CtlVoid::CtlVoid(CtlRegistry *src, LSPVoid *widget): CtlWidget(src, widget)
        {
            pClass          = &metadata;
        }
        
        CtlVoid::~CtlVoid()
        {
        }

        void CtlVoid::set(widget_attribute_t att, const char *value)
        {
            LSPVoid *v = widget_cast<LSPVoid>(pWidget);

            switch (att)
            {
                case A_MIN_WIDTH:
                    if (v != NULL)
                        PARSE_INT(value, v->constraints()->set_min_width(__));
                    break;
                case A_MIN_HEIGHT:
                    if (v != NULL)
                        PARSE_INT(value, v->constraints()->set_min_height(__));
                    break;
                case A_MAX_WIDTH:
                    if (v != NULL)
                        PARSE_INT(value, v->constraints()->set_max_width(__));
                    break;
                case A_MAX_HEIGHT:
                    if (v != NULL)
                        PARSE_INT(value, v->constraints()->set_max_height(__));
                    break;
                default:
                {
                    CtlWidget::set(att, value);
                    break;
                }
            }
        }

    } /* namespace ctl */
} /* namespace lsp */
