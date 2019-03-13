/*
 * charset.cpp
 *
 *  Created on: 18 июн. 2018 г.
 *      Author: sadko
 */

#include <core/io/charset.h>
#include <dsp/endian.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

namespace lsp
{
#if defined(PLATFORM_WINDOWS)
    typedef struct codepage_t
    {
        const char *name;
        size_t      codepage;
    } codepage_t;

    // This is a generated list of codepages supported by Windows,
    // see script:   scripts/perl/core/oi/charset/gen_cp.pl
    static const codepage_t win_codepages[] = {
            { "037", 37 },
            { "10000", 10000 },
            { "10001", 10001 },
            { "10002", 10002 },
            { "10003", 10003 },
            { "10004", 10004 },
            { "10005", 10005 },
            { "10006", 10006 },
            { "10007", 10007 },
            { "10008", 10008 },
            { "10010", 10010 },
            { "10017", 10017 },
            { "10021", 10021 },
            { "10029", 10029 },
            { "10079", 10079 },
            { "10081", 10081 },
            { "10082", 10082 },
            { "1026", 1026 },
            { "1047", 1047 },
            { "1140", 1140 },
            { "1141", 1141 },
            { "1142", 1142 },
            { "1143", 1143 },
            { "1144", 1144 },
            { "1145", 1145 },
            { "1146", 1146 },
            { "1147", 1147 },
            { "1148", 1148 },
            { "1149", 1149 },
            { "1200", 1200 },
            { "12000", 12000 },
            { "12001", 12001 },
            { "1201", 1201 },
            { "1250", 1250 },
            { "1251", 1251 },
            { "1252", 1252 },
            { "1253", 1253 },
            { "1254", 1254 },
            { "1255", 1255 },
            { "1256", 1256 },
            { "1257", 1257 },
            { "1258", 1258 },
            { "1361", 1361 },
            { "20000", 20000 },
            { "20001", 20001 },
            { "20002", 20002 },
            { "20003", 20003 },
            { "20004", 20004 },
            { "20005", 20005 },
            { "20105", 20105 },
            { "20106", 20106 },
            { "20107", 20107 },
            { "20108", 20108 },
            { "20127", 20127 },
            { "20261", 20261 },
            { "20269", 20269 },
            { "20273", 20273 },
            { "20277", 20277 },
            { "20278", 20278 },
            { "20280", 20280 },
            { "20284", 20284 },
            { "20285", 20285 },
            { "20290", 20290 },
            { "20297", 20297 },
            { "20420", 20420 },
            { "20423", 20423 },
            { "20424", 20424 },
            { "20833", 20833 },
            { "20838", 20838 },
            { "20866", 20866 },
            { "20871", 20871 },
            { "20880", 20880 },
            { "20905", 20905 },
            { "20924", 20924 },
            { "20932", 20932 },
            { "20936", 20936 },
            { "20949", 20949 },
            { "21025", 21025 },
            { "21027", 21027 },
            { "21866", 21866 },
            { "28591", 28591 },
            { "28592", 28592 },
            { "28593", 28593 },
            { "28594", 28594 },
            { "28595", 28595 },
            { "28596", 28596 },
            { "28597", 28597 },
            { "28598", 28598 },
            { "28599", 28599 },
            { "28603", 28603 },
            { "28605", 28605 },
            { "29001", 29001 },
            { "37", 37 },
            { "38598", 38598 },
            { "437", 437 },
            { "500", 500 },
            { "50220", 50220 },
            { "50221", 50221 },
            { "50222", 50222 },
            { "50225", 50225 },
            { "50227", 50227 },
            { "50229", 50229 },
            { "50930", 50930 },
            { "50931", 50931 },
            { "50933", 50933 },
            { "50935", 50935 },
            { "50936", 50936 },
            { "50937", 50937 },
            { "50939", 50939 },
            { "51932", 51932 },
            { "51936", 51936 },
            { "51949", 51949 },
            { "51950", 51950 },
            { "52936", 52936 },
            { "54936", 54936 },
            { "57002", 57002 },
            { "57003", 57003 },
            { "57004", 57004 },
            { "57005", 57005 },
            { "57006", 57006 },
            { "57007", 57007 },
            { "57008", 57008 },
            { "57009", 57009 },
            { "57010", 57010 },
            { "57011", 57011 },
            { "65000", 65000 },
            { "65001", 65001 },
            { "708", 708 },
            { "709", 709 },
            { "710", 710 },
            { "720", 720 },
            { "737", 737 },
            { "775", 775 },
            { "850", 850 },
            { "852", 852 },
            { "855", 855 },
            { "857", 857 },
            { "858", 858 },
            { "860", 860 },
            { "861", 861 },
            { "862", 862 },
            { "863", 863 },
            { "864", 864 },
            { "865", 865 },
            { "866", 866 },
            { "869", 869 },
            { "870", 870 },
            { "874", 874 },
            { "875", 875 },
            { "932", 932 },
            { "936", 936 },
            { "949", 949 },
            { "950", 950 },
            { "asmo-708", 708 },
            { "asmo708", 708 },
            { "big5", 950 },
            { "cp-1025", 21025 },
            { "cp-1250", 1250 },
            { "cp-1251", 1251 },
            { "cp-1252", 1252 },
            { "cp-1253", 1253 },
            { "cp-1254", 1254 },
            { "cp-1255", 1255 },
            { "cp-1256", 1256 },
            { "cp-1257", 1257 },
            { "cp-1258", 1258 },
            { "cp-21027", 21027 },
            { "cp-50229", 50229 },
            { "cp-50930", 50930 },
            { "cp-50931", 50931 },
            { "cp-50933", 50933 },
            { "cp-50935", 50935 },
            { "cp-50936", 50936 },
            { "cp-50937", 50937 },
            { "cp-50939", 50939 },
            { "cp-51950", 51950 },
            { "cp-709", 709 },
            { "cp-710", 710 },
            { "cp-866", 866 },
            { "cp-874", 874 },
            { "cp-875", 875 },
            { "cp1025", 21025 },
            { "cp1250", 1250 },
            { "cp1251", 1251 },
            { "cp1252", 1252 },
            { "cp1253", 1253 },
            { "cp1254", 1254 },
            { "cp1255", 1255 },
            { "cp1256", 1256 },
            { "cp1257", 1257 },
            { "cp1258", 1258 },
            { "cp21027", 21027 },
            { "cp50229", 50229 },
            { "cp50930", 50930 },
            { "cp50931", 50931 },
            { "cp50933", 50933 },
            { "cp50935", 50935 },
            { "cp50936", 50936 },
            { "cp50937", 50937 },
            { "cp50939", 50939 },
            { "cp51950", 51950 },
            { "cp709", 709 },
            { "cp710", 710 },
            { "cp866", 866 },
            { "cp874", 874 },
            { "cp875", 875 },
            { "csiso2022jp", 50221 },
            { "dos-720", 720 },
            { "dos-862", 862 },
            { "dos720", 720 },
            { "dos862", 862 },
            { "euc-cn", 51936 },
            { "euc-jp", 51932 },
            { "euc-kr", 51949 },
            { "gb18030", 54936 },
            { "gb2312", 936 },
            { "hz-gb-2312", 52936 },
            { "hz-gb2312", 52936 },
            { "ibm-thai", 20838 },
            { "ibm00858", 858 },
            { "ibm00924", 20924 },
            { "ibm01047", 1047 },
            { "ibm01140", 1140 },
            { "ibm01141", 1141 },
            { "ibm01142", 1142 },
            { "ibm01143", 1143 },
            { "ibm01144", 1144 },
            { "ibm01145", 1145 },
            { "ibm01146", 1146 },
            { "ibm01147", 1147 },
            { "ibm01148", 1148 },
            { "ibm01149", 1149 },
            { "ibm037", 37 },
            { "ibm1026", 1026 },
            { "ibm273", 20273 },
            { "ibm277", 20277 },
            { "ibm278", 20278 },
            { "ibm280", 20280 },
            { "ibm284", 20284 },
            { "ibm285", 20285 },
            { "ibm290", 20290 },
            { "ibm297", 20297 },
            { "ibm420", 20420 },
            { "ibm423", 20423 },
            { "ibm424", 20424 },
            { "ibm437", 437 },
            { "ibm500", 500 },
            { "ibm737", 737 },
            { "ibm775", 775 },
            { "ibm850", 850 },
            { "ibm852", 852 },
            { "ibm855", 855 },
            { "ibm857", 857 },
            { "ibm860", 860 },
            { "ibm861", 861 },
            { "ibm863", 863 },
            { "ibm864", 864 },
            { "ibm865", 865 },
            { "ibm869", 869 },
            { "ibm870", 870 },
            { "ibm871", 20871 },
            { "ibm880", 20880 },
            { "ibm905", 20905 },
            { "iso-2022-jp", 50222 },
            { "iso-2022-kr", 50225 },
            { "iso-2022jp", 50222 },
            { "iso-2022kr", 50225 },
            { "iso-8859-1", 28591 },
            { "iso-8859-13", 28603 },
            { "iso-8859-15", 28605 },
            { "iso-8859-2", 28592 },
            { "iso-8859-3", 28593 },
            { "iso-8859-4", 28594 },
            { "iso-8859-5", 28595 },
            { "iso-8859-6", 28596 },
            { "iso-8859-7", 28597 },
            { "iso-8859-8", 28598 },
            { "iso-8859-8-i", 38598 },
            { "iso-8859-8i", 38598 },
            { "iso-8859-9", 28599 },
            { "iso2022-jp", 50222 },
            { "iso2022-kr", 50225 },
            { "iso2022jp", 50222 },
            { "iso2022kr", 50225 },
            { "iso8859-1", 28591 },
            { "iso8859-13", 28603 },
            { "iso8859-15", 28605 },
            { "iso8859-2", 28592 },
            { "iso8859-3", 28593 },
            { "iso8859-4", 28594 },
            { "iso8859-5", 28595 },
            { "iso8859-6", 28596 },
            { "iso8859-7", 28597 },
            { "iso8859-8", 28598 },
            { "iso8859-8-i", 38598 },
            { "iso8859-8i", 38598 },
            { "iso8859-9", 28599 },
            { "johab", 1361 },
            { "koi8-r", 20866 },
            { "koi8-u", 21866 },
            { "koi8r", 20866 },
            { "koi8u", 21866 },
            { "ks-c-5601-1987", 949 },
            { "ks_c_5601-1987", 949 },
            { "macintosh", 10000 },
            { "shift-jis", 932 },
            { "shift_jis", 932 },
            { "unicodefffe", 1201 },
            { "us-ascii", 20127 },
            { "utf-16", 1200 },
            { "utf-16be", 1201 },
            { "utf-16le", 1200 },
            { "utf-32", 12000 },
            { "utf-32be", 12001 },
            { "utf-32le", 12000 },
            { "utf-7", 65000 },
            { "utf-8", 65001 },
            { "utf16", 1200 },
            { "utf16be", 1201 },
            { "utf16le", 1200 },
            { "utf32", 12000 },
            { "utf32be", 12001 },
            { "utf32le", 12000 },
            { "utf7", 65000 },
            { "utf8", 65001 },
            { "windows-1250", 1250 },
            { "windows-1251", 1251 },
            { "windows-1252", 1252 },
            { "windows-1253", 1253 },
            { "windows-1254", 1254 },
            { "windows-1255", 1255 },
            { "windows-1256", 1256 },
            { "windows-1257", 1257 },
            { "windows-1258", 1258 },
            { "windows-874", 874 },
            { "windows1250", 1250 },
            { "windows1251", 1251 },
            { "windows1252", 1252 },
            { "windows1253", 1253 },
            { "windows1254", 1254 },
            { "windows1255", 1255 },
            { "windows1256", 1256 },
            { "windows1257", 1257 },
            { "windows1258", 1258 },
            { "windows874", 874 },
            { "x-chinese-cns", 20000 },
            { "x-chinese-eten", 20002 },
            { "x-chinese_cns", 20000 },
            { "x-cp-20001", 20001 },
            { "x-cp-20003", 20003 },
            { "x-cp-20004", 20004 },
            { "x-cp-20005", 20005 },
            { "x-cp-20261", 20261 },
            { "x-cp-20269", 20269 },
            { "x-cp-20936", 20936 },
            { "x-cp-20949", 20949 },
            { "x-cp-50227", 50227 },
            { "x-cp20001", 20001 },
            { "x-cp20003", 20003 },
            { "x-cp20004", 20004 },
            { "x-cp20005", 20005 },
            { "x-cp20261", 20261 },
            { "x-cp20269", 20269 },
            { "x-cp20936", 20936 },
            { "x-cp20949", 20949 },
            { "x-cp50227", 50227 },
            { "x-ebcdic-koreanextended", 20833 },
            { "x-europa", 29001 },
            { "x-ia5", 20105 },
            { "x-ia5-german", 20106 },
            { "x-ia5-norwegian", 20108 },
            { "x-ia5-swedish", 20107 },
            { "x-ia5german", 20106 },
            { "x-ia5norwegian", 20108 },
            { "x-ia5swedish", 20107 },
            { "x-iscii-as", 57006 },
            { "x-iscii-be", 57003 },
            { "x-iscii-de", 57002 },
            { "x-iscii-gu", 57010 },
            { "x-iscii-ka", 57008 },
            { "x-iscii-ma", 57009 },
            { "x-iscii-or", 57007 },
            { "x-iscii-pa", 57011 },
            { "x-iscii-ta", 57004 },
            { "x-iscii-te", 57005 },
            { "x-mac-arabic", 10004 },
            { "x-mac-ce", 10029 },
            { "x-mac-chinesesimp", 10008 },
            { "x-mac-chinesetrad", 10002 },
            { "x-mac-croatian", 10082 },
            { "x-mac-cyrillic", 10007 },
            { "x-mac-greek", 10006 },
            { "x-mac-hebrew", 10005 },
            { "x-mac-icelandic", 10079 },
            { "x-mac-japanese", 10001 },
            { "x-mac-korean", 10003 },
            { "x-mac-romanian", 10010 },
            { "x-mac-thai", 10021 },
            { "x-mac-turkish", 10081 },
            { "x-mac-ukrainian", 10017 },
            { "x_chinese-eten", 20002 }
    };

