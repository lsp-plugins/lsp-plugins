/*
 * LSPCChunkReader.cpp
 *
 *  Created on: 14 янв. 2018 г.
 *      Author: sadko
 */

#include <core/endian.h>
#include <core/files/lspc/lspc.h>
#include <core/files/lspc/LSPCChunkReader.h>

namespace lsp
{
    
    LSPCChunkReader::LSPCChunkReader(LSPCResource *fd, uint32_t magic, uint32_t uid):
        LSPCChunkAccessor(fd, magic)
    {
        nUnread     = 0;
        nBufTail    = 0;
        nFileOff    = 0;
        nUID        = uid;
    }
    
    LSPCChunkReader::~LSPCChunkReader()
    {
    }

    ssize_t LSPCChunkReader::read(void *buf, size_t count)
    {
        if (pFile == NULL)
            return -set_error(STATUS_CLOSED);

        lspc_chunk_header_t hdr;

        uint8_t *dst        = static_cast<uint8_t *>(buf);
        ssize_t total       = 0;

        while (count > 0)
        {
            size_t to_read = nBufTail - nBufPos;

            if (to_read > 0) // There is data in the buffer?
            {
                if (to_read > count)
                    to_read     = count;

                // Copy memory from buffer
                memcpy(dst, &pBuffer[nBufPos], to_read);

                // Update pointer
                dst        += to_read;
                nBufPos    += to_read;
                count      -= to_read;
                total      += to_read;
            }
            else if (nUnread > 0)
            {
                if (nUnread <= count)
                {
                    // Read data
                    ssize_t n   = pFile->read(nFileOff, dst, nUnread);
                    if (n <= 0)
                        return total;

                    // Update pointer
                    dst        += n;
                    count      -= n;
                    total      += n;
                    nUnread    -= n;
                    nFileOff   += n;
                }
                else // Fill buffer
                {
                    to_read     = (nUnread < nBufSize) ? nUnread : nBufSize;

                    // Read data
                    ssize_t n   = pFile->read(nFileOff, pBuffer, to_read);
                    if (n <= 0)
                        return total;

                    // Update pointer
                    nBufPos     = 0;
                    nBufTail    = n;
                    nFileOff   += n;
                    nUnread    -= n;
                }
            }
            else // Seek for the next valid chunk
            {
                // Read chunk header
                ssize_t n   = pFile->read(nFileOff, &hdr, sizeof(lspc_chunk_header_t));
                if (n < ssize_t(sizeof(lspc_chunk_header_t)))
                {
                    set_error(STATUS_EOF);
                    return 0;
                }
                nFileOff   += sizeof(lspc_chunk_header_t);

                // Validate chunk header
                if ((BE_TO_CPU(hdr.magic) == nMagic) && (BE_TO_CPU(hdr.uid) == nUID)) // We've found our chunk, remember unread bytes count
                    nUnread         = BE_TO_CPU(hdr.size);
                else // Skip this chunk
                    nFileOff       += BE_TO_CPU(hdr.size);
            }
        }

        return total;
    }

    ssize_t LSPCChunkReader::skip(size_t count)
    {
        if (pFile == NULL)
            return -set_error(STATUS_CLOSED);

        lspc_chunk_header_t hdr;

        ssize_t total       = 0;

        while (count > 0)
        {
            size_t to_read = nBufTail - nBufPos;

            if (to_read > 0) // There is data in the buffer?
            {
                if (to_read > count)
                    to_read     = count;

                // Update pointer
                nBufPos    += to_read;
                count      -= to_read;
                total      += to_read;
            }
            else if (nUnread > 0)
            {
                if (nUnread <= count)
                {
                    // Update coutners
                    count      -= nUnread;
                    total      += nUnread;
                    nFileOff   += nUnread;
                    nUnread     = 0;
                }
                else // Fill buffer
                {
                    nUnread    -= count;
                    nFileOff   += count;
                    total      += count;
                    count       = 0;
                }
            }
            else // Seek for the next valid chunk
            {
                // Read chunk header
                ssize_t n   = pFile->read(nFileOff, &hdr, sizeof(lspc_chunk_header_t));
                if (n < ssize_t(sizeof(lspc_chunk_header_t)))
                {
                    set_error(STATUS_EOF);
                    return 0;
                }
                nFileOff   += sizeof(lspc_chunk_header_t);

                // Validate chunk header
                if ((BE_TO_CPU(hdr.magic) == nMagic) && (BE_TO_CPU(hdr.uid) == nUID)) // We've found our chunk, remember unread bytes count
                    nUnread         = BE_TO_CPU(hdr.size);
                else // Skip this chunk
                    nFileOff       += BE_TO_CPU(hdr.size);
            }
        }

        return total;
    }

} /* namespace lsp */
