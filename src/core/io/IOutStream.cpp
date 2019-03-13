/*
 * IOutputStream.cpp
 *
 *  Created on: 14 нояб. 2017 г.
 *      Author: sadko
 */

#include <core/io/IOutStream.h>
#include <core/status.h>

namespace lsp
{
    namespace io
    {
        
        IOutStream::IOutStream()
        {
            nErrorCode      = STATUS_OK;
        }
        
        IOutStream::~IOutStream()
        {
            nErrorCode      = STATUS_OK;
        }

        wssize_t IOutStream::position()
        {
            return - set_error(STATUS_NOT_IMPLEMENTED);
        }

        ssize_t IOutStream::write(const void *buf, size_t count)
        {
            return - set_error(STATUS_NOT_IMPLEMENTED);
        }

        wssize_t IOutStream::seek(wsize_t position)
        {
            return - set_error(STATUS_NOT_IMPLEMENTED);
        }

        status_t IOutStream::flush()
        {
            return - set_error(STATUS_NOT_IMPLEMENTED);
        }

        status_t IOutStream::close()
        {
            return set_error(STATUS_OK);
        }
    
    } /* namespace ws */
} /* namespace lsp */
