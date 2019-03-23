/*
 * File.cpp
 *
 *  Created on: 6 мар. 2019 г.
 *      Author: sadko
 */

#include <core/io/File.h>
#include <core/debug.h>

#ifdef PLATFORM_WINDOWS
    #include <fileapi.h>
    #include <io.h>
#else
    #include <sys/stat.h>
    #include <errno.h>
    #include <unistd.h>
#endif /* PLATFORM_WINDOWS */

namespace lsp
{
    namespace io
    {
        
        File::File()
        {
            nErrorCode  = STATUS_OK;
        }
        
        File::~File()
        {
            close();
        }

        ssize_t File::read(void *dst, size_t count)
        {
            return -set_error(STATUS_NOT_SUPPORTED);
        }

        ssize_t File::pread(wsize_t pos, void *dst, size_t count)
        {
            return -set_error(STATUS_NOT_SUPPORTED);
        }

        ssize_t File::write(const void *src, size_t count)
        {
            return -set_error(STATUS_NOT_SUPPORTED);
        }

        ssize_t File::pwrite(wsize_t pos, const void *src, size_t count)
        {
            return -set_error(STATUS_NOT_SUPPORTED);
        }

        status_t File::seek(wssize_t pos, size_t type)
        {
            return set_error(STATUS_NOT_SUPPORTED);
        }

        wssize_t File::position()
        {
            return -set_error(STATUS_NOT_SUPPORTED);
        }

        wssize_t File::size()
        {
            return -set_error(STATUS_NOT_SUPPORTED);
        }

        status_t File::stat(fattr_t *attr)
        {
            return -set_error(STATUS_NOT_SUPPORTED);
        }

        status_t File::truncate(wsize_t length)
        {
            return set_error(STATUS_NOT_SUPPORTED);
        }

        status_t File::flush()
        {
            return set_error(STATUS_NOT_SUPPORTED);
        }

        status_t File::sync()
        {
            return set_error(STATUS_NOT_SUPPORTED);
        }

        status_t File::close()
        {
            return set_error(STATUS_OK);
        }
    
        status_t File::stat(const char *path, fattr_t *attr)
        {
            if ((path == NULL) || (attr == NULL))
                return STATUS_BAD_ARGUMENTS;

            LSPString spath;
            if (!spath.set_utf8(path))
                return STATUS_NO_MEM;
            return stat(&spath, attr);
        }


        status_t File::stat(const Path *path, fattr_t *attr)
        {
            if ((path == NULL) || (attr == NULL))
                return STATUS_BAD_ARGUMENTS;
            return stat(path->as_string(), attr);
        }

        status_t File::sym_stat(const char *path, fattr_t *attr)
        {
            if ((path == NULL) || (attr == NULL))
                return STATUS_BAD_ARGUMENTS;

            LSPString spath;
            if (!spath.set_utf8(path))
                return STATUS_NO_MEM;
            return sym_stat(&spath, attr);
        }

        status_t File::sym_stat(const Path *path, fattr_t *attr)
        {
            if ((path == NULL) || (attr == NULL))
                return STATUS_BAD_ARGUMENTS;
            return sym_stat(path->as_string(), attr);
        }

        status_t File::stat(FILE *fd, fattr_t *attr)
        {
            if (fd == NULL)
                return STATUS_BAD_ARGUMENTS;

            #ifdef PLATFORM_WINDOWS
                return stat((HANDLE)::_get_osfhandle(::_fileno(fd)), attr);
            #else
                return stat(fileno(fd), attr);
            #endif
        }

