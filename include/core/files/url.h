/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 21 дек. 2019 г.
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

#ifndef CORE_FILES_URL_H_
#define CORE_FILES_URL_H_

#include <core/LSPString.h>

namespace lsp
{
    /**
     * Decode URL-encoded string
     * @param dst destination string to store decoded string
     * @param src source string to parse
     * @return status of operation
     */
    status_t    url_decode(LSPString *dst, const LSPString *src);

    /**
     * Decode URL-encoded string
     * @param dst destination string to store decoded string
     * @param src source string to parse
     * @param first first character of the source string to start parsing
     * @return status of operation
     */
    status_t    url_decode(LSPString *dst, const LSPString *src, size_t first);

    /**
     * Decode URL-encoded string
     * @param dst destination string to store decoded string
     * @param src source string to parse
     * @param first first character of the source string to start parsing
     * @param last last character of the source string to start parsing
     * @return status of operation
     */
    status_t    url_decode(LSPString *dst, const LSPString *src, size_t first, size_t last);
}

#endif /* INCLUDE_CORE_FILES_URL_H_ */
