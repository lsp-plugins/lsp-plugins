/*
 * File.cpp
 *
 *  Created on: 6 мар. 2019 г.
 *      Author: sadko
 */

#include <core/io/IFile.h>

namespace lsp
{
    namespace io
    {
        
        IFile::IFile()
        {
            nErrorCode  = STATUS_OK;
        }
        
        IFile::~IFile()
        {
            close();
        }

        ssize_t IFile::read(void *dst, size_t count)
        {
            return -set_error(STATUS_NOT_SUPPORTED);
        }

        ssize_t IFile::pread(wsize_t pos, void *dst, size_t count)
        {
            return -set_error(STATUS_NOT_SUPPORTED);
        }

        status_t IFile::write(const void *src, size_t count)
        {
            return set_error(STATUS_NOT_SUPPORTED);
        }

        status_t IFile::write(wsize_t pos, const void *src, size_t count)
        {
            return set_error(STATUS_NOT_SUPPORTED);
        }

        status_t IFile::seek(wssize_t pos, size_t type)
        {
            return set_error(STATUS_NOT_SUPPORTED);
        }

        wssize_t IFile::position()
        {
            return -set_error(STATUS_NOT_SUPPORTED);
        }

        status_t IFile::flush()
        {
            return set_error(STATUS_NOT_SUPPORTED);
        }

        status_t IFile::close()
        {
            return set_error(STATUS_OK);
        }
    
    } /* namespace io */
} /* namespace lsp */
