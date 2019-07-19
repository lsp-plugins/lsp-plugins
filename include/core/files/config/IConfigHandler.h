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
#include <core/KVTStorage.h>

namespace lsp
{
    namespace config
    {
        class IConfigHandler
        {
            private:
                IConfigHandler & operator = (const IConfigHandler &);

            public:
                explicit IConfigHandler();
                virtual ~IConfigHandler();

            public:
                /**
                 * Main interface
                 * @param name parameter name
                 * @param value parameter value
                 * @param flags additional parameter flags
                 * @return status of operation
                 */
                virtual status_t handle_parameter(const LSPString *name, const LSPString *value, size_t flags);

                /**
                 * Legacy interface
                 * @param name parameter name
                 * @param value parameter value
                 * @param flags additional parameter flags
                 * @return status of operation
                 */
                virtual status_t handle_parameter(const char *name, const char *value, size_t flags);

                /**
                 * Handle KVT parameter
                 * @param name parameter name
                 * @param param parameter value
                 * @param flags parameter flags
                 * @return status of operation
                 */
                virtual status_t handle_kvt_parameter(const LSPString *name, const kvt_param_t *param, size_t flags);

                /**
                 * Handle KVT parameter
                 * @param name parameter name
                 * @param param parameter value
                 * @param flags parameter flags
                 * @return status of operation
                 */
                virtual status_t handle_kvt_parameter(const char *name, const kvt_param_t *param, size_t flags);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* CORE_FILES_CONFIG_ICONFIGHANDLER_H_ */
