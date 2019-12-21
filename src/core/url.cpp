/*
 * url.cpp
 *
 *  Created on: 21 дек. 2019 г.
 *      Author: sadko
 */

#include <core/files/url.h>

namespace lsp
{
    static inline int decode_hex(lsp_wchar_t c)
    {
        if ((c >= '0') && (c <= '9'))
            return c - '0';
        else if ((c >= 'a') && (c <= 'f'))
            return c - 'a' + 10;
        else if ((c >= 'A') && (c <= 'F'))
            return c - 'A' + 10;
        return -1;
    }

    status_t url_decode(LSPString *dst, const LSPString *src)
    {
        return url_decode(dst, src, 0, src->length());
    }

    status_t url_decode(LSPString *dst, const LSPString *src, size_t first)
    {
        return url_decode(dst, src, first, src->length());
    }

    status_t url_decode(LSPString *dst, const LSPString *src, size_t first, size_t last)
    {
        char *buf = NULL;
        int code;
        size_t bcap=0, blen=0;
        LSPString tmp;

        while (first < last)
        {
            lsp_wchar_t c = src->char_at(first);
            if (c == '%')
            {
                // Read hex codes
                do
                {
                    // Check availability
                    if ((last - first) < 3)
                    {
                        if (buf != NULL)
                            ::free(buf);
                        return STATUS_CORRUPTED;
                    }

                    // Fetch byte code
                    code    = decode_hex(src->char_at(++first)) << 4;
                    code   |= decode_hex(src->char_at(++first));
                    if (code < 0)
                    {
                        if (buf != NULL)
                            ::free(buf);
                        return STATUS_CORRUPTED;
                    }

                    // Append code to buffer
                    if (blen >= bcap)
                    {
                        bcap           += ((blen + 0x10) & ~0xf);
                        char *nbuf      = reinterpret_cast<char *>(::realloc(buf, bcap * sizeof(char)));
                        if (nbuf == NULL)
                        {
                            if (buf != NULL)
                                ::free(buf);
                            return STATUS_NO_MEM;
                        }
                        buf             = nbuf;
                    }
                    buf[blen++]     = code;

                    // Read next character
                    c = src->char_at(++first);
                } while (c == '%');

                // Perform encoding
                if (!tmp.set_utf8(buf, blen))
                {
                    if (buf != NULL)
                        ::free(buf);
                    return STATUS_CORRUPTED;
                }

                // Append data
                if (!dst->append(&tmp))
                {
                    if (buf != NULL)
                        ::free(buf);
                    return STATUS_NO_MEM;
                }

                // Reset buffer length
                blen    = 0;
            }
            else if (!dst->append(c))
            {
                if (buf != NULL)
                    ::free(buf);
                return STATUS_NO_MEM;
            }
            else
                ++first;
        }

        // Destroy buffer
        if (buf != NULL)
            ::free(buf);

        return STATUS_OK;
    }
}

