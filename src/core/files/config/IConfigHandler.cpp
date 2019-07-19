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

        status_t IConfigHandler::handle_parameter(const LSPString *name, const LSPString *value, size_t flags)
        {
            return handle_parameter(name->get_utf8(), value->get_utf8(), flags);
        }

        status_t IConfigHandler::handle_parameter(const char *name, const char *value, size_t flags)
        {
            return STATUS_OK;
        }

        status_t IConfigHandler::handle_kvt_parameter(const LSPString *name, const kvt_param_t *param, size_t flags)
        {
            return handle_kvt_parameter(name->get_utf8(), param, flags);
        }

        status_t IConfigHandler::handle_kvt_parameter(const char *name, const kvt_param_t *param, size_t flags)
        {
            return STATUS_OK;
        }
    
    } /* namespace ctl */
} /* namespace lsp */
