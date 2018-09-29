/*
 * Writer.cpp
 *
 *  Created on: 14 июн. 2018 г.
 *      Author: sadko
 */

#include <core/io/Writer.h>

namespace lsp
{
    namespace io
    {
        Writer::Writer()
        {
        }

        Writer::~Writer()
        {
        }

        status_t Writer::write(lsp_wchar_t c)
        {
            return STATUS_OK;
        }

        status_t Writer::write(const lsp_wchar_t *c, size_t count)
        {
            return STATUS_OK;
        }

        status_t Writer::write_ascii(const char *s)
        {
            return STATUS_OK;
        }

        status_t Writer::write(const LSPString *s)
        {
            return write(s->characters(), s->length());
        }

        status_t Writer::write(const LSPString *s, ssize_t first)
        {
            ssize_t len = s->length();
            if (first < 0)
            {
                if ((first += len) < 0)
                    return STATUS_OVERFLOW;
            }
            else if (first >= len)
                return (first > len) ? STATUS_OVERFLOW : STATUS_OK;
    
            const lsp_wchar_t *v = s->characters();
            return write(&v[first], len - first);
        }
    
        status_t Writer::write(const LSPString *s, ssize_t first, ssize_t last)
        {
            ssize_t len = s->length();
            if (first < 0)
            {
                if ((first += len) < 0)
                    return STATUS_OVERFLOW;
            }
            else if (first >= len)
                return (first > len) ? STATUS_OVERFLOW : STATUS_OK;

            if (last < 0)
            {
                if ((last += len) < 0)
                    return STATUS_OVERFLOW;
            }
            else if (last >= len)
                return (last > len) ? STATUS_OVERFLOW : STATUS_OK;

            ssize_t count = last - first;
            if (count <= 0)
                return (count < 0) ? STATUS_OVERFLOW : STATUS_OK;

            const lsp_wchar_t *v = s->characters();
            return write(&v[first], count);
        }

        status_t Writer::writeln(lsp_wchar_t c)
        {
            lsp_wchar_t w[2];
            w[0] = c;
            w[1] = '\n';
            return write(w, 2);
        }

        status_t Writer::writeln(const lsp_wchar_t *c, size_t count)
        {
            LSP_STATUS_ASSERT(write(c, count));
            return write('\n');
        }

        status_t Writer::writeln_ascii(const char *s)
        {
            LSP_STATUS_ASSERT(write_ascii(s));
            return write('\n');
        }

        status_t Writer::writeln(const LSPString *s)
        {
            LSP_STATUS_ASSERT(write(s));
            return write('\n');
        }

        status_t Writer::writeln(const LSPString *s, ssize_t first)
        {
            LSP_STATUS_ASSERT(write(s, first));
            return write('\n');
        }

        status_t Writer::writeln(const LSPString *s, ssize_t first, ssize_t last)
        {
            LSP_STATUS_ASSERT(write(s, first, last));
            return write('\n');
        }

        status_t Writer::Writer::flush()
        {
            return STATUS_OK;
        }

        status_t Writer::close()
        {
            return STATUS_OK;
        }
    }

} /* namespace lsp */
