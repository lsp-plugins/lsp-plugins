/*
 * File.cpp
 *
 *  Created on: 6 мар. 2019 г.
 *      Author: sadko
 */

#include <core/io/File.h>

namespace lsp
{
    namespace io
    {
        
        File::File()
        {
            nErrorCode  = STATUS_OK;
        }
        
        File::~File()
        {
            close();
        }

        ssize_t File::read(void *dst, size_t count)
        {
            return -set_error(STATUS_NOT_SUPPORTED);
        }

        ssize_t File::pread(wsize_t pos, void *dst, size_t count)
        {
            return -set_error(STATUS_NOT_SUPPORTED);
        }

        ssize_t File::write(const void *src, size_t count)
        {
            return -set_error(STATUS_NOT_SUPPORTED);
        }

        ssize_t File::pwrite(wsize_t pos, const void *src, size_t count)
        {
            return -set_error(STATUS_NOT_SUPPORTED);
        }

        status_t File::seek(wssize_t pos, size_t type)
        {
            return set_error(STATUS_NOT_SUPPORTED);
        }

        wssize_t File::position()
        {
            return -set_error(STATUS_NOT_SUPPORTED);
        }

        wssize_t File::size()
        {
            return -set_error(STATUS_NOT_SUPPORTED);
        }

        status_t File::truncate(wsize_t length)
        {
            return set_error(STATUS_NOT_SUPPORTED);
        }

        status_t File::flush()
        {
            return set_error(STATUS_NOT_SUPPORTED);
        }

        status_t File::sync()
        {
            return set_error(STATUS_NOT_SUPPORTED);
        }

        status_t File::close()
        {
            return set_error(STATUS_OK);
        }
    
    } /* namespace io */
} /* namespace lsp */
