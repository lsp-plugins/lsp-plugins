/*
 * LSPCFile.cpp
 *
 *  Created on: 14 янв. 2018 г.
 *      Author: sadko
 */

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include <dsp/endian.h>
#include <core/debug.h>
#include <core/files/lspc/lspc.h>
#include <core/files/LSPCFile.h>

namespace lsp
{
    
    LSPCFile::LSPCFile()
    {
        pFile       = NULL;
        bWrite      = false;
        nHdrSize    = 0;
    }
    
    LSPCFile::~LSPCFile()
    {
        close();
    }

    LSPCResource *LSPCFile::create_resource(int fd)
    {
        LSPCResource *res = new LSPCResource;
        if (res == NULL)
            return NULL;

        res->fd         = fd;
        res->refs       = 1;
        res->bufsize    = 0x10000;
        res->chunk_id   = 0;
        res->length     = 0;

        return res;
    }

    status_t LSPCFile::open(const char *path)
    {
        if (pFile != NULL)
            return STATUS_BAD_STATE;

        int fd          = ::open(path, O_RDONLY);
        if (fd < 0)
            return STATUS_IO_ERROR;

        lspc_root_header_t hdr;
        read(fd, &hdr, sizeof(lspc_root_header_t));
        if (BE_TO_CPU(hdr.magic) != LSPC_ROOT_MAGIC)
        {
            ::close(fd);
            return STATUS_BAD_FORMAT;
        }
        else if (BE_TO_CPU(hdr.version) != 1)
        {
            ::close(fd);
            return STATUS_BAD_FORMAT;
        }
        nHdrSize            = BE_TO_CPU(hdr.size);

        LSPCResource *res   = create_resource(fd);
        if (res == NULL)
        {
            ::close(fd);
            return STATUS_NO_MEM;
        }
        pFile               = res;
        bWrite              = false;

        return STATUS_OK;
    }

    status_t LSPCFile::create(const char *path)
    {
        if (pFile != NULL)
            return STATUS_BAD_STATE;

        int fd          = ::open(path, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (fd < 0)
            return STATUS_IO_ERROR;

        lspc_root_header_t hdr;
        memset(&hdr, 0, sizeof(lspc_root_header_t));
        hdr.magic       = LSPC_ROOT_MAGIC;
        hdr.version     = 1;
        hdr.size        = sizeof(hdr);

        hdr.magic       = CPU_TO_BE(hdr.magic);
        hdr.version     = CPU_TO_BE(hdr.version);
        hdr.size        = CPU_TO_BE(hdr.size);

        write(fd, &hdr, sizeof(lspc_root_header_t));

        LSPCResource *res   = create_resource(fd);
        if (res == NULL)
        {
            ::close(fd);
            return STATUS_NO_MEM;
        }
        res->length         = sizeof(hdr);
        pFile               = res;
        bWrite              = true;

        return STATUS_OK;
    }

    status_t LSPCFile::close()
    {
        if (pFile == NULL)
            return STATUS_BAD_STATE;
        status_t res = pFile->release();
        pFile   = NULL;
        return res;
    }

    LSPCChunkWriter *LSPCFile::write_chunk(uint32_t magic)
    {
        if ((pFile == NULL) || (!bWrite))
            return NULL;

        LSPCChunkWriter *wr = new LSPCChunkWriter(pFile, magic);
        return wr;
    }

    LSPCChunkReader *LSPCFile::read_chunk(uint32_t uid)
    {
        if ((pFile == NULL) || (bWrite))
            return NULL;

        // Find the initial position of the chunk in file
        lspc_chunk_header_t hdr;
        wsize_t pos         = nHdrSize;
        while (true)
        {
            ssize_t res = pFile->read(pos, &hdr, sizeof(lspc_chunk_header_t));
            if (res != sizeof(lspc_chunk_header_t))
                return NULL;
            pos        += sizeof(lspc_chunk_header_t);

            hdr.magic   = BE_TO_CPU(hdr.magic);
            hdr.uid     = BE_TO_CPU(hdr.uid);
            hdr.flags   = BE_TO_CPU(hdr.flags);
            hdr.size    = BE_TO_CPU(hdr.size);

//            lsp_trace("chunk header uid=%x, magic=%x, flags=%x, search_uid=%x, size=%llx",
//                    int(hdr.uid), int(hdr.magic), int(hdr.flags), int(uid), (long long)(hdr.size));
            if (hdr.uid == uid)
                break;
            pos        += hdr.size;
        }

        // Create reader
        LSPCChunkReader *rd = new LSPCChunkReader(pFile, hdr.magic, uid);
        if (rd == NULL)
            return NULL;
        rd->nFileOff        = pos;
        rd->nUnread         = hdr.size;
        return rd;
    }

    LSPCChunkReader *LSPCFile::read_chunk(uint32_t uid, uint32_t magic)
    {
        if ((pFile == NULL) || (bWrite))
            return NULL;

        // Find the initial position of the chunk in file
        lspc_chunk_header_t hdr;
        wsize_t pos         = nHdrSize;
        while (true)
        {
            ssize_t res = pFile->read(pos, &hdr, sizeof(lspc_chunk_header_t));
            if (res != sizeof(lspc_chunk_header_t))
                return NULL;
            pos        += sizeof(lspc_chunk_header_t);

            hdr.magic   = BE_TO_CPU(hdr.magic);
            hdr.uid     = BE_TO_CPU(hdr.uid);
            hdr.flags   = BE_TO_CPU(hdr.flags);
            hdr.size    = BE_TO_CPU(hdr.size);

            if ((hdr.uid == uid) && (hdr.magic == magic))
                break;
            pos        += hdr.size;
        }

        // Create reader
        LSPCChunkReader *rd = new LSPCChunkReader(pFile, hdr.magic, uid);
        if (rd == NULL)
            return NULL;
        rd->nFileOff        = pos;
        rd->nUnread         = hdr.size;
        return rd;
    }

    LSPCChunkReader *LSPCFile::find_chunk(uint32_t magic, uint32_t *id, uint32_t start_id)
    {
        if ((pFile == NULL) || (bWrite))
            return NULL;

        // Find the initial position of the chunk in file
        lspc_chunk_header_t hdr;
        wsize_t pos         = nHdrSize;
        while (true)
        {
            ssize_t res = pFile->read(pos, &hdr, sizeof(lspc_chunk_header_t));
            if (res != sizeof(lspc_chunk_header_t))
                return NULL;
            pos        += sizeof(lspc_chunk_header_t);

            hdr.magic   = BE_TO_CPU(hdr.magic);
            hdr.uid     = BE_TO_CPU(hdr.uid);
            hdr.flags   = BE_TO_CPU(hdr.flags);
            hdr.size    = BE_TO_CPU(hdr.size);

            if ((hdr.uid >= start_id) && (hdr.magic == magic))
                break;
            pos        += hdr.size;
        }

        // Create reader
        LSPCChunkReader *rd = new LSPCChunkReader(pFile, hdr.magic, hdr.uid);
        if (rd == NULL)
            return NULL;
        if (id != NULL)
            *id                 = rd->unique_id();
        rd->nFileOff        = pos;
        rd->nUnread         = hdr.size;
        return rd;
    }

} /* namespace lsp */
