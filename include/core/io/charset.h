/*
 * charset.h
 *
 *  Created on: 18 июн. 2018 г.
 *      Author: sadko
 */

#ifndef CORE_IO_CHARSET_H_
#define CORE_IO_CHARSET_H_

#include <core/types.h>

#if defined(PLATFORM_WINDOWS)
    #include <winnls.h>
#else
    #include <iconv.h>
    #include <locale.h>
#endif /* PLATFORM_WINDOWS */

#define LSP_UTF32_EOF       lsp_utf32_t(-1)

namespace lsp
{
#if defined(PLATFORM_WINDOWS)

    ssize_t get_codepage(LCID locale, bool ansi = true);

    ssize_t codepage_from_name(const char *charset);

    /**
     * This is a MultiByteToWideChar wrapping function that handles additional cases
     * for non-MBCS encodings
     * @param cp code page
     * @param src source buffer
     * @param nsrc number of bytes available in source buffer (will be updated on exit)
     * @param dst destination buffer (can be NULL for just estimating the length)
     * @param ndst number of characters in destination buffer (will be updated on exit,
     *        can be NULL or point to zero value for just estimating the length)
     * @return number of UTF-16 characters written to destination buffer or negative error code
     */
    ssize_t multibyte_to_widechar(size_t cp, LPCCH src, size_t *nsrc, LPWSTR dst, size_t *ndst);

    /**
     * This is a WideCharToMultiByte wrapping function that handles additional cases
     * for non-MBCS encodings
     * @param cp code page
     * @param src source wide string to perform encoding
     * @param nsrc number of characters available in source wide string (will be updated on exit)
     * @param dst target string to perform encoding (can be NULL for just estimating the length)
     * @param ndst number of bytes in destination buffer (will be updated on exit,
     *        can be NULL or point to zero value for just estimating the length)
     * @return number of bytes written to destination buffer or negative error code
     */
    ssize_t widechar_to_multibyte(size_t cp, LPCWCH src, size_t *nsrc, LPSTR dst, size_t *ndst);
#else

    iconv_t init_iconv_to_wchar_t(const char *charset);

    iconv_t init_iconv_from_wchar_t(const char *charset);

#endif /* PLATFORM_WINDOWS */

    /**
     * Read UTF-16 codepoint from the NULL-terminated UTF-16 string, replace invalid
     * code sequence by 0xfffd code point
     * @param str pointer to the NULL-terminated UTF-16 string
     * @return code point
     */
    lsp_utf32_t     read_utf16le_codepoint(const lsp_utf16_t **str);
    lsp_utf32_t     read_utf16be_codepoint(const lsp_utf16_t **str);
    inline lsp_utf32_t  read_utf16_codepoint(const lsp_utf16_t **str) { return __IF_LEBE(read_utf16le_codepoint, read_utf16be_codepoint)(str); };

    /**
     * Read UTF-16 codepoint from the NULL-terminated UTF-16 string in streaming mode,
     * replace invalid code sequence by 0xfffd code point
     * @param str pointer to the pointer to the NULL-terminated UTF-16 string
     * @param nsrc counter containing number of unread array elements
     * @param force process data as there will be no future data on the input
     * @return parsed code point or LSP_UTF32_EOF as end of sequence
     */
    lsp_utf32_t         read_utf16le_streaming(const lsp_utf16_t **str, size_t *nsrc, bool force);
    lsp_utf32_t         read_utf16be_streaming(const lsp_utf16_t **str, size_t *nsrc, bool force);
    inline lsp_utf32_t  read_utf16_streaming(const lsp_utf16_t **str, size_t *nsrc, bool force) { return __IF_LEBE(read_utf16le_streaming, read_utf16be_streaming)(str, nsrc, force); }

    /**
     * Read UTF-16 codepoint from the NULL-terminated UTF-8 string, replace invalid
     * code sequence by 0xfffd code point
     * @param str pointer to the NULL-terminated UTF-8 string
     * @return code point
     */
    lsp_utf32_t     read_utf8_codepoint(const char **str);

    /**
     * Read UTF-8 codepoint from the NULL-terminated UTF-8 string in streaming mode,
     * replace invalid code sequence by 0xfffd code point
     * @param str pointer to the pointer to the NULL-terminated UTF-8 string
     * @param nsrc counter containing number of unread array elements
     * @param force process data as there will be no future data on the input
     * @return parsed code point or LSP_UTF32_EOF as end of sequence
     */
    lsp_utf32_t     read_utf8_streaming(const char **str, size_t *nsrc, bool force);

    /**
     * Write UTF-8 code point to buffer, buffer should be of enough size
     * @param str pointer to target buffer
     * @param cp code point to write
     */
    void            write_utf8_codepoint(char **str, lsp_utf32_t cp);

