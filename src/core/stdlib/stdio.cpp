/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 29 дек. 2018 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
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
