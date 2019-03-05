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
     * Encode NULL-terminated UTF-8 string to NULL-terminated UTF-16 string
     * @param str string to encode
     * @return pointer to allocated UTF-16 string that should be free()'d after use
     */
    lsp_utf16_t    *utf8_to_utf16(const char *str);

    /**
     * Encode sequence of UTF-8 characters into sequence of UTF-16 characters
     * @param dst target buffer to store characters
     * @param ndst number of elements available in target buffer
     * @param src source buffer to read characters
     * @param nsrc number of elements available in source buffer
     * @param force force flag that treats the input block as last in the character sequence
     * @return number of processed code points
     */
    size_t          utf8_to_utf16(lsp_utf16_t *dst, size_t *ndst, const char *src, size_t *nsrc, bool force);

    /**
     * Encode NULL-terminated UTF-8 string to NULL-terminated UTF-32 string
     * @param str string to encode
     * @return pointer to allocated UTF-32 string that should be free()'d after use
     */
    lsp_utf32_t    *utf8_to_utf32(const char *str);

    /**
     * Encode sequence of UTF-8 characters into sequence of UTF-32 characters
     * @param dst target buffer to store characters
     * @param ndst number of elements available in target buffer
     * @param src source buffer to read characters
     * @param nsrc number of elements available in source buffer
     * @param force force flag that treats the input block as last in the character sequence
     * @return number of processed code points
     */
    size_t          utf8_to_utf32(lsp_utf32_t *dst, size_t *ndst, const char *src, size_t *nsrc, bool force);

    /**
     * Encode NULL-terminated UTF-16 string to NULL-terminated UTF-8 string
     * @param str string to encode
     * @return pointer to allocated UTF-8 string that should be free()'d after use
     */
    char           *utf16_to_utf8(const lsp_utf16_t *str);

    /**
     * Encode sequence of UTF-16 characters into sequence of UTF-16 characters
     * @param dst target buffer to store characters
     * @param ndst number of elements available in target buffer
     * @param src source buffer to read characters
     * @param nsrc number of elements available in source buffer
     * @param force force flag that treats the input block as last in the character sequence
     * @return number of processed code points
     */
    size_t          utf16_to_utf8(char *dst, size_t *ndst, const lsp_utf16_t *src, size_t *nsrc, bool force);

    /**
     * Encode NULL-terminated UTF-16 string to NULL-terminated UTF-32 string
     * @param str string to encode
     * @return pointer to allocated UTF-32 string that should be free()'d after use
     */
    lsp_utf32_t    *utf16_to_utf32(const lsp_utf16_t *str);

    /**
     * Encode sequence of UTF-16 characters into sequence of UTF-32 characters
     * @param dst target buffer to store characters
     * @param ndst number of elements available in target buffer
     * @param src source buffer to read characters
     * @param nsrc number of elements available in source buffer
     * @param force force flag that treats the input block as last in the character sequence
     * @return number of processed code points
     */
    size_t          utf16_to_utf32(lsp_utf32_t *dst, size_t *ndst, const lsp_utf16_t *src, size_t *nsrc, bool force);

    /**
     * Encode NULL-terminated UTF-32 string to NULL-terminated UTF-8 string
     * @param str string to encode
     * @return pointer to allocated UTF-16 string that should be free()'d after use
     */
    char           *utf32_to_utf8(const lsp_utf32_t *str);

    /**
     * Encode sequence of UTF-8 characters into sequence of UTF-16 characters
     * @param dst target buffer to store characters
     * @param ndst number of elements available in target buffer
     * @param src source buffer to read characters
     * @param nsrc number of elements available in source buffer
     * @param force force flag that treats the input block as last in the character sequence
     * @return number of processed code points
     */
    size_t          utf32_to_utf8(char *dst, size_t *ndst, const lsp_utf32_t *src, size_t *nsrc, bool force);

    /**
     * Encode NULL-terminated UTF-32 string to NULL-terminated UTF-16 string
     * @param str string to encode
     * @return pointer to allocated UTF-16 string that should be free()'d after use
     */
    lsp_utf16_t    *utf32_to_utf16(const lsp_utf32_t *str);

    /**
     * Encode sequence of UTF-8 characters into sequence of UTF-16 characters
     * @param dst target buffer to store characters
     * @param ndst number of elements available in target buffer
     * @param src source buffer to read characters
     * @param nsrc number of elements available in source buffer
     * @param force force flag that treats the input block as last in the character sequence
     * @return number of processed code points
     */
    size_t          utf32_to_utf8(lsp_utf16_t *dst, size_t *ndst, const lsp_utf32_t *src, size_t *nsrc, bool force);

}

#endif /* INCLUDE_CORE_IO_CHARSET_H_ */
