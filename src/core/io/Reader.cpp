/*
 * Reader.cpp
 *
 *  Created on: 14 июн. 2018 г.
 *      Author: sadko
 */

#include <core/io/Reader.h>

namespace lsp
{
    namespace io
    {
        static lsp_wchar_t skip_buf[0x1000];

        Reader::Reader()
        {
        }

        Reader::~Reader()
        {
        }

        ssize_t Reader::read(lsp_wchar_t *dst, size_t count)
        {
            return -1;
        }

        int Reader::read()
        {
            return -1;
        }

        status_t Reader::read_line(LSPString *s, bool force)
        {
            return STATUS_EOF;
        }

        ssize_t Reader::skip(size_t count)
        {
            ssize_t skipped = 0;

            while (count > 0)
            {
                size_t to_read  = (count > ((sizeof(skip_buf))/sizeof(lsp_wchar_t))) ?
                        ((sizeof(skip_buf))/sizeof(lsp_wchar_t)) : count;
                ssize_t nread   = read(skip_buf, to_read);
                if (nread <= 0)
                    break;

                count      -= nread;
                skipped    += nread;
            }
            return skipped;
        }

        status_t Reader::error()
        {
            return STATUS_EOF;
        }
    
        status_t Reader::close()
        {
            return STATUS_OK;
        }

    }
} /* namespace lsp */
