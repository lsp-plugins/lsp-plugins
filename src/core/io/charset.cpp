/*
 * charset.cpp
 *
 *  Created on: 18 июн. 2018 г.
 *      Author: sadko
 */

#include <core/io/charset.h>
#include <locale.h>
#include <stdlib.h>

namespace lsp
{
    iconv_t init_iconv_to_wchar_t(const char *charset)
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
            memcpy(psaved, current, len);
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
                memcpy(psaved, &current[1], len);
            }

            // Restore saved locale
            setlocale(LC_CTYPE, charset);

            // Update locale
            charset  = (current != NULL) ? psaved : "UTF-8";
        }

        // Open conversion
        return iconv_open(__IF_LEBE("UTF-16LE", "UTF-16BE"), charset);
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
            memcpy(psaved, current, len);
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
                memcpy(psaved, &current[1], len);
            }

            // Restore saved locale
            setlocale(LC_CTYPE, charset);

            // Update charset
            charset  = (current != NULL) ? psaved : "UTF-8";
        }

        // Open conversion
        return iconv_open(charset, __IF_LEBE("UTF-16LE", "UTF-16BE"));
    }
}
