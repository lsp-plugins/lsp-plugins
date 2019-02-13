/*
 * Path.h
 *
 *  Created on: 8 февр. 2019 г.
 *      Author: sadko
 */

#ifndef INCLUDE_CORE_IO_PATH_H_
#define INCLUDE_CORE_IO_PATH_H_

#include <core/status.h>
#include <core/LSPString.h>

namespace lsp
{
    namespace io
    {
        class Path
        {
            private:
                LSPString   sPath;

            public:
                explicit Path();
                ~Path();
                
            public:
                status_t    set(const char *path);
                status_t    set(const LSPString *path);
                status_t    set(const Path *path);

                inline const char *get() const                          { return sPath.get_utf8();  }
                status_t    get(char *path, size_t maxlen) const;
                status_t    get(LSPString *path) const;
                status_t    get(Path *path) const;

                status_t    set_last(const char *path);
                status_t    set_last(const LSPString *path);
                status_t    set_last(const Path *path);

                status_t    get_last(char *path, size_t maxlen) const;
                status_t    get_last(LSPString *path) const;
                status_t    get_last(Path *path) const;

                status_t    get_parent(char *path, size_t maxlen) const;
                status_t    get_parent(LSPString *path) const;
                status_t    get_parent(Path *path) const;

                status_t    set_parent(const char *path);
                status_t    set_parent(LSPString *path);
                status_t    set_parent(Path *path);

                status_t    concat(const char *path);
                status_t    concat(LSPString *path);
                status_t    concat(Path *path);

                status_t    append_child(const char *path);
                status_t    append_child(LSPString *path);
                status_t    append_child(Path *path);

                status_t    remove_last();
                status_t    remove_last(char *path, size_t maxlen);
                status_t    remove_last(LSPString *path);
                status_t    remove_last(Path *path);

                status_t    remove_base(const char *path);
                status_t    remove_base(const LSPString *path);
                status_t    remove_base(const Path *path);

                bool        is_absolute() const;
                bool        is_relative() const;
                bool        is_canonical() const;
                bool        is_root() const;
                inline bool is_empty() const                        { return sPath.is_empty();  }

                inline void clear()                                 { sPath.clear();    }

                inline void swap(Path *path)                        { sPath.swap(&path->sPath); }

                status_t    canonicalize();

                status_t    root();

                status_t    get_canonical(char *path, size_t maxlen) const;
                status_t    get_canonical(LSPString *path) const;
                status_t    get_canonical(Path *path) const;

                inline status_t as_relative(const char *path)       { return remove_base(path); }
                inline status_t as_relative(const LSPString *path)  { return remove_base(path); }
                inline status_t as_relative(const Path *path)       { return remove_base(path); }

                bool        equals(const Path *path) const;
                bool        equals(const LSPString *path) const;
                bool        equals(const char *path) const;
        };
    }
} /* namespace lsp */

#endif /* INCLUDE_CORE_IO_PATH_H_ */
