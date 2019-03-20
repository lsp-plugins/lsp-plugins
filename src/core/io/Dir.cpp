/*
 * Dir.cpp
 *
 *  Created on: 18 мар. 2019 г.
 *      Author: sadko
 */

#include <core/io/Dir.h>

#ifdef PLATFORM_WINDOWS
    #include <fileapi.h>
#else
    #include <sys/stat.h>
    #include <errno.h>
    #include <fcntl.h>
#endif /* PLATFORM_WINDOWS */

#ifdef PLATFORM_WINDOWS
    #define FAKE_HANDLE             ((HANDLE)(-1))
    #define check_closed(hdir)      (hdir == INVALID_HANDLE_VALUE)
#else
    #define check_closed(hdir)      (hdir == NULL)
#endif /* PLATFORM_WINDOWS */

namespace lsp
{
    namespace io
    {
        
        Dir::Dir()
        {
            nErrorCode  = STATUS_OK;
            nPosition   = 0;
#ifdef PLATFORM_WINDOWS
            hDir        = INVALID_HANDLE_VALUE;
            nPending    = STATUS_OK;
#else
            hDir        = NULL;
#endif /* PLATFORM_WINDOWS */
        }
        
        Dir::~Dir()
        {
            close();
        }

        status_t Dir::open(const char *path)
        {
            if (!check_closed(hDir))
                return set_error(STATUS_BAD_STATE);
            else if (path == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

            LSPString spath;
            if (!spath.set_utf8(path))
                return set_error(STATUS_NO_MEM);
            return open (&spath);
        }

        status_t Dir::open(const LSPString *path)
        {
            if (!check_closed(hDir))
                return set_error(STATUS_BAD_STATE);
            else if (path == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

            if (sPath.set(path) != STATUS_OK)
                return set_error(STATUS_NO_MEM);

#ifdef PLATFORM_WINDOWS
            // Create search mask
            Path mask;
            status_t res = mask.set(path);
            if (res == STATUS_OK)
                res = mask.append_child("*");
            if (res != STATUS_OK)
            {
                sPath.clear();
                return set_error(res);
            }

            // Call API for FindFirst
            status_t pending    = STATUS_OK;
            HANDLE dh           = ::FindFirstFileW(mask.as_string()->get_utf16(), &sData);
            if (dh == INVALID_HANDLE_VALUE)
            {
                DWORD err = ::GetLastError();
                switch (err)
                {
                    case ERROR_NO_MORE_FILES:
                    case ERROR_FILE_NOT_FOUND:
                        dh          = FAKE_HANDLE;
                        pending     = STATUS_EOF;
                        break;
                    default:
                        return set_error(STATUS_UNKNOWN_ERR);
                }
            }

            nPending        = pending;
#else
            DIR *dh = ::opendir(path->get_native());
            if (dh == NULL)
            {
                sPath.clear();
                int error = errno;
                switch (error)
                {
                    case EACCES: return set_error(STATUS_PERMISSION_DENIED);
                    case EMFILE:
                    case ENFILE: return set_error(STATUS_TOO_BIG);
                    case ENOENT: return set_error(STATUS_NOT_FOUND);
                    case ENOMEM: return set_error(STATUS_NO_MEM);
                    case ENOTDIR: return set_error(STATUS_BAD_TYPE);
                    default:
                        return set_error(STATUS_UNKNOWN_ERR);
                }
            }

#endif /* PLATFORM_WINDOWS */

            hDir        = dh;
            nPosition   = 0;
            return set_error(STATUS_OK);
        }

        status_t Dir::open(const Path *path)
        {
            if (!check_closed(hDir))
                return set_error(STATUS_BAD_STATE);
            else if (path == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);
            return open(path->as_string());
        }

        status_t Dir::rewind()
        {
            if (check_closed(hDir))
                return set_error(STATUS_BAD_STATE);

#ifdef PLATFORM_WINDOWS
            // Create search mask
            Path mask;
            status_t res = mask.set(&sPath);
            if (res == STATUS_OK)
                res = mask.append_child("*");
            if (res != STATUS_OK)
            {
                sPath.clear();
                return set_error(res);
            }

            // Call API for FindFirst
            status_t pending    = STATUS_OK;
            HANDLE dh   = ::FindFirstFileW(mask.as_string()->get_utf16(), &sData);
            if (dh == INVALID_HANDLE_VALUE)
            {
                DWORD err = ::GetLastError();
                switch (err)
                {
                    case ERROR_NO_MORE_FILES:
                    case ERROR_FILE_NOT_FOUND:
                        dh          = FAKE_HANDLE;
                        pending     = STATUS_EOF;
                        break;
                    default:
                        return set_error(STATUS_UNKNOWN_ERR);
                }
            }

            // Close current  handle
            if (hDir != FAKE_HANDLE)
                ::FindClose(hDir);

            // Replace closed handle by new handle
            hDir        = dh;
            nPending    = pending;
#else
            ::rewinddir(hDir);
#endif /* PLATFORM_WINDOWS */

            nPosition = 0;
            return set_error(STATUS_OK);
        }

        status_t Dir::read(LSPString *path, bool full)
        {
            if (check_closed(hDir))
                return set_error(STATUS_BAD_STATE);
            else if (path == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

#ifdef PLATFORM_WINDOWS
            if (nPending != STATUS_OK)
                return set_error(nPending);
            else if (hDir == FAKE_HANDLE)
                return set_error(STATUS_BAD_STATE);

            // Set result
            if (!path->set_utf16(sData.cFileName))
                return set_error(STATUS_NO_MEM);

            // Perform next iteration
            if (!::FindNextFileW(hDir, &sData))
            {
                DWORD err = ::GetLastError();
                switch (err)
                {
                    case ERROR_NO_MORE_FILES:
                    case ERROR_FILE_NOT_FOUND:
                        nPending    = STATUS_EOF;
                        break;
                    default:
                        nPending    = STATUS_UNKNOWN_ERR;
                        break;
                }
            }
#else
            // Read directory
            errno = 0;
            struct dirent *dent = readdir(hDir);
            if (dent == NULL)
            {
                if (errno == 0)
                    return set_error(STATUS_EOF);
                return set_error(STATUS_UNKNOWN_ERR);
            }

            // Return value
            if (!path->set_native(dent->d_name))
                return set_error(STATUS_NO_MEM);

#endif /* PLATFORM_WINDOWS */
            return set_error(STATUS_OK);
        }

        status_t Dir::read(Path *path, bool full)
        {
            if (path == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

            LSPString xpath;
            status_t res = read(&xpath, false);
            if (res == STATUS_OK)
            {
                if (full)
                {
                    Path tmp;
                    res = tmp.set(&sPath);
                    if (res == STATUS_OK)
                        res = tmp.append_child(&xpath);
                    if (res == STATUS_OK)
                        path->take(&tmp);
                }
                else
                    res = path->set(&xpath);
            }
            return set_error(res);
        }

        status_t Dir::reads(LSPString *path, fattr_t *attr, bool full)
        {
            if (check_closed(hDir))
                return set_error(STATUS_BAD_STATE);
            else if (path == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

#ifdef PLATFORM_WINDOWS
            if (nPending != STATUS_OK)
                return set_error(nPending);
            else if (hDir == FAKE_HANDLE)
                return set_error(STATUS_BAD_STATE);

            // Set result
            if (!path->set_utf16(sData.cFileName))
                return set_error(STATUS_NO_MEM);

            // Perform next iteration
            if (!::FindNextFileW(hDir, &sData))
            {
                DWORD err = ::GetLastError();
                switch (err)
                {
                    case ERROR_NO_MORE_FILES:
                    case ERROR_FILE_NOT_FOUND:
                        nPending    = STATUS_EOF;
                        break;
                    default:
                        nPending    = STATUS_UNKNOWN_ERR;
                        break;
                }
            }

            // Decode file state
            // Decode file type
            attr->type      = fattr_t::FT_UNKNOWN;
            if (sData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                attr->type      = fattr_t::FT_DIRECTORY;
            else if (sData.dwFileAttributes & FILE_ATTRIBUTE_NORMAL)
                attr->type      = fattr_t::FT_REGULAR;

            if (sData.dwFileAttributes & FILE_ATTRIBUTE_SPARSE_FILE)
            {
                if (sData.dwReserved0 & IO_REPARSE_TAG_SYMLINK)
                    attr->type = fattr_t::FT_SYMLINK;
            }

            attr->blk_size  = 4096;
            attr->size      = (wsize_t(sData.nFileSizeHigh) << 32) | sData.nFileSizeLow;
            attr->inode     = (wsize_t(sData.nFileIndexHigh) << 32) | sData.nFileIndexLow;
            attr->ctime     = (wsize_t(sData.ftCreationTime.dwHighDateTime) << 32) | sData.ftCreationTime.dwLowDateTime) / 10000;
            attr->mtime     = (wsize_t(sData.ftLastWriteTime.dwHighDateTime) << 32) | sData.ftLastWriteTime.dwLowDateTime) / 10000;
            attr->atime     = (wsize_t(sData.ftLastAccessTime.dwHighDateTime) << 32) | sData.ftLastAccessTime.dwLowDateTime) / 10000;
#else
            // Read directory
            errno = 0;
            struct dirent *dent = ::readdir(hDir);
            if (dent == NULL)
            {
                if (errno == 0)
                    return set_error(STATUS_EOF);
                return set_error(STATUS_UNKNOWN_ERR);
            }

            // Stat the record
            struct stat sb;
#if ((_POSIX_C_SOURCE >= 200809L) || defined(_ATFILE_SOURCE))
            int code = ::fstatat(::dirfd(hDir), dent->d_name, &sb, 0);
#else
            LSPString xpath, xname;
            if (!xname.set_native(dent->d_name))
                return set_error(STATUS_NO_MEM);
            if (!xpath.set(sPath.as_string()))
                return set_error(STATUS_NO_MEM);
            if (!xpath.append(FILE_SEPARATOR_C))
                return set_error(STATUS_NO_MEM);
            if (!xpath.append(&xname))
                return set_error(STATUS_NO_MEM);
            int code = ::stat(xpath.get_native(), &sb);
#endif
            if (code != 0)
            {
                code = errno;
                switch (code)
                {
                    case EACCES: return set_error(STATUS_PERMISSION_DENIED);
                    case EBADF: return set_error(STATUS_INVALID_VALUE);
                    case ENAMETOOLONG: return set_error(STATUS_OVERFLOW);
                    case EOVERFLOW: return set_error(STATUS_OVERFLOW);
                    case ENOENT: return set_error(STATUS_NOT_FOUND);
                    case ENOMEM: return set_error(STATUS_NO_MEM);
                    default: break;
                }
                return set_error(STATUS_IO_ERROR);
            }

            // Return value
            if (!path->set_native(dent->d_name))
                return set_error(STATUS_NO_MEM);

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
            return set_error(STATUS_OK);
        }

        status_t Dir::reads(Path *path, fattr_t *attr, bool full)
        {
            if (path == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

            LSPString xpath;
            fattr_t xattr;

            status_t res = reads(&xpath, &xattr, false);
            if (res == STATUS_OK)
            {
                if (full)
                {
                    Path tmp;
                    res = tmp.set(&sPath);
                    if (res == STATUS_OK)
                        res = tmp.append_child(&xpath);
                    if (res == STATUS_OK)
                        path->take(&tmp);
                }
                else
                    res = path->set(&xpath);

                if (res == STATUS_OK)
                    *attr = xattr;
            }
            return set_error(res);
        }

        status_t Dir::close()
        {
            if (check_closed(hDir))
                return set_error(STATUS_BAD_STATE);

#ifdef PLATFORM_WINDOWS
            if (hDir != FAKE_HANDLE)
            {
                if (!::FindClose(hDir))
                    return set_error(STATUS_UNKNOWN_ERR);
                hDir    = INVALID_HANDLE_VALUE;
            }
#else
            if (::closedir(hDir) != 0)
            {
                int error = errno;
                if (error == EBADF)
                    return set_error(STATUS_BAD_STATE);
                else
                    return set_error(STATUS_IO_ERROR);
            }
            hDir    = NULL;
#endif /* PLATFORM_WINDOWS */

            nPosition = 0;
            return set_error(STATUS_OK);
        }
    
    } /* namespace io */
} /* namespace lsp */
