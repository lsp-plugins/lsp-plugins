/*
 * StringInputStream.cpp
 *
 *  Created on: 14 июн. 2018 г.
 *      Author: sadko
 */

#include <core/io/StringReader.h>

namespace lsp
{
    namespace io
    {
        StringReader::StringReader(const LSPString *s, bool destroy):
                pString(s)
        {
            bDestroy    = destroy;
            nOffset     = 0;
            nError      = STATUS_OK;
        }

        StringReader::~StringReader()
        {
            do_close();
        }

        void StringReader::do_close()
        {
            if (pString == NULL)
                return;
            if (bDestroy)
                delete const_cast<LSPString *>(pString);
            pString     = NULL;
            bDestroy    = false;
        }

        ssize_t StringReader::read(lsp_wchar_t *dst, size_t count)
        {
            if (pString == NULL)
            {
                nError = STATUS_CLOSED;
                return -1;
            }

            size_t avail = pString->length() - nOffset;
            if (count > avail)
                count = avail;
            if (count <= 0)
                return 0;

            // Perform a copy
            const lsp_wchar_t *v = pString->characters();
            v       += nOffset;
            nOffset += count;
            memcpy(dst, v, avail * sizeof(lsp_wchar_t));

            return count;
        }
    
        int StringReader::read()
        {
            if (pString == NULL)
            {
                nError = STATUS_CLOSED;
                return -1;
            }

            return (nOffset < pString->length()) ?
                pString->char_at(nOffset++) : -1;
        }

        status_t StringReader::read_line(LSPString *s, bool force)
        {
            if (pString == NULL)
            {
                nError = STATUS_CLOSED;
                return -1;
            }

            ssize_t idx = pString->index_of(nOffset, '\n');
            if ((idx < 0) && (!force))
                return STATUS_EOF;

            LSPString *res = pString->substring(nOffset, idx);
            if (res == NULL)
                return nError = STATUS_NO_MEM;

            ssize_t len = res->length();
            if (len > 0)
            {
                if (res->char_at(len-1) == '\r')
                    res->truncate(--len);
            }

            s->take(res);
            return STATUS_OK;
        }

        ssize_t StringReader::skip(size_t count)
        {
            if (pString == NULL)
            {
                nError = STATUS_CLOSED;
                return -1;
            }

            size_t avail = pString->length() - nOffset;
            if (count > avail)
                count = avail;
            nOffset    += count;
            return count;
        }

        status_t StringReader::error()
        {
            return nError;
        }

        status_t StringReader::close()
        {
            do_close();
            return nError;
        }
    }
} /* namespace lsp */
