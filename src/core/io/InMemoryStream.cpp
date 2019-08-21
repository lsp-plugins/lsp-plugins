/*
 * InMemoryStream.cpp
 *
 *  Created on: 21 авг. 2019 г.
 *      Author: sadko
 */

#include <core/io/InMemoryStream.h>

namespace lsp
{
    namespace io
    {
        
        InMemoryStream::InMemoryStream(void *data, size_t size, drop_t drop)
        {
            pData   = reinterpret_cast<uint8_t *>(data);
            nOffset = 0;
            nSize   = size;
            enDrop  = drop;
        }
        
        InMemoryStream::~InMemoryStream()
        {
            drop();
        }

        bool InMemoryStream::drop(drop_t drop)
        {
            if (pData == NULL)
                return false;
            switch (drop)
            {
                case DROP_FREE: ::free(pData); break;
                case DROP_DELETE: delete pData; break;
                case DROP_ARR_DELETE: delete [] pData; break;
                default: break;
            }

            pData       = NULL;
            nOffset     = 0;
            nSize       = 0;
            enDrop      = DROP_NONE;

            return true;
        }

        wssize_t InMemoryStream::avail()
        {
            if (pData == NULL)
                return -set_error(STATUS_NO_DATA);
            return nSize - nOffset;
        }

        wssize_t InMemoryStream::position()
        {
            if (pData == NULL)
                return -set_error(STATUS_NO_DATA);
            return nOffset;
        }

        ssize_t InMemoryStream::read(void *dst, size_t count)
        {
            if (pData == NULL)
                return -set_error(STATUS_NO_DATA);

            size_t avail = nSize - nOffset;
            if (count > avail)
                count = avail;
            if (count <= 0)
                return -set_error(STATUS_EOF);

            ::memcpy(dst, &pData[nOffset], count);
            nOffset    += count;
            return count;
        }

        wssize_t InMemoryStream::seek(wsize_t position)
        {
            if (pData == NULL)
                return -set_error(STATUS_NO_DATA);
            if (position > nSize)
                position = nSize;

            return nOffset = position;
        }

        status_t InMemoryStream::close()
        {
            drop(enDrop);
            return STATUS_OK;
        }
    
    } /* namespace io */
} /* namespace lsp */
