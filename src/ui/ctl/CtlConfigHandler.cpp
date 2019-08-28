/*
 * CtlConfigHandler.cpp
 *
 *  Created on: 26 июн. 2018 г.
 *      Author: sadko
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
