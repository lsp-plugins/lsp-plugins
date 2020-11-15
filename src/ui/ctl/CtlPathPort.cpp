/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 13 нояб. 2017 г.
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
        CtlPathPort::CtlPathPort(const port_t *meta, plugin_ui *ui): CtlPort(meta)
        {
            sPath[0]    = '\0';
            pUI         = ui;
        }

        CtlPathPort::~CtlPathPort()
        {
            sPath[0]    = '\0';
            pUI         = NULL;
        }

        void CtlPathPort::write(const void* buffer, size_t size)
        {
            // Check that attribute didn't change
            if ((size == strlen(sPath)) && (memcmp(sPath, buffer, size) == 0))
                return;

            if ((buffer != NULL) && (size > 0))
            {
                size_t copy     = (size >= PATH_MAX) ? PATH_MAX-1 : size;
                memcpy(sPath, buffer, size);
                sPath[copy]     = '\0';
            }
            else
                sPath[0]        = '\0';

            // Save config (if possible)
            if (pUI != NULL)
                pUI->save_global_config();
        }

        void *CtlPathPort::get_buffer()
        {
            return sPath;
        }
    
    } /* namespace ctl */
} /* namespace lsp */
