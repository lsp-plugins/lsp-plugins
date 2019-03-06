/*
 * stdio.cpp
 *
 *  Created on: 29 дек. 2018 г.
 *      Author: sadko
 */

#include <core/stdlib/stdio.h>
#include <unistd.h>

#if defined(PLATFORM_WINDOWS)
    int vasprintf(char **res, const char *fmt, va_list ap)
    {
        int len = vsnprintf(NULL, 0, fmt, ap);
        if (len < 0)
            return -1;

        char *str = reinterpret_cast<char *>(malloc(size_t(len) + 1));
        if (str == NULL)
            return -1;

        int r = vsnprintf(str, len + 1, fmt, ap);
        if (r < 0)
        {
            free(str);
            return -1;
        }

        *res = str;
        return r;
    }

    int asprintf(char **strp, const char *fmt, ...)
    {
        va_list ap;
        va_start(ap, fmt);

        int r = vasprintf(strp, fmt, ap);

        va_end(ap);
        return r;
    }

    int fdsync(FILE *fd)
    {
        return (FlushFileBuffers((HANDLE)_fileno(fd))) ? 0 : -1;
    }

#endif /* PLATFORM_WINDOWS */

#if defined(PLATFORM_UNIX_COMPATIBLE)

    int fdsync(FILE *fd)
    {
        return fsync(fileno(fd));
    }

#endif
