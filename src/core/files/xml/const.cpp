/*
 * const.cpp
 *
 *  Created on: 26 окт. 2019 г.
 *      Author: sadko
 */

#include <core/files/xml/const.h>

namespace lsp
{
    namespace xml
    {
        bool is_valid_char(lsp_swchar_t c, xml_version_t version)
        {
            if (version == XML_VERSION_1_0)
            {
                if ((c >= 0x20) && (c <= 0xd7ff))
                    return true;
                if ((c == 0x9) || (c == 0xa) || (c == 0xd))
                    return true;
            }
            else
            {
                if ((c >= 1) && (c <= 0xd7ff))
                    return true;
            }
            if ((c >= 0xe000) && (c <= 0xfffd))
                return true;
            return (c >= 0x10000) && (c <= 0x10ffff);
        }

        bool is_name_first(lsp_swchar_t c)
        {
            if ((c >= 'a') && (c <= 'z'))
                return true;
            if ((c >= 'A') && (c <= 'Z'))
                return true;
            if ((c == ':') || (c == '_'))
                return true;
            if ((c >= 0xc0) && (c <= 0xd6))
                return true;
            if ((c >= 0xd8) && (c <= 0xf6))
                return true;
            if ((c >= 0xf8) && (c <= 0x2ff))
                return true;
            if ((c >= 0x370) && (c <= 0x37d))
                return true;
            if ((c >= 0x37f) && (c <= 0x1fff))
                return true;
            if ((c >= 0x200c) && (c <= 0x200d))
                return true;
            if ((c >= 0x2070) && (c <= 0x218f))
                return true;
            if ((c >= 0x2c00) && (c <= 0x2fef))
                return true;
            if ((c >= 0x3001) && (c <= 0xd7ff))
                return true;
            if ((c >= 0xf900) && (c <= 0xfdcf))
                return true;
            if ((c >= 0xfdf0) && (c <= 0xfffd))
                return true;
            if ((c >= 0x10000) && (c <= 0xeffff))
                return true;

            return false;
        }

        bool is_name_next(lsp_swchar_t c)
        {
            if ((c >= '0') && (c <= '9'))
                return true;
            if ((c == '-') || (c == '.') || (c == 0xb7))
                return true;
            if (is_name_first(c))
                return true;
            if ((c >= 0x300) && (c <= 0x36f))
                return true;
            if ((c >= 0x203f) && (c <= 0x2040))
                return true;
            return false;
        }

        bool is_whitespace(lsp_swchar_t c)
        {
            switch (c)
            {
                case 0x20:
                case 0x09:
                case 0x0d:
                case 0x0a:
                    return true;
            }
            return false;
        }

        bool is_pubid_char(lsp_swchar_t c)
        {
            if ((c >= 'a') && (c <= 'z'))
                return true;
            if ((c >= 'A') && (c <= 'Z'))
                return true;
            if ((c >= '0') && (c <= '9'))
                return true;

            switch (c)
            {
                case 0x20: case 0x0d: case 0x0a: case '\'':
                case '-': case '(': case ')': case '+':
                case ',': case '.': case '/': case ':':
                case '=': case '?': case ';': case '!':
                case '*': case '#': case '@': case '$':
                case '_': case '%':
                    return true;
            }
            return false;
        }

        bool is_restricted_char(lsp_swchar_t c, xml_version_t version)
        {
            if (version <= XML_VERSION_1_0)
                return false;

            if ((c >= 0x01) && (c <= 0x08))
                return true;
            if ((c >= 0x0b) && (c <= 0x0c))
                return true;
            if ((c >= 0x0e) && (c <= 0x1f))
                return true;
            if ((c >= 0x7f) && (c <= 0x84))
                return true;
            if ((c >= 0x86) && (c <= 0x9f))
                return true;

            return false;
        }

        bool is_encoding_first(lsp_swchar_t c)
        {
            if ((c >= 'a') && (c <= 'z'))
                return true;
            if ((c >= 'A') && (c <= 'Z'))
                return true;
            return false;
        }

        bool is_encoding_next(lsp_swchar_t c)
        {
            if (is_encoding_first(c))
                return true;
            if ((c >= '0') && (c <= '9'))
                return true;

            return ((c == '_') || (c == '.') || (c == '-'));
        }
    }
}


