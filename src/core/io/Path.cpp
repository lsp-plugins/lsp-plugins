/*
 * Path.cpp
 *
 *  Created on: 8 февр. 2019 г.
 *      Author: sadko
 */

#include <core/io/Path.h>
#include <string.h>

#if defined(PLATFORM_WINDOWS)
    #include <windows.h>
    #include <shlwapi.h>
#endif /* defined(PLATFORM_WINDOWS) */

namespace lsp
{
    namespace io
    {
        Path::Path()
        {
        }
        
        Path::~Path()
        {
        }

        status_t Path::set(const char *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            return (sPath.set_utf8(path)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Path::set(const LSPString *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            return (sPath.set(path)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Path::set(const Path *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            return (sPath.set(&path->sPath)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Path::get(char *path, size_t maxlen) const
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            const char *utf8 = sPath.get_utf8();
            if (utf8 == NULL)
                return STATUS_NO_MEM;

            size_t len = ::strlen(utf8);
            if (len >= maxlen)
                return STATUS_TOO_BIG;

            ::memcpy(path, utf8, len+1); // Copy including '\0' character
            return STATUS_OK;
        }

        status_t Path::get(LSPString *path) const
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            return (path->set(&sPath)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Path::get(Path *path) const
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            return (path->sPath.set(&sPath)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Path::set_last(const char *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            else if (strlen(path) == 0)
                return remove_last();

            ssize_t len     = sPath.length();
            ssize_t idx     = sPath.rindex_of(FILE_SEPARATOR_C);
            idx             = (idx < 0) ? 0 : idx + 1;
            sPath.set_length(idx);
            if (sPath.append_utf8(path))
                return STATUS_OK;

            sPath.set_length(len);
            return STATUS_NO_MEM;
        }

        status_t Path::set_last(const LSPString *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            else if (path->length() <= 0)
                return remove_last();

            ssize_t len     = sPath.length();
            ssize_t idx     = sPath.rindex_of(FILE_SEPARATOR_C);
            idx             = (idx < 0) ? 0 : idx + 1;
            sPath.set_length(idx);
            if (sPath.append(path))
                return STATUS_OK;

            sPath.set_length(len);
            return STATUS_NO_MEM;
        }

        status_t Path::set_last(const Path *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            else if (path->sPath.length() <= 0)
                return remove_last();

            ssize_t len     = sPath.length();
            ssize_t idx     = sPath.rindex_of(FILE_SEPARATOR_C);
            idx             = (idx < 0) ? 0 : idx + 1;
            sPath.set_length(idx);
            if (sPath.append(&path->sPath))
                return STATUS_OK;

            sPath.set_length(len);
            return STATUS_NO_MEM;
        }

        status_t Path::get_last(char *path, size_t maxlen) const
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            ssize_t idx = sPath.rindex_of(FILE_SEPARATOR_C);
            idx     = (idx < 0) ? 0 : idx + 1;

            const char *utf8 = sPath.get_utf8(idx);
            if (utf8 == NULL)
                return STATUS_NO_MEM;

            size_t len = ::strlen(utf8);
            if (len >= maxlen)
                return STATUS_TOO_BIG;

            ::memcpy(path, utf8, len + 1);
            return STATUS_OK;
        }

        status_t Path::get_last(LSPString *path) const
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            ssize_t idx = sPath.rindex_of(FILE_SEPARATOR_C);
            idx     = (idx < 0) ? 0 : idx + 1;

            return (path->set(&sPath, idx)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Path::get_last(Path *path) const
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            ssize_t idx = sPath.rindex_of(FILE_SEPARATOR_C);
            idx     = (idx < 0) ? 0 : idx + 1;

            return (path->sPath.set(&sPath, idx)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Path::get_parent(char *path, size_t maxlen) const
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            else if (is_root())
                return STATUS_NOT_FOUND;

            ssize_t idx = sPath.rindex_of(FILE_SEPARATOR_C);
            if (idx < 0)
                return STATUS_NOT_FOUND;

            const char *utf8 = sPath.get_utf8(0, idx);
            if (utf8 == NULL)
                return STATUS_NO_MEM;

            size_t len = ::strlen(utf8);
            if (len >= maxlen)
                return STATUS_TOO_BIG;

            ::memcpy(path, utf8, len + 1);
            return STATUS_OK;
        }

        status_t Path::get_parent(LSPString *path) const
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            else if (is_root())
                return STATUS_NOT_FOUND;

            ssize_t idx = sPath.rindex_of(FILE_SEPARATOR_C);
            if (idx < 0)
                return STATUS_NOT_FOUND;

            return (path->set(&sPath, 0, idx)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Path::get_parent(Path *path) const
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            else if (is_root())
                return STATUS_NOT_FOUND;

            ssize_t idx = sPath.rindex_of(FILE_SEPARATOR_C);
            if (idx < 0)
                return STATUS_NOT_FOUND;

            return (path->sPath.set(&sPath, 0, idx)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Path::set_parent(const char *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            else if (is_root())
                return STATUS_BAD_STATE;

            LSPString tmp;
            if (!tmp.set_utf8(path))
                return STATUS_NO_MEM;
            while (tmp.ends_with(FILE_SEPARATOR_C))
                tmp.set_length(tmp.length() - 1);

            bool success = tmp.append(FILE_SEPARATOR_C);
            if (success)
                success = tmp.append(&sPath);
            if (success)
                sPath.swap(&tmp);
            return (success) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Path::set_parent(LSPString *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            else if (is_root())
                return STATUS_BAD_STATE;

            LSPString tmp;
            if (!tmp.set(path))
                return STATUS_NO_MEM;
            while (tmp.ends_with(FILE_SEPARATOR_C))
                tmp.set_length(tmp.length() - 1);

            bool success = tmp.append(FILE_SEPARATOR_C);
            if (success)
                success = tmp.append(&sPath);
            if (success)
                sPath.swap(&tmp);
            return (success) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Path::set_parent(Path *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            else if (is_root())
                return STATUS_BAD_STATE;

            LSPString tmp;
            if (!tmp.set(&path->sPath))
                return STATUS_NO_MEM;
            while (tmp.ends_with(FILE_SEPARATOR_C))
                tmp.set_length(tmp.length() - 1);

            bool success = tmp.append(FILE_SEPARATOR_C);
            if (success)
                success = tmp.append(&sPath);
            if (success)
                sPath.swap(&tmp);
            return (success) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Path::concat(const char *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            return (sPath.append_utf8(path)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Path::concat(LSPString *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            return (sPath.append(path)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Path::concat(Path *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            return (sPath.append(&path->sPath)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Path::append_child(const char *path)
        {
            Path tmp;
            status_t res = tmp.set(path);
            if (res != STATUS_OK)
                return res;
            else if (tmp.is_empty())
                return STATUS_OK;
            else if (tmp.is_root())
                return STATUS_INVALID_VALUE;

            size_t len = sPath.length();
            bool success = (sPath.ends_with(FILE_SEPARATOR_C)) ? true : sPath.append(FILE_SEPARATOR_C);
            if (success)
                success = sPath.append(&tmp.sPath);
            if (!success)
                sPath.set_length(len);
            return (success) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Path::append_child(LSPString *path)
        {
            Path tmp;
            status_t res = tmp.set(path);
            if (res != STATUS_OK)
                return res;
            else if (tmp.is_empty())
                return STATUS_OK;
            else if (tmp.is_root())
                return STATUS_INVALID_VALUE;

            size_t len = sPath.length();
            bool success = (sPath.ends_with(FILE_SEPARATOR_C)) ? true : sPath.append(FILE_SEPARATOR_C);
            if (success)
                success = sPath.append(&tmp.sPath);
            if (!success)
                sPath.set_length(len);
            return (success) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Path::append_child(Path *path)
        {
            Path tmp;
            status_t res = tmp.set(path);
            if (res != STATUS_OK)
                return res;
            else if (tmp.is_empty())
                return STATUS_OK;
            else if (tmp.is_root())
                return STATUS_INVALID_VALUE;

            size_t len = sPath.length();
            bool success = (sPath.ends_with(FILE_SEPARATOR_C)) ? true : sPath.append(FILE_SEPARATOR_C);
            if (success)
                success = sPath.append(&tmp.sPath);
            if (!success)
                sPath.set_length(len);
            return (success) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Path::remove_last()
        {
            if (is_root())
                return STATUS_OK;

            ssize_t idx     = sPath.rindex_of(FILE_SEPARATOR_C);
            if (is_relative())
            {
                if (idx < 0)
                    idx             = 0;
                sPath.set_length(idx);
            }
            else if (idx > 0)
            {
                ssize_t idx2    = sPath.rindex_of(idx - 1, FILE_SEPARATOR_C);
                if (idx2 < 0)
                    idx             = idx + 1;
                sPath.set_length(idx);
            }
            return STATUS_OK;
        }

        status_t Path::remove_last(char *path, size_t maxlen)
        {
            Path tmp;
            status_t res = tmp.set(&this->sPath);
            if (res == STATUS_OK)
                res         = tmp.remove_last();
            if (res == STATUS_OK)
                res         = tmp.get(path, maxlen);
            return res;
        }

        status_t Path::remove_last(LSPString *path)
        {
            Path tmp;
            status_t res = tmp.set(&this->sPath);
            if (res == STATUS_OK)
                res         = tmp.remove_last();
            if (res == STATUS_OK)
                res         = tmp.get(path);
            return res;
        }

        status_t Path::remove_last(Path *path)
        {
            Path tmp;
            status_t res = tmp.set(&this->sPath);
            if (res == STATUS_OK)
                res         = tmp.remove_last();
            if (res == STATUS_OK)
                res         = tmp.get(path);
            return res;
        }

        status_t Path::root()
        {
#if defined(PLATFORM_WINDOWS)
            if (!is_absolute())
                return STATUS_BAD_STATE;
            ssize_t idx = sPath.index_of(FILE_SEPARATOR_C);
            if (idx < 0)
                return STATUS_BAD_STATE;
            sPath.set_length(idx);
            return STATUS_OK;
#else
            return (sPath.set(FILE_SEPARATOR_C)) ? STATUS_OK : STATUS_NO_MEM;
#endif
        }

        status_t Path::remove_base(const char *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;

            LSPString tmp;
            if (!tmp.set_utf8(path))
                return STATUS_NO_MEM;
            return remove_base(&tmp);
        }

        status_t Path::remove_base(const LSPString *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (!sPath.starts_with(path))
                return false;
            size_t index = path->length(), max = sPath.length();
            if (index >= max)
            {
                sPath.clear();
                return STATUS_OK;
            }

            size_t removed = 0;
            while (index < max)
            {
                if (sPath.char_at(index) != FILE_SEPARATOR_C)
                    break;
                ++removed;
                ++index;
            }
            if (removed <= 0)
                return STATUS_INVALID_VALUE;

            LSPString tmp;
            if (!tmp.set(&sPath, index, max))
                return STATUS_NO_MEM;
            sPath.swap(&tmp);
            return STATUS_OK;
        }

        status_t Path::remove_base(const Path *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            return remove_base(&path->sPath);
        }

        bool Path::is_absolute() const
        {
#if defined(PLATFORM_WINDOWS)
            return !PathIsRelativeW(reinterpret_cast<LPCWSTR>(sPath.get_utf16()));
#else
            return (sPath.first() == FILE_SEPARATOR_C);
#endif
        }

        bool Path::is_relative() const
        {
#if defined(PLATFORM_WINDOWS)
            return PathIsRelativeW(reinterpret_cast<LPCWSTR>(sPath.get_utf16()));
#else
            return (sPath.first() != FILE_SEPARATOR_C);
#endif
        }

        bool Path::is_canonical() const
        {
            lsp_wchar_t c;
            ssize_t start = 0, len = sPath.length();

            while (true)
            {
                start = sPath.index_of(start, FILE_SEPARATOR_C);
                if (start < 0)
                    return true;

                // "/", "./", "../"
                if ((++start) >= len)
                    return true;
                c = sPath.char_at(start);
                if (c == FILE_SEPARATOR_C)
                    return false;
                else if (c != '.')
                    continue;

                // "/", "./"
                if ((++start) >= len)
                    return true;
                c = sPath.char_at(start);
                if (c == FILE_SEPARATOR_C)
                    return false;
                else if (c != '.')
                    continue;

                // "/"
                if ((++start) >= len)
                    return true;
                c = sPath.char_at(start);
                if (c == FILE_SEPARATOR_C)
                    return false;
                ++start;
            }

            return false;
        }

        bool Path::is_root() const
        {
#if defined(PLATFORM_WINDOWS)
            return PathIsRootW(reinterpret_cast<LPCWSTR>(sPath.get_utf16()));
#else
            return (sPath.length() == 1) &&
                    (sPath.first() == FILE_SEPARATOR_C);
#endif
        }

        enum state_t
        {
            S_SEEK,
            S_DOT,
            S_DOTDOT,
            S_PATHNAME
        };

        status_t Path::canonicalize()
        {
#if defined(PLATFORM_WINDOWS)
            WCHAR path[PATH_MAX];
            if (!PathCanonicalizeW(path, reinterpret_cast<LPCWSTR>(sPath.get_utf16())))
                return STATUS_BAD_STATE;
            return (sPath.set_utf16(reinterpret_cast<uint16_t *>(path))) ? STATUS_OK : STATUS_NO_MEM;
#else
            size_t len  = sPath.length();
            if (len <= 0)
                return STATUS_OK;

            lsp_wchar_t *start  = const_cast<lsp_wchar_t *>(sPath.characters());
            lsp_wchar_t *end    = &start[len];
            if (*start == FILE_SEPARATOR_C)
                ++ start;

            lsp_wchar_t *wptr   = start; // Write pointer
            lsp_wchar_t *cptr   = start; // Commit pointer
            state_t state       = S_SEEK;

            for (lsp_wchar_t *rptr; rptr < end; ++rptr)
            {
                switch (state)
                {
                    case S_SEEK:
                        if (*rptr == FILE_SEPARATOR_C)
                            cptr        = rptr + 1; // Move commit pointer
                        else if (*rptr == '.')
                            state       = S_DOT;
                        else
                            state       = S_PATHNAME;
                        break;
                    case S_DOT:
                        if (*rptr == FILE_SEPARATOR_C)
                        {
                            cptr        = rptr + 1; // Move commit pointer
                            state       = S_SEEK;
                        }
                        else if (*rptr == '.')
                            state       = S_DOTDOT;
                        else
                            state       = S_PATHNAME;
                        break;
                    case S_DOTDOT:
                        if (*rptr == FILE_SEPARATOR_C)
                        {
                            if (wptr <= start)
                                break;
                            if (*wptr == FILE_SEPARATOR_C)
                                --wptr;
                            while ((wptr > start) && (*wptr != FILE_SEPARATOR_C))
                                --wptr;
                        }
                        else
                            state       = S_PATHNAME;
                        break;
                    case S_PATHNAME:
                        if (*rptr == FILE_SEPARATOR_C)
                        {
                            if (wptr < cptr)
                            {
                                while (cptr <= rptr)
                                    *(wptr++) = *(cptr++);
                            }
                            else
                            {
                                cptr        = rptr + 1;
                                wptr        = rptr + 1;
                            }
                        }
                        state       = S_SEEK;
                        break;
                }
            }

            if ((cptr < end) && (wptr < cptr))
            {
                while (cptr <= end)
                    *(wptr++) = *(cptr++);
            }

            if ((wptr > start) && (*wptr == FILE_SEPARATOR_C))
                --wptr;

            sPath.set_length(wptr - start);

            return STATUS_OK;
#endif
        }

        status_t Path::get_canonical(char *path, size_t maxlen) const
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            Path tmp;
            status_t res = tmp.set(&sPath);
            if (res == STATUS_OK)
                res     = tmp.canonicalize();
            if (res == STATUS_OK)
                res     = tmp.get(path, maxlen);
            return res;
        }

        status_t Path::get_canonical(LSPString *path) const
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            Path tmp;
            status_t res = tmp.set(&sPath);
            if (res == STATUS_OK)
                res     = tmp.canonicalize();
            if (res == STATUS_OK)
                res     = tmp.get(path);
            return res;
        }

        status_t Path::get_canonical(Path *path) const
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            Path tmp;
            status_t res = tmp.set(&sPath);
            if (res == STATUS_OK)
                res     = tmp.canonicalize();
            if (res == STATUS_OK)
                res     = tmp.get(path);
            return res;
        }

        bool Path::equals(const Path *path) const
        {
            return (path != NULL) ? sPath.equals(&path->sPath) : false;
        }

        bool Path::equals(const LSPString *path) const
        {
            return (path != NULL) ? sPath.equals(path) : false;
        }

        bool Path::equals(const char *path) const
        {
            if (path == NULL)
                return false;

            LSPString tmp;
            return (tmp.set_utf8(path)) ? tmp.equals(&sPath) : false;
        }
    }
} /* namespace lsp */
