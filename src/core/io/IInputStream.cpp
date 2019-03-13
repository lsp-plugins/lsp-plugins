/*
 * IClipStream.cpp
 *
 *  Created on: 14 нояб. 2017 г.
 *      Author: sadko
 */

#include <core/status.h>
#include <core/io/IInputStream.h>

namespace lsp
{
    namespace io
    {
        
        IInputStream::IInputStream()
        {
            nErrorCode      = STATUS_OK;
        }
        
        IInputStream::~IInputStream()
        {
            nErrorCode      = STATUS_OK;
        }
    
        wssize_t IInputStream::avail()
        {
            return - set_error(STATUS_NOT_IMPLEMENTED);
        }

        wssize_t IInputStream::position()
        {
            return - set_error(STATUS_NOT_IMPLEMENTED);
        }

        ssize_t IInputStream::read(void *dst, size_t count)
        {
            return - set_error(STATUS_NOT_IMPLEMENTED);
        }

        wssize_t IInputStream::seek(wsize_t position)
        {
            return - set_error(STATUS_NOT_IMPLEMENTED);
        }

        status_t IInputStream::close()
        {
            return set_error(nErrorCode);
        }

    } /* namespace ws */
} /* namespace lsp */
