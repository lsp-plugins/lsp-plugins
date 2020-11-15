/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 26 июн. 2018 г.
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
        CtlConfigHandler::CtlConfigHandler()
        {
            nPortID = 0;
        }
        
        CtlConfigHandler::~CtlConfigHandler()
        {
        }

        status_t CtlConfigHandler::handle_parameter(const LSPString *name, const LSPString *value, size_t flags)
        {
            for (size_t i=0, n = vPorts.size(); i<n; ++i)
            {
                port_ref_t *ref = vPorts.at(i);
                if (ref == NULL)
                    continue;
                if (!ref->sName.equals(name))
                    continue;
                const char *v = value->get_utf8();
                if (v == NULL)
                    return STATUS_NO_MEM;
                LSP_BOOL_ASSERT(set_port_value(ref->pPort, v, 0), STATUS_BAD_ARGUMENTS);
                ref->pPort->notify_all();
            }

            return STATUS_OK;
        }
    
    } /* namespace Ctl */
} /* namespace lsp */
