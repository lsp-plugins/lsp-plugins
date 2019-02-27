/*
 * LSPCChunkAccessor.cpp
 *
 *  Created on: 14 янв. 2018 г.
 *      Author: sadko
 */

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <core/debug.h>
#include <core/files/lspc/LSPCChunkAccessor.h>

#if defined(PLATFORM_WINDOWS)
    #define FD_INVALID(fd)      (fd) == INVALID_HANDLE_VALUE
#else
    #define FD_INVALID(fd)      (fd) < 0
#endif /* PLATFORM_WINDOWS */

namespace lsp
{
    status_t LSPCResource::acquire()
    {
        if (FD_INVALID(fd))
            return STATUS_CLOSED;
        ++refs;
        return STATUS_OK;
    }
    
    status_t LSPCResource::release()
    {
        if (FD_INVALID(fd))
            return STATUS_CLOSED;
        if ((--refs) <= 0)
        {
#if defined(PLATFORM_WINDOWS)
            CloseHandle(fd);
            fd      = INVALID_HANDLE_VALUE;
#else
            close(fd);
            fd      = -1;
#endif /* PLATFORM_WINDOWS */
        }

        return STATUS_OK;
    }

    status_t LSPCResource::allocate(uint32_t *id)
    {
        uint32_t cid = chunk_id + 1;
        if (cid == 0)
            return STATUS_OVERFLOW;
        *id = chunk_id = cid;
        return STATUS_OK;
    }

    status_t LSPCResource::write(const void *buf, size_t count)
    {
        if (FD_INVALID(fd))
            return STATUS_CLOSED;

        // Write data at the end of file
        const uint8_t *bptr = static_cast<const uint8_t *>(buf);
        while (count > 0)
        {
#if defined(PLATFORM_WINDOWS)
            DWORD written = 0;
            if (!WriteFile(fd, bptr, count, &written, NULL))
            {
                DWORD error = GetLastError();
                if (error != ERROR_IO_PENDING)
                {
                    lsp_trace("Error write: GetLastError()=%d", int(error));
                    return STATUS_IO_ERROR;
                }
                written = 0;
            }
#else
            errno       = 0;

            ssize_t written  = pwrite(fd, bptr, count, length);
            if (written < ssize_t(count))
            {
                int error = errno;
                if (error != 0)
                {
                    lsp_trace("Error write: errno=%d", error);
                    return STATUS_IO_ERROR;
                }
            }
#endif /* PLATFORM_WINDOWS */

            bptr       += written;
            length     += written;
            count      -= written;
        }

        return STATUS_OK;
    }

    ssize_t LSPCResource::read(wsize_t pos, void *buf, size_t count)
    {
        if (FD_INVALID(fd))
            return -STATUS_CLOSED;

        // Write data at the end of file
        uint8_t *bptr   = static_cast<uint8_t *>(buf);
        ssize_t total   = 0;

#if defined(PLATFORM_WINDOWS)
        LARGE_INTEGER set_pos;
        LARGE_INTEGER seek_pos;

        set_pos.QuadPart    = pos;
        if (!SetFilePointerEx(fd, set_pos, &seek_pos, FILE_BEGIN))
            return -STATUS_IO_ERROR;
        else if (seek_pos.QuadPart != set_pos.QuadPart)
            return -STATUS_IO_ERROR;
#endif /* PLATFORM_WINDOWS */

        while (count > 0)
        {
#if defined(PLATFORM_WINDOWS)
            DWORD read = 0;
            if (!ReadFile(fd, bptr, count, &read, NULL))
            {
                DWORD error = GetLastError();
                if (error != ERROR_IO_PENDING)
                    return -STATUS_IO_ERROR;
                read = 0;
            }
#else
            size_t read = pread(fd, bptr, count, pos);
#endif /* PLATFORM_WINDOWS */
            if (read < count)
                return total;

            bptr       += read;
            count      -= read;
            total      += read;
        }

        return total;
    }

    LSPCChunkAccessor::LSPCChunkAccessor(LSPCResource *fd, uint32_t magic)
    {
        pFile           = fd;
        set_error((pFile != NULL) ? pFile->acquire() : STATUS_BAD_STATE);
        nMagic          = magic;
        nBufSize        = (pFile != NULL) ? pFile->bufsize : 0;

        if (nBufSize > 0)
        {
            // Adjust buffer size
            if (nBufSize < MIN_BUF_SIZE)
                nBufSize        = MIN_BUF_SIZE;

            // Allocate buffer
            pBuffer         = static_cast<uint8_t *>(malloc(nBufSize));
            if (pBuffer == NULL)
            {
                set_error(STATUS_NO_MEM);
                return;
            }
            nBufPos         = 0;
        }

        nUID            = 0;

        set_error(STATUS_OK);
    }
    
    LSPCChunkAccessor::~LSPCChunkAccessor()
    {
        do_close();
    }

    status_t LSPCChunkAccessor::do_close()
    {
        if (pBuffer != NULL)
        {
            free(pBuffer);
            pBuffer = NULL;
        }
        if (pFile == NULL)
            return set_error(STATUS_CLOSED);
        set_error(pFile->release());
        if (pFile->refs <= 0)
            delete pFile;
        pFile = NULL;
        return last_error();
    }

    status_t LSPCChunkAccessor::close()
    {
        return do_close();
    }

} /* namespace lsp */
