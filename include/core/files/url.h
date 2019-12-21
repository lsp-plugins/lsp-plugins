/*
 * url.h
 *
 *  Created on: 21 дек. 2019 г.
 *      Author: sadko
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