    ssize_t get_codepage(LCID locale, bool ansi)
    {
        char buf[32];

        int res = GetLocaleInfoA(locale, (ansi) ? LOCALE_IDEFAULTANSICODEPAGE : LOCALE_IDEFAULTCODEPAGE, buf, sizeof(buf)-1);
        if (res == 0)
        {
            switch (GetLastError())
            {
                case ERROR_INSUFFICIENT_BUFFER:
                    return -STATUS_NO_MEM;
                case ERROR_INVALID_FLAGS:
                case ERROR_INVALID_PARAMETER:
                    return -STATUS_BAD_ARGUMENTS;
                default:
                    return -STATUS_UNKNOWN_ERR;
            }
        }

        errno = 0;
        ssize_t cp_num = strtol(buf, NULL, 10);
        if (errno != 0)
            return -STATUS_UNSUPPORTED_FORMAT;
        return cp_num;
    }

    ssize_t codepage_from_name(const char *charset)
    {
        if (charset != NULL)
        {
            // Do lower-case the character set
            size_t n = strlen(charset) + 1;
            char *lower = static_cast<char *>(alloca(n));
            for (size_t i=0; i<n; ++i)
                lower[i] = tolower(charset[i]);

            // Perform binary search of character set
            size_t first = 0, last = sizeof(win_codepages)/sizeof(codepage_t);
            while (first < last)
            {
                size_t middle = (first + last) >> 1;
                int n = strcmp(lower, win_codepages[middle].name);
                if (n == 0)
                    return win_codepages[middle].codepage;
                else if (n < 0)
                    last = middle;
                else
                    first = middle + 1;
            }

            return -1;
        }

//        printf("LOCALE_CUSTOM_DEFAULT = %d\n", int(get_codepage(LOCALE_CUSTOM_DEFAULT)));
//        printf("LOCALE_USER_DEFAULT = %d\n", int(get_codepage(LOCALE_USER_DEFAULT)));
//        printf("LOCALE_SYSTEM_DEFAULT = %d\n", int(get_codepage(LOCALE_SYSTEM_DEFAULT)));
//        printf("LOCALE_CUSTOM_UNSPECIFIED = %d\n", int(get_codepage(LOCALE_CUSTOM_UNSPECIFIED)));
//        printf("LOCALE_CUSTOM_UI_DEFAULT = %d\n", int(get_codepage(LOCALE_CUSTOM_UI_DEFAULT)));
//        printf("LOCALE_INVARIANT = %d\n", int(get_codepage(LOCALE_INVARIANT)));
//        printf("GetConsoleWindow() = %d\n", int(GetConsoleWindow()));
//        printf("GetConsoleOutputCP() = %d\n", int(GetConsoleOutputCP()));
//        fflush(stdout);

        // Obtain system character set
        //ssize_t cp = (GetConsoleWindow() != 0) ? GetConsoleOutputCP() : get_codepage(LOCALE_CUSTOM_DEFAULT);
        ssize_t cp = get_codepage(LOCALE_CUSTOM_DEFAULT);
        if (cp < 0)
            cp = get_codepage(LOCALE_USER_DEFAULT);
        if (cp < 0)
            cp = get_codepage(LOCALE_SYSTEM_DEFAULT);
        return cp;
    }

#else
    iconv_t init_iconv_to_wchar_t(const char *charset)
    {
        // Fetch system character set if it is not set
        if (charset == NULL)
        {
            // Save current locale
            char *current = setlocale(LC_CTYPE, NULL);
            if (current == NULL)
                return iconv_t(-1);
            size_t len = strlen(current) + 1;
            char *psaved = static_cast<char *>(alloca(len));
            ::memcpy(psaved, current, len);
            charset = psaved;

            // Get system locale
            current = setlocale(LC_CTYPE, "");
            if (current != NULL)
                current = strchr(current, '.');

            // Scan for character set
            if (current != NULL)
            {
                len = strlen(current);
                psaved = static_cast<char *>(alloca(len));
                ::memcpy(psaved, &current[1], len);
            }

            // Restore saved locale
            setlocale(LC_CTYPE, charset);

            // Update locale
            charset  = (current != NULL) ? psaved : "UTF-8";
        }

        // Open conversion
        return iconv_open(__IF_LEBE("UTF-32LE", "UTF-32BE"), charset);
    }

