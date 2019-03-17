/*
 * IClipStream.cpp
 *
 *  Created on: 14 нояб. 2017 г.
 *      Author: sadko
 */

#include <core/io/IInStream.h>
#include <core/status.h>

namespace lsp
{
    namespace io
    {
        
        IInStream::IInStream()
        {
            nErrorCode      = STATUS_OK;
        }
        
        IInStream::~IInStream()
        {
            nErrorCode      = STATUS_OK;
        }
    
        wssize_t IInStream::avail()
        {
            return - set_error(STATUS_NOT_IMPLEMENTED);
        }

        wssize_t IInStream::position()
        {
            return - set_error(STATUS_NOT_IMPLEMENTED);
        }

        ssize_t IInStream::read(void *dst, size_t count)
        {
            return - set_error(STATUS_NOT_IMPLEMENTED);
        }

        wssize_t IInStream::seek(wsize_t position)
        {
            return - set_error(STATUS_NOT_IMPLEMENTED);
        }

        status_t IInStream::close()
        {
            return set_error(nErrorCode);
        }

    } /* namespace ws */
} /* namespace lsp */
