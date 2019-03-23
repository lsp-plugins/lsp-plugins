/*
 * libpath.h
 *
 *  Created on: 26 янв. 2019 г.
 *      Author: sadko
 */

#ifndef INCLUDE_CONTAINER_COMMON_LIBPATH_H_
#define INCLUDE_CONTAINER_COMMON_LIBPATH_H_

#include <core/types.h>

#ifndef PLATFORM_UNIX_COMPATIBLE
    #error "This header should be used for Unix-compatible OS only"
#endif /* PLATFORM_UNIX_COMPATIBLE */

#include <core/debug.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>

namespace lsp
{
    typedef struct getlibpath_path_t
    {
        char      **paths;
        size_t      np;
        size_t      cp;

        explicit getlibpath_path_t()
        {
            paths   = NULL;
            np      = 0;
            cp      = 0;
        }
    } getlibpath_path_t;

    typedef struct getlibpath_buf_t
    {
        char       *s;
        size_t      pos;
        size_t      size;
        size_t      cap;

        explicit getlibpath_buf_t()
        {
            s       = NULL;
            pos     = 0;
            size    = 0;
            cap     = 0;
        }

        ~getlibpath_buf_t()
        {
            if (s != NULL)
            {
                free(s);
                s = NULL;
            }
        }
    } getlibpath_buf_t;

    char *getlibpath_skip_space(char *p, char *end)
    {
        while (p < end)
        {
            if ((*p != ' ') && (*p != '\t'))
                return ((*p == '\n') || (*p == '\0')) ? end : p;
            ++p;
        }

        return end;
    }

    char *getlibpath_skip_field(char *p, char *end)
    {
        p = getlibpath_skip_space(p, end);

        while (p < end)
        {
            switch (*p)
            {
                case ' ': case '\t': return p;
                case '\0': case '\n': return end;
                default: break;
            }
            ++p;
        }
        return end;
    }

    char *getlibpath_trim_file(char *p, char *end)
    {
        if ((p < end) && (*p != FILE_SEPARATOR_C))
            return end;

        char *last = p; // Position of last file separator

        for (char *s = p; s < end; ++s)
        {
            if (*s == FILE_SEPARATOR_C)
                last = s;
        }

        *last = '\0';
        return (last > p) ? p : end;
    }

    bool getlibpath_check_presence(const char **paths, const char *path)
    {
        if (paths == NULL)
            return false;
        while (*paths != NULL)
        {
            if (strcmp(*paths, path) == 0)
                return true;
            paths++;
        }
        return false;
    }

    char *getlibpath_find_end(char *line, size_t len)
    {
        char *end = &line[len];
        while (line < end)
        {
            if (*line == '\n')
                return line;
            ++line;
        }
        return line;
    }

    bool getlibpath_add_path(getlibpath_path_t *res, const char *path, const char **exclude)
    {
        // Check if path is present
        if (getlibpath_check_presence(exclude, path))
            return true;
        if (getlibpath_check_presence(const_cast<const char **>(res->paths), path))
            return true;

        // Allocate new item in paths
        if (res->np >= res->cp)
        {
            char **npaths   = reinterpret_cast<char **>(realloc(res->paths, sizeof(char **) * (res->cp + 17)));
            if (npaths == NULL)
                return false;
            res->paths      = npaths;
            res->cp        += 16;
            for (size_t i=res->np; i<=res->cp; ++i)
                res->paths[i]   = NULL;
        }

        // Put item to paths
        char *dup   = strdup(path);
        if (dup == NULL)
            return false;
        res->paths[res->np++]   = dup;
        return true;
    }

