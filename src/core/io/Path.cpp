/*
 * Path.cpp
 *
 *  Created on: 8 февр. 2019 г.
 *      Author: sadko
 */

#include <core/io/Path.h>
#include <core/io/File.h>
#include <core/io/Dir.h>
#include <string.h>

#if defined(PLATFORM_WINDOWS)
    #include <windows.h>
    #include <shlwapi.h>
#else
    #include <fcntl.h>
    #include <sys/stat.h>
    #include <errno.h>
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

        inline void Path::fixup_path()
        {
#ifdef PLATFORM_WINDOWS
            sPath.replace_all('/', '\\');
#else
            sPath.replace_all('\\', '/');
#endif /* PLATFORM_WINDOWS */
        }

        status_t Path::set(const char *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (!sPath.set_utf8(path))
                return STATUS_NO_MEM;
            fixup_path();
            return STATUS_OK;
        }

        status_t Path::set(const LSPString *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (!sPath.set(path))
                return STATUS_NO_MEM;
            fixup_path();
            return STATUS_OK;
        }

        status_t Path::set(const Path *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (!sPath.set(&path->sPath))
                return STATUS_NO_MEM;
            fixup_path();
            return STATUS_OK;
        }

        status_t Path::set(const char *path, const char *child)
        {
            Path tmp;
            status_t res = tmp.set(path);
            if (res == STATUS_OK)
                res = tmp.append_child(child);
            if (res == STATUS_OK)
                swap(&tmp);
            return res;
        }

        status_t Path::set(const char *path, const LSPString *child)
        {
            Path tmp;
            status_t res = tmp.set(path);
            if (res == STATUS_OK)
                res = tmp.append_child(child);
            if (res == STATUS_OK)
                swap(&tmp);
            return res;
        }

        status_t Path::set(const char *path, const Path *child)
        {
            Path tmp;
            status_t res = tmp.set(path);
            if (res == STATUS_OK)
                res = tmp.append_child(child);
            if (res == STATUS_OK)
                swap(&tmp);
            return res;
        }

        status_t Path::set(const LSPString *path, const char *child)
        {
            Path tmp;
            status_t res = tmp.set(path);
            if (res == STATUS_OK)
                res = tmp.append_child(child);
            if (res == STATUS_OK)
                swap(&tmp);
            return res;
        }

        status_t Path::set(const LSPString *path, const LSPString *child)
        {
            Path tmp;
            status_t res = tmp.set(path);
            if (res == STATUS_OK)
                res = tmp.append_child(child);
            if (res == STATUS_OK)
                swap(&tmp);
            return res;
        }

        status_t Path::set(const LSPString *path, const Path *child)
        {
            Path tmp;
            status_t res = tmp.set(path);
            if (res == STATUS_OK)
                res = tmp.append_child(child);
            if (res == STATUS_OK)
                swap(&tmp);
            return res;
        }

        status_t Path::set(const Path *path, const char *child)
        {
            Path tmp;
            status_t res = tmp.set(path);
            if (res == STATUS_OK)
                res = tmp.append_child(child);
            if (res == STATUS_OK)
                swap(&tmp);
            return res;
        }

        status_t Path::set(const Path *path, const LSPString *child)
        {
            Path tmp;
            status_t res = tmp.set(path);
            if (res == STATUS_OK)
                res = tmp.append_child(child);
            if (res == STATUS_OK)
                swap(&tmp);
            return res;
        }

        status_t Path::set(const Path *path, const Path *child)
        {
            Path tmp;
            status_t res = tmp.set(path);
            if (res == STATUS_OK)
                res = tmp.append_child(child);
            if (res == STATUS_OK)
                swap(&tmp);
            return res;
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
            {
                fixup_path();
                return STATUS_OK;
            }

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
            {
                fixup_path();
                return STATUS_OK;
            }

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
            {
                fixup_path();
                return STATUS_OK;
            }

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
            {
                sPath.swap(&tmp);
                fixup_path();
            }
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
            {
                sPath.swap(&tmp);
                fixup_path();
            }
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
            {
                sPath.swap(&tmp);
                fixup_path();
            }
            return (success) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Path::concat(const char *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (!sPath.append_utf8(path))
                return STATUS_NO_MEM;
            fixup_path();
            return STATUS_OK;
        }

        status_t Path::concat(LSPString *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (!sPath.append(path))
                return STATUS_NO_MEM;
            fixup_path();
            return STATUS_OK;
        }

        status_t Path::concat(Path *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (!sPath.append(&path->sPath))
                return STATUS_NO_MEM;
            fixup_path();
            return STATUS_OK;
        }

        status_t Path::append_child(const char *path)
        {
            Path tmp;
            status_t res = tmp.set(path);
            if (res != STATUS_OK)
                return res;
            else if (tmp.is_empty())
                return STATUS_OK;
            else if (tmp.is_absolute())
                return STATUS_INVALID_VALUE;

            size_t len = sPath.length();
            bool success = ((len <= 0) || (sPath.ends_with(FILE_SEPARATOR_C))) ? true : sPath.append(FILE_SEPARATOR_C);
            if (success)
                success = sPath.append(&tmp.sPath);
            if (success)
                fixup_path();
            else
                sPath.set_length(len);

            return (success) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Path::append_child(const LSPString *path)
        {
            Path tmp;
            status_t res = tmp.set(path);
            if (res != STATUS_OK)
                return res;
            else if (tmp.is_empty())
                return STATUS_OK;
            else if (tmp.is_absolute())
                return STATUS_INVALID_VALUE;

            size_t len = sPath.length();
            bool success = ((len <= 0) || (sPath.ends_with(FILE_SEPARATOR_C))) ? true : sPath.append(FILE_SEPARATOR_C);
            if (success)
                success = sPath.append(&tmp.sPath);
            if (success)
                fixup_path();
            else
                sPath.set_length(len);

            return (success) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Path::append_child(const Path *path)
        {
            if (path == NULL)
                return STATUS_BAD_ARGUMENTS;
            else if (path->is_empty())
                return STATUS_OK;
            else if (path->is_absolute())
                return STATUS_INVALID_VALUE;

            size_t len = sPath.length();
            bool success = ((len <= 0) || (sPath.ends_with(FILE_SEPARATOR_C))) ? true : sPath.append(FILE_SEPARATOR_C);
            if (success)
                success = sPath.append(&path->sPath);
            if (success)
                fixup_path();
            else
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
            if (is_relative())
                return STATUS_BAD_STATE;
            else if (is_root())
                return STATUS_OK;
#if defined(PLATFORM_WINDOWS)
            ssize_t idx = sPath.index_of(FILE_SEPARATOR_C);
            if (idx < 0)
                return STATUS_BAD_STATE;
            sPath.set_length(idx+1);
            return STATUS_OK;
#else
            return (sPath.set(FILE_SEPARATOR_C)) ? STATUS_OK : STATUS_NO_MEM;
#endif
        }

        status_t Path::parent()
        {
            if (is_root())
                return STATUS_OK;
            ssize_t idx = sPath.rindex_of(FILE_SEPARATOR_C);
            if (idx < 0)
                idx = 0;
            sPath.set_length(idx);
            return STATUS_OK;
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
            if (sPath.length() <= 0)
                return false;
#if defined(PLATFORM_WINDOWS)
            return !PathIsRelativeW(reinterpret_cast<LPCWSTR>(sPath.get_utf16()));
#else
            return (sPath.first() == FILE_SEPARATOR_C);
#endif
        }

        bool Path::is_dot() const
        {
            ssize_t idx = sPath.rindex_of(FILE_SEPARATOR_C);
            if (idx > 0)
            {
                ssize_t len = sPath.length() - 2;
                if (idx != len)
                    return false;
            }
            else if (sPath.length() != 1)
                return false;
            return sPath.last() == '.';
        }

        bool Path::is_dotdot() const
        {
            ssize_t idx = sPath.rindex_of(FILE_SEPARATOR_C);
            if (idx > 0)
            {
                ssize_t len = sPath.length() - 3;
                if (idx != len)
                    return false;

                return (sPath.char_at(len + 1) == '.') &&
                        (sPath.char_at(len + 2) == '.');
            }
            else if (sPath.length() != 2)
                return false;

            return (sPath.first() == '.') && (sPath.last() == '.');
        }

        bool Path::is_relative() const
        {
            if (sPath.length() <= 0)
                return true;
#if defined(PLATFORM_WINDOWS)
            return ::PathIsRelativeW(reinterpret_cast<LPCWSTR>(sPath.get_utf16()));
#else
            return (sPath.first() != FILE_SEPARATOR_C);
#endif
        }

        bool Path::is_canonical() const
        {
            enum state_t
            {
                S_SEEK,
                S_SEPARATOR,
                S_DOT,
                S_DOTDOT
            };

            if (is_root())
                return true;

            lsp_wchar_t c;
            size_t len              = sPath.length();
            const lsp_wchar_t *p    = sPath.characters();
            const lsp_wchar_t *e    = &p[len];
            state_t state           = S_SEEK;

            while (p < e)
            {
                c  = *p++;

                switch (state)
                {
                    case S_SEEK:
                        if (c == FILE_SEPARATOR_C)
                            state       = S_SEPARATOR;
                        else if (c == '.')
                            state       = S_DOT;
                        break;
                    case S_SEPARATOR:
                        if (c == FILE_SEPARATOR_C)
                            return false;
                        else if (c == '.')
                            state       = S_DOT;
                        else
                            state       = S_SEEK;
                        break;
                    case S_DOT:
                        if (c == FILE_SEPARATOR_C)
                            return false;
                        else if (c == '.')
                            state       = S_DOTDOT;
                        else
                            state       = S_SEEK;
                        break;
                    case S_DOTDOT:
                        if (c == FILE_SEPARATOR_C)
                            return false;
                        else
                            state       = S_SEEK;
                        break;
                }
            }

            return state == S_SEEK;
        }

        bool Path::is_root() const
        {
#if defined(PLATFORM_WINDOWS)
            return ::PathIsRootW(reinterpret_cast<LPCWSTR>(sPath.get_utf16()));
#else
            return (sPath.length() == 1) &&
                    (sPath.first() == FILE_SEPARATOR_C);
#endif
        }

        status_t Path::canonicalize()
        {
            enum state_t
            {
                S_SEEK,
                S_SEPARATOR,
                S_DOT,
                S_DOTDOT
            };

            lsp_wchar_t c;
            size_t len              = sPath.length();
            lsp_wchar_t *s          = const_cast<lsp_wchar_t *>(sPath.characters());
            lsp_wchar_t *e          = &s[len];
            state_t state           = S_SEEK;

            if (is_absolute())
            {
                while (*(s++) != FILE_SEPARATOR_C)
                    /* loop */ ;
                state               = S_SEPARATOR;
            }

            lsp_wchar_t *p          = s;
            lsp_wchar_t *w          = s;

            while (p < e)
            {
                c  = *p++;

                switch (state)
                {
                    case S_SEEK:
                        if (c == FILE_SEPARATOR_C)
                        {
                            state       = S_SEPARATOR;
                            *w++        = c;
                        }
                        else if (c == '.')
                            state       = S_DOT;
                        else
                            *w++    = c;
                        break;
                    case S_SEPARATOR:
                        if (c == FILE_SEPARATOR_C)
                            break;
                        else if (c == '.')
                            state       = S_DOT;
                        else
                        {
                            *w++    = c;
                            state   = S_SEEK;
                        }
                        break;
                    case S_DOT:
                        if (c == FILE_SEPARATOR_C)
                            state       = S_SEPARATOR;
                        else if (c == '.')
                            state       = S_DOTDOT;
                        else
                        {
                            *w++        = '.';
                            *w++        = c;
                            state       = S_SEEK;
                        }
                        break;
                    case S_DOTDOT:
                        if (c == FILE_SEPARATOR_C)
                        {
                            state       = S_SEPARATOR;
                            do // Roll-back path
                            {
                                if (w <= s)
                                    break;
                                --w;
                            }
                            while (w[-1] != FILE_SEPARATOR_C);
                        }
                        else
                        {
                            *w++        = '.';
                            *w++        = '.';
                            *w++        = c;
                            state       = S_SEEK;
                        }
                        break;
                }
            }

            while ((w > s) && (w[-1] == FILE_SEPARATOR_C))
                --w;

            sPath.set_length(w - const_cast<lsp_wchar_t *>(sPath.characters()));

            return STATUS_OK;
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
                tmp.sPath.swap(path);
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
                tmp.swap(path);
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

        status_t Path::stat(fattr_t *attr) const
        {
            return File::stat(&sPath, attr);
        }

        status_t Path::sym_stat(fattr_t *attr) const
        {
            return File::sym_stat(&sPath, attr);
        }

        wssize_t Path::size() const
        {
            fattr_t attr;
            status_t res = File::stat(&sPath, &attr);
            return (res != STATUS_OK) ? attr.size : -res;
        }

        bool Path::exists() const
        {
            fattr_t attr;
            status_t res = File::stat(&sPath, &attr);
            return res == STATUS_OK;
        }

        bool Path::is_reg() const
        {
            fattr_t attr;
            status_t res = File::sym_stat(&sPath, &attr);
            return (res == STATUS_OK) && (attr.type == fattr_t::FT_BLOCK);
        }

        bool Path::is_dir() const
        {
            fattr_t attr;
            status_t res = File::sym_stat(&sPath, &attr);
            return (res == STATUS_OK) && (attr.type == fattr_t::FT_DIRECTORY);
        }

        bool Path::is_block_dev() const
        {
            fattr_t attr;
            status_t res = File::sym_stat(&sPath, &attr);
            return (res == STATUS_OK) && (attr.type == fattr_t::FT_BLOCK);
        }

        bool Path::is_char_dev() const
        {
            fattr_t attr;
            status_t res = File::sym_stat(&sPath, &attr);
            return (res == STATUS_OK) && (attr.type == fattr_t::FT_CHARACTER);
        }

        bool Path::is_fifo() const
        {
            fattr_t attr;
            status_t res = File::sym_stat(&sPath, &attr);
            return (res == STATUS_OK) && (attr.type == fattr_t::FT_FIFO);
        }

        bool Path::is_symlink() const
        {
            fattr_t attr;
            status_t res = File::stat(&sPath, &attr);
            return (res == STATUS_OK) && (attr.type == fattr_t::FT_SYMLINK);
        }

        bool Path::is_socket() const
        {
            fattr_t attr;
            status_t res = File::sym_stat(&sPath, &attr);
            return (res == STATUS_OK) && (attr.type == fattr_t::FT_SOCKET);
        }

        status_t Path::mkdir() const
        {
            return Dir::create(&sPath);
        }

        status_t Path::mkdir(bool recursive) const
        {
            // Try to create directory
            status_t res = Dir::create(&sPath);
            if ((res == STATUS_OK) || (!recursive))
                return res;

            // No success?
            // First, canonicalize path
            Path path;
            path.set(this);
            res = path.canonicalize();
            if (res != STATUS_OK)
                return res;

            // Prepare the loopp
            LSPString tmp;
            ssize_t off = path.sPath.index_of(0, FILE_SEPARATOR_C);
            if (off < 0)
                return STATUS_INVALID_VALUE;
            else if (path.is_absolute())
            {
                off = path.sPath.index_of(off+1, FILE_SEPARATOR_C);
                if (off < 0) // Tried to create root directory?
                    return STATUS_OK;
            }

            // Perform iterative directory creation
            while (off >= 0)
            {
                if (!tmp.set(&path.sPath, 0, off))
                    return STATUS_NO_MEM;

                res = Dir::create(&tmp);
                if (res != STATUS_OK)
                    return res;

                // Lookup for next separator
                off     = path.sPath.index_of(off+1, FILE_SEPARATOR_C);
            }

            return Dir::create(&sPath);
        }

        status_t Path::remove() const
        {
            status_t res = File::remove(&sPath);
            if (res == STATUS_IS_DIRECTORY)
                res = Dir::remove(&sPath);
            return (res == STATUS_NOT_DIRECTORY) ? STATUS_IO_ERROR : res;
        }

        void Path::take(LSPString *src)
        {
            sPath.take(src);
            fixup_path();
        }

        status_t Path::current()
        {
            status_t res = Dir::get_current(&sPath);
            if (res == STATUS_OK)
                fixup_path();
            return res;
        }
    }
} /* namespace lsp */
