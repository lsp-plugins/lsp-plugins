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
        
        InMemoryStream::InMemoryStream()
        {
            pData   = NULL;
            nOffset = 0;
            nSize   = 0;
            enDrop  = MEMDROP_NONE;
        }

        InMemoryStream::InMemoryStream(void *data, size_t size, lsp_memdrop_t drop)
        {
            pData   = reinterpret_cast<uint8_t *>(data);
            nOffset = 0;
            nSize   = size;
            enDrop  = drop;
        }
        
        InMemoryStream::InMemoryStream(const void *data, size_t size)
        {
            pData   = reinterpret_cast<uint8_t *>(const_cast<void *>(data));
            nOffset = 0;
            nSize   = size;
            enDrop  = MEMDROP_NONE;
        }

        InMemoryStream::~InMemoryStream()
        {
            drop();
        }

        void InMemoryStream::wrap(void *data, size_t size, lsp_memdrop_t drop)
        {
            InMemoryStream::drop();
            pData   = reinterpret_cast<uint8_t *>(data);
            nOffset = 0;
            nSize   = size;
            enDrop  = drop;
        }

        void InMemoryStream::wrap(const void *data, size_t size)
        {
            InMemoryStream::drop();
            pData   = reinterpret_cast<uint8_t *>(const_cast<void *>(data));
            nOffset = 0;
            nSize   = size;
            enDrop  = MEMDROP_NONE;
        }

        bool InMemoryStream::drop(lsp_memdrop_t drop)
        {
            if (pData == NULL)
                return false;
            switch (drop)
            {
                case MEMDROP_FREE: ::free(pData); break;
                case MEMDROP_DELETE: delete pData; break;
                case MEMDROP_ARR_DELETE: delete [] pData; break;
                default: break;
            }

            pData       = NULL;
            nOffset     = 0;
            nSize       = 0;
            enDrop      = MEMDROP_NONE;

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

        wssize_t InMemoryStream::skip(wsize_t amount)
        {
            if (pData == NULL)
                return -set_error(STATUS_NO_DATA);
            size_t avail = nSize - nOffset;
            if (avail > amount)
                avail       = amount;
            nOffset    += avail;
            return avail;
        }

        status_t InMemoryStream::close()
        {
            drop(enDrop);
            return STATUS_OK;
        }
    
    } /* namespace io */
} /* namespace lsp */
