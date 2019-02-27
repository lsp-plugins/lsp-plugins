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

    LSPCResource *LSPCFile::create_resource(lsp_fhandle_t fd)
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
        LSPString fpath;
        if (!fpath.set_utf8(path))
            return STATUS_NO_MEM;
        return open(&fpath);
    }

    status_t LSPCFile::open(const io::Path *path)
    {
        LSPString fpath;
        if (!path->get(&fpath))
            return STATUS_NO_MEM;
        return open(&fpath);
    }

    status_t LSPCFile::open(const LSPString *path)
    {
        lspc_root_header_t hdr;
        if (path == NULL)
            return STATUS_BAD_ARGUMENTS;
        else if (pFile != NULL)
            return STATUS_BAD_STATE;

#if defined(PLATFORM_WINDOWS)
        lsp_fhandle_t fd = CreateFileW(
                path->get_utf16(), GENERIC_READ,
                FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
                NULL, OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
                NULL
            );
        if (fd == INVALID_HANDLE_VALUE)
            return STATUS_IO_ERROR;

        LSPCResource *res   = create_resource(fd);
        if (res == NULL)
        {
            CloseHandle(fd);
            return STATUS_NO_MEM;
        }
#else
        lsp_fhandle_t fd    = ::open(path->get_utf8(), O_RDONLY);
        if (fd < 0)
            return STATUS_IO_ERROR;

        LSPCResource *res   = create_resource(fd);
        if (res == NULL)
        {
            ::close(fd);
            return STATUS_NO_MEM;
        }
#endif /* PLATFORM_WINDOWS */

        ssize_t bytes = res->read(0, &hdr, sizeof(lspc_root_header_t));

        if ((bytes < ssize_t(sizeof(lspc_root_header_t))) ||
            (BE_TO_CPU(hdr.size) < sizeof(lspc_root_header_t)) ||
            (BE_TO_CPU(hdr.magic) != LSPC_ROOT_MAGIC) ||
            (BE_TO_CPU(hdr.version) != 1))
        {
            res->release();
            delete res;
            return STATUS_BAD_FORMAT;
        }

        nHdrSize            = BE_TO_CPU(hdr.size);
        pFile               = res;
        bWrite              = false;

        return STATUS_OK;
    }

    status_t LSPCFile::create(const char *path)
    {
        LSPString fpath;
        if (!fpath.set_utf8(path))
            return STATUS_NO_MEM;
        return create(&fpath);
    }

    status_t LSPCFile::create(const io::Path *path)
    {
        LSPString fpath;
        if (!path->get(&fpath))
            return STATUS_NO_MEM;
        return create(&fpath);
    }

    status_t LSPCFile::create(const LSPString *path)
    {
        if (path == NULL)
            return STATUS_BAD_ARGUMENTS;
        else if (pFile != NULL)
            return STATUS_BAD_STATE;

#if defined(PLATFORM_WINDOWS)
        lsp_fhandle_t fd = CreateFileW(
                path->get_utf16(), GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
                NULL, CREATE_ALWAYS,
                FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
                NULL
            );
        if (fd == INVALID_HANDLE_VALUE)
            return STATUS_IO_ERROR;

        LSPCResource *res   = create_resource(fd);
        if (res == NULL)
        {
            CloseHandle(fd);
            return STATUS_NO_MEM;
        }
#else
        int fd          = ::open(path->get_utf8(), O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (fd < 0)
            return STATUS_IO_ERROR;

        LSPCResource *res   = create_resource(fd);
        if (res == NULL)
        {
            ::close(fd);
            return STATUS_NO_MEM;
        }
#endif /* PLATFORM_WINDOWS */

        lspc_root_header_t hdr;
        memset(&hdr, 0, sizeof(lspc_root_header_t));
        hdr.magic       = LSPC_ROOT_MAGIC;
        hdr.version     = 1;
        hdr.size        = sizeof(hdr);

        hdr.magic       = CPU_TO_BE(hdr.magic);
        hdr.version     = CPU_TO_BE(hdr.version);
        hdr.size        = CPU_TO_BE(hdr.size);

        status_t io_res = res->write(&hdr, sizeof(lspc_root_header_t));
        if (io_res != STATUS_OK)
        {
            res->release();
            delete res;
            return io_res;
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
        if (pFile->refs <= 0)
            delete pFile;
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
