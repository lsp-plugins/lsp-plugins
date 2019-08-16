/*
 * IDataSource.cpp
 *
 *  Created on: 16 авг. 2019 г.
 *      Author: sadko
 */

#include <ui/ws/ws.h>

namespace lsp
{
    namespace ws
    {
        
        IDataSource::IDataSource()
        {
            nReferences     = 0;
        }
        
        IDataSource::~IDataSource()
        {
        }

        size_t IDataSource::mime_types()
        {
            return 0;
        }

        const char *IDataSource::mime_type(size_t id)
        {
            return NULL;
        }

        status_t IDataSource::sink(const char *mime, IDataSink *sink)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

//        status_t IDataSource::fetch(const char *mime, IDataFetch **fetch)
//        {
//            return STATUS_NOT_IMPLEMENTED;
//        }

        status_t IDataSource::abort()
        {
            return STATUS_OK;
        }

        size_t IDataSource::acquire()
        {
            return ++nReferences;
        }

        size_t IDataSource::release()
        {
            ssize_t refs    = --nReferences;
            if (refs <= 0)
                delete this;

            return refs;
        }
    
    } /* namespace ctl */
} /* namespace lsp */