    iconv_t init_iconv_from_wchar_t(const char *charset)
    {
        // Fetch system charset if it is not set
        if (charset == NULL)
        {
            // Save current locale
            char *current = setlocale(LC_CTYPE, NULL);
            if (current == NULL)
                return iconv_t(-1);
            size_t len = strlen(current) + 1;
            char *psaved = static_cast<char *>(alloca(len));
            ::memcpy(psaved, current, len);
            charset = psaved;

            // Get system locale
            current = setlocale(LC_CTYPE, "");
            if (current != NULL)
                current = strchr(current, '.');

            // Scan for character set
            if (current != NULL)
            {
                len = strlen(current);
                psaved = static_cast<char *>(alloca(len));
                ::memcpy(psaved, &current[1], len);
            }

            // Restore saved locale
            setlocale(LC_CTYPE, charset);

            // Update charset
            charset  = (current != NULL) ? psaved : "UTF-8";
        }

        // Open conversion
        return iconv_open(charset, __IF_LEBE("UTF-32LE", "UTF-32BE"));
    }
#endif

    //-------------------------------------------------------------------------
    // UTF-16 helper routines
    lsp_utf32_t read_utf16le_codepoint(const lsp_utf16_t **str)
    {
        uint32_t cp, sc;
        const lsp_utf16_t *s = *str;

        cp = LE_TO_CPU(*(s++));
        if (cp == 0)
            return cp;

        sc = cp & 0xfc00;
        if (sc == 0xd800) // cp = Surrogate high
        {
            sc = LE_TO_CPU(*s);
            if ((sc & 0xfc00) == 0xdc00)
            {
                ++s;
                cp  = 0x10000 | ((cp & 0x3ff) << 10) | (sc & 0x3ff);
            }
            else
                cp  = 0xfffd;
        }
        else if (sc == 0xdc00) // Surrogate low?
        {
            sc = LE_TO_CPU(*s);
            if ((sc & 0xfc00) == 0xd800)
            {
                ++s;
                cp  = 0x10000 | ((sc & 0x3ff) << 10) | (cp & 0x3ff);
            }
            else
                cp  = 0xfffd;
        }

        *str = s;
        return cp;
    }

    lsp_utf32_t read_utf16be_codepoint(const lsp_utf16_t **str)
    {
        uint32_t cp, sc;
        const lsp_utf16_t *s = *str;

        cp = BE_TO_CPU(*(s++));
        if (cp == 0)
            return cp;

        sc = cp & 0xfc00;
        if (sc == 0xd800) // cp = Surrogate high
        {
            sc = BE_TO_CPU(*s);
            if ((sc & 0xfc00) == 0xdc00)
            {
                ++s;
                cp  = 0x10000 | ((cp & 0x3ff) << 10) | (sc & 0x3ff);
            }
            else
                cp  = 0xfffd;
        }
        else if (sc == 0xdc00) // Surrogate low?
        {
            sc = BE_TO_CPU(*s);
            if ((sc & 0xfc00) == 0xd800)
            {
                ++s;
                cp  = 0x10000 | ((sc & 0x3ff) << 10) | (cp & 0x3ff);
            }
            else
                cp  = 0xfffd;
        }

        *str = s;
        return cp;
    }

    lsp_utf32_t read_utf16le_streaming(const lsp_utf16_t **str, size_t *nsrc, bool force)
    {
        if (*nsrc <= 0)
            return LSP_UTF32_EOF;

        uint32_t cp, sc;
        const lsp_utf16_t *s = *str;

        cp = LE_TO_CPU(*(s++));
        sc = cp & 0xfc00;
        if (sc == 0xd800) // cp = Surrogate high
        {
            if (*nsrc > 1)
                sc      = LE_TO_CPU(*s);
            else if (force)
                sc      = 0;
            else
                return LSP_UTF32_EOF;

            if ((sc & 0xfc00) == 0xdc00)
            {
                ++s;
                cp  = 0x10000 | ((cp & 0x3ff) << 10) | (sc & 0x3ff);
            }
            else
                cp  = 0xfffd;
        }
        else if (sc == 0xdc00) // Surrogate low?
        {
            if (*nsrc > 1)
                sc      = LE_TO_CPU(*s);
            else if (force)
                sc      = 0;
            else
                return LSP_UTF32_EOF;

            if ((sc & 0xfc00) == 0xd800)
            {
                ++s;
                cp  = 0x10000 | ((sc & 0x3ff) << 10) | (cp & 0x3ff);
            }
            else
                cp  = 0xfffd;
        }

        *nsrc  -= (s - *str);
        *str    = s;
        return cp;
    }

    lsp_utf32_t read_utf16be_streaming(const lsp_utf16_t **str, size_t *nsrc, bool force)
    {
        if (*nsrc <= 0)
            return LSP_UTF32_EOF;

        uint32_t cp, sc;
        const lsp_utf16_t *s = *str;

        cp = BE_TO_CPU(*(s++));
        sc = cp & 0xfc00;
        if (sc == 0xd800) // cp = Surrogate high
        {
            if (*nsrc > 1)
                sc      = BE_TO_CPU(*s);
            else if (force)
                sc      = 0;
            else
                return LSP_UTF32_EOF;

            if ((sc & 0xfc00) == 0xdc00)
            {
                ++s;
                cp  = 0x10000 | ((cp & 0x3ff) << 10) | (sc & 0x3ff);
            }
            else
                cp  = 0xfffd;
        }
        else if (sc == 0xdc00) // Surrogate low?
        {
            if (*nsrc > 1)
                sc      = BE_TO_CPU(*s);
            else if (force)
                sc      = 0;
            else
                return LSP_UTF32_EOF;

            if ((sc & 0xfc00) == 0xd800)
            {
                ++s;
                cp  = 0x10000 | ((sc & 0x3ff) << 10) | (cp & 0x3ff);
            }
            else
                cp  = 0xfffd;
        }

        *nsrc  -= (s - *str);
        *str    = s;
        return cp;
    }

    inline size_t sizeof_utf16(lsp_utf32_t cp)
    {
        return (cp < 0x10000) ? 2 : 4;
    }

    inline size_t count_utf16(lsp_utf32_t cp)
    {
        return (cp < 0x10000) ? 1 : 2;
    }

    void write_utf16le_codepoint(lsp_utf16_t **str, lsp_utf32_t cp)
    {
        lsp_utf16_t *dst = *str;
        if (cp < 0x10000)
            *(dst++)        = CPU_TO_LE(lsp_utf16_t(cp));
        else
        {
            cp     -= 0x10000;
            dst[0]  = CPU_TO_LE(lsp_utf16_t(0xd800 | (cp >> 10)));
            dst[1]  = CPU_TO_LE(lsp_utf16_t(0xdc00 | (cp & 0x3ff)));
            dst    += 2;
        }
        *str    = dst;
    }

    void write_utf16be_codepoint(lsp_utf16_t **str, lsp_utf32_t cp)
    {
        lsp_utf16_t *dst = *str;
        if (cp < 0x10000)
            *(dst++)        = CPU_TO_BE(lsp_utf16_t(cp));
        else
        {
            cp     -= 0x10000;
            dst[0]  = CPU_TO_BE(lsp_utf16_t(0xd800 | (cp >> 10)));
            dst[1]  = CPU_TO_BE(lsp_utf16_t(0xdc00 | (cp & 0x3ff)));
            dst    += 2;
        }
        *str    = dst;
    }

    //-------------------------------------------------------------------------
    // UTF-8 helper routines
    lsp_utf32_t read_utf8_codepoint(const char **str)
    {
        lsp_utf32_t cp, sp;
        size_t bytes;
        const char *s = *str;

        // Decode primary byte
        cp = uint8_t(*s);
        if (cp <= 0x7f)
        {
            *str    = (cp == 0) ? s : s+1;
            return cp;
        }

        ++s;
        if ((cp & 0xe0) == 0xc0) // 2 bytes: 110xxxxx 10xxxxxx
        {
            cp     &= 0x1f;
            bytes   = (cp >= 0x02) ? 1 : 0;
        }
        else if ((cp & 0xf0) == 0xe0) // 3 bytes: 1110xxxx 10xxxxxx 10xxxxxx
        {
            cp     &= 0x0f;
            bytes   = (cp) ? 2 : 0;
        }
        else if ((cp & 0xf8) == 0xf0) // 4 bytes: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        {
            cp     &= 0x07;
            bytes   = 3;
        }
        else
            bytes   = 0;

        // Invalid first byte sequence?
        if (!bytes)
        {
            *str    = s;
            return 0xfffd;
        }

        // Decode extension bytes
        for (size_t i=0; i<bytes; ++i)
        {
            sp  = uint8_t(*s);
            if ((sp & 0xc0) != 0x80) // Invalid sequence?
            {
                *str    = (sp == 0) ? s : s+1;
                return 0xfffd;
            }
            cp     = (cp << 6) | (sp & 0x3f);
            ++s;
        }

