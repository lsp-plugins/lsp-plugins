/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 15 февр. 2019 г.
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

#ifndef CORE_STDLIB_STRING_H_
#define CORE_STDLIB_STRING_H_

#include <common/types.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

#if defined(PLATFORM_WINDOWS)
    inline char *stpcpy(char *dst, const char *src)
    {
        size_t len = ::strlen(src);
        ::memcpy(dst, src, len + 1);
        return &dst[len];
    }

    inline void bzero(void *dst, size_t count)
    {
        ::memset(dst, 0, count);
    }

    inline char *strndup(const char *src, size_t clen)
    {
        size_t nlen = ::strnlen(src, clen);
        char *ptr   = reinterpret_cast<char *>(::malloc(nlen + 1));
        if (ptr != NULL)
        {
            ::memcpy(ptr, src, nlen);
            ptr[nlen]   = '\0';
        }

        return ptr;
    }
#endif /* PLATFORM_WINDOWS */

    inline void *lsp_memdup(const void *src, size_t count)
    {
        void *dst = ::malloc(count);
        if (count > 0)
            ::memcpy(dst, src, count);
        return dst;
    }

#endif /* CORE_STDLIB_STRING_H_ */
