/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 23 окт. 2017 г.
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
        CtlPortAlias::CtlPortAlias(CtlRegistry *src): CtlWidget(src, NULL)
        {
            sID     = NULL;
            sAlias  = NULL;
        }

        CtlPortAlias::~CtlPortAlias()
        {
            if (sID != NULL)
            {
                lsp_free(sID);
                sID     = NULL;
            }
            if (sAlias != NULL)
            {
                lsp_free(sAlias);
                sAlias  = NULL;
            }
        }
    
        void CtlPortAlias::set(widget_attribute_t att, const char *value)
        {
            switch (att)
            {
                case A_ID:
                    if (sID != NULL)
                        lsp_free(sID);
                    sID     = lsp_strdup(value);
                    break;
                case A_VALUE:
                    if (sAlias != NULL)
                        lsp_free(sAlias);
                    sAlias  = lsp_strdup(value);
                    break;
                default:
                    CtlWidget::set(att, value);
                    break;
            }
        }
    } /* namespace ctl */
} /* namespace lsp */
