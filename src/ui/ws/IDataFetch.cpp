/*
 * IDataFetch.cpp
 *
 *  Created on: 16 авг. 2019 г.
 *      Author: sadko
 */

#include <ui/ws/ws.h>

namespace lsp
{
    namespace ws
    {
        
        IDataFetch::IDataFetch(const char *mime)
        {
        }
        
        IDataFetch::~IDataFetch()
        {
        }

        ssize_t IDataFetch::read(void *buf, size_t count)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t IDataFetch::abort()
        {
            return STATUS_OK;
        }

        status_t IDataFetch::close()
        {
            return STATUS_OK;
        }
    
    } /* namespace ws */
} /* namespace lsp */
