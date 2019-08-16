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
            nReferences     = 0;
        }
        
        IDataSink::~IDataSink()
        {
        }

        ssize_t IDataSink::open(const char *const *mime_type)
        {
            return -STATUS_NOT_IMPLEMENTED;
        }

        status_t IDataSink::write(const void *buf, size_t count)
        {
            return -STATUS_NOT_IMPLEMENTED;
        }

        status_t IDataSink::close(status_t code)
        {
            return STATUS_OK;
        }

        size_t IDataSink::acquire()
        {
            return ++nReferences;
        }

        size_t IDataSink::release()
        {
            ssize_t refs    = --nReferences;
            if (refs <= 0)
                delete this;

            return refs;
        }
    
    } /* namespace ws */
} /* namespace lsp */
