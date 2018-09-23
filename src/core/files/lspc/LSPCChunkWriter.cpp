/*
 * LSPChunkWriter.cpp
 *
 *  Created on: 14 янв. 2018 г.
 *      Author: sadko
 */

#include <dsp/endian.h>
#include <core/files/lspc/LSPCChunkWriter.h>

namespace lsp
{
    
    LSPCChunkWriter::LSPCChunkWriter(LSPCResource *fd, uint32_t magic):
        LSPCChunkAccessor(fd, magic)
    {
        if (last_error() != STATUS_OK)
            return;
        nChunksOut      = 0;
        if (pFile != NULL)
            set_error(pFile->allocate(&nUID));
    }
    
    LSPCChunkWriter::~LSPCChunkWriter()
    {
    }

    status_t LSPCChunkWriter::do_flush(bool force)
    {
        if (pFile == NULL)
            return set_error(STATUS_CLOSED);

        if ((nBufPos > 0) || ((force) && (nChunksOut <= 0)))
        {
            lspc_chunk_header_t hdr;
            hdr.magic       = nMagic;
            hdr.size        = nBufPos;
            hdr.uid         = nUID;

            // Convert CPU -> BE
            hdr.size        = CPU_TO_BE(hdr.size);
            hdr.uid         = CPU_TO_BE(hdr.uid);

            // Write buffer header and data to file
            status_t res    = pFile->write(&hdr, sizeof(lspc_chunk_header_t));
            if (res == STATUS_OK)
                pFile->write(pBuffer, nBufPos);
            if (set_error(res) != STATUS_OK)
                return res;

            // Flush the buffer
            nBufPos         = 0;
            nChunksOut      ++;
        }

        return STATUS_OK;
    }

    status_t LSPCChunkWriter::write(const void *buf, size_t count)
    {
        if (pFile == NULL)
            return set_error(STATUS_CLOSED);

        lspc_chunk_header_t hdr;
        const uint8_t *src = static_cast<const uint8_t *>(buf);

        while (count > 0)
        {
            size_t can_write    = nBufSize - nBufPos;

            if ((nBufPos > 0) || (count < can_write))
            {
                if (can_write > count)
                    can_write       = count;

                // Copy data to buffer
                memcpy(&pBuffer[nBufPos], src, can_write);
                nBufPos        += can_write;
                count          -= can_write;
                src            += can_write;

                // Check buffer size
                if (nBufPos >= nBufSize)
                {
                    hdr.magic       = nMagic;
                    hdr.size        = nBufSize;
                    hdr.uid         = nUID;

                    // Convert CPU -> BE
                    hdr.size        = CPU_TO_BE(hdr.size);
                    hdr.uid         = CPU_TO_BE(hdr.uid);

                    // Write buffer header and data to file
                    status_t res    = pFile->write(&hdr, sizeof(lspc_chunk_header_t));
                    if (res == STATUS_OK)
                        res             = pFile->write(pBuffer, nBufSize);
                    if (set_error(res) != STATUS_OK)
                        return res;

                    // Update position and counter
                    nBufPos         = 0;
                    nChunksOut     ++;
                }
            }
            else // Write directly avoiding buffer
            {
                hdr.magic       = nMagic;
                hdr.size        = nBufSize;
                hdr.uid         = nUID;

                // Convert CPU -> BE
                hdr.size        = CPU_TO_BE(hdr.size);
                hdr.uid         = CPU_TO_BE(hdr.uid);

                // Write buffer header and data to file
                status_t res    = pFile->write(&hdr, sizeof(lspc_chunk_header_t));
                if (res == STATUS_OK)
                    pFile->write(src, nBufSize);
                if (set_error(res) != STATUS_OK)
                    return res;

                // Update position and counter
                count          -= nBufSize;
                src            += nBufSize;
                nChunksOut     ++;
            }
        }

        return set_error(STATUS_OK);
    }

    status_t LSPCChunkWriter::write_header(const void *buf)
    {
        if (pFile == NULL)
            return set_error(STATUS_CLOSED);

        const lspc_header_t *phdr = reinterpret_cast<const lspc_header_t *>(buf);
        if (phdr->size < sizeof(lspc_header_t))
            return set_error(STATUS_BAD_ARGUMENTS);

        // Write encoded header
        lspc_header_t shdr;
        shdr.size           = CPU_TO_BE(phdr->size);
        shdr.version        = CPU_TO_BE(phdr->version);
        status_t res        = write(&shdr, sizeof(shdr));
        if (res != STATUS_OK)
            return res;

        // Write header data
        return write(&phdr[1], phdr->size - sizeof(lspc_header_t));
    }

    status_t LSPCChunkWriter::flush()
    {
        return do_flush(false);
    }

    status_t LSPCChunkWriter::close()
    {
        status_t result = do_flush(true);
        status_t result2 = LSPCChunkAccessor::close();
        return set_error((result == STATUS_OK) ? result2 : result);
    }

} /* namespace lsp */
