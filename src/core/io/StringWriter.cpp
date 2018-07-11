/*
 * StringOutputStream.cpp
 *
 *  Created on: 14 июн. 2018 г.
 *      Author: sadko
 */

#include <core/io/StringWriter.h>

namespace lsp
{
    namespace io
    {
        StringWriter::StringWriter(LSPString *out)
        {
            pOut = out;
        }

        StringWriter::~StringWriter()
        {
            pOut = NULL;
        }

        status_t StringWriter::write(lsp_wchar_t c)
        {
            if (pOut == NULL)
                return STATUS_CLOSED;

            return (pOut->append(c)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t StringWriter::write(const lsp_wchar_t *c, size_t count)
        {
            if (pOut == NULL)
                return STATUS_CLOSED;

            return (pOut->append(c, count)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t StringWriter::write_ascii(const char *s)
        {
            if (pOut == NULL)
                return STATUS_CLOSED;
            return (pOut->append_ascii(s)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t StringWriter::write(const LSPString *s)
        {
            if (pOut == NULL)
                return STATUS_CLOSED;

            return (pOut->append(s)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t StringWriter::write(const LSPString *s, ssize_t first)
        {
            if (pOut == NULL)
                return STATUS_CLOSED;

            return (pOut->append(s, first)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t StringWriter::write(const LSPString *s, ssize_t first, ssize_t last)
        {
            if (pOut == NULL)
                return STATUS_CLOSED;

            return (pOut->append(s, first, last)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t StringWriter::flush()
        {
            return STATUS_OK;
        }
    
        status_t StringWriter::close()
        {
            pOut = NULL;
            return STATUS_OK;
        }
    }
} /* namespace lsp */