    ssize_t getlibpath_getline(char **line, getlibpath_buf_t *buf, FILE *fd)
    {
        // Prepare buffer for reading
        if (buf->s == NULL)
        {
            // Allocate new data
            buf->s      = reinterpret_cast<char *>(malloc(0x10));
            if (buf->s == NULL)
                return -1;
            buf->cap    = 0x10;
            buf->size   = 0;
            buf->pos    = 0;
        }
        else
        {
            // Remove current line from buffer
            if (buf->pos < buf->size)
            {
                buf->size   -= buf->pos;
                if (buf->pos > 0)
                    ::memmove(buf->s, &buf->s[buf->pos], buf->size);
            }
            else
                buf->size   = 0;
            buf->pos    = 0;
        }

        // Perform reading cycle
        while (true)
        {
            // Scan buffer for line delimiter
            for ( ; buf->pos < buf->size; ++buf->pos)
                if (buf->s[buf->pos] == '\n')
                {
                    *line = buf->s;
                    buf->s[buf->pos] = '\0'; // Add line termination character
//                    lsp_trace("got line: %s", buf->s);
                    return buf->pos++;
                }

            // No more space in buffer, extend it
            if (buf->size >= buf->cap)
            {
                char *nl    = reinterpret_cast<char *>(realloc(buf->s, (buf->cap << 1) + 1));
                if (nl == NULL)
                    return -1;
                buf->s      = nl;
                buf->cap  <<= 1;
            }

            // No more data in buffer, try to read from file
            ssize_t n = fread(&buf->s[buf->size], sizeof(char), buf->cap - buf->size, fd);
            if (n <= 0)
            {
                if ((feof(fd)) && (buf->pos > 0))
                {
                    buf->s[buf->pos] = '\0'; // Add line termination character
                    *line = buf->s;
//                    lsp_trace("got line: %s", buf->s);
                    return buf->pos;
                }

                return -1;
            }

            // Update end pointer by amount of read bytes
            buf->size  += n;
        }
    }

#if defined(PLATFORM_LINUX)
    /**
     * Return additional unique paths where the core library can be located
     * @param exclude NULL-terminated list of strings for paths that should be excluded
     * @return NULL-terminated list of library paths
     */
    bool getlibpath_proc(char ***paths, const char **exclude)
    {
        // Open file for reading
        FILE *fd = fopen("/proc/self/maps", "r");
        if (fd == NULL)
            return NULL;

        char *line      = NULL;
        ssize_t len     = 0;
        getlibpath_path_t res;
        getlibpath_buf_t  buf;
        bool success    = true;

        while ((len = getlibpath_getline(&line, &buf, fd)) >= 0)
        {
            // -> 7fd376cca000-7fd376e7b000 r-xp 00000000 103:04 3276809                   /lib64/libc-2.26.so
            char *end   = &line[len];
            char *p     = getlibpath_skip_field(line, end); // -> r-xp 00000000 103:04 3276809                   /lib64/libc-2.26.so
            p           = getlibpath_skip_field(p, end); // -> 00000000 103:04 3276809                   /lib64/libc-2.26.so
            p           = getlibpath_skip_field(p, end); // -> 103:04 3276809                   /lib64/libc-2.26.so
            p           = getlibpath_skip_field(p, end); // -> 3276809                   /lib64/libc-2.26.so
            p           = getlibpath_skip_field(p, end); // ->                   /lib64/libc-2.26.so
            p           = getlibpath_skip_space(p, end); // /lib64/libc-2.26.so
            p           = getlibpath_trim_file(p, end);  // /lib64

            if (p == end)
                continue;
            if (!getlibpath_add_path(&res, p, exclude))
            {
                success = false;
                break;
            }
        }

        fclose(fd);

        if (success)
            *paths = res.paths;
        return success;
    }
#elif defined(PLATFORM_BSD)
    bool getlibpath_proc(char ***paths, const char **exclude)
    {
        // Open file for reading
        FILE *fd = fopen("/proc/curproc/map", "r");
        if (fd == NULL)
            return NULL;

        char *line      = NULL;
        ssize_t len     = 0;
        getlibpath_path_t res;
        getlibpath_buf_t  buf;
        bool success    = true;

        while ((len = getlibpath_getline(&line, &buf, fd)) >= 0)
        {
            // -> 0x800602000 0x800622000 32 0 0xfffff8000397e780 r-x 31 0 0x1000 COW NC vnode /libexec/ld-elf.so.1 NCH -1
            char *end   = &line[len];
            char *p     = getlibpath_skip_field(line, end); // -> 0x800622000 32 0 0xfffff8000397e780 r-x 31 0 0x1000 COW NC vnode /libexec/ld-elf.so.1 NCH -1
            p           = getlibpath_skip_field(p, end); // -> 32 0 0xfffff8000397e780 r-x 31 0 0x1000 COW NC vnode /libexec/ld-elf.so.1 NCH -1
            p           = getlibpath_skip_field(p, end); // -> 0 0xfffff8000397e780 r-x 31 0 0x1000 COW NC vnode /libexec/ld-elf.so.1 NCH -1
            p           = getlibpath_skip_field(p, end); // -> 0xfffff8000397e780 r-x 31 0 0x1000 COW NC vnode /libexec/ld-elf.so.1 NCH -1
            p           = getlibpath_skip_field(p, end); // -> r-x 31 0 0x1000 COW NC vnode /libexec/ld-elf.so.1 NCH -1
            p           = getlibpath_skip_field(p, end); // -> 31 0 0x1000 COW NC vnode /libexec/ld-elf.so.1 NCH -1
            p           = getlibpath_skip_field(p, end); // -> 0 0x1000 COW NC vnode /libexec/ld-elf.so.1 NCH -1
            p           = getlibpath_skip_field(p, end); // -> 0x1000 COW NC vnode /libexec/ld-elf.so.1 NCH -1
            p           = getlibpath_skip_field(p, end); // -> COW NC vnode /libexec/ld-elf.so.1 NCH -1
            p           = getlibpath_skip_field(p, end); // -> NC vnode /libexec/ld-elf.so.1 NCH -1
            p           = getlibpath_skip_field(p, end); // -> vnode /libexec/ld-elf.so.1 NCH -1
            p           = getlibpath_skip_field(p, end); // -> /libexec/ld-elf.so.1 NCH -1
            p           = getlibpath_skip_space(p, end); // /libexec/ld-elf.so.1 NCH -1
            p           = getlibpath_trim_file(p, end);  // /libexec

            if (p == end)
                continue;
            if (!getlibpath_add_path(&res, p, exclude))
            {
                success = false;
                break;
            }
        }

        fclose(fd);
        if (success)
            *paths = res.paths;

        return success;
    }

