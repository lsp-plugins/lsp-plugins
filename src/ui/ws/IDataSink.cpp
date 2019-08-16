/*
 * IDataSink.cpp
 *
 *  Created on: 16 авг. 2019 г.
 *      Author: sadko
 */

#include <ui/ws/ws.h>

namespace lsp
{
    namespace ws
    {
        IDataSink::IDataSink()
        {
        }
        
        IDataSink::~IDataSink()
        {
        }

        status_t IDataSink::open(const char *mime_type)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t IDataSink::write(const void *buf, size_t count)
        {
            return -STATUS_NOT_IMPLEMENTED;
        }

        status_t IDataSink::commit()
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t IDataSink::abort(status_t code)
        {
            return STATUS_NOT_IMPLEMENTED;
        }
    
    } /* namespace ws */
} /* namespace lsp */
