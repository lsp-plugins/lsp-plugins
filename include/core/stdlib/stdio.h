/*
 * stdio.h
 *
 *  Created on: 29 дек. 2018 г.
 *      Author: sadko
 */

#ifndef INCLUDE_CORE_STDLIB_STDIO_H_
#define INCLUDE_CORE_STDLIB_STDIO_H_

#include <core/types.h>
#include <stdio.h>

#if defined(PLATFORM_WINDOWS)

    int vasprintf(char **strp, const char *fmt, va_list ap);

    int asprintf(char **strp, const char *fmt, ...);

    int fdsync(FILE *fd);

#elif defined(PLATFORM_UNIX_COMPATIBLE)

    int fdsync(FILE *fd);

#endif /* PLATFORM_WINDOWS */

#endif /* INCLUDE_CORE_STDLIB_STDIO_H_ */
