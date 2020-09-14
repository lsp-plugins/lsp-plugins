/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 11 июн. 2018 г.
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
