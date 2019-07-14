/*
 * Dir.cpp
 *
 *  Created on: 18 мар. 2019 г.
 *      Author: sadko
 */

#include <core/io/Dir.h>

#ifdef PLATFORM_WINDOWS
    #include <fileapi.h>
    #include <winbase.h>
#else
    #include <sys/stat.h>
    #include <errno.h>
    #include <fcntl.h>
    #include <unistd.h>
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

            LSPString out;

#ifdef PLATFORM_WINDOWS
            if (nPending != STATUS_OK)
                return set_error(nPending);
            else if (hDir == FAKE_HANDLE)
                return set_error(STATUS_BAD_STATE);

            // Set result
            if (!out.set_utf16(sData.cFileName))
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
            struct dirent *dent = ::readdir(hDir);
            if (dent == NULL)
            {
                if (errno == 0)
                    return set_error(STATUS_EOF);
                return set_error(STATUS_UNKNOWN_ERR);
            }

            // Return value
            if (!out.set_native(dent->d_name))
                return set_error(STATUS_NO_MEM);

#endif /* PLATFORM_WINDOWS */
            if (full)
            {
                Path tmp;
                status_t res = tmp.set(&sPath);
                if (res == STATUS_OK)
                    res = tmp.append_child(&out);
                if (res == STATUS_OK)
                    res = (out.set(tmp.as_string())) ? STATUS_OK : STATUS_NO_MEM;
                if (res != STATUS_OK)
                    set_error(res);
            }

            path->swap(&out);
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

            LSPString out;

