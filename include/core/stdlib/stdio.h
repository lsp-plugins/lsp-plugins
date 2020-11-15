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
