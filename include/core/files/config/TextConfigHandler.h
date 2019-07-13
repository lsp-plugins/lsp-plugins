/*
 * TextConfigHandler.h
 *
 *  Created on: 13 июл. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_CONFIG_TEXTCONFIGHANDLER_H_
#define CORE_FILES_CONFIG_TEXTCONFIGHANDLER_H_

#include <core/files/config/flags.h>
#include <core/files/config/IConfigHandler.h>
#include <core/KVTStorage.h>

namespace lsp
{
    namespace config
    {
        class TextConfigHandler: public IConfigHandler
        {
            private:
                TextConfigHandler & operator = (const TextConfigHandler &);

            public:
                explicit TextConfigHandler();
                virtual ~TextConfigHandler();

            public:
                virtual status_t handle_parameter(const char *name, const char *value, size_t flags);

                /**
                 * Callback method for parsing regular parameters
                 * @param name regular parameter name
                 * @param value regular parameter value string representation
                 * @param flags parameter flags
                 * @return status of operation
                 */
                virtual status_t handle_regular_parameter(const char *name, const char *value, size_t flags);

                /**
                 * Callback method for parsing KVT parameters
                 * @param name regular parameter name
                 * @param value KVT parameter value string representation
                 * @param flags parameter flags
                 * @return status of operation
                 */
                virtual status_t handle_kvt_parameter(const char *name, const kvt_param_t *param, size_t flags);
        };
    }

} /* namespace lsp */

#endif /* CORE_FILES_CONFIG_TEXTCONFIGHANDLER_H_ */
