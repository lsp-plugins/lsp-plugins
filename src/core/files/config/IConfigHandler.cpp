/*
 * IParameterHandler.cpp
 *
 *  Created on: 11 июн. 2018 г.
 *      Author: sadko
 */

#include <core/files/config/IConfigHandler.h>

namespace lsp
{
    namespace config
    {
        IConfigHandler::IConfigHandler()
        {
        }
        
        IConfigHandler::~IConfigHandler()
        {
        }

        status_t IConfigHandler::handle_parameter(const LSPString *name, const LSPString *value)
        {
            return handle_parameter(name->get_utf8(), value->get_utf8());
        }

        status_t IConfigHandler::handle_parameter(const char *name, const char *value)
        {
            return STATUS_OK;
        }
    
    } /* namespace ctl */
} /* namespace lsp */
