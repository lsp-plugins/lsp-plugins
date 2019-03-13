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
        StringWriter::StringWriter(LSPString *out, bool del)
        {
            pOut = out;
            bDelete = del;
        }

        StringWriter::~StringWriter()
        {
            if (pOut == NULL)
                return;

            if (bDelete)
                delete pOut;

            pOut = NULL;
            bDelete = false;
        }

        status_t StringWriter::close()
        {
            if (pOut != NULL)
            {
                if (bDelete)
                    delete pOut;
                pOut = NULL;
                bDelete = false;
            }
            return set_error(STATUS_OK);
        }

        status_t StringWriter::wrap(LSPString *out, bool del)
        {
            if (pOut != NULL)
                return set_error(STATUS_BAD_STATE);
            else if (out == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

            pOut        = out;
            bDelete     = del;
            return set_error(STATUS_OK);
        }

        status_t StringWriter::write(lsp_wchar_t c)
        {
            if (pOut == NULL)
                return set_error(STATUS_CLOSED);

            return set_error((pOut->append(c)) ? STATUS_OK : STATUS_NO_MEM);
        }

        status_t StringWriter::write(const lsp_wchar_t *c, size_t count)
        {
            if (pOut == NULL)
                return set_error(STATUS_CLOSED);

            return set_error((pOut->append(c, count)) ? STATUS_OK : STATUS_NO_MEM);
        }

        status_t StringWriter::write_ascii(const char *s)
        {
            if (pOut == NULL)
                return set_error(STATUS_CLOSED);

            return set_error((pOut->append_ascii(s)) ? STATUS_OK : STATUS_NO_MEM);
        }

        status_t StringWriter::write(const LSPString *s)
        {
            if (pOut == NULL)
                return set_error(STATUS_CLOSED);

            return set_error((pOut->append(s)) ? STATUS_OK : STATUS_NO_MEM);
        }

        status_t StringWriter::write(const LSPString *s, ssize_t first)
        {
            if (pOut == NULL)
                return set_error(STATUS_CLOSED);

            return set_error((pOut->append(s, first)) ? STATUS_OK : STATUS_NO_MEM);
        }

        status_t StringWriter::write(const LSPString *s, ssize_t first, ssize_t last)
        {
            if (pOut == NULL)
                return set_error(STATUS_CLOSED);

            return set_error((pOut->append(s, first, last)) ? STATUS_OK : STATUS_NO_MEM);
        }

        status_t StringWriter::flush()
        {
            return set_error(STATUS_OK);
        }
    }
} /* namespace lsp */