        if ((bytes == 3) && (cp < 0x10000)) // Check that 4-byte sequence is valid
            cp      = 0xfffd;
        else if ((cp >= 0xd800) && (cp < 0xe000)) // Check for surrogates
            cp      = 0xfffd;

        *str = s;
        return cp;
    }

    lsp_utf32_t read_utf8_streaming(const char **str, size_t *nsrc, bool force)
    {
        if (*nsrc <= 0)
            return LSP_UTF32_EOF;

        lsp_utf32_t cp, sp;
        size_t bytes;
        const char *s = *str;

        // Decode primary byte
        cp = uint8_t(*s);
        if (cp <= 0x7f)
        {
            *str    = (cp == 0) ? s : s+1;
            --(*nsrc);
            return cp;
        }

        // Multi-byte sequence
        ++s;
        if ((cp & 0xe0) == 0xc0) // 2 bytes: 110xxxxx 10xxxxxx
        {
            cp     &= 0x1f;
            bytes   = (cp >= 0x02) ? 1 : 0;
        }
        else if ((cp & 0xf0) == 0xe0) // 3 bytes: 1110xxxx 10xxxxxx 10xxxxxx
        {
            cp     &= 0x0f;
            bytes   = (cp) ? 2 : 0;
        }
        else if ((cp & 0xf8) == 0xf0) // 4 bytes: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        {
            cp     &= 0x07;
            bytes   = 3;
        }
        else
            bytes   = 0;

        // Invalid first byte sequence?
        if (!bytes)
        {
            *str    = s;
            --(*nsrc);
            return 0xfffd;
        }
        else if (bytes >= *nsrc)
        {
            if (force)
            {
                *nsrc   = 0;
                return 0xfffd;
            }
            return LSP_UTF32_EOF;
        }

        // Decode extension bytes
        for (size_t i=0; i<bytes; ++i)
        {
            sp  = uint8_t(*s);
            if ((sp & 0xc0) != 0x80) // Invalid sequence?
            {
                if (sp == 0)
                    ++s;
                *nsrc  -= (s - *str);
                *str    = s;
                return 0xfffd;
            }
            cp     = (cp << 6) | (sp & 0x3f);
            ++s;
        }

        if ((bytes == 3) && (cp < 0x10000)) // Check that 4-byte sequence is valid
            cp      = 0xfffd;
        else if ((cp >= 0xd800) && (cp < 0xe000)) // Check for surrogates
            cp      = 0xfffd;

        *nsrc      -= (s - *str);
        *str        = s;
        return cp;
    }

    inline size_t sizeof_utf8(lsp_utf32_t cp)
    {
        if (cp >= 0x800)
            return ((cp < 0x10000) || (cp >= 0x200000)) ? 3 : 4;
        else
            return (cp >= 0x80) ? 2 : 1;
    }

    inline size_t count_utf8(lsp_utf32_t cp)
    {
        if (cp >= 0x800)
            return ((cp < 0x10000) || (cp >= 0x200000)) ? 3 : 4;
        else
            return (cp >= 0x80) ? 2 : 1;
    }

    void write_utf8_codepoint(char **str, lsp_utf32_t cp)
    {
        char *dst = *str;
        if (cp >= 0x800) // 3-4 bytes
        {
            if (cp < 0x10000) // 3 bytes
            {
                dst[0]      = (cp >> 12) | 0xe0;
                dst[1]      = ((cp >> 6) & 0x3f) | 0x80;
                dst[2]      = (cp & 0x3f) | 0x80;
                dst        += 3;
            }
            else if (cp < 0x200000) // 4 bytes
            {
                dst[0]      = (cp >> 16) | 0xf0;
                dst[1]      = ((cp >> 12) & 0x3f) | 0x80;
                dst[2]      = ((cp >> 6) & 0x3f) | 0x80;
                dst[3]      = (cp & 0x3f) | 0x80;
                dst        += 4;
            }
            else // Invalid character, emit 3 bytes of 0xfffd code point value
            {
                dst[0]      = 0xef;
                dst[1]      = 0xbf;
                dst[2]      = 0xbd;
                dst        += 3;
            }
        }
        else // 1-2 bytes
        {
            if (cp >= 0x80) // 2 bytes
            {
                dst[0]      = (cp >> 6) | 0xc0;
                dst[1]      = (cp & 0x3f) | 0x80;
                dst        += 2;
            }
            else // 1 byte
                *(dst++)    = char(cp);
        }
        *str    = dst;
    }

    //-------------------------------------------------------------------------
    // UTF-8 non-streaming routines
    lsp_utf16_t *utf8_to_utf16le(const char *str)
    {
        // Estimate number of bytes
        lsp_utf32_t cp;
        size_t bytes    = 0;
        const char *p = str;
        do
        {
            cp      = read_utf8_codepoint(&p);
            bytes  += sizeof_utf16(cp);
        } while (cp != 0);

        // Allocate memory
        lsp_utf16_t *utf16  = reinterpret_cast<lsp_utf16_t *>(::malloc(bytes));
        if (utf16 == NULL)
            return NULL;

        // Perform encoding
        lsp_utf16_t *dst = utf16;
        p               = str;
        while ((cp = read_utf8_codepoint(&p)) != 0)
            write_utf16le_codepoint(&dst, cp);
        *dst        = 0;

        return utf16;
    }

    lsp_utf16_t *utf8_to_utf16be(const char *str)
    {
        // Estimate number of bytes
        lsp_utf32_t cp;
        size_t bytes    = 0;
        const char *p = str;
        do
        {
            cp      = read_utf8_codepoint(&p);
            bytes  += sizeof_utf16(cp);
        } while (cp != 0);

        // Allocate memory
        lsp_utf16_t *utf16  = reinterpret_cast<lsp_utf16_t *>(::malloc(bytes));
        if (utf16 == NULL)
            return NULL;

        // Perform encoding
        lsp_utf16_t *dst = utf16;
        p               = str;
        while ((cp = read_utf8_codepoint(&p)) != 0)
            write_utf16be_codepoint(&dst, cp);
        *dst        = 0;

        return utf16;
    }

    lsp_utf32_t *utf8_to_utf32le(const char *str)
    {
        // Estimate number of bytes
        lsp_utf32_t cp;
        size_t bytes    = 0;
        const char *p = str;
        do
        {
            cp      = read_utf8_codepoint(&p);
            bytes  += sizeof(lsp_utf32_t);
        } while (cp != 0);

        // Allocate memory
        lsp_utf32_t *utf32  = reinterpret_cast<lsp_utf32_t *>(::malloc(bytes));
        if (utf32 == NULL)
            return NULL;

        // Perform encoding
        lsp_utf32_t *dst = utf32;
        p               = str;
        while ((cp = read_utf8_codepoint(&p)) != 0)
            *(dst++)    = CPU_TO_LE(cp);
        *dst        = 0;

        return utf32;
    }

    lsp_utf32_t *utf8_to_utf32be(const char *str)
    {
        // Estimate number of bytes
        lsp_utf32_t cp;
        size_t bytes    = 0;
        const char *p = str;
        do
        {
            cp      = read_utf8_codepoint(&p);
            bytes  += sizeof(lsp_utf32_t);
        } while (cp != 0);

        // Allocate memory
        lsp_utf32_t *utf32  = reinterpret_cast<lsp_utf32_t *>(::malloc(bytes));
        if (utf32 == NULL)
            return NULL;

        // Perform encoding
        lsp_utf32_t *dst = utf32;
        p               = str;
        while ((cp = read_utf8_codepoint(&p)) != 0)
            *(dst++)    = CPU_TO_BE(cp);
        *dst        = 0;

        return utf32;
    }

    //-------------------------------------------------------------------------
    // UTF-16 non-streaming routines
    char *utf16le_to_utf8(const lsp_utf16_t *str)
    {
        // Estimate number of bytes
        lsp_utf32_t cp;
        size_t bytes = 0;
        const lsp_utf16_t *p = str;
        do
        {
            cp          = read_utf16le_codepoint(&p);
            bytes      += sizeof_utf8(cp);
        } while (cp != 0);

        // Allocate memory
        char *utf8  = reinterpret_cast<char *>(::malloc(bytes));
        if (utf8 == NULL)
            return NULL;

        // Now perform encoding
        char *dst   = utf8;
        p           = str;
        while ((cp = read_utf16le_codepoint(&p)) != 0)
            write_utf8_codepoint(&dst, cp);
        *dst = '\0';

        return utf8;
    }

    char *utf16be_to_utf8(const lsp_utf16_t *str)
    {
        // Estimate number of bytes
        lsp_utf32_t cp;
        size_t bytes = 0;
        const lsp_utf16_t *p = str;
        do
        {
            cp          = read_utf16be_codepoint(&p);
            bytes      += sizeof_utf8(cp);
        } while (cp != 0);

        // Allocate memory
        char *utf8  = reinterpret_cast<char *>(::malloc(bytes));
        if (utf8 == NULL)
            return NULL;

        // Now perform encoding
        char *dst   = utf8;
        p           = str;
        while ((cp = read_utf16be_codepoint(&p)) != 0)
            write_utf8_codepoint(&dst, cp);
        *dst = '\0';

        return utf8;
    }

    lsp_utf32_t *utf16le_to_utf32le(const lsp_utf16_t *str)
    {
        // Estimate number of bytes
        lsp_utf32_t cp;
        size_t bytes = 0;
        const lsp_utf16_t *p = str;
        do
        {
            cp          = read_utf16le_codepoint(&p);
            bytes      += sizeof(lsp_utf32_t);
        } while (cp != 0);

        // Allocate memory
        lsp_utf32_t *utf32  = reinterpret_cast<lsp_utf32_t *>(::malloc(bytes));
        if (utf32 == NULL)
            return NULL;

        // Perform encoding
        p               = str;
        lsp_utf32_t *dst= utf32;
        while ((cp = read_utf16le_codepoint(&p)) != 0)
            *(dst++)        = CPU_TO_LE(cp);
        *dst            = 0;

        return utf32;
    }

    lsp_utf32_t *utf16le_to_utf32be(const lsp_utf16_t *str)
    {
        // Estimate number of bytes
        lsp_utf32_t cp;
        size_t bytes = 0;
        const lsp_utf16_t *p = str;
        do
        {
            cp          = read_utf16le_codepoint(&p);
            bytes      += sizeof(lsp_utf32_t);
        } while (cp != 0);

        // Allocate memory
        lsp_utf32_t *utf32  = reinterpret_cast<lsp_utf32_t *>(::malloc(bytes));
        if (utf32 == NULL)
            return NULL;

        // Perform encoding
        p               = str;
        lsp_utf32_t *dst= utf32;
        while ((cp = read_utf16le_codepoint(&p)) != 0)
            *(dst++)        = CPU_TO_BE(cp);
        *dst            = 0;

        return utf32;
    }

    lsp_utf32_t *utf16be_to_utf32le(const lsp_utf16_t *str)
    {
        // Estimate number of bytes
        lsp_utf32_t cp;
        size_t bytes = 0;
        const lsp_utf16_t *p = str;
        do
        {
            cp          = read_utf16be_codepoint(&p);
            bytes      += sizeof(lsp_utf32_t);
        } while (cp != 0);

        // Allocate memory
        lsp_utf32_t *utf32  = reinterpret_cast<lsp_utf32_t *>(::malloc(bytes));
        if (utf32 == NULL)
            return NULL;

        // Perform encoding
        p               = str;
        lsp_utf32_t *dst= utf32;
        while ((cp = read_utf16be_codepoint(&p)) != 0)
            *(dst++)        = CPU_TO_LE(cp);
        *dst            = 0;

        return utf32;
    }

    lsp_utf32_t *utf16be_to_utf32be(const lsp_utf16_t *str)
    {
        // Estimate number of bytes
        lsp_utf32_t cp;
        size_t bytes = 0;
        const lsp_utf16_t *p = str;
        do
        {
            cp          = read_utf16be_codepoint(&p);
            bytes      += sizeof(lsp_utf32_t);
        } while (cp != 0);

        // Allocate memory
        lsp_utf32_t *utf32  = reinterpret_cast<lsp_utf32_t *>(::malloc(bytes));
        if (utf32 == NULL)
            return NULL;

        // Perform encoding
        p               = str;
        lsp_utf32_t *dst= utf32;
        while ((cp = read_utf16be_codepoint(&p)) != 0)
            *(dst++)        = CPU_TO_BE(cp);
        *dst            = 0;

        return utf32;
    }

    //-------------------------------------------------------------------------
    // UTF-32 non-streaming routines
    char *utf32le_to_utf8(const lsp_utf32_t *str)
    {
        lsp_utf32_t cp;
        size_t bytes = 0;
        const lsp_utf32_t *p = str;

        // Estimate length
        do
        {
            cp          = LE_TO_CPU(*(p++));
            bytes      += sizeof_utf8(cp);
        } while (cp != 0);

        // Allocate memory
        char *utf8      = reinterpret_cast<char *>(::malloc(bytes));
        if (utf8 == NULL)
            return NULL;

        // Perform encoding
        p               = str;
        char *dst       = utf8;
        while ((cp = *(p++)) != 0)
            write_utf8_codepoint(&dst, cp);

        *dst = 0;
        return utf8;
    }

    char *utf32be_to_utf8(const lsp_utf32_t *str)
    {
        lsp_utf32_t cp;
        size_t bytes = 0;
        const lsp_utf32_t *p = str;

        // Estimate length
        do
        {
            cp          = BE_TO_CPU(*(p++));
            bytes      += sizeof_utf8(cp);
        } while (cp != 0);

        // Allocate memory
        char *utf8      = reinterpret_cast<char *>(::malloc(bytes));
        if (utf8 == NULL)
            return NULL;

        // Perform encoding
        p               = str;
        char *dst       = utf8;
        while ((cp = *(p++)) != 0)
            write_utf8_codepoint(&dst, cp);

        *dst = 0;
        return utf8;
    }

    lsp_utf16_t *utf32le_to_utf16le(const lsp_utf32_t *str)
    {
        lsp_utf32_t cp;
        size_t bytes = 0;
        const lsp_utf32_t *p = str;

        // Estimate length
        do
        {
            cp          = LE_TO_CPU(*(p++));
            bytes      += sizeof_utf16(cp);
        } while (cp != 0);

        // Allocate memory
        lsp_utf16_t *utf16  = reinterpret_cast<lsp_utf16_t *>(::malloc(bytes));
        if (utf16 == NULL)
            return NULL;

        // Perform encoding
        p               = str;
        lsp_utf16_t *dst= utf16;
        while ((cp = *(p++)) != 0)
            write_utf16le_codepoint(&dst, cp);

        *dst = 0;
        return utf16;
    }

    lsp_utf16_t *utf32le_to_utf16be(const lsp_utf32_t *str)
    {
        lsp_utf32_t cp;
        size_t bytes = 0;
        const lsp_utf32_t *p = str;

        // Estimate length
        do
        {
            cp          = LE_TO_CPU(*(p++));
            bytes      += sizeof_utf16(cp);
        } while (cp != 0);

        // Allocate memory
        lsp_utf16_t *utf16  = reinterpret_cast<lsp_utf16_t *>(::malloc(bytes));
        if (utf16 == NULL)
            return NULL;

        // Perform encoding
        p               = str;
        lsp_utf16_t *dst= utf16;
        while ((cp = *(p++)) != 0)
            write_utf16be_codepoint(&dst, cp);

        *dst = 0;
        return utf16;
    }

    lsp_utf16_t *utf32be_to_utf16le(const lsp_utf32_t *str)
    {
        lsp_utf32_t cp;
        size_t bytes = 0;
        const lsp_utf32_t *p = str;

        // Estimate length
        do
        {
            cp          = BE_TO_CPU(*(p++));
            bytes      += sizeof_utf16(cp);
        } while (cp != 0);

        // Allocate memory
        lsp_utf16_t *utf16  = reinterpret_cast<lsp_utf16_t *>(::malloc(bytes));
        if (utf16 == NULL)
            return NULL;

        // Perform encoding
        p               = str;
        lsp_utf16_t *dst= utf16;
        while ((cp = *(p++)) != 0)
            write_utf16le_codepoint(&dst, cp);

        *dst = 0;
        return utf16;
    }

    lsp_utf16_t *utf32be_to_utf16be(const lsp_utf32_t *str)
    {
        lsp_utf32_t cp;
        size_t bytes = 0;
        const lsp_utf32_t *p = str;

        // Estimate length
        do
        {
            cp          = BE_TO_CPU(*(p++));
            bytes      += sizeof_utf16(cp);
        } while (cp != 0);

        // Allocate memory
        lsp_utf16_t *utf16  = reinterpret_cast<lsp_utf16_t *>(::malloc(bytes));
        if (utf16 == NULL)
            return NULL;

        // Perform encoding
        p               = str;
        lsp_utf16_t *dst= utf16;
        while ((cp = *(p++)) != 0)
            write_utf16be_codepoint(&dst, cp);

        *dst = 0;
        return utf16;
    }

    //-------------------------------------------------------------------------
    // UTF-8 streaming routines
    size_t utf8_to_utf16le(lsp_utf16_t *dst, size_t *ndst, const char *src, size_t *nsrc, bool force)
    {
        lsp_utf32_t cp;
        size_t processed = 0;

        while (*ndst > 0)
        {
            // Read code point
            size_t nin  = *nsrc;
            cp          = read_utf8_streaming(&src, &nin, force);
            if (cp == LSP_UTF32_EOF) // No data ?
                break;

            // Encode code point
            size_t nout = count_utf16(cp);
            if (nout > *ndst)
                break;
            write_utf16le_codepoint(&dst, cp);
            *nsrc       = nin;
            *ndst      -= nout;

            // Update statistics
            ++processed;
        }

        return processed;
    }

    size_t utf8_to_utf16be(lsp_utf16_t *dst, size_t *ndst, const char *src, size_t *nsrc, bool force)
    {
        lsp_utf32_t cp;
        size_t processed = 0;

        while (*ndst > 0)
        {
            // Read code point
            size_t nin  = *nsrc;
            cp          = read_utf8_streaming(&src, &nin, force);
            if (cp == LSP_UTF32_EOF) // No data ?
                break;

            // Encode code point
            size_t nout = count_utf16(cp);
            if (nout > *ndst)
                break;
            write_utf16be_codepoint(&dst, cp);
            *nsrc       = nin;
            *ndst      -= nout;

            // Update statistics
            ++processed;
        }

        return processed;
    }

    size_t utf8_to_utf32le(lsp_utf32_t *dst, size_t *ndst, const char *src, size_t *nsrc, bool force)
    {
        lsp_utf32_t cp;
        size_t processed = 0;

        while (*ndst > 0)
        {
            // Read code point
            size_t nin  = *nsrc;
            cp          = read_utf8_streaming(&src, &nin, force);
            if (cp == LSP_UTF32_EOF) // No data ?
                break;

            // Encode code point
            *(dst++)    = CPU_TO_LE(cp);
            *nsrc       = nin;
            --(*ndst);

            // Update statistics
            ++processed;
        }

        return processed;
    }

    size_t utf8_to_utf32be(lsp_utf32_t *dst, size_t *ndst, const char *src, size_t *nsrc, bool force)
    {
        lsp_utf32_t cp;
        size_t processed = 0;

        while (*ndst > 0)
        {
            // Read code point
            size_t nin  = *nsrc;
            cp          = read_utf8_streaming(&src, &nin, force);
            if (cp == LSP_UTF32_EOF) // No data ?
                break;

            // Encode code point
            *(dst++)    = CPU_TO_BE(cp);
            *nsrc       = nin;
            --(*ndst);

            // Update statistics
            ++processed;
        }

        return processed;
    }

    //-------------------------------------------------------------------------
    // UTF-16 streaming routines
    size_t utf16le_to_utf8(char *dst, size_t *ndst, const lsp_utf16_t *src, size_t *nsrc, bool force)
    {
        lsp_utf32_t cp;
        size_t processed = 0;

        while (*ndst > 0)
        {
            // Read code point
            size_t nin  = *nsrc;
            cp          = read_utf16le_streaming(&src, &nin, force);
            if (cp == LSP_UTF32_EOF) // No data ?
                break;

            // Encode code point
            size_t nout = count_utf8(cp);
            if (nout > *ndst)
                break;
            write_utf8_codepoint(&dst, cp);
            *nsrc       = nin;
            *ndst      -= nout;

            // Update statistics
            ++processed;
        }

        return processed;
    }

    size_t utf16be_to_utf8(char *dst, size_t *ndst, const lsp_utf16_t *src, size_t *nsrc, bool force)
    {
        lsp_utf32_t cp;
        size_t processed = 0;

        while (*ndst > 0)
        {
            // Read code point
            size_t nin  = *nsrc;
            cp          = read_utf16be_streaming(&src, &nin, force);
            if (cp == LSP_UTF32_EOF) // No data ?
                break;

            // Encode code point
            size_t nout = count_utf8(cp);
            if (nout > *ndst)
                break;
            write_utf8_codepoint(&dst, cp);
            *nsrc       = nin;
            *ndst      -= nout;

            // Update statistics
            ++processed;
        }

        return processed;
    }

    size_t utf16le_to_utf32le(lsp_utf32_t *dst, size_t *ndst, const lsp_utf16_t *src, size_t *nsrc, bool force)
    {
        lsp_utf32_t cp;
        size_t processed = 0;

        while (*ndst > 0)
        {
            // Read code point
            size_t nin  = *nsrc;
            cp          = read_utf16le_streaming(&src, &nin, force);
            if (cp == LSP_UTF32_EOF) // No data ?
                break;

            // Encode code point
            *(dst++)    = CPU_TO_LE(cp);
            *nsrc       = nin;
            --(*ndst);

            // Update statistics
            ++processed;
        }

        return processed;
    }

    size_t utf16be_to_utf32le(lsp_utf32_t *dst, size_t *ndst, const lsp_utf16_t *src, size_t *nsrc, bool force)
    {
        lsp_utf32_t cp;
        size_t processed = 0;

        while (*ndst > 0)
        {
            // Read code point
            size_t nin  = *nsrc;
            cp          = read_utf16be_streaming(&src, &nin, force);
            if (cp == LSP_UTF32_EOF) // No data ?
                break;

            // Encode code point
            *(dst++)    = CPU_TO_LE(cp);
            *nsrc       = nin;
            --(*ndst);

            // Update statistics
            ++processed;
        }

        return processed;
    }

    size_t utf16le_to_utf32be(lsp_utf32_t *dst, size_t *ndst, const lsp_utf16_t *src, size_t *nsrc, bool force)
    {
        lsp_utf32_t cp;
        size_t processed = 0;

        while (*ndst > 0)
        {
            // Read code point
            size_t nin  = *nsrc;
            cp          = read_utf16le_streaming(&src, &nin, force);
            if (cp == LSP_UTF32_EOF) // No data ?
                break;

            // Encode code point
            *(dst++)    = CPU_TO_BE(cp);
            *nsrc       = nin;
            --(*ndst);

            // Update statistics
            ++processed;
        }

        return processed;
    }

    size_t utf16be_to_utf32be(lsp_utf32_t *dst, size_t *ndst, const lsp_utf16_t *src, size_t *nsrc, bool force)
    {
        lsp_utf32_t cp;
        size_t processed = 0;

        while (*ndst > 0)
        {
            // Read code point
            size_t nin  = *nsrc;
            cp          = read_utf16be_streaming(&src, &nin, force);
            if (cp == LSP_UTF32_EOF) // No data ?
                break;

            // Encode code point
            *(dst++)    = CPU_TO_BE(cp);
            *nsrc       = nin;
            --(*ndst);

            // Update statistics
            ++processed;
        }

        return processed;
    }

    //-------------------------------------------------------------------------
    // UTF-32 streaming routines
    size_t utf32le_to_utf8(char *dst, size_t *ndst, const lsp_utf32_t *src, size_t *nsrc, bool force)
    {
        lsp_utf32_t cp;
        size_t processed = 0;

        while (*ndst > 0)
        {
            // Read code point
            if (*nsrc <= 0)
                break;
            cp          = LE_TO_CPU(*(src++));

            // Encode code point
            size_t nout = count_utf8(cp);
            if (nout > *ndst)
                break;
            write_utf8_codepoint(&dst, cp);
            --(*nsrc);
            *ndst      -= nout;

            // Update statistics
            ++processed;
        }

        return processed;
    }

    size_t utf32be_to_utf8(char *dst, size_t *ndst, const lsp_utf32_t *src, size_t *nsrc, bool force)
    {
        lsp_utf32_t cp;
        size_t processed = 0;

        while (*ndst > 0)
        {
            // Read code point
            if (*nsrc <= 0)
                break;
            cp          = BE_TO_CPU(*(src++));

            // Encode code point
            size_t nout = count_utf8(cp);
            if (nout > *ndst)
                break;
            write_utf8_codepoint(&dst, cp);
            --(*nsrc);
            *ndst      -= nout;

            // Update statistics
            ++processed;
        }

        return processed;
    }

    size_t utf32le_to_utf16le(lsp_utf16_t *dst, size_t *ndst, const lsp_utf32_t *src, size_t *nsrc, bool force)
    {
        lsp_utf32_t cp;
        size_t processed = 0;

        while (*ndst > 0)
        {
            // Read code point
            if (*nsrc <= 0)
                break;
            cp          = LE_TO_CPU(*(src++));

            // Encode code point
            size_t nout = count_utf16(cp);
            if (nout > *ndst)
                break;
            write_utf16le_codepoint(&dst, cp);
            --(*nsrc);
            *ndst      -= nout;

            // Update statistics
            ++processed;
        }

        return processed;
    }

    size_t utf32le_to_utf16be(lsp_utf16_t *dst, size_t *ndst, const lsp_utf32_t *src, size_t *nsrc, bool force)
    {
        lsp_utf32_t cp;
        size_t processed = 0;

        while (*ndst > 0)
        {
            // Read code point
            if (*nsrc <= 0)
                break;
            cp          = LE_TO_CPU(*(src++));

            // Encode code point
            size_t nout = count_utf16(cp);
            if (nout > *ndst)
                break;
            write_utf16be_codepoint(&dst, cp);
            --(*nsrc);
            *ndst      -= nout;

            // Update statistics
            ++processed;
        }

        return processed;
    }

    size_t utf32be_to_utf16le(lsp_utf16_t *dst, size_t *ndst, const lsp_utf32_t *src, size_t *nsrc, bool force)
    {
        lsp_utf32_t cp;
        size_t processed = 0;

        while (*ndst > 0)
        {
            // Read code point
            if (*nsrc <= 0)
                break;
            cp          = BE_TO_CPU(*(src++));

            // Encode code point
            size_t nout = count_utf16(cp);
            if (nout > *ndst)
                break;
            write_utf16le_codepoint(&dst, cp);
            --(*nsrc);
            *ndst      -= nout;

            // Update statistics
            ++processed;
        }

        return processed;
    }

    size_t utf32be_to_utf16be(lsp_utf16_t *dst, size_t *ndst, const lsp_utf32_t *src, size_t *nsrc, bool force)
    {
        lsp_utf32_t cp;
        size_t processed = 0;

        while (*ndst > 0)
        {
            // Read code point
            if (*nsrc <= 0)
                break;
            cp          = BE_TO_CPU(*(src++));

            // Encode code point
            size_t nout = count_utf16(cp);
            if (nout > *ndst)
                break;
            write_utf16be_codepoint(&dst, cp);
            --(*nsrc);
            *ndst      -= nout;

            // Update statistics
            ++processed;
        }

        return processed;
    }

