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
            nError      = STATUS_OK;
        }
        
        IInputStream::~IInputStream()
        {
            nError      = STATUS_OK;
        }
    
        wssize_t IInputStream::avail()
        {
            return - (nError = STATUS_NOT_IMPLEMENTED);
        }

        wssize_t IInputStream::position()
        {
            return - (nError = STATUS_NOT_IMPLEMENTED);
        }

        ssize_t IInputStream::read(void *dst, size_t count)
        {
            return - (nError = STATUS_NOT_IMPLEMENTED);
        }

        wssize_t IInputStream::seek(wsize_t position)
        {
            return - (nError = STATUS_NOT_IMPLEMENTED);
        }

        status_t IInputStream::close()
        {
            return nError      = STATUS_OK;
        }

    } /* namespace ws */
} /* namespace lsp */
