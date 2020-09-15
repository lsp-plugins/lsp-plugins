/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 13 июл. 2019 г.
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

#ifndef INCLUDE_CORE_PARSE_H_
#define INCLUDE_CORE_PARSE_H_

#include <core/types.h>

#include <locale.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <core/stdlib/math.h>

// Data parsing
#define PARSE_INT(var, code) \
    { \
        errno = 0; \
        char *__endptr = NULL; \
        long __ = ::strtol(var, &__endptr, 10); \
        if ((errno == 0) && (*__endptr == '\0')) \
            { code; } \
    }

    #define PARSE_UINT(var, code) \
        { \
            errno = 0; \
            char *__endptr = NULL; \
            unsigned long __ = ::strtoul(var, &__endptr, 10); \
            if ((errno == 0) && (*__endptr == '\0')) \
                { code; } \
        }

#ifdef PLATFORM_BSD

    #define PARSE_LLINT(var, code) \
        { \
            errno = 0; \
            char *__endptr = NULL; \
            long long __ = ::strtol(var, &__endptr, 10); \
            if ((errno == 0) && (*__endptr == '\0')) \
                { code; } \
        }

    #define PARSE_ULLINT(var, code) \
        { \
            errno = 0; \
            char *__endptr = NULL; \
            unsigned long long __ = ::strtoul(var, &__endptr, 10); \
            if ((errno == 0) && (*__endptr == '\0')) \
                { code; } \
        }

#else

    #define PARSE_LLINT(var, code) \
        { \
            errno = 0; \
            char *__endptr = NULL; \
            long long __ = ::strtoll(var, &__endptr, 10); \
            if ((errno == 0) && (*__endptr == '\0')) \
                { code; } \
        }

    #define PARSE_ULLINT(var, code) \
        { \
            errno = 0; \
            char *__endptr = NULL; \
            unsigned long long __ = ::strtoull(var, &__endptr, 10); \
            if ((errno == 0) && (*__endptr == '\0')) \
                { code; } \
        }
#endif /* PLATFORM_BSD */

#define PARSE_BOOL(var, code) \
    { \
        bool __ = !::strcasecmp(var, "true"); \
        if (! __ ) \
            __ = !::strcasecmp(var, "1"); \
        { code; } \
    }

#define PARSE_FLAG(var, dst, flag) PARSE_BOOL(var, if (__) dst |= flag; else dst &= ~flag)
#define UPDATE_LOCALE(out_var, lc, value) \
       char *out_var = setlocale(lc, NULL); \
       if (out_var != NULL) \
       { \
           size_t ___len = strlen(out_var) + 1; \
           char *___copy = static_cast<char *>(alloca(___len)); \
           memcpy(___copy, out_var, ___len); \
           out_var = ___copy; \
       } \
       setlocale(lc, value);

#define PARSE_FLOAT(var, code) \
    { \
        float __; \
        if (parse_float(var, &__)) \
            { code; } \
    }

#define PARSE_DOUBLE(var, code) \
    { \
        double __; \
        if (parse_double(var, &__)) \
            { code; } \
    }

#define PARSE_STRING(var, store) \
    { \
        char *__ = strdup(var);\
        if (__ != NULL) \
        { \
            if ((store) != NULL) \
                free(store); \
            store = __; \
        } \
    }

namespace lsp
{
    bool parse_float(const char *variable, float *res);

    bool parse_double(const char *variable, double *res);
}

#endif /* INCLUDE_CORE_PARSE_H_ */