#if defined(PLATFORM_WINDOWS)
    static ssize_t multibyte_to_widechar_utf16le(LPCCH src, size_t *nsrc, LPWSTR dst, size_t *ndst)
    {
        lsp_wchar_t cp;
        ssize_t nconv   = 0;
        const lsp_utf16_t *xsrc = reinterpret_cast<const lsp_utf16_t *>(src);
        size_t nin  = (*nsrc) >> 1;
        size_t nout = *ndst;

        while (nin > 0)
        {
            // Read code point
            size_t xin  = nin;
            cp          = read_utf16le_streaming(&xsrc, &xin, false);
            if (cp == LSP_UTF32_EOF) // No data ?
                break;

            // Check that we have enough space
            size_t len = count_utf16(cp);
            if (nout < len)
                break;

            // Write code point
            write_utf16_codepoint(&dst, cp);
            nin         = xin;
            nout       -= len;
            nconv      += len;
        }

        *nsrc       = ((*nsrc) & 1) + (nin << 1);
        *ndst       = nout;
        return nconv;
    }

    static ssize_t multibyte_to_widechar_utf16be(LPCCH src, size_t *nsrc, LPWSTR dst, size_t *ndst)
    {
        lsp_wchar_t cp;
        ssize_t nconv   = 0;
        const lsp_utf16_t *xsrc = reinterpret_cast<const lsp_utf16_t *>(src);
        size_t nin  = (*nsrc) >> 1;
        size_t nout = *ndst;

        while (nin > 0)
        {
            // Read code point
            size_t xin  = nin;
            cp          = read_utf16be_streaming(&xsrc, &xin, false);
            if (cp == LSP_UTF32_EOF) // No data ?
                break;

            // Check that we have enough space
            size_t len = count_utf16(cp);
            if (nout < len)
                break;

            // Write code point
            write_utf16_codepoint(&dst, cp);
            nin         = xin;
            nout       -= len;
            nconv      += len;
        }

        *nsrc       = ((*nsrc) & 1) + (nin << 1);
        *ndst       = nout;
        return nconv;
    }

    static ssize_t est_multibyte_to_widechar_utf16le(LPCCH src, size_t nsrc)
    {
        lsp_wchar_t cp;
        ssize_t nconv   = 0;
        const lsp_utf16_t *xsrc = reinterpret_cast<const lsp_utf16_t *>(src);
        nsrc >>= 1;

        while (nsrc > 0)
        {
            // Read code point
            cp         = read_utf16le_streaming(&xsrc, &nsrc, false);
            if (cp == LSP_UTF32_EOF) // No data ?
                break;

            // Check that we have enough space
            nconv      += count_utf16(cp);
        }

        return nconv;
    }

    static ssize_t est_multibyte_to_widechar_utf16be(LPCCH src, size_t nsrc)
    {
        lsp_wchar_t cp;
        ssize_t nconv   = 0;
        const lsp_utf16_t *xsrc = reinterpret_cast<const lsp_utf16_t *>(src);
        nsrc >>= 1;

        while (nsrc > 0)
        {
            // Read code point
            cp         = read_utf16le_streaming(&xsrc, &nsrc, false);
            if (cp == LSP_UTF32_EOF) // No data ?
                break;

            // Check that we have enough space
            nconv      += count_utf16(cp);
        }

        return nconv;
    }

    static ssize_t multibyte_to_widechar_utf32le(LPCCH src, size_t *nsrc, LPWSTR dst, size_t *ndst)
    {
        lsp_wchar_t cp;
        ssize_t nconv   = 0;
        const lsp_utf32_t *xsrc = reinterpret_cast<const lsp_utf32_t *>(src);
        size_t nin  = (*nsrc) >> 2;
        size_t nout = *ndst;

        while (nin > 0)
        {
            // Read code point
            cp          = LE_TO_CPU(*(xsrc++));

            // Check that we have enough space
            size_t len = count_utf16(cp);
            if (nout < len)
                break;

            // Write code point
            write_utf16_codepoint(&dst, cp);
            nin        -= 1;
            nout       -= len;
            nconv      += len;
        }

        *nsrc       = ((*nsrc) & 3) + (nin << 2);
        *ndst       = nout;
        return nconv;
    }

    static ssize_t multibyte_to_widechar_utf32be(LPCCH src, size_t *nsrc, LPWSTR dst, size_t *ndst)
    {
        lsp_wchar_t cp;
        ssize_t nconv   = 0;
        const lsp_utf32_t *xsrc = reinterpret_cast<const lsp_utf32_t *>(src);
        size_t nin  = (*nsrc) >> 2;
        size_t nout = *ndst;

        while (nin > 0)
        {
            // Read code point
            cp          = BE_TO_CPU(*(xsrc++));

            // Check that we have enough space
            size_t len = count_utf16(cp);
            if (nout < len)
                break;

            // Write code point
            write_utf16_codepoint(&dst, cp);
            nin        -= 1;
            nout       -= len;
            nconv      += len;
        }

        *nsrc       = ((*nsrc) & 3) + (nin << 2);
        *ndst       = nout;
        return nconv;
    }

    static ssize_t est_multibyte_to_widechar_utf32le(LPCCH src, size_t nsrc)
    {
        lsp_wchar_t cp;
        ssize_t nconv   = 0;
        const lsp_utf32_t *xsrc = reinterpret_cast<const lsp_utf32_t *>(src);
        nsrc           >>= 2;

        while (nsrc > 0)
        {
            // Read code point
            cp          = LE_TO_CPU(*(xsrc++));
            nconv      += count_utf16(cp);
        }

        return nconv;
    }

    static ssize_t est_multibyte_to_widechar_utf32be(LPCCH src, size_t nsrc)
    {
        lsp_wchar_t cp;
        ssize_t nconv   = 0;
        const lsp_utf32_t *xsrc = reinterpret_cast<const lsp_utf32_t *>(src);
        nsrc           >>= 2;

        while (nsrc > 0)
        {
            // Read code point
            cp          = BE_TO_CPU(*(xsrc++));
            nconv      += count_utf16(cp);
        }

        return nconv;
    }

    ssize_t multibyte_to_widechar(size_t cp, LPCCH src, size_t *nsrc, LPWSTR dst, size_t *ndst)
    {
        ssize_t nconv;

        switch (cp)
        {
            case 1200:  // UTF-16LE
                nconv = ((dst == NULL) || (ndst == NULL) || (ssize_t(*ndst) <= 0)) ?
                        est_multibyte_to_widechar_utf16le(src, *nsrc) :
                        multibyte_to_widechar_utf16le(src, nsrc, dst, ndst);
                break;
            case 1201:  // UTF-16BE
                nconv = ((dst == NULL) || (ndst == NULL) || (ssize_t(*ndst) <= 0)) ?
                        est_multibyte_to_widechar_utf16be(src, *nsrc) :
                        multibyte_to_widechar_utf16be(src, nsrc, dst, ndst);
                break;
            case 12000: // UTF-32LE
                nconv = ((dst == NULL) || (ndst == NULL) || (ssize_t(*ndst) <= 0)) ?
                        est_multibyte_to_widechar_utf32le(src, *nsrc) :
                        multibyte_to_widechar_utf32le(src, nsrc, dst, ndst);
                break;
            case 12001: // UTF-32BE
                nconv = ((dst == NULL) || (ndst == NULL) || (ssize_t(*ndst) <= 0)) ?
                        est_multibyte_to_widechar_utf32be(src, *nsrc) :
                        multibyte_to_widechar_utf32be(src, nsrc, dst, ndst);
                break;
            default:
                // We need just to estimate the size?
                if ((dst == NULL) || (ndst == NULL) || (ssize_t(*ndst) <= 0))
                    return ::MultiByteToWideChar(cp, 0, src, *nsrc, 0, 0);

                // Do the conversion
                nconv = ::MultiByteToWideChar(cp, 0, src, *nsrc, dst, *ndst);
                if (nconv == 0)
                {
                    switch (GetLastError())
                    {
                        case ERROR_SUCCESS:
                            return 0;
                        case ERROR_INSUFFICIENT_BUFFER:
                            return -STATUS_NO_MEM;
                        case ERROR_INVALID_FLAGS:
                        case ERROR_INVALID_PARAMETER:
                            return -STATUS_BAD_STATE;
                        case ERROR_NO_UNICODE_TRANSLATION:
                            return -STATUS_BAD_LOCALE;
                        default:
                            return -STATUS_UNKNOWN_ERR;
                    }
                }

                // There are converted characters, analyze output
                // If function meets invalid sequence, it replaces the code point with such magic value
                // We should know if function has failed
                if (dst[nconv-1] == 0xfffd)
                    --nconv;

                if (nconv > 0)
                {
                    // Estimate number of bytes decoded (yep, this is dumb but no way...)
                    ssize_t nbytes  = ::WideCharToMultiByte(cp, 0, dst, nconv, NULL, 0, 0, 0);
                    if (nbytes <= 0)
                        return -STATUS_IO_ERROR;

                    *nsrc  -= nbytes;
                    *ndst  -= nconv;
                }

                break;
        }

        return nconv;
    }

    static ssize_t widechar_to_multibyte_utf16le(const lsp_utf16_t *src, size_t *nsrc, char *dst, size_t *ndst)
    {
        lsp_wchar_t cp;
        ssize_t nconv = 0;
        size_t nin  = *nsrc;
        size_t nout = *ndst;
        lsp_utf16_t *xdst = reinterpret_cast<lsp_utf16_t *>(dst);

        while (nin > 0)
        {
            size_t xin  = nin;
            cp          = read_utf16_streaming(&src, &xin, false);
            if (cp == LSP_UTF32_EOF) // No data ?
                break;

            // Check that we have enough space
            size_t len = sizeof_utf16(cp);
            if (nout < len)
                break;

            // Write code point
            write_utf16le_codepoint(&xdst, cp);
            nin         = xin;
            nout       -= len;
            nconv      += len;
        }

        *nsrc   = nin;
        *ndst   = nout;
        return nconv;
    }

    static ssize_t widechar_to_multibyte_utf16be(const lsp_utf16_t *src, size_t *nsrc, char *dst, size_t *ndst)
    {
        lsp_wchar_t cp;
        ssize_t nconv = 0;
        size_t nin  = *nsrc;
        size_t nout = *ndst;
        lsp_utf16_t *xdst = reinterpret_cast<lsp_utf16_t *>(dst);

        while (nin > 0)
        {
            size_t xin  = nin;
            cp          = read_utf16_streaming(&src, &xin, false);
            if (cp == LSP_UTF32_EOF) // No data ?
                break;

            // Check that we have enough space
            size_t len = sizeof_utf16(cp);
            if (nout < len)
                break;

            // Write code point
            write_utf16be_codepoint(&xdst, cp);
            nin         = xin;
            nout       -= len;
            nconv      += len;
        }

        *nsrc   = nin;
        *ndst   = nout;
        return nconv;
    }

    static ssize_t est_widechar_to_multibyte_utf16(const lsp_utf16_t *src, size_t nsrc)
    {
        lsp_wchar_t cp;
        ssize_t nconv = 0;

        while (nsrc > 0)
        {
            cp      = read_utf16_streaming(&src, &nsrc, false);
            if (cp == LSP_UTF32_EOF) // No data ?
                break;

            // Check that we have enough space
            nconv  += sizeof_utf16(cp);
        }

        return nconv;
    }

    static ssize_t widechar_to_multibyte_utf32le(const lsp_utf16_t *src, size_t *nsrc, char *dst, size_t *ndst)
    {
        lsp_wchar_t cp;
        ssize_t nconv = 0;
        size_t nin  = *nsrc;
        size_t nout = *ndst;
        lsp_utf32_t *xdst = reinterpret_cast<lsp_utf32_t *>(dst);

        while (nin > 0)
        {
            size_t xin  = nin;
            cp          = read_utf16_streaming(&src, &xin, false);
            if (cp == LSP_UTF32_EOF) // No data ?
                break;

            // Check that we have enough space
            if (nout < sizeof(lsp_utf32_t))
                break;

            // Write code point
            *(xdst++)   = CPU_TO_LE(cp);
            nin         = xin;
            nout       -= sizeof(lsp_utf32_t);
            nconv      += sizeof(lsp_utf32_t);
        }

        *nsrc   = nin;
        *ndst   = nout;
        return nconv;
    }

    static ssize_t widechar_to_multibyte_utf32be(const lsp_utf16_t *src, size_t *nsrc, char *dst, size_t *ndst)
    {
        lsp_wchar_t cp;
        ssize_t nconv = 0;
        size_t nin  = *nsrc;
        size_t nout = *ndst;
        lsp_utf32_t *xdst = reinterpret_cast<lsp_utf32_t *>(dst);

        while (nin > 0)
        {
            size_t xin  = nin;
            cp          = read_utf16_streaming(&src, &xin, false);
            if (cp == LSP_UTF32_EOF) // No data ?
                break;

            // Check that we have enough space
            if (nout < sizeof(lsp_utf32_t))
                break;

            // Write code point
            *(xdst++)   = CPU_TO_BE(cp);
            nin         = xin;
            nout       -= sizeof(lsp_utf32_t);
            nconv      += sizeof(lsp_utf32_t);
        }

        *nsrc   = nin;
        *ndst   = nout;
        return nconv;
    }

    static ssize_t est_widechar_to_multibyte_utf32(const lsp_utf16_t *src, size_t nsrc)
    {
        lsp_wchar_t cp;
        ssize_t nconv = 0;

        while (nsrc > 0)
        {
            cp          = read_utf16_streaming(&src, &nsrc, false);
            if (cp == LSP_UTF32_EOF) // No data ?
                break;
            nconv      += sizeof(lsp_utf32_t);
        }

        return nconv;
    }

    static size_t widechar_to_multibyte_split(const lsp_utf16_t *src, size_t limit)
    {
        // Estimate the middle of an array
        size_t half     = limit >> 1;
        if (half <= 0)
            return half;

        // Now scan valid code points until we reach the end of array
        lsp_wchar_t cp;
        limit           = half;
        while (true)
        {
            cp          = read_utf16_streaming(&src, &limit, false);
            if (cp == LSP_UTF32_EOF) // No data ?
                break;
        }

        // Return the result as middle of array without remained points in limit
        return half - limit;
    }

    ssize_t widechar_to_multibyte(size_t cp, LPCWCH src, size_t *nsrc, LPSTR dst, size_t *ndst)
    {
        ssize_t nconv;

        switch (cp)
        {
            case 1200:  // UTF-16LE
                nconv = ((dst == NULL) || (ndst == NULL) || (ssize_t(*ndst) <= 0)) ?
                        est_widechar_to_multibyte_utf16(src, *nsrc) :
                        widechar_to_multibyte_utf16le(src, nsrc, dst, ndst);
                break;
            case 1201:  // UTF-16BE
                nconv = ((dst == NULL) || (ndst == NULL) || (ssize_t(*ndst) <= 0)) ?
                        est_widechar_to_multibyte_utf16(src, *nsrc) :
                        widechar_to_multibyte_utf16be(src, nsrc, dst, ndst);
                break;
            case 12000: // UTF-32LE
                nconv = ((dst == NULL) || (ndst == NULL) || (ssize_t(*ndst) <= 0)) ?
                        est_widechar_to_multibyte_utf32(src, *nsrc) :
                        widechar_to_multibyte_utf32le(src, nsrc, dst, ndst);
                break;
            case 12001: // UTF-32BE
                nconv = ((dst == NULL) || (ndst == NULL) || (ssize_t(*ndst) <= 0)) ?
                        est_widechar_to_multibyte_utf32(src, *nsrc) :
                        widechar_to_multibyte_utf32be(src, nsrc, dst, ndst);
                break;
            default:
            {
                // Just estimate number of characters?
                if ((dst == NULL) || (ndst == NULL) || (ssize_t(*ndst) <= 0))
                {
                    nconv           = ::WideCharToMultiByte(cp, 0, src, *nsrc, NULL, 0, 0, FALSE);
                    if (nconv == 0)
                    {
                        switch (::GetLastError())
                        {
                            case ERROR_SUCCESS:
                                return 0;
                            case ERROR_INSUFFICIENT_BUFFER:
                                return -STATUS_NO_MEM;
                            case ERROR_INVALID_FLAGS:
                            case ERROR_INVALID_PARAMETER:
                                return -STATUS_BAD_STATE;
                            case ERROR_NO_UNICODE_TRANSLATION:
                                return -STATUS_BAD_LOCALE;
                            default:
                                return -STATUS_UNKNOWN_ERR;
                        }
                    }
                    return nconv;
                }

                // Perform first try
                size_t xnsrc    = *nsrc;
                nconv = ::WideCharToMultiByte(cp, 0, src, xnsrc, dst, *ndst, 0, FALSE);

                // Do while conversion is unsuccessful
                while (nconv <= 0)
                {
                    // There was a fail, analyze it
                    switch (::GetLastError())
                    {
                        case ERROR_SUCCESS:
                            return 0;
                        case ERROR_INSUFFICIENT_BUFFER:
                            break;  // Will retry with twice lesser input buffer
                        case ERROR_INVALID_FLAGS:
                        case ERROR_INVALID_PARAMETER:
                            return -STATUS_BAD_STATE;
                        case ERROR_NO_UNICODE_TRANSLATION:
                            return -STATUS_BAD_LOCALE;
                        default:
                            return -STATUS_UNKNOWN_ERR;
                    }

                    // Try to twice reduce the buffer size, validate data for surrogates
                    xnsrc = widechar_to_multibyte_split(src, xnsrc);
                    if (xnsrc <= 0)
                        break;

                    // Perform next conversion try with lesser buffer
                    nconv = ::WideCharToMultiByte(cp, 0, src, xnsrc, dst, *ndst, 0, FALSE);
                }

                *ndst      -= nconv;
                *nsrc      -= xnsrc;
            }
            break;
        }

        return nconv;
    }
#endif /* PLATFORM_WINDOWS */
}