    /**
     * Write UTF-16 code point to buffer, buffer should be of enough size
     * @param str pointer to target buffer
     * @param cp code point to write
     */
    void            write_utf16le_codepoint(lsp_utf16_t **str, lsp_utf32_t cp);
    void            write_utf16be_codepoint(lsp_utf16_t **str, lsp_utf32_t cp);
    inline void     write_utf16_codepoint(lsp_utf16_t **str, lsp_utf32_t cp) { return __IF_LEBE(write_utf16le_codepoint, write_utf16be_codepoint)(str, cp); }

    /**
     * Encode NULL-terminated UTF-8 string to NULL-terminated UTF-16 string
     * @param str string to encode
     * @return pointer to allocated UTF-16 string that should be free()'d after use
     */
    lsp_utf16_t    *utf8_to_utf16le(const char *str);
    lsp_utf16_t    *utf8_to_utf16be(const char *str);
    inline lsp_utf16_t    *utf8_to_utf16(const char *str) { return __IF_LEBE(utf8_to_utf16le, utf8_to_utf16be)(str); }

    /**
     * Encode sequence of UTF-8 characters into sequence of UTF-16 characters
     * @param dst target buffer to store characters
     * @param ndst number of elements available in target buffer
     * @param src source buffer to read characters
     * @param nsrc number of elements available in source buffer
     * @param force force flag that treats the input block as last in the character sequence
     * @return number of processed code points
     */
    size_t          utf8_to_utf16le(lsp_utf16_t *dst, size_t *ndst, const char *src, size_t *nsrc, bool force);
    size_t          utf8_to_utf16be(lsp_utf16_t *dst, size_t *ndst, const char *src, size_t *nsrc, bool force);
    inline size_t   utf8_to_utf16(lsp_utf16_t *dst, size_t *ndst, const char *src, size_t *nsrc, bool force) { return __IF_LEBE(utf8_to_utf16le, utf8_to_utf16be)(dst, ndst, src, nsrc, force); }

    /**
     * Encode NULL-terminated UTF-8 string to NULL-terminated UTF-32 string
     * @param str string to encode
     * @return pointer to allocated UTF-32 string that should be free()'d after use
     */
    lsp_utf32_t    *utf8_to_utf32le(const char *str);
    lsp_utf32_t    *utf8_to_utf32be(const char *str);
    inline lsp_utf32_t    *utf8_to_utf32(const char *str) { return __IF_LEBE(utf8_to_utf32le, utf8_to_utf32be)(str); };

    /**
     * Encode sequence of UTF-8 characters into sequence of UTF-32 characters
     * @param dst target buffer to store characters
     * @param ndst number of elements available in target buffer
     * @param src source buffer to read characters
     * @param nsrc number of elements available in source buffer
     * @param force force flag that treats the input block as last in the character sequence
     * @return number of processed code points
     */
    size_t          utf8_to_utf32le(lsp_utf32_t *dst, size_t *ndst, const char *src, size_t *nsrc, bool force);
    size_t          utf8_to_utf32be(lsp_utf32_t *dst, size_t *ndst, const char *src, size_t *nsrc, bool force);
    inline size_t   utf8_to_utf32(lsp_utf32_t *dst, size_t *ndst, const char *src, size_t *nsrc, bool force) { return __IF_LEBE(utf8_to_utf32le, utf8_to_utf32be)(dst, ndst, src, nsrc, force); };

    /**
     * Encode NULL-terminated UTF-16 string to NULL-terminated UTF-8 string
     * @param str string to encode
     * @return pointer to allocated UTF-8 string that should be free()'d after use
     */
    char           *utf16le_to_utf8(const lsp_utf16_t *str);
    char           *utf16be_to_utf8(const lsp_utf16_t *str);
    inline char    *utf16_to_utf8(const lsp_utf16_t *str) { return __IF_LEBE(utf16le_to_utf8, utf16be_to_utf8)(str); };

    /**
     * Encode sequence of UTF-16 characters into sequence of UTF-16 characters
     * @param dst target buffer to store characters
     * @param ndst number of elements available in target buffer
     * @param src source buffer to read characters
     * @param nsrc number of elements available in source buffer
     * @param force force flag that treats the input block as last in the character sequence
     * @return number of processed code points
     */
    size_t          utf16le_to_utf8(char *dst, size_t *ndst, const lsp_utf16_t *src, size_t *nsrc, bool force);
    size_t          utf16be_to_utf8(char *dst, size_t *ndst, const lsp_utf16_t *src, size_t *nsrc, bool force);
    inline size_t   utf16_to_utf8(char *dst, size_t *ndst, const lsp_utf16_t *src, size_t *nsrc, bool force) { return __IF_LEBE(utf16le_to_utf8, utf16be_to_utf8)(dst, ndst, src, nsrc, force); };

