/*
 * LSPCChunkReader.cpp
 *
 *  Created on: 14 янв. 2018 г.
 *      Author: sadko
 */

#include <dsp/endian.h>
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
        bLast       = false;
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
                // There is no chunk after current
                if (bLast)
                {
                    set_error(STATUS_EOF);
                    return total;
                }

                // Read chunk header
                ssize_t n   = pFile->read(nFileOff, &hdr, sizeof(lspc_chunk_header_t));
                if (n < ssize_t(sizeof(lspc_chunk_header_t)))
                {
                    set_error(STATUS_EOF);
                    return total;
                }
                nFileOff   += sizeof(lspc_chunk_header_t);

                hdr.magic       = BE_TO_CPU(hdr.magic);
                hdr.flags       = BE_TO_CPU(hdr.flags);
                hdr.size        = BE_TO_CPU(hdr.size);
                hdr.uid         = BE_TO_CPU(hdr.uid);

                // Validate chunk header
                if ((hdr.magic == nMagic) && (hdr.uid == nUID)) // We've found our chunk, remember unread bytes count
                {
                    bLast           = hdr.flags & LSPC_CHUNK_FLAG_LAST;
                    nUnread         = hdr.size;
                }
                else // Skip this chunk
                    nFileOff       += hdr.size;
            }
        }

        return total;
    }

    ssize_t LSPCChunkReader::read_header(void *hdr, size_t size)
    {
        if (size < sizeof(lspc_header_t))
            return -set_error(STATUS_BAD_ARGUMENTS);

        // Read header data first
        lspc_header_t shdr;
        ssize_t count   = read(&shdr, sizeof(lspc_header_t));
        if (count < 0)
            return count;
        else if (count < ssize_t(sizeof(lspc_header_t)))
            return -set_error(STATUS_EOF); // Unexpected end of file

        // Now read header
        lspc_chunk_raw_header_t *dhdr = reinterpret_cast<lspc_chunk_raw_header_t *>(hdr);
        size_t hdr_size             = BE_TO_CPU(shdr.size);
        if (hdr_size < sizeof(lspc_header_t)) // header size should be at least of sizeof(lspc_header_t)
            return -set_error(STATUS_CORRUPTED_FILE);
        dhdr->common.size           = hdr_size;
        dhdr->common.version        = BE_TO_CPU(shdr.version);
        hdr_size                   -= sizeof(lspc_header_t);
        size                       -= sizeof(lspc_header_t);

        // Read header contents
        ssize_t to_read = (size > hdr_size) ? hdr_size : size;
        count           = read(&dhdr->data, to_read);
        if (count < 0)
            return count;
        else if (count < to_read)
            return -set_error(STATUS_EOF); // Unexpected end of file

        // Analyze size of header
        if (size < hdr_size) // Requested size less than actual header size?
        {
            // We need to skip extra bytes that do not fit into header
            to_read     = hdr_size - size;
            count       = skip(to_read);
            if (count < 0)
                return count;
            else if (count < to_read)
                return -set_error(STATUS_EOF); // Unexpected end of file

            // Patch the header size to be at most of size bytes
            dhdr->common.size           = size + sizeof(lspc_header_t);
        }
        else if (size > hdr_size)
            bzero(&dhdr->data[count], size - hdr_size);

        return dhdr->common.size;
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
                    // Update counters
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
                // There is no chunk after current
                if (bLast)
                {
                    set_error(STATUS_EOF);
                    return total;
                }

                // Read chunk header
                ssize_t n   = pFile->read(nFileOff, &hdr, sizeof(lspc_chunk_header_t));
                if (n < ssize_t(sizeof(lspc_chunk_header_t)))
                {
                    set_error(STATUS_EOF);
                    return 0;
                }
                nFileOff   += sizeof(lspc_chunk_header_t);

                hdr.magic       = BE_TO_CPU(hdr.magic);
                hdr.flags       = BE_TO_CPU(hdr.flags);
                hdr.size        = BE_TO_CPU(hdr.size);
                hdr.uid         = BE_TO_CPU(hdr.uid);

                // Validate chunk header
                if ((hdr.magic == nMagic) && (hdr.uid == nUID)) // We've found our chunk, remember unread bytes count
                {
                    bLast           = hdr.flags & LSPC_CHUNK_FLAG_LAST;
                    nUnread         = hdr.size;
                }
                else // Skip this chunk
                    nFileOff       += hdr.size;
            }
        }

        return total;
    }

} /* namespace lsp */
