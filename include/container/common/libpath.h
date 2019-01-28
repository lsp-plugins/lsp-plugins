/*
 * libpath.h
 *
 *  Created on: 26 янв. 2019 г.
 *      Author: sadko
 */

#ifndef INCLUDE_CONTAINER_COMMON_LIBPATH_H_
#define INCLUDE_CONTAINER_COMMON_LIBPATH_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <core/types.h>

#define FBUF_SIZE   4096

namespace lsp
{
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

        char *s = end;
        while ((--s) > p)
        {
            if (*s == FILE_SEPARATOR_C)
            {
                *s = '\0';
                return p;
            }
        }
        return end;
    }

    bool getlibpath_check_presence(char **paths, const char *path)
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

    /**
     * Return additional unique paths where the core library can be located
     * @return NULL-terminated list of library paths
     */
    char **get_library_paths()
    {
        char *line, **paths;
        size_t len, np, cp;

        // Open file for reading
        FILE *fd = fopen("/proc/self/maps", "r");
        if (fd == NULL)
            return NULL;

        paths           = NULL;
        line            = NULL;
        len             = 0;
        np              = 0;
        cp              = 0;

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

            // Check if path is present
            if (getlibpath_check_presence(paths, p))
                continue;

            // Allocate new item in paths
            if (np >= cp)
            {
                char **npaths   = reinterpret_cast<char **>(realloc(paths, sizeof(char **) * (cp + 17)));
                if (npaths == NULL)
                    break;
                paths           = npaths;
                cp             += 16;
                for (size_t i=np; i<=cp; ++i)
                    paths[i]        = NULL;
            }

            // Put item to paths
            char *dup   = strdup(p);
            if (dup == NULL)
                break;
            paths[np++]     = dup;
        }

        if (line != NULL)
            free(line);

        fclose(fd);
        return paths;
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