    /**
     * Encode NULL-terminated UTF-16 string to NULL-terminated UTF-32 string
     * @param str string to encode
     * @return pointer to allocated UTF-32 string that should be free()'d after use
     */
    lsp_utf32_t    *utf16le_to_utf32le(const lsp_utf16_t *str);
    lsp_utf32_t    *utf16le_to_utf32be(const lsp_utf16_t *str);
    lsp_utf32_t    *utf16be_to_utf32le(const lsp_utf16_t *str);
    lsp_utf32_t    *utf16be_to_utf32be(const lsp_utf16_t *str);

    inline lsp_utf32_t    *utf16le_to_utf32(const lsp_utf16_t *str) { return __IF_LEBE(utf16le_to_utf32le, utf16le_to_utf32be)(str); }
    inline lsp_utf32_t    *utf16be_to_utf32(const lsp_utf16_t *str) { return __IF_LEBE(utf16be_to_utf32le, utf16be_to_utf32be)(str); }
    inline lsp_utf32_t    *utf16_to_utf32le(const lsp_utf16_t *str) { return __IF_LEBE(utf16le_to_utf32le, utf16be_to_utf32le)(str); }
    inline lsp_utf32_t    *utf16_to_utf32be(const lsp_utf16_t *str) { return __IF_LEBE(utf16le_to_utf32be, utf16be_to_utf32be)(str); }
    inline lsp_utf32_t    *utf16_to_utf32(const lsp_utf16_t *str) { return __IF_LEBE(utf16le_to_utf32le, utf16be_to_utf32be)(str); }

    /**
     * Encode sequence of UTF-16 characters into sequence of UTF-32 characters
     * @param dst target buffer to store characters
     * @param ndst number of elements available in target buffer
     * @param src source buffer to read characters
     * @param nsrc number of elements available in source buffer
     * @param force force flag that treats the input block as last in the character sequence
     * @return number of processed code points
     */
    size_t          utf16le_to_utf32le(lsp_utf32_t *dst, size_t *ndst, const lsp_utf16_t *src, size_t *nsrc, bool force);
    size_t          utf16le_to_utf32be(lsp_utf32_t *dst, size_t *ndst, const lsp_utf16_t *src, size_t *nsrc, bool force);
    size_t          utf16be_to_utf32le(lsp_utf32_t *dst, size_t *ndst, const lsp_utf16_t *src, size_t *nsrc, bool force);
    size_t          utf16be_to_utf32be(lsp_utf32_t *dst, size_t *ndst, const lsp_utf16_t *src, size_t *nsrc, bool force);

    inline size_t  utf16le_to_utf32(lsp_utf32_t *dst, size_t *ndst, const lsp_utf16_t *src, size_t *nsrc, bool force) { return __IF_LEBE(utf16le_to_utf32le, utf16le_to_utf32be)(dst, ndst, src, nsrc, force); }
    inline size_t  utf16be_to_utf32(lsp_utf32_t *dst, size_t *ndst, const lsp_utf16_t *src, size_t *nsrc, bool force) { return __IF_LEBE(utf16be_to_utf32le, utf16be_to_utf32be)(dst, ndst, src, nsrc, force); }
    inline size_t  utf16_to_utf32le(lsp_utf32_t *dst, size_t *ndst, const lsp_utf16_t *src, size_t *nsrc, bool force) { return __IF_LEBE(utf16le_to_utf32le, utf16be_to_utf32le)(dst, ndst, src, nsrc, force); }
    inline size_t  utf16_to_utf32be(lsp_utf32_t *dst, size_t *ndst, const lsp_utf16_t *src, size_t *nsrc, bool force) { return __IF_LEBE(utf16le_to_utf32be, utf16be_to_utf32be)(dst, ndst, src, nsrc, force); }
    inline size_t  utf16_to_utf32(lsp_utf32_t *dst, size_t *ndst, const lsp_utf16_t *src, size_t *nsrc, bool force) { return __IF_LEBE(utf16le_to_utf32le, utf16be_to_utf32be)(dst, ndst, src, nsrc, force); }

    /**
     * Encode NULL-terminated UTF-32 string to NULL-terminated UTF-8 string
     * @param str string to encode
     * @return pointer to allocated UTF-16 string that should be free()'d after use
     */
    char           *utf32le_to_utf8(const lsp_utf32_t *str);
    char           *utf32be_to_utf8(const lsp_utf32_t *str);
    inline char    *utf32_to_utf8(const lsp_utf32_t *str) { return __IF_LEBE(utf32le_to_utf8, utf32be_to_utf8)(str); };

