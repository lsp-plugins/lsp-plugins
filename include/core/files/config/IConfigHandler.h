/*
 * IParameterHandler.h
 *
 *  Created on: 11 июн. 2018 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_CONFIG_ICONFIGHANDLER_H_
#define CORE_FILES_CONFIG_ICONFIGHANDLER_H_

#include <core/types.h>
#include <core/status.h>
#include <core/LSPString.h>

namespace lsp
{
    namespace config
    {
        class IConfigHandler
        {
            public:
                IConfigHandler();
                virtual ~IConfigHandler();

            public:
                /**
                 * Main interface
                 * @param name parameter name
                 * @param value parameter value
                 * @return status of operation
                 */
                virtual status_t handle_parameter(const LSPString *name, const LSPString *value);

                /**
                 * Legacy interface
                 * @param name parameter name
                 * @param value parameter value
                 * @return status of operation
                 */
                virtual status_t handle_parameter(const char *name, const char *value);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* CORE_FILES_CONFIG_ICONFIGHANDLER_H_ */
