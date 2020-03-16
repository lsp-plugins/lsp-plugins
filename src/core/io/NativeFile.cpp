/*
 * NativeFile.cpp
 *
 *  Created on: 7 мар. 2019 г.
 *      Author: sadko
 */

#include <core/io/NativeFile.h>

#if defined(PLATFORM_WINDOWS)
    #include <fileapi.h>
#endif /* PLATFORM_WINDOWS */

#if defined(PLATFORM_UNIX_COMPATIBLE)
    #include <unistd.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <errno.h>
#endif /* PLATFORM_UNIX_COMPATIBLE */

#define BAD_FD      lsp_fhandle_t(-1)

namespace lsp
{
    namespace io
    {
        
        NativeFile::NativeFile()
        {
            hFD     = BAD_FD;
            nFlags  = 0;
        }
        
        NativeFile::~NativeFile()
        {
            if (hFD != BAD_FD)
            {
                if (nFlags & SF_CLOSE)
                {
                    #if defined(PLATFORM_WINDOWS)
                        CloseHandle(hFD);
                    #else
                        ::close(hFD);
                    #endif
                }
                hFD     = BAD_FD;
            }
            nFlags  = 0;
        }

        status_t NativeFile::open(const char *path, size_t mode)
        {
            if (path == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

            LSPString tmp;
            if (!tmp.set_utf8(path))
                return set_error(STATUS_NO_MEM);
            return open(&tmp, mode);
        }

    #if defined(PLATFORM_WINDOWS)
        status_t NativeFile::open(const LSPString *path, size_t mode)
        {
            if (path == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

            fattr_t stat;
            if (File::stat(path, &stat) == STATUS_OK)
            {
                if (stat.type == fattr_t::FT_DIRECTORY)
                    return (mode & FM_CREATE) ? STATUS_ALREADY_EXISTS : STATUS_NOT_FOUND;
            }

            int oflags;
            size_t fflags;
            size_t shflags = FILE_SHARE_DELETE;
            size_t cmode;
            size_t atts = FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS;
            if (mode & FM_READ)
            {
                oflags      = (mode & FM_WRITE) ? GENERIC_READ | GENERIC_WRITE : GENERIC_READ;
                shflags    |= (mode & FM_WRITE) ? FILE_SHARE_READ | FILE_SHARE_WRITE : FILE_SHARE_READ;
                fflags      = (mode & FM_WRITE) ? SF_READ | SF_WRITE : SF_READ;
            }
            else if (mode & FM_WRITE)
            {
                oflags      = GENERIC_WRITE;
                shflags    |= FILE_SHARE_WRITE;
                fflags      = SF_WRITE;
            }
            else
                return set_error(STATUS_INVALID_VALUE);

            if (mode & FM_CREATE)
                cmode       = (mode & FM_TRUNC) ? CREATE_ALWAYS : CREATE_NEW;
            else if (mode & FM_TRUNC)
                cmode       = TRUNCATE_EXISTING;
            else
                cmode       = OPEN_EXISTING;

            if (mode & FM_DIRECT)
                atts           |= FILE_FLAG_NO_BUFFERING;

            lsp_fhandle_t fd = CreateFileW(path->get_utf16(), oflags, shflags, NULL, cmode, atts, NULL);
            if (fd == INVALID_HANDLE_VALUE)
                return set_error(STATUS_IO_ERROR);

            hFD         = fd;
            nFlags      = fflags | SF_CLOSE;

            return set_error(STATUS_OK);
        }
    #else
        status_t NativeFile::open(const LSPString *path, size_t mode)
        {
            if (path == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

            fattr_t stat;
            if (File::stat(path, &stat) == STATUS_OK)
            {
                if (stat.type == fattr_t::FT_DIRECTORY)
                    return (mode & FM_CREATE) ? STATUS_ALREADY_EXISTS : STATUS_NOT_FOUND;
            }

            int oflags;
            size_t fflags;
            if (mode & FM_READ)
            {
                oflags      = (mode & FM_WRITE) ? O_RDWR : O_RDONLY;
                fflags      = (mode & FM_WRITE) ? SF_READ | SF_WRITE : SF_READ;
            }
            else if (mode & FM_WRITE)
            {
                oflags      = O_WRONLY;
                fflags      = SF_WRITE;
            }
            else
                return set_error(STATUS_INVALID_VALUE);
            if (mode & FM_CREATE)
                oflags     |= O_CREAT;
            if (mode & FM_TRUNC)
                oflags     |= O_TRUNC;

            #ifdef __USE_GNU
                if (mode & FM_DIRECT)
                    oflags     |= O_DIRECT;
            #endif /* __USE_GNU */

            lsp_fhandle_t fd    = ::open(path->get_native(), oflags, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if (fd < 0)
            {
                int code = errno;
                status_t res = STATUS_IO_ERROR;

                switch (code)
                {
                    case EPERM: case EACCES: res = STATUS_PERMISSION_DENIED; break;
                    case EEXIST: res = STATUS_ALREADY_EXISTS; break;
                    case EINVAL: res = STATUS_INVALID_VALUE; break;
                    case EISDIR: res = STATUS_IS_DIRECTORY; break;
                    case ENAMETOOLONG: res = STATUS_OVERFLOW; break;
                    case ENOENT: res = STATUS_NOT_FOUND; break;
                    case ENOMEM: res = STATUS_NO_MEM; break;
                    case ENOTDIR: res = STATUS_NOT_DIRECTORY; break;
                    case EROFS: res = STATUS_READONLY; break;
                    default: break;
                }

                return set_error(res);
            }

            hFD         = fd;
            nFlags      = fflags | SF_CLOSE;

            return set_error(STATUS_OK);
        }
    #endif /* PLATFORM_WINDOWS */

        status_t NativeFile::open(const Path *path, size_t mode)
        {
            if (path == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);
            return open(path->as_string(), mode);
        }

        status_t NativeFile::wrap(lsp_fhandle_t fd, bool close)
        {
            // Check state
            if (hFD != BAD_FD)
                return set_error(STATUS_BAD_STATE);

            hFD     = fd;
            nFlags  = (close) ? SF_READ | SF_WRITE | SF_CLOSE : SF_READ | SF_WRITE;
            return set_error(STATUS_OK);
        }

        status_t NativeFile::wrap(lsp_fhandle_t fd, size_t mode, bool close)
        {
            // Check state
            if (hFD != BAD_FD)
                return set_error(STATUS_BAD_STATE);

            size_t flags = (close) ? SF_CLOSE : 0;
            if (mode & FM_READ)
                flags  |= SF_READ;
            if (mode & FM_WRITE)
                flags  |= SF_WRITE;

            hFD     = fd;
            nFlags  = flags;

            return set_error(STATUS_OK);
        }

        ssize_t NativeFile::read(void *dst, size_t count)
        {
            // Check state
            if (hFD == BAD_FD)
                return -set_error(STATUS_BAD_STATE);
            else if (!(nFlags & SF_READ))
                return -set_error(STATUS_PERMISSION_DENIED);

            uint8_t *ptr    = reinterpret_cast<uint8_t *>(dst);
            size_t bread    = 0;
            bool eof        = false;

            // Perofrm read
            #ifdef PLATFORM_WINDOWS
                while (bread < count)
                {
                    size_t to_read      = count - bread;
                    if (to_read >= 0x40000000)
                        to_read             = 0x40000000;
                    DWORD n_read        = 0;

                    if (!ReadFile(hFD, ptr, DWORD(to_read), &n_read, NULL))
                        break;
                    if (n_read <= 0)
                    {
                        eof = true;
                        break;
                    }

                    ptr    += n_read;
                    bread  += n_read;
                }
            #else
                while (bread < count)
                {
                    size_t to_read  = count - bread;
                    size_t n_read   = ::read(hFD, ptr, to_read);

                    if (n_read <= 0)
                    {
                        eof = true;
                        break;
                    }

                    ptr    += n_read;
                    bread  += n_read;
                }

            #endif
            if ((bread > 0) || (count <= 0) || (!eof))
            {
                set_error(STATUS_OK);
                return bread;
            }
            return -set_error(STATUS_EOF);
        }

        ssize_t NativeFile::pread(wsize_t pos, void *dst, size_t count)
        {
            // Check state
            if (hFD == BAD_FD)
                return -set_error(STATUS_BAD_STATE);
            else if (!(nFlags & SF_READ))
                return -set_error(STATUS_PERMISSION_DENIED);

            uint8_t *ptr    = reinterpret_cast<uint8_t *>(dst);
            size_t bread    = 0;
            bool eof        = false;

            #ifdef PLATFORM_WINDOWS
                LARGE_INTEGER off, save;
                off.QuadPart = 0;

                // Obtrain current file pointer
                if (!SetFilePointerEx(hFD, off, &save, FILE_CURRENT))
                    return -set_error(STATUS_IO_ERROR);

                // Change file pointer
                off.QuadPart = pos;
                if (!SetFilePointerEx(hFD, off, NULL, FILE_BEGIN))
                    return -set_error(STATUS_IO_ERROR);

                // Perform positioned read
                while (bread < count)
                {
                    size_t to_read      = count - bread;
                    if (to_read >= 0x40000000)
                        to_read             = 0x40000000;
                    DWORD n_read        = 0;

                    if (!ReadFile(hFD, ptr, DWORD(to_read), &n_read, NULL))
                        break;
                    if (n_read <= 0)
                    {
                        eof = true;
                        break;
                    }

                    ptr    += n_read;
                    bread  += n_read;
                }

                // Restore position
                if (!SetFilePointerEx(hFD, save, NULL, FILE_BEGIN))
                    return -set_error(STATUS_IO_ERROR);
            #else
                // Perform positioned read
                while (bread < count)
                {
                    size_t to_read  = count - bread;
                    size_t n_read   = ::pread(hFD, ptr, to_read, pos);

                    if (n_read <= 0)
                    {
                        eof = true;
                        break;
                    }

                    ptr    += n_read;
                    bread  += n_read;
                    pos    += n_read;
                }
            #endif

            if ((bread > 0) || (count <= 0) || (!eof))
            {
                set_error(STATUS_OK);
                return bread;
            }
            return -set_error(STATUS_EOF);
        }

        ssize_t NativeFile::write(const void *src, size_t count)
        {
            // Check state
            if (hFD == BAD_FD)
                return -set_error(STATUS_BAD_STATE);
            else if (!(nFlags & SF_WRITE))
                return -set_error(STATUS_PERMISSION_DENIED);

            const uint8_t *ptr  = reinterpret_cast<const uint8_t *>(src);
            size_t bwritten     = 0;

            // Perform write
            #ifdef PLATFORM_WINDOWS
                while (bwritten < count)
                {
                    size_t to_write     = count - bwritten;
                    if (to_write >= 0x40000000)
                        to_write            = 0x40000000;
                    DWORD n_written     = 0;

                    if (!WriteFile(hFD, ptr, to_write, &n_written, NULL))
                        break;

                    ptr        += n_written;
                    bwritten   += n_written;
                }
            #else
                while (bwritten < count)
                {
                    size_t to_write     = count - bwritten;
                    ssize_t n_written   = ::write(hFD, ptr, to_write);

                    if (n_written <= 0)
                        break;

                    ptr        += n_written;
                    bwritten   += n_written;
                }
            #endif

            if ((bwritten > 0) || (count <= 0))
            {
                set_error(STATUS_OK);
                return bwritten;
            }

            return -set_error(STATUS_IO_ERROR);
        }

        ssize_t NativeFile::pwrite(wsize_t pos, const void *src, size_t count)
        {
            // Check state
            if (hFD == BAD_FD)
                return -set_error(STATUS_BAD_STATE);
            else if (!(nFlags & SF_WRITE))
                return -set_error(STATUS_PERMISSION_DENIED);

            const uint8_t *ptr  = reinterpret_cast<const uint8_t *>(src);
            size_t bwritten     = 0;

            #ifdef PLATFORM_WINDOWS
                LARGE_INTEGER off, save;
                off.QuadPart = 0;

                // Obtrain current file pointer
                if (!SetFilePointerEx(hFD, off, &save, FILE_CURRENT))
                    return -set_error(STATUS_IO_ERROR);

                // Change file pointer
                off.QuadPart = pos;
                if (!SetFilePointerEx(hFD, off, NULL, FILE_BEGIN))
                    return -set_error(STATUS_IO_ERROR);

                // Perform positioned write
                while (bwritten < count)
                {
                    size_t to_write     = count - bwritten;
                    if (to_write >= 0x40000000)
                        to_write            = 0x40000000;
                    DWORD n_written     = 0;

                    if (!WriteFile(hFD, ptr, to_write, &n_written, NULL))
                        break;

                    ptr        += n_written;
                    bwritten   += n_written;
                }

                // Restore position
                if (!SetFilePointerEx(hFD, save, NULL, FILE_BEGIN))
                    return -set_error(STATUS_IO_ERROR);
            #else
                // Perform positioned write
                while (bwritten < count)
                {
                    size_t to_write     = count - bwritten;
                    size_t n_written    = ::pwrite(hFD, ptr, to_write, pos);

                    if (n_written <= 0)
                        break;

                    ptr        += n_written;
                    bwritten   += n_written;
                    pos        += n_written;
                }
            #endif

            if ((bwritten > 0) || (count <= 0))
            {
                set_error(STATUS_OK);
                return bwritten;
            }

            return -set_error(STATUS_IO_ERROR);
        }

        status_t NativeFile::seek(wssize_t pos, size_t type)
        {
            if (hFD == BAD_FD)
                return set_error(STATUS_BAD_STATE);

            #ifdef PLATFORM_WINDOWS
                DWORD method;
                LARGE_INTEGER off;
                switch (type)
                {
                    case FSK_SET: method  = FILE_BEGIN; break;
                    case FSK_CUR: method  = FILE_CURRENT; break;
                    case FSK_END: method  = FILE_END; break;
                    default: return set_error(STATUS_INVALID_VALUE);
                }
                off.QuadPart = pos;
                if (!SetFilePointerEx(hFD, off, NULL, method))
                    return set_error(STATUS_IO_ERROR);
            #else
                int whence;
                switch (type)
                {
                    case FSK_SET:  whence = SEEK_SET; break;
                    case FSK_CUR:  whence = SEEK_CUR; break;
                    case FSK_END:  whence = SEEK_END; break;
                    default:
                        return set_error(STATUS_BAD_ARGUMENTS);
                }

                if (lseek(hFD, pos, whence) < 0)
                {
                    int code = errno;
                    if (code == ESPIPE)
                        return set_error(STATUS_NOT_SUPPORTED);
                    return set_error(STATUS_IO_ERROR);
                }
            #endif /* PLATFORM_WINDOWS */

            return set_error(STATUS_OK);
        }

        wssize_t NativeFile::position()
        {
            if (hFD == BAD_FD)
                return -set_error(STATUS_BAD_STATE);

            #ifdef PLATFORM_WINDOWS
                LARGE_INTEGER off, pos;
                off.QuadPart = 0;
                if (!SetFilePointerEx(hFD, off, &pos, FILE_CURRENT))
                    return -set_error(STATUS_IO_ERROR);
                wssize_t result = pos.QuadPart;
            #else
                wssize_t result = lseek(hFD, 0, SEEK_CUR);
                if (result < 0)
                    return -set_error(STATUS_IO_ERROR);
            #endif /* PLATFORM_WINDOWS */

            return result;
        }

        wssize_t NativeFile::size()
        {
            if (hFD == BAD_FD)
                return -set_error(STATUS_BAD_STATE);

            #ifdef PLATFORM_WINDOWS
                LARGE_INTEGER sizebuf;
                if (!GetFileSizeEx(hFD, &sizebuf))
                    return -set_error(STATUS_IO_ERROR);
                wssize_t pos    = sizebuf.QuadPart;
            #else
                struct stat statbuf;
                if (fstat(hFD, &statbuf) != 0)
                    return -set_error(STATUS_IO_ERROR);
                wssize_t pos    = statbuf.st_size;
            #endif  /* PLATFORM_WINDOWS */

            set_error(STATUS_OK);
            return pos;
        }

        status_t NativeFile::stat(fattr_t *attr)
        {
            if (hFD == BAD_FD)
                return -set_error(STATUS_BAD_STATE);

            return set_error(File::stat(hFD, attr));
        }

        status_t NativeFile::truncate(wsize_t length)
        {
            // Check state
            if (hFD == BAD_FD)
                return set_error(STATUS_BAD_STATE);
            else if (!(nFlags & SF_WRITE))
                return set_error(STATUS_PERMISSION_DENIED);

            #ifdef PLATFORM_WINDOWS
                LARGE_INTEGER off, pos;

                off.QuadPart = 0;
                if (!SetFilePointerEx(hFD, off, &pos, FILE_CURRENT))
                    return -set_error(STATUS_IO_ERROR);

                off.QuadPart = length;
                if (!SetFilePointerEx(hFD, off, NULL, FILE_BEGIN))
                    return -set_error(STATUS_IO_ERROR);

                status_t res = (SetEndOfFile(hFD)) ? STATUS_OK : STATUS_IO_ERROR;

                if (!SetFilePointerEx(hFD, pos, NULL, FILE_BEGIN))
                    res = STATUS_IO_ERROR;

                return set_error(res);
            #else
                if (ftruncate(hFD, length) != 0)
                    return set_error(STATUS_IO_ERROR);
                return set_error(STATUS_OK);
            #endif  /* PLATFORM_WINDOWS */
        }

        status_t NativeFile::flush()
        {
            // Check state
            if (hFD == BAD_FD)
                return set_error(STATUS_BAD_STATE);
            else if (!(nFlags & SF_WRITE))
                return set_error(STATUS_PERMISSION_DENIED);

            #ifdef PLATFORM_WINDOWS
                if (!FlushFileBuffers(hFD))
                    return set_error(STATUS_IO_ERROR);
            #else
                if (fdatasync(hFD) != 0)
                    return set_error(STATUS_IO_ERROR);
            #endif  /* PLATFORM_WINDOWS */

            return set_error(STATUS_OK);
        }

        status_t NativeFile::sync()
        {
            // Check state
            if (hFD == BAD_FD)
                return set_error(STATUS_BAD_STATE);
            else if (!(nFlags & SF_WRITE))
                return set_error(STATUS_PERMISSION_DENIED);

            #ifdef PLATFORM_WINDOWS
                if (!FlushFileBuffers(hFD))
                    return set_error(STATUS_IO_ERROR);
            #else
                if (fsync(hFD) != 0)
                    return set_error(STATUS_IO_ERROR);
            #endif  /* PLATFORM_WINDOWS */

            return set_error(STATUS_OK);
        }

        status_t NativeFile::close()
        {
            if (hFD != BAD_FD)
            {
                if (nFlags & SF_CLOSE)
                {
                #ifdef PLATFORM_WINDOWS
                    if (!CloseHandle(hFD))
                        return set_error(STATUS_IO_ERROR);
                #else
                    if (::close(hFD) != 0)
                        return set_error(STATUS_IO_ERROR);
                #endif  /* PLATFORM_WINDOWS */
                }

                hFD     = BAD_FD;
                nFlags  = 0;
            }

            return set_error(STATUS_OK);
        }
    
    } /* namespace io */
} /* namespace lsp */
