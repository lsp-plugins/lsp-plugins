/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 29 мая 2019 г.
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

#ifndef CORE_PROTOCOL_OSC_PATTERN_H_
#define CORE_PROTOCOL_OSC_PATTERN_H_

#include <core/protocol/osc/types.h>

namespace lsp
{
    namespace osc
    {
        typedef struct pattern_t
        {
            size_t          nparts;
            char           *format;
            char          **parts;
        } pattern_t;

        /**
         * Create and prepare pattern, this is not RT-safe method.
         *
         * @param pattern pattern to compile
         * @param format pattern format
         * @return status of operation
         */
        status_t    pattern_create(pattern_t *pattern, const char *format);

        /**
         * Get original format text of the pattern
         * @param pattern pattern to retrieve text format
         * @param text text
         * @return status of operation
         */
        status_t    pattern_get_format(pattern_t *pattern, const char **format);

        /**
         * Match the address to the pattern, this is thread-safe and RT-safe method
         * @param pattern the pattern to perform match
         * @param address address to match
         * @return true if pattern matches, false if not or error occurred
         */
        bool        pattern_match(const pattern_t *pattern, const char *address);

        /**
         * Destroy pattern
         * @param pattern pattern to destroy
         * @return status of operation
         */
        status_t    pattern_destroy(pattern_t *pattern);
    }
}



#endif /* CORE_PROTOCOL_OSC_PATTERN_H_ */
