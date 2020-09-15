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
