/*
 * string.h
 *
 *  Created on: 15 февр. 2019 г.
 *      Author: sadko
 */

#ifndef INCLUDE_CORE_STDLIB_STRING_H_
#define INCLUDE_CORE_STDLIB_STRING_H_

#include <core/types.h>
#include <string.h>

#if defined(PLATFORM_WINDOWS)
    inline char *stpcpy(char *dst, const char *src)
    {
        size_t len = strlen(src);
        ::memcpy(dst, src, len + 1);
        return &dst[len];
    }

    inline void bzero(void *dst, size_t count)
    {
        ::memset(dst, 0, count);
    }
#endif /* PLATFORM_WINDOWS */

#endif /* INCLUDE_CORE_STDLIB_STRING_H_ */
