/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 12 апр. 2016 г.
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

#ifndef CORE_BUFFER_H_
#define CORE_BUFFER_H_

#include <core/types.h>

namespace lsp
{
    typedef struct buffer_t
    {
        char       *pString;
        size_t      nCapacity;
        size_t      nLength;
    } buffer_t;

    bool init_buf(buffer_t *buf);
    bool init_buf(buffer_t *buf, size_t reserve);
    bool append_buf(buffer_t *buf, const char *str, size_t count);
    bool append_buf(buffer_t *buf, const char *str);
    bool append_buf(buffer_t *buf, char ch);
    bool extend_buf(buffer_t *buf, size_t reserve);
    void destroy_buf(buffer_t *buf);
    void clear_buf(buffer_t *buf);
    char *release_buf(buffer_t *buf);

}

#endif /* CORE_BUFFER_H_ */
