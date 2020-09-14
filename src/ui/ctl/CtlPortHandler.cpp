/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 27 июн. 2018 г.
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
        
        CtlPortHandler::CtlPortHandler()
        {
            nPortID     = 0;
        }
        
        CtlPortHandler::~CtlPortHandler()
        {
            for (size_t i=0, n = vPorts.size(); i<n; ++i)
            {
                port_ref_t *ref = vPorts.at(i);
                if (ref != NULL)
                    delete ref;
            }
            vPorts.flush();
        }

        status_t CtlPortHandler::add_port(const char *name, CtlPort *port)
        {
            LSPString pname;
            if (!pname.set_native(name))
                return STATUS_NO_MEM;
            return add_port(&pname, port);
        }

        status_t CtlPortHandler::add_port(const LSPString *name, CtlPort *port)
        {
            // Check ort type
            if (port == NULL)
                return STATUS_BAD_ARGUMENTS;
            const port_t *meta = port->metadata();
            if (meta == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (!IS_IN_PORT(meta))
                return STATUS_OK;

            // Add port to list
            port_ref_t *ref = new port_ref_t;
            if (ref == NULL)
                return STATUS_NO_MEM;
            if (!ref->sName.set(name))
            {
                delete ref;
                return STATUS_NO_MEM;
            }

            ref->pPort      = port;
            if (!vPorts.add(ref))
            {
                delete ref;
                return STATUS_NO_MEM;
            }
            return STATUS_OK;
        }

        status_t CtlPortHandler::add_port(CtlPort *port)
        {
            const port_t *meta = port->metadata();
            if (meta == NULL)
                return STATUS_BAD_ARGUMENTS;
            return add_port(meta->name, port);
        }
    
    } /* namespace ctl */
} /* namespace lsp */
