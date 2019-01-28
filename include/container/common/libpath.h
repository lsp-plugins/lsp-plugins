/*
 * libpath.h
 *
 *  Created on: 26 янв. 2019 г.
 *      Author: sadko
 */

#ifndef INCLUDE_CONTAINER_COMMON_LIBPATH_H_
#define INCLUDE_CONTAINER_COMMON_LIBPATH_H_

#include <core/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define FBUF_SIZE   4096

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

#if defined(PLATFORM_LINUX)
    /**
     * Return additional unique paths where the core library can be located
     * @param exclude NULL-terminated list of strings for paths that should be excluded
     * @return NULL-terminated list of library paths
     */
    char **getlibpath_proc(const char **exclude)
    {
        // Open file for reading
        FILE *fd = fopen("/proc/self/maps", "r");
        if (fd == NULL)
            return NULL;

        char *line      = NULL;
        size_t len      = 0;
        getlibpath_path_t res;

        while (getline(&line, &len, fd) >= 0)
        {
            // -> 7fd376cca000-7fd376e7b000 r-xp 00000000 103:04 3276809                   /lib64/libc-2.26.so
            char *end   = getlibpath_find_end(line, len);
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
                break;
        }

        if (line != NULL)
            free(line);

        fclose(fd);
        return res.paths;
    }
#elif defined(PLATFORM_BSD)
    char **getlibpath_proc(const char **exclude)
    {
        // Open file for reading
        FILE *fd = fopen("/proc/curproc/map", "r");
        if (fd == NULL)
            return NULL;

        char *line      = NULL;
        size_t len      = 0;
        getlibpath_path_t res;

        while (getline(&line, &len, fd) >= 0)
        {
            // -> 0x800602000 0x800622000 32 0 0xfffff8000397e780 r-x 31 0 0x1000 COW NC vnode /libexec/ld-elf.so.1 NCH -1
            char *end   = getlibpath_find_end(line, len);
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
                break;
        }

        if (line != NULL)
            free(line);

        fclose(fd);
        return res.paths;
    }

    char **getlibpath_procstat(const char **exclude)
    {
        char cmd[80];
        pid_t pid = getpid();
        snprintf(cmd, sizeof(cmd), "procstat -v %d", long(pid));
        cmd[79] = '\0';

        // Open file for reading
        FILE *fd = popen(cmd, "r");
        if (fd == NULL)
            return NULL;

        char *line      = NULL;
        size_t len      = 0;
        size_t linenum  = 0;
        getlibpath_path_t res;

        while (getline(&line, &len, fd) >= 0)
        {
            // Skip heading line
            if (linenum++ == 0)
                continue;

            // ->   983        0x800ae4000        0x800af1000 r-x   13   14   9   4 CN-- vn /lib/libcrypt.so.5
            char *end   = getlibpath_find_end(line, len);
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
                break;
        }

        if (line != NULL)
            free(line);

        fclose(fd);
        return res.paths;
    }
#endif

    /**
     * Return additional unique paths where the core library can be located
     * @param exclude NULL-terminated list of strings for paths that should be excluded
     * @return NULL-terminated list of library paths
     */
    char **get_library_paths(const char **exclude)
    {
        char **res = getlibpath_proc(exclude);
#if defined(PLATFORM_BSD)
        if (res == NULL)
            res         = getlibpath_procstat(exclude);
#endif /* PLATFORM_BSD */

        return res;
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
}

#endif /* INCLUDE_CONTAINER_COMMON_LIBPATH_H_ */
