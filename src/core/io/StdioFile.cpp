/*
 * StdlibFile.cpp
 *
 *  Created on: 6 мар. 2019 г.
 *      Author: sadko
 */

#include <core/io/StdioFile.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

namespace lsp
{
    namespace io
    {
        StdioFile::StdioFile()
        {
            pFD     = NULL;
            nFlags  = 0;
        }
        
        StdioFile::~StdioFile()
        {
            if (pFD != NULL)
            {
                if (nFlags & SF_CLOSE)
                    fclose(pFD);
                pFD     = NULL;
            }
            nFlags  = 0;
        }

        status_t StdioFile::open(const char *path, size_t mode)
        {
            if (path == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

            LSPString tmp;
            if (!tmp.set_utf8(path))
                return set_error(STATUS_NO_MEM);
            return open(&tmp, mode);
        }

        status_t StdioFile::open(const LSPString *path, size_t mode)
        {
            const char *fmode;
            size_t flags;

            // Check state
            if (pFD != NULL)
                return set_error(STATUS_BAD_STATE);

            // Determine mode
            if (mode & FM_READ)
            {
                if (mode & FM_WRITE)
                {
                    flags   = SF_READ | SF_WRITE;
                    if (mode & (FM_CREATE | FM_TRUNC))
                        fmode   = "wb+";
                    else
                        fmode   = "rb+";
                }
                else // "r"
                {
                    flags   = SF_READ;
                    fmode   = "rb";
                }
            }
            else if (mode & FM_WRITE)
            {
                flags   = SF_WRITE;
                if (mode & (FM_CREATE | FM_TRUNC))
                    fmode   = "wb";
                else
                    fmode   = "rb+";
            }
            else
                return set_error(STATUS_INVALID_VALUE);

            // Open file
            FILE *fd    = fopen(path->get_native(), fmode);
            if (fd == NULL)
            {
                int code = errno;
                switch (code)
                {
                    case EINVAL:
                        return set_error(STATUS_BAD_ARGUMENTS);
                    default:
                        return set_error(STATUS_UNKNOWN_ERR);
                }
            }

            // Update state
            pFD     = fd;
            nFlags  = flags | SF_CLOSE;

            return set_error(STATUS_OK);
        }

        status_t StdioFile::open(const Path *path, size_t mode)
        {
            if (path == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);
            return open(path->as_string(), mode);
        }

        status_t StdioFile::wrap(FILE *fd, bool close)
        {
            // Check state
            if (fd == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);
            else if (pFD != NULL)
                return set_error(STATUS_BAD_STATE);

            pFD     = fd;
            nFlags  = (close) ? SF_READ | SF_WRITE | SF_CLOSE : SF_READ | SF_WRITE;
            return set_error(STATUS_OK);
        }

        status_t StdioFile::wrap(FILE *fd, size_t mode, bool close)
        {
            // Check state
            if (fd == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);
            else if (pFD != NULL)
                return set_error(STATUS_BAD_STATE);

            size_t flags = (close) ? SF_CLOSE : 0;
            if (mode & FM_READ)
                flags  |= SF_READ;
            if (mode & FM_WRITE)
                flags  |= SF_WRITE;

            pFD     = fd;
            nFlags  = flags;

            return set_error(STATUS_OK);
        }

        ssize_t StdioFile::read(void *dst, size_t count)
        {
            // Check state
            if (pFD == NULL)
                return -set_error(STATUS_BAD_STATE);
            else if (!(nFlags & SF_READ))
                return -set_error(STATUS_PERMISSION_DENIED);

            // Perform read
            uint8_t *ptr    = reinterpret_cast<uint8_t *>(dst);
            size_t bread    = 0;
            bool eof        = false;

            while (bread < count)
            {
                size_t to_read  = count - bread;
                size_t n_read   = fread(ptr, 1, to_read, pFD);

                if (n_read <= 0)
                {
                    eof = feof(pFD);
                    break;
                }

                ptr    += n_read;
                bread  += n_read;
            }

            if ((bread > 0) || (count <= 0) || (!eof))
            {
                set_error(STATUS_OK);
                return bread;
            }
            return -set_error(STATUS_EOF);
        }

        ssize_t StdioFile::pread(wsize_t pos, void *dst, size_t count)
        {
            // Check state
            if (pFD == NULL)
                return -set_error(STATUS_BAD_STATE);
            else if (!(nFlags & SF_READ))
                return -set_error(STATUS_PERMISSION_DENIED);

            // Store previous position
            wsize_t save = ftello(pFD);
            if (save < 0)
                return -set_error(STATUS_IO_ERROR);

            // Update position
            if (pos != save)
            {
                if (fseeko(pFD, pos, SEEK_SET) != 0)
                    return -set_error(STATUS_IO_ERROR);
            }

            // Perform read
            uint8_t *ptr    = reinterpret_cast<uint8_t *>(dst);
            size_t bread    = 0;
            bool eof        = false;

            while (bread < count)
            {
                size_t to_read  = count - bread;
                size_t n_read   = fread(ptr, 1, to_read, pFD);

                if (n_read <= 0)
                {
                    eof = feof(pFD);
                    break;
                }

                ptr    += n_read;
                bread  += n_read;
            }

            // Restore position
            if (pos != save)
            {
                if (fseeko(pFD, save, SEEK_SET) != 0)
                    return -set_error(STATUS_IO_ERROR);
            }

            if ((bread > 0) || (count <= 0) || (!eof))
            {
                set_error(STATUS_OK);
                return bread;
            }
            return -set_error(STATUS_EOF);
        }

        ssize_t StdioFile::write(const void *src, size_t count)
        {
            // Check state
            if (pFD == NULL)
                return -set_error(STATUS_BAD_STATE);
            else if (!(nFlags & SF_WRITE))
                return -set_error(STATUS_PERMISSION_DENIED);

            // Perform write
            const uint8_t *ptr  = reinterpret_cast<const uint8_t *>(src);
            size_t bwritten     = 0;

            while (bwritten < count)
            {
                size_t to_write = count - bwritten;
                size_t n_written= fwrite(ptr, 1, to_write, pFD);

                if (n_written <= 0)
                    break;

                ptr        += n_written;
                bwritten   += n_written;
            }

            if ((bwritten > 0) || (count <= 0))
            {
                set_error(STATUS_OK);
                return bwritten;
            }
            return  -set_error(STATUS_IO_ERROR);
        }

        ssize_t StdioFile::pwrite(wsize_t pos, const void *src, size_t count)
        {
            // Check state
            if (pFD == NULL)
                return -set_error(STATUS_BAD_STATE);
            else if (!(nFlags & SF_WRITE))
                return -set_error(STATUS_PERMISSION_DENIED);

            // Store previous position
            wsize_t save = ftello(pFD);
            if (save < 0)
                return -set_error(STATUS_IO_ERROR);

            // Update position
            if (pos != save)
            {
                if (fseeko(pFD, pos, SEEK_SET) != 0)
                    return -set_error(STATUS_IO_ERROR);
            }

            // Perform write
            const uint8_t *ptr  = reinterpret_cast<const uint8_t *>(src);
            size_t bwritten     = 0;

            while (bwritten < count)
            {
                size_t to_write = count - bwritten;
                size_t n_written= fwrite(ptr, 1, to_write, pFD);

                if (n_written <= 0)
                    break;

                ptr        += n_written;
                bwritten   += n_written;
            }

            // Restore position
            if (pos != save)
            {
                if (fseeko(pFD, save, SEEK_SET) != 0)
                    return -set_error(STATUS_IO_ERROR);
            }

            if ((bwritten > 0) || (count <= 0))
            {
                set_error(STATUS_OK);
                return bwritten;
            }

            return -set_error(STATUS_IO_ERROR);
        }

        status_t StdioFile::seek(wssize_t pos, size_t type)
        {
            if (pFD == NULL)
                return set_error(STATUS_BAD_STATE);

            int whence;
            switch (type)
            {
                case FSK_SET:  whence = SEEK_SET; break;
                case FSK_CUR:  whence = SEEK_CUR; break;
                case FSK_END:  whence = SEEK_END; break;
                default:
                    return set_error(STATUS_BAD_ARGUMENTS);
            }

            if (::fseeko(pFD, pos, whence) != 0)
            {
                int code = errno;
                if (code == ESPIPE)
                    return set_error(STATUS_NOT_SUPPORTED);
                return set_error(STATUS_IO_ERROR);
            }

            return STATUS_OK;
        }

        wssize_t StdioFile::position()
        {
            if (pFD == NULL)
                return -set_error(STATUS_BAD_STATE);

            wssize_t pos = ftello(pFD);
            if (pos < 0)
                return -set_error(STATUS_IO_ERROR);
            set_error(STATUS_OK);
            return pos;
        }

        wssize_t StdioFile::size()
        {
            if (pFD == NULL)
                return -set_error(STATUS_BAD_STATE);

            #ifdef PLATFORM_WINDOWS
                LARGE_INTEGER sizebuf;
                if (!GetFileSizeEx((HANDLE)_fileno(pFD), &sizebuf))
                    return -set_error(STATUS_IO_ERROR);
                wssize_t pos    = sizebuf.QuadPart;
            #else
                struct stat statbuf;
                if (fstat(fileno(pFD), &statbuf) != 0)
                    return -set_error(STATUS_IO_ERROR);
                wssize_t pos    = statbuf.st_size;
            #endif

            set_error(STATUS_OK);
            return pos;
        }

        status_t StdioFile::stat(fattr_t *attr)
        {
            if (pFD == NULL)
                return -set_error(STATUS_BAD_STATE);
            return set_error(File::stat(pFD, attr));
        }

        status_t StdioFile::truncate(wsize_t length)
        {
            // Check state
            if (pFD == NULL)
                return set_error(STATUS_BAD_STATE);
            else if (!(nFlags & SF_WRITE))
                return set_error(STATUS_PERMISSION_DENIED);

            if (fflush(pFD) != 0)
                return set_error(STATUS_IO_ERROR);
            if (ftruncate(fileno(pFD), length) != 0)
                return set_error(STATUS_IO_ERROR);
            return set_error(STATUS_OK);
        }

        status_t StdioFile::flush()
        {
            // Check state
            if (pFD == NULL)
                return set_error(STATUS_BAD_STATE);
            else if (!(nFlags & SF_WRITE))
                return set_error(STATUS_PERMISSION_DENIED);

            if (fflush(pFD) != 0)
                return set_error(STATUS_IO_ERROR);
            return set_error(STATUS_OK);
        }

        status_t StdioFile::sync()
        {
            // Check state
            if (pFD == NULL)
                return set_error(STATUS_BAD_STATE);
            else if (!(nFlags & SF_WRITE))
                return set_error(STATUS_PERMISSION_DENIED);

            if (fflush(pFD) != 0)
                return set_error(STATUS_IO_ERROR);
            #ifdef PLATFORM_WINDOWS
                if (_commit(_fileno(pFD)) != 0)
                    return set_error(STATUS_IO_ERROR);
            #else
                if (fsync(fileno(pFD)) != 0)
                    return set_error(STATUS_IO_ERROR);
            #endif

            return set_error(STATUS_OK);
        }

        status_t StdioFile::close()
        {
            if (pFD != NULL)
            {
                if (nFlags & SF_CLOSE)
                {
                    if (fclose(pFD) != 0)
                        return set_error(STATUS_IO_ERROR);
                }

                pFD     = NULL;
                nFlags  = 0;
            }

            return set_error(STATUS_OK);
        }
    
    } /* namespace io */
} /* namespace lsp */