    /**
     * Encode sequence of UTF-8 characters into sequence of UTF-16 characters
     * @param dst target buffer to store characters
     * @param ndst number of elements available in target buffer
     * @param src source buffer to read characters
     * @param nsrc number of elements available in source buffer
     * @param force force flag that treats the input block as last in the character sequence
     * @return number of processed code points
     */
    size_t          utf32le_to_utf8(char *dst, size_t *ndst, const lsp_utf32_t *src, size_t *nsrc, bool force);
    size_t          utf32be_to_utf8(char *dst, size_t *ndst, const lsp_utf32_t *src, size_t *nsrc, bool force);
    inline size_t   utf32_to_utf8(char *dst, size_t *ndst, const lsp_utf32_t *src, size_t *nsrc, bool force) { return __IF_LEBE(utf32le_to_utf8, utf32be_to_utf8)(dst, ndst, src, nsrc, force); }

    /**
     * Encode NULL-terminated UTF-32 string to NULL-terminated UTF-16 string
     * @param str string to encode
     * @return pointer to allocated UTF-16 string that should be free()'d after use
     */
    lsp_utf16_t    *utf32le_to_utf16le(const lsp_utf32_t *str);
    lsp_utf16_t    *utf32le_to_utf16be(const lsp_utf32_t *str);
    lsp_utf16_t    *utf32be_to_utf16le(const lsp_utf32_t *str);
    lsp_utf16_t    *utf32be_to_utf16be(const lsp_utf32_t *str);

    inline lsp_utf16_t *utf32le_to_utf16(const lsp_utf32_t *str) { return __IF_LEBE(utf32le_to_utf16le, utf32le_to_utf16be)(str);  }
    inline lsp_utf16_t *utf32be_to_utf16(const lsp_utf32_t *str) { return __IF_LEBE(utf32be_to_utf16le, utf32be_to_utf16be)(str);  }
    inline lsp_utf16_t *utf32_to_utf16le(const lsp_utf32_t *str) { return __IF_LEBE(utf32le_to_utf16le, utf32be_to_utf16le)(str);  }
    inline lsp_utf16_t *utf32_to_utf16be(const lsp_utf32_t *str) { return __IF_LEBE(utf32le_to_utf16be, utf32be_to_utf16be)(str);  }
    inline lsp_utf16_t *utf32_to_utf16(const lsp_utf32_t *str) { return __IF_LEBE(utf32le_to_utf16le, utf32be_to_utf16be)(str);  }

    /**
     * Encode sequence of UTF-8 characters into sequence of UTF-16 characters
     * @param dst target buffer to store characters
     * @param ndst number of elements available in target buffer
     * @param src source buffer to read characters
     * @param nsrc number of elements available in source buffer
     * @param force force flag that treats the input block as last in the character sequence
     * @return number of processed code points
     */
    size_t          utf32le_to_utf16le(lsp_utf16_t *dst, size_t *ndst, const lsp_utf32_t *src, size_t *nsrc, bool force);
    size_t          utf32le_to_utf16be(lsp_utf16_t *dst, size_t *ndst, const lsp_utf32_t *src, size_t *nsrc, bool force);
    size_t          utf32be_to_utf16le(lsp_utf16_t *dst, size_t *ndst, const lsp_utf32_t *src, size_t *nsrc, bool force);
    size_t          utf32be_to_utf16be(lsp_utf16_t *dst, size_t *ndst, const lsp_utf32_t *src, size_t *nsrc, bool force);

    inline size_t   utf32_to_utf16le(lsp_utf16_t *dst, size_t *ndst, const lsp_utf32_t *src, size_t *nsrc, bool force) { return __IF_LEBE(utf32le_to_utf16le, utf32be_to_utf16le)(dst, ndst, src, nsrc, force); }
    inline size_t   utf32_to_utf16be(lsp_utf16_t *dst, size_t *ndst, const lsp_utf32_t *src, size_t *nsrc, bool force) { return __IF_LEBE(utf32le_to_utf16be, utf32be_to_utf16be)(dst, ndst, src, nsrc, force); }
    inline size_t   utf32le_to_utf16(lsp_utf16_t *dst, size_t *ndst, const lsp_utf32_t *src, size_t *nsrc, bool force) { return __IF_LEBE(utf32le_to_utf16le, utf32le_to_utf16be)(dst, ndst, src, nsrc, force); }
    inline size_t   utf32be_to_utf16(lsp_utf16_t *dst, size_t *ndst, const lsp_utf32_t *src, size_t *nsrc, bool force) { return __IF_LEBE(utf32be_to_utf16le, utf32be_to_utf16be)(dst, ndst, src, nsrc, force); }
    inline size_t   utf32_to_utf16(lsp_utf16_t *dst, size_t *ndst, const lsp_utf32_t *src, size_t *nsrc, bool force)   { return __IF_LEBE(utf32le_to_utf16le, utf32be_to_utf16be)(dst, ndst, src, nsrc, force); }

}

#endif /* CORE_IO_CHARSET_H_ */
