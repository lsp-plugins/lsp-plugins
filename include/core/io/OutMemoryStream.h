/*
 * OutMemoryStream.h
 *
 *  Created on: 19 авг. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_IO_OUTMEMORYSTREAM_H_
#define CORE_IO_OUTMEMORYSTREAM_H_

#include <core/io/IOutStream.h>

namespace lsp
{
    namespace io
    {
        
        class OutMemoryStream: public IOutStream
        {
            private:
                uint8_t    *pData;
                size_t      nSize;
                size_t      nCapacity;
                size_t      nQuantity;
                size_t      nPosition;

            public:
                explicit OutMemoryStream();
                explicit OutMemoryStream(size_t quantity);
                virtual ~OutMemoryStream();

            public:
                /**
                 * Get current contents of the memory buffer
                 * @return contents of the memory buffer, may be NULL if there is no data
                 */
                const uint8_t  *data() const        { return pData; }

                /**
                 * Get current size of memory buffer
                 * @return size of memory buffer
                 */
                const size_t    size() const        { return nSize; }

                /**
                 * Get capacity of the memory buffer
                 * @return capacity of the memory buffer
                 */
                const size_t    capacity() const    { return nCapacity; }

                /**
                 * Get grow quantity of the memory buffer
                 * @return grow quantity
                 */
                const size_t    quantity() const    { return nQuantity; }

                /**
                 * Release the internal buffer and return it's contents
                 * @return the pointer to data that should be free()'d after use
                 */
                uint8_t        *release();

                /**
                 * Drop internal stream data and reset position
                 */
                void            drop();

                /**
                 * Reserve amount of data
                 * @param amount amount of bytes to reserve
                 * @return status of operation
                 */
                status_t        reserve(size_t amount);

            public:
                virtual ssize_t     write(const void *buf, size_t count);

                virtual wssize_t    seek(wsize_t position);

                virtual status_t    flush();

                virtual status_t    close();
        };
    
    } /* namespace io */
} /* namespace lsp */

#endif /* CORE_IO_OUTMEMORYSTREAM_H_ */
