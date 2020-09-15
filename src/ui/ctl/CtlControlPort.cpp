/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 11 нояб. 2017 г.
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
#include <ui/ui.h>
#include <ui/plugin_ui.h>

namespace lsp
{
    namespace ctl
    {
        
        CtlControlPort::CtlControlPort(const port_t *meta, plugin_ui *ui): CtlPort(meta)
        {
            fValue  = meta->start;
            pUI     = ui;
        }

        CtlControlPort::~CtlControlPort()
        {
            pUI     = NULL;
        }

        float CtlControlPort::get_value()
        {
            return fValue;
        }

        void CtlControlPort::set_value(float value)
        {
            float v  = limit_value(pMetadata, value);
            if (fValue != v)
            {
                fValue  = v;
                if (pUI != NULL)
                    pUI->save_global_config();
            }
        }
    } /* namespace ctl */
} /* namespace lsp */
