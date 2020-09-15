/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 28 авг. 2019 г.
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

#include <core/files/java/IObjectStreamHandler.h>

namespace lsp
{
    namespace java
    {
        IObjectStreamHandler::IObjectStreamHandler()
        {
        }
        
        IObjectStreamHandler::~IObjectStreamHandler()
        {
        }

        status_t IObjectStreamHandler::handle_start(size_t version)
        {
            return STATUS_OK;
        }

        status_t IObjectStreamHandler::handle_end()
        {
            return STATUS_OK;
        }

        status_t IObjectStreamHandler::close(status_t res)
        {
            return STATUS_OK;
        }

        status_t IObjectStreamHandler::handle_reset()
        {
            return STATUS_OK;
        }

        status_t IObjectStreamHandler::handle_null()
        {
            return STATUS_OK;
        }

        status_t IObjectStreamHandler::handle_block_data(const void *buf, size_t size)
        {
            return STATUS_OK;
        }

        status_t IObjectStreamHandler::handle_end_block_data()
        {
            return STATUS_OK;
        }

        status_t IObjectStreamHandler::handle_string(const LSPString *str, bool unshared)
        {
            return STATUS_OK;
        }

        status_t IObjectStreamHandler::handle_reference(size_t ref_num)
        {
            return STATUS_OK;
        }
    }
} /* namespace lsp */