        status_t File::stat(const LSPString *path, fattr_t *attr)
        {
            if ((path == NULL) || (attr == NULL))
                return STATUS_BAD_ARGUMENTS;

            #ifdef PLATFORM_WINDOWS
                WIN32_FIND_DATAW hfi;

                HANDLE dh   = ::FindFirstFileW(path->get_utf16(), &hfi);
                if (dh == INVALID_HANDLE_VALUE)
                {
                    DWORD err = ::GetLastError();
                    switch (err)
                    {
                        case ERROR_NO_MORE_FILES:
                        case ERROR_FILE_NOT_FOUND:
                            return STATUS_NOT_FOUND;
                        default:
                            return STATUS_IO_ERROR;
                    }
                }
                ::FindClose(dh);

                // Decode file type
                attr->type      = fattr_t::FT_REGULAR;
                if (hfi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                    attr->type      = fattr_t::FT_DIRECTORY;
                else if (hfi.dwFileAttributes & FILE_ATTRIBUTE_DEVICE)
                    attr->type      = fattr_t::FT_BLOCK;

                attr->blk_size  = 4096;
                attr->size      = (wsize_t(hfi.nFileSizeHigh) << 32) | hfi.nFileSizeLow;
                attr->inode     = 0;
                attr->ctime     = ((wsize_t(hfi.ftCreationTime.dwHighDateTime) << 32) | hfi.ftCreationTime.dwLowDateTime) / 10000;
                attr->mtime     = ((wsize_t(hfi.ftLastWriteTime.dwHighDateTime) << 32) | hfi.ftLastWriteTime.dwLowDateTime) / 10000;
                attr->atime     = ((wsize_t(hfi.ftLastAccessTime.dwHighDateTime) << 32) | hfi.ftLastAccessTime.dwLowDateTime) / 10000;
            #else
                struct stat sb;
                if (::lstat(path->get_native(), &sb) != 0)
                {
                    int code = errno;
                    switch (code)
                    {
                        case EACCES: return STATUS_PERMISSION_DENIED;
                        case EBADF: return STATUS_INVALID_VALUE;
                        case ENAMETOOLONG: return STATUS_OVERFLOW;
                        case EOVERFLOW: return STATUS_OVERFLOW;
                        case ENOENT: return STATUS_NOT_FOUND;
                        case ENOMEM: return STATUS_NO_MEM;
                        default: break;
                    }
                    return STATUS_IO_ERROR;
                }

                // Decode file type
                switch (sb.st_mode & S_IFMT) {
                    case S_IFBLK:  attr->type = fattr_t::FT_BLOCK;      break;
                    case S_IFCHR:  attr->type = fattr_t::FT_CHARACTER;  break;
                    case S_IFDIR:  attr->type = fattr_t::FT_DIRECTORY;  break;
                    case S_IFIFO:  attr->type = fattr_t::FT_FIFO;       break;
                    case S_IFLNK:  attr->type = fattr_t::FT_SYMLINK;    break;
                    case S_IFREG:  attr->type = fattr_t::FT_REGULAR;    break;
                    case S_IFSOCK: attr->type = fattr_t::FT_SOCKET;     break;
                    default:       attr->type = fattr_t::FT_UNKNOWN;    break;
                }

                attr->blk_size  = sb.st_blksize;
                attr->size      = sb.st_size;
                attr->inode     = sb.st_ino;
                attr->ctime     = (sb.st_ctim.tv_sec * 1000L) + (sb.st_ctim.tv_nsec / 1000000);
                attr->mtime     = (sb.st_mtim.tv_sec * 1000L) + (sb.st_mtim.tv_nsec / 1000000);
                attr->atime     = (sb.st_atim.tv_sec * 1000L) + (sb.st_atim.tv_nsec / 1000000);
            #endif /* PLATFORM_WINDOWS */

            return STATUS_OK;
        }

        status_t File::stat(lsp_fhandle_t fd, fattr_t *attr)
        {
            if (attr == NULL)
                return STATUS_BAD_ARGUMENTS;

        #ifdef PLATFORM_WINDOWS
            BY_HANDLE_FILE_INFORMATION hfi;

            if (!::GetFileInformationByHandle(fd, &hfi))
                return STATUS_IO_ERROR;

            // Decode file type
            attr->type      = fattr_t::FT_REGULAR;
            if (hfi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                attr->type      = fattr_t::FT_DIRECTORY;
            else if (hfi.dwFileAttributes & FILE_ATTRIBUTE_DEVICE)
                attr->type      = fattr_t::FT_BLOCK;

            attr->blk_size  = 4096;
            attr->size      = (wsize_t(hfi.nFileSizeHigh) << 32) | hfi.nFileSizeLow;
            attr->inode     = (wsize_t(hfi.nFileIndexHigh) << 32) | hfi.nFileIndexLow;
            attr->ctime     = ((wsize_t(hfi.ftCreationTime.dwHighDateTime) << 32) | hfi.ftCreationTime.dwLowDateTime) / 10000;
            attr->mtime     = ((wsize_t(hfi.ftLastWriteTime.dwHighDateTime) << 32) | hfi.ftLastWriteTime.dwLowDateTime) / 10000;
            attr->atime     = ((wsize_t(hfi.ftLastAccessTime.dwHighDateTime) << 32) | hfi.ftLastAccessTime.dwLowDateTime) / 10000;
        #else
            struct stat sb;
            if (::fstat(fd, &sb) != 0)
            {
                int code = errno;
                switch (code)
                {
                    case EACCES: return STATUS_PERMISSION_DENIED;
                    case EBADF: return STATUS_INVALID_VALUE;
                    case ENAMETOOLONG: return STATUS_OVERFLOW;
                    case EOVERFLOW: return STATUS_OVERFLOW;
                    case ENOENT: return STATUS_NOT_FOUND;
                    case ENOMEM: return STATUS_NO_MEM;
                    default: break;
                }
                return STATUS_IO_ERROR;
            }

            // Decode file type
            switch (sb.st_mode & S_IFMT) {
                case S_IFBLK:  attr->type = fattr_t::FT_BLOCK;      break;
                case S_IFCHR:  attr->type = fattr_t::FT_CHARACTER;  break;
                case S_IFDIR:  attr->type = fattr_t::FT_DIRECTORY;  break;
                case S_IFIFO:  attr->type = fattr_t::FT_FIFO;       break;
                case S_IFLNK:  attr->type = fattr_t::FT_SYMLINK;    break;
                case S_IFREG:  attr->type = fattr_t::FT_REGULAR;    break;
                case S_IFSOCK: attr->type = fattr_t::FT_SOCKET;     break;
                default:       attr->type = fattr_t::FT_UNKNOWN;    break;
            }

            attr->blk_size  = sb.st_blksize;
            attr->size      = sb.st_size;
            attr->inode     = sb.st_ino;
            attr->ctime     = (sb.st_ctim.tv_sec * 1000L) + (sb.st_ctim.tv_nsec / 1000000);
            attr->mtime     = (sb.st_mtim.tv_sec * 1000L) + (sb.st_mtim.tv_nsec / 1000000);
            attr->atime     = (sb.st_atim.tv_sec * 1000L) + (sb.st_atim.tv_nsec / 1000000);
        #endif  /* PLATFORM_WINDOWS */

            return STATUS_OK;
        }

        status_t File::sym_stat(const LSPString *path, fattr_t *attr)
        {
            if ((path == NULL) || (attr == NULL))
                return STATUS_BAD_ARGUMENTS;

            #ifdef PLATFORM_WINDOWS
                WIN32_FIND_DATAW hfi;

                HANDLE dh   = ::FindFirstFileW(path->get_utf16(), &hfi);
                if (dh == INVALID_HANDLE_VALUE)
                {
                    DWORD err = ::GetLastError();
                    switch (err)
                    {
                        case ERROR_NO_MORE_FILES:
                        case ERROR_FILE_NOT_FOUND:
                            return STATUS_NOT_FOUND;
                        default:
                            return STATUS_IO_ERROR;
                    }
                }
                ::FindClose(dh);

                // Decode file type
                attr->type      = fattr_t::FT_REGULAR;
                if (hfi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                    attr->type      = fattr_t::FT_DIRECTORY;
                else if (hfi.dwFileAttributes & FILE_ATTRIBUTE_DEVICE)
                    attr->type      = fattr_t::FT_BLOCK;

                attr->blk_size  = 4096;
                attr->size      = (wsize_t(hfi.nFileSizeHigh) << 32) | hfi.nFileSizeLow;
                attr->inode     = 0;
                attr->ctime     = ((wsize_t(hfi.ftCreationTime.dwHighDateTime) << 32) | hfi.ftCreationTime.dwLowDateTime) / 10000;
                attr->mtime     = ((wsize_t(hfi.ftLastWriteTime.dwHighDateTime) << 32) | hfi.ftLastWriteTime.dwLowDateTime) / 10000;
                attr->atime     = ((wsize_t(hfi.ftLastAccessTime.dwHighDateTime) << 32) | hfi.ftLastAccessTime.dwLowDateTime) / 10000;
            #else
                struct stat sb;
                const char *s = path->get_native();
                if (::stat(s, &sb) != 0)
                {
                    int code = errno;
                    switch (code)
                    {
                        case EACCES: return STATUS_PERMISSION_DENIED;
                        case EBADF: return STATUS_INVALID_VALUE;
                        case ENAMETOOLONG: return STATUS_OVERFLOW;
                        case EOVERFLOW: return STATUS_OVERFLOW;
                        case ENOENT: return STATUS_NOT_FOUND;
                        case ENOMEM: return STATUS_NO_MEM;
                        default: break;
                    }
                    return STATUS_IO_ERROR;
                }

                // Decode file type
                switch (sb.st_mode & S_IFMT) {
                    case S_IFBLK:  attr->type = fattr_t::FT_BLOCK;      break;
                    case S_IFCHR:  attr->type = fattr_t::FT_CHARACTER;  break;
                    case S_IFDIR:  attr->type = fattr_t::FT_DIRECTORY;  break;
                    case S_IFIFO:  attr->type = fattr_t::FT_FIFO;       break;
                    case S_IFLNK:  attr->type = fattr_t::FT_SYMLINK;    break;
                    case S_IFREG:  attr->type = fattr_t::FT_REGULAR;    break;
                    case S_IFSOCK: attr->type = fattr_t::FT_SOCKET;     break;
                    default:       attr->type = fattr_t::FT_UNKNOWN;    break;
                }

                attr->blk_size  = sb.st_blksize;
                attr->size      = sb.st_size;
                attr->inode     = sb.st_ino;
                attr->ctime     = (sb.st_ctim.tv_sec * 1000L) + (sb.st_ctim.tv_nsec / 1000000);
                attr->mtime     = (sb.st_mtim.tv_sec * 1000L) + (sb.st_mtim.tv_nsec / 1000000);
                attr->atime     = (sb.st_atim.tv_sec * 1000L) + (sb.st_atim.tv_nsec / 1000000);
            #endif /* PLATFORM_WINDOWS */

            return STATUS_OK;
        }

        status_t File::remove(const char *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            LSPString spath;
            if (!spath.set_utf8(path))
                return STATUS_NO_MEM;
            return remove(&spath);
        }

        status_t File::remove(const Path *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            return remove(path->as_string());
        }

        status_t File::remove(const LSPString *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

#ifdef PLATFORM_WINDOWS
            if (::DeleteFileW(path->get_utf16()))
                return STATUS_OK;

            // Analyze error code
            DWORD code = ::GetLastError();
            switch (code)
            {
                case ERROR_ACCESS_DENIED:
                {
                    fattr_t attr;
                    status_t res = stat(path, &attr);
                    if ((res == STATUS_OK) && (attr.type == fattr_t::FT_DIRECTORY))
                        return STATUS_IS_DIRECTORY;
                    return STATUS_PERMISSION_DENIED;
                }
                case ERROR_PATH_NOT_FOUND:
                    return STATUS_NOT_FOUND;
                default:
                    return STATUS_IO_ERROR;
            }
#else
            // Try to remove file
            if (::unlink(path->get_native()) == 0)
                return STATUS_OK;

            // Analyze error code
            int code = errno;
            lsp_trace("code=%d", int(code));
            switch (code)
            {
                case EACCES:
                    return STATUS_PERMISSION_DENIED;
                case EPERM:
                {
                    fattr_t attr;
                    status_t res = stat(path, &attr);
                    if ((res == STATUS_OK) && (attr.type == fattr_t::FT_DIRECTORY))
                        return STATUS_IS_DIRECTORY;
                    return STATUS_PERMISSION_DENIED;
                }
                case EDQUOT:
                case ENOSPC:
                    return STATUS_OVERFLOW;
                case EISDIR:
                    return STATUS_IS_DIRECTORY;
                case EFAULT:
                case EINVAL:
                case ENAMETOOLONG:
                    return STATUS_BAD_ARGUMENTS;
                case ENOTDIR:
                    return STATUS_BAD_TYPE;
                case ENOENT:
                    return STATUS_NOT_FOUND;
                case ENOTEMPTY:
                    return STATUS_NOT_EMPTY;
                default:
                    return STATUS_IO_ERROR;
            }
#endif /* PLATFORM_WINDOWS */
            return STATUS_OK;
        }

    } /* namespace io */
} /* namespace lsp */
