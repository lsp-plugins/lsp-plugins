/*
 * charset.h
 *
 *  Created on: 18 июн. 2018 г.
 *      Author: sadko
 */

#ifndef INCLUDE_CORE_IO_CHARSET_H_
#define INCLUDE_CORE_IO_CHARSET_H_

#include <core/types.h>

#if defined(PLATFORM_WINDOWS)
    #include <winnls.h>
#else
    #include <iconv.h>
    #include <locale.h>
#endif /* PLATFORM_WINDOWS */

namespace lsp
{
#if defined(PLATFORM_WINDOWS)

    ssize_t get_codepage(LCID locale, bool ansi = true);

    ssize_t codepage_from_name(const char *charset);

#else

    iconv_t init_iconv_to_wchar_t(const char *charset);

    iconv_t init_iconv_from_wchar_t(const char *charset);

#endif /* PLATFORM_WINDOWS */

    /**
     * Encode UTF-16 string to UTF-8 string
     * @param str string to encode
     * @return pointer to allocated UTF-8 string that should be free()'d after use
     */
    char               *utf16_to_utf8(const lsp_utf16_t *str);

    /**
     * Encode UTF-8 string to UTF-16 string
     * @param str string to encode
     * @return pointer to allocated UTF-16 string that should be free()'d after use
     */
    lsp_utf16_t   *utf8_to_utf16(const char *str);
}

#endif /* INCLUDE_CORE_IO_CHARSET_H_ */
