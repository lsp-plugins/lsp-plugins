/*
 * Dir.cpp
 *
 *  Created on: 18 мар. 2019 г.
 *      Author: sadko
 */

#include <core/io/Dir.h>
#include <errno.h>

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