#ifdef PLATFORM_WINDOWS
            if (nPending != STATUS_OK)
                return set_error(nPending);
            else if (hDir == FAKE_HANDLE)
                return set_error(STATUS_BAD_STATE);

            // Set result
            if (!out.set_utf16(sData.cFileName))
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
            attr->type      = fattr_t::FT_REGULAR;
            if (sData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                attr->type      = fattr_t::FT_DIRECTORY;
            else if (sData.dwFileAttributes & FILE_ATTRIBUTE_DEVICE)
                attr->type      = fattr_t::FT_BLOCK;

            attr->blk_size  = 4096;
            attr->size      = (wsize_t(sData.nFileSizeHigh) << 32) | sData.nFileSizeLow;
            attr->inode     = 0;
            attr->ctime     = ((wsize_t(sData.ftCreationTime.dwHighDateTime) << 32) | sData.ftCreationTime.dwLowDateTime) / 10000;
            attr->mtime     = ((wsize_t(sData.ftLastWriteTime.dwHighDateTime) << 32) | sData.ftLastWriteTime.dwLowDateTime) / 10000;
            attr->atime     = ((wsize_t(sData.ftLastAccessTime.dwHighDateTime) << 32) | sData.ftLastAccessTime.dwLowDateTime) / 10000;
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
            int code = ::fstatat(::dirfd(hDir), dent->d_name, &sb, AT_SYMLINK_NOFOLLOW);
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
            int code = ::lstat(xpath.get_native(), &sb);
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
            if (!out.set_native(dent->d_name))
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

            if (full)
            {
                Path tmp;
                status_t res = tmp.set(&sPath);
                if (res == STATUS_OK)
                    res = tmp.append_child(&out);
                if (res == STATUS_OK)
                    res = (out.set(tmp.as_string())) ? STATUS_OK : STATUS_NO_MEM;
                if (res != STATUS_OK)
                    set_error(res);
            }

            path->swap(&out);

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

        status_t Dir::stat(const char *path, fattr_t *attr)
        {
            if ((path == NULL) || (attr == NULL))
                return set_error(STATUS_BAD_ARGUMENTS);
            if (check_closed(hDir))
                return set_error(STATUS_BAD_STATE);

            Path xpath;
            status_t res = xpath.set(&sPath);
            if (res == STATUS_OK)
                res = xpath.append_child(path);
            if (res != STATUS_OK)
                return set_error(res);
            return set_error(File::stat(xpath.as_string(), attr));
        }

        status_t Dir::stat(const LSPString *path, fattr_t *attr)
        {
            if ((path == NULL) || (attr == NULL))
                return set_error(STATUS_BAD_ARGUMENTS);
            if (check_closed(hDir))
                return set_error(STATUS_BAD_STATE);

            Path xpath;
            status_t res = xpath.set(&sPath);
            if (res == STATUS_OK)
                res = xpath.append_child(path);
            if (res != STATUS_OK)
                return set_error(res);
            return set_error(File::stat(&xpath, attr));
        }

        status_t Dir::stat(const Path *path, fattr_t *attr)
        {
            if ((path == NULL) || (attr == NULL))
                return set_error(STATUS_BAD_ARGUMENTS);
            if (check_closed(hDir))
                return set_error(STATUS_BAD_STATE);

            Path xpath;
            status_t res = xpath.set(&sPath);
            if (res == STATUS_OK)
                res = xpath.append_child(path);
            if (res != STATUS_OK)
                return set_error(res);
            return set_error(File::stat(xpath.as_string(), attr));
        }

        status_t Dir::sym_stat(const char *path, fattr_t *attr)
        {
            if ((path == NULL) || (attr == NULL))
                return set_error(STATUS_BAD_ARGUMENTS);
            if (check_closed(hDir))
                return set_error(STATUS_BAD_STATE);

            Path xpath;
            status_t res = xpath.set(&sPath);
            if (res == STATUS_OK)
                res = xpath.append_child(path);
            if (res != STATUS_OK)
                return set_error(res);
            return set_error(File::sym_stat(xpath.as_string(), attr));
        }

        status_t Dir::sym_stat(const LSPString *path, fattr_t *attr)
        {
            if ((path == NULL) || (attr == NULL))
                return set_error(STATUS_BAD_ARGUMENTS);
            if (check_closed(hDir))
                return set_error(STATUS_BAD_STATE);

            Path xpath;
            status_t res = xpath.set(&sPath);
            if (res == STATUS_OK)
                res = xpath.append_child(path);
            if (res != STATUS_OK)
                return set_error(res);
            return set_error(File::sym_stat(xpath.as_string(), attr));
        }

        status_t Dir::sym_stat(const Path *path, fattr_t *attr)
        {
            if ((path == NULL) || (attr == NULL))
                return set_error(STATUS_BAD_ARGUMENTS);
            if (check_closed(hDir))
                return set_error(STATUS_BAD_STATE);

            Path xpath;
            status_t res = xpath.set(&sPath);
            if (res == STATUS_OK)
                res = xpath.append_child(path);
            if (res != STATUS_OK)
                return set_error(res);
            return set_error(File::sym_stat(xpath.as_string(), attr));
        }

        status_t Dir::create(const char *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            LSPString spath;
            if (!spath.set_utf8(path))
                return STATUS_NO_MEM;
            return create(&spath);
        }

        status_t Dir::create(const Path *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            return create(path->as_string());
        }

        status_t Dir::create(const LSPString *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

#ifdef PLATFORM_WINDOWS
            const WCHAR *xp = path->get_utf16();
            if (::CreateDirectoryW(xp, NULL))
                return STATUS_OK;

            // Analyze error code
            DWORD code = ::GetLastError();
            switch (code)
            {
                case ERROR_ALREADY_EXISTS:
                    return STATUS_OK;
                case ERROR_PATH_NOT_FOUND:
                    return STATUS_NOT_FOUND;
                default:
                    return STATUS_IO_ERROR;
            }
#else
            // Try to create directory
            if (::mkdir(path->get_native(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == 0)
                return STATUS_OK;

            // Analyze error code
            int code = errno;
            switch (code)
            {
                case EACCES:
                case EPERM:
                    return STATUS_PERMISSION_DENIED;
                case EDQUOT:
                case ENOSPC:
                    return STATUS_OVERFLOW;
                case ENOTDIR:
                    return STATUS_BAD_HIERARCHY;
                case EFAULT:
                case EINVAL:
                case ENAMETOOLONG:
                    return STATUS_BAD_ARGUMENTS;
                case EEXIST: // pathname already exists (not necessarily as a directory).  This includes the case where pathname is a symbolic link, dangling or not.
                {
                    fattr_t attr;
                    File::sym_stat(path, &attr);
                    if (attr.type == fattr_t::FT_DIRECTORY)
                        return STATUS_OK;
                    return STATUS_ALREADY_EXISTS;
                }
                case ENOENT: // A directory component in pathname does not exist or is a dangling symbolic link.
                    return STATUS_NOT_FOUND;
                default:
                    return STATUS_IO_ERROR;
            }
#endif /* PLATFORM_WINDOWS */
            return STATUS_OK;
        }

        status_t Dir::remove(const char *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            LSPString spath;
            if (!spath.set_utf8(path))
                return STATUS_NO_MEM;
            return remove(&spath);
        }

        status_t Dir::remove(const Path *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            return remove(path->as_string());
        }

        status_t Dir::remove(const LSPString *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

#ifdef PLATFORM_WINDOWS
            if (::RemoveDirectoryW(path->get_utf16()))
                return STATUS_OK;

            // Analyze error code
            DWORD code = ::GetLastError();
            switch (code)
            {
                case ERROR_PATH_NOT_FOUND:
                    return STATUS_NOT_FOUND;
                case ERROR_DIR_NOT_EMPTY:
                    return STATUS_NOT_EMPTY;
                default:
                    return STATUS_IO_ERROR;
            }
#else
            // Try to remove directory
            if (::rmdir(path->get_native()) == 0)
                return STATUS_OK;

            // Analyze error code
            int code = errno;
            switch (code)
            {
                case EACCES:
                case EPERM:
                    return STATUS_PERMISSION_DENIED;
                case EDQUOT:
                case ENOSPC:
                    return STATUS_OVERFLOW;
                case EFAULT:
                case EINVAL:
                case ENAMETOOLONG:
                    return STATUS_BAD_ARGUMENTS;
                case ENOTDIR:
                    return STATUS_NOT_DIRECTORY;
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

        status_t Dir::get_current(LSPString *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

#ifdef PLATFORM_WINDOWS
            DWORD len = ::GetCurrentDirectoryW(0, NULL);
            if (len == 0)
                return STATUS_UNKNOWN_ERR;

            WCHAR *buf = reinterpret_cast<WCHAR *>(::malloc(sizeof(WCHAR) * (len + 1)));
            if (buf == NULL)
                return STATUS_NO_MEM;

            len = ::GetCurrentDirectoryW(len, buf);
            if (len == 0)
            {
                ::free(buf);
                return STATUS_UNKNOWN_ERR;
            }

            status_t res = (path->set_utf16(buf, len)) ? STATUS_OK : STATUS_NO_MEM;
            ::free(buf);
            return res;
#else
            char spath[PATH_MAX];
            char *p = ::getcwd(spath, PATH_MAX);
            if (p == NULL)
            {
                int code = errno;
                switch (code)
                {
                    case EACCES:
                    case EPERM:
                        return STATUS_PERMISSION_DENIED;
                    case ENOENT:
                        return STATUS_REMOVED;
                    case ENAMETOOLONG:
                        return STATUS_OVERFLOW;
                    case ENOMEM:
                        return STATUS_NO_MEM;
                    default:
                        return STATUS_IO_ERROR;
                }
            }

            return (path->set_native(p)) ? STATUS_OK : STATUS_NO_MEM;
#endif /* PLATFORM_WINDOWS */
        }

        status_t Dir::get_current(Path *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPString tmp;
            status_t res = get_current(&tmp);
            if (res == STATUS_OK)
                path->take(&tmp);
            return res;
        }
    
    } /* namespace io */
} /* namespace lsp */