    bool getlibpath_procstat(char ***paths, const char **exclude)
    {
        char cmd[80];
        pid_t pid = getpid();
        snprintf(cmd, sizeof(cmd)/sizeof(char), "procstat -v %ld", long(pid));
        cmd[79] = '\0';

        // Open file for reading
        FILE *fd = popen(cmd, "r");
        if (fd == NULL)
            return NULL;

        char *line      = NULL;
        ssize_t len     = 0;
        size_t linenum  = 0;
        getlibpath_path_t res;
        getlibpath_buf_t  buf;
        bool success    = true;

        while ((len = getlibpath_getline(&line, &buf, fd)) >= 0)
        {
            // Skip heading line
            if (linenum++ == 0)
                continue;

            // ->   983        0x800ae4000        0x800af1000 r-x   13   14   9   4 CN-- vn /lib/libcrypt.so.5
            char *end   = &line[len];
            char *p     = getlibpath_skip_field(line, end); // ->        0x800ae4000        0x800af1000 r-x   13   14   9   4 CN-- vn /lib/libcrypt.so.5
            p           = getlibpath_skip_field(p, end); // ->        0x800af1000 r-x   13   14   9   4 CN-- vn /lib/libcrypt.so.5
            p           = getlibpath_skip_field(p, end); // -> r-x   13   14   9   4 CN-- vn /lib/libcrypt.so.5
            p           = getlibpath_skip_field(p, end); // ->   13   14   9   4 CN-- vn /lib/libcrypt.so.5
            p           = getlibpath_skip_field(p, end); // ->   14   9   4 CN-- vn /lib/libcrypt.so.5
            p           = getlibpath_skip_field(p, end); // ->   9   4 CN-- vn /lib/libcrypt.so.5
            p           = getlibpath_skip_field(p, end); // ->   4 CN-- vn /lib/libcrypt.so.5
            p           = getlibpath_skip_field(p, end); // -> CN-- vn /lib/libcrypt.so.5
            p           = getlibpath_skip_field(p, end); // -> vn /lib/libcrypt.so.5
            p           = getlibpath_skip_field(p, end); // -> /lib/libcrypt.so.5
            p           = getlibpath_skip_space(p, end); // /lib/libcrypt.so.5
            p           = getlibpath_trim_file(p, end);  // /lib

            if (p == end)
                continue;
            if (!getlibpath_add_path(&res, p, exclude))
            {
                success = false;
                break;
            }
        }

        fclose(fd);

        if (success)
            *paths = res.paths;

        return success;
    }
#endif

    /**
     * Return additional unique paths where the core library can be located
     * @param exclude NULL-terminated list of strings for paths that should be excluded
     * @return NULL-terminated list of library paths
     */
    char **get_library_paths(const char **exclude)
    {
        char **res = NULL;
        if (getlibpath_proc(&res, exclude))
            return res;
#if defined(PLATFORM_BSD)
        if (getlibpath_procstat(&res, exclude))
            return res;
#endif /* PLATFORM_BSD */

        return NULL;
    }

    /**
     * Free list returned by the get_library_paths() routine
     * @param paths list of paths to free
     */
    void free_library_paths(char **paths)
    {
        if (paths == NULL)
            return;

        for (char **p = paths; *p != NULL; ++p)
        {
            free(*p);
            *p = NULL;
        }
        free(paths);
    }

    char *get_library_path()
    {
        Dl_info dli;
        int res = ::dladdr(reinterpret_cast<void *>(get_library_path), &dli);
        if ((res == 0) || (dli.dli_fname == NULL))
            return NULL;
        char *path = ::strdup(dli.dli_fname);
        if (path == NULL)
            return NULL;
        char *p = strchr(path, '\0');
        while ((--p) > path)
            if (*p == FILE_SEPARATOR_C)
            {
                *p = '\0';
                break;
            }
        return path;
    }
}

#endif /* INCLUDE_CONTAINER_COMMON_LIBPATH_H_ */
