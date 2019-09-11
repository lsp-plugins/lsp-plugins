/*
 * IObjectStreamHandler.cpp
 *
 *  Created on: 28 авг. 2019 г.
 *      Author: sadko
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
