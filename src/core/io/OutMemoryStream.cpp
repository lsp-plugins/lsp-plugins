/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 19 авг. 2019 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#include <core/io/OutMemoryStream.h>

namespace lsp
{
    namespace io
    {
        
        OutMemoryStream::OutMemoryStream()
        {
            pData       = NULL;
            nSize       = 0;
            nCapacity   = 0;
            nQuantity   = 0x1000;
            nPosition   = 0;
        }
        
        OutMemoryStream::OutMemoryStream(size_t quantity)
        {
            pData       = NULL;
            nSize       = 0;
            nCapacity   = 0;
            nQuantity   = quantity;
            nPosition   = 0;
        }

        OutMemoryStream::~OutMemoryStream()
        {
            drop();
        }

        ssize_t OutMemoryStream::write(const void *buf, size_t count)
        {
            size_t sz       = nPosition + count;
            status_t res    = reserve(sz);
            if (res != STATUS_OK)
                return -res;

            // Append data
            ::memcpy(&pData[nPosition], buf, count);
            nPosition   = sz;
            if (nSize < sz)
                nSize       = sz;

            set_error(STATUS_OK);
            return count;
        }

        wssize_t OutMemoryStream::seek(wsize_t position)
        {
            if (position > nSize)
                position    = nSize;
            set_error(STATUS_OK);
            return nPosition = position;
        }

        status_t OutMemoryStream::flush()
        {
            return set_error(STATUS_OK);
        }

        uint8_t *OutMemoryStream::release()
        {
            uint8_t *data   = pData;
            pData           = NULL;
            nSize           = 0;
            nCapacity       = 0;
            nPosition       = 0;

            return data;
        }

        void OutMemoryStream::drop()
        {
            if (pData != NULL)
                ::free(pData);
            pData       = NULL;
            nSize       = 0;
            nCapacity   = 0;
            nPosition   = 0;
        }

        status_t OutMemoryStream::reserve(size_t amount)
        {
            if (amount <= nCapacity)
                return set_error(STATUS_OK);

            size_t ncap = ((amount + nQuantity - 1) / nQuantity) * nQuantity; // Quantify capacity
            uint8_t *p  = reinterpret_cast<uint8_t *>(::realloc(pData, ncap));
            if (p == NULL)
                return set_error(STATUS_NO_MEM);
            pData       = p;
            nCapacity   = ncap;
            return set_error(STATUS_OK);
        }

        status_t OutMemoryStream::close()
        {
            return set_error(STATUS_OK);
        }
    
    } /* namespace io */
} /* namespace lsp */
