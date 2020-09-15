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

