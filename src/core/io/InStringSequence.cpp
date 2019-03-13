/*
 * StringInputStream.cpp
 *
 *  Created on: 14 июн. 2018 г.
 *      Author: sadko
 */

#include <core/io/InStringSequence.h>

namespace lsp
{
    namespace io
    {
        InStringSequence::InStringSequence(const LSPString *s, bool destroy):
                pString(s)
        {
            bDelete    = destroy;
            nOffset     = 0;
        }

        InStringSequence::~InStringSequence()
        {
            do_close();
        }

        status_t InStringSequence::wrap(const LSPString *in, bool del)
        {
            if (pString != NULL)
                return set_error(STATUS_BAD_STATE);
            pString     = in;
            bDelete     = del;
            return set_error(STATUS_OK);
        }

        void InStringSequence::do_close()
        {
            if (pString == NULL)
                return;
            if (bDelete)
                delete const_cast<LSPString *>(pString);
            pString     = NULL;
            bDelete     = false;
        }

        ssize_t InStringSequence::read(lsp_wchar_t *dst, size_t count)
        {
            if (pString == NULL)
                return set_error(STATUS_CLOSED);

            size_t avail = pString->length() - nOffset;
            if (count > avail)
                count = avail;
            if (count <= 0)
                return 0;

            // Perform a copy
            const lsp_wchar_t *v = pString->characters();
            v       += nOffset;
            nOffset += count;
            ::memcpy(dst, v, avail * sizeof(lsp_wchar_t));

            set_error(STATUS_OK);
            return count;
        }
    
        int InStringSequence::read()
        {
            if (pString == NULL)
                return set_error(STATUS_CLOSED);

            if (nOffset < pString->length())
            {
                set_error(STATUS_OK);
                return pString->char_at(nOffset++);
            }
            return set_error(STATUS_EOF);
        }

        status_t InStringSequence::read_line(LSPString *s, bool force)
        {
            if (pString == NULL)
                return set_error(STATUS_CLOSED);

            ssize_t idx = pString->index_of(nOffset, '\n');
            if ((idx < 0) && (!force))
                return set_error(STATUS_EOF);

            if (!s->set(pString, nOffset, idx))
                return set_error(STATUS_NO_MEM);

            ssize_t len = s->length();
            if ((len--) > 0)
            {
                if (s->char_at(len) == '\r')
                    s->set_length(len);
            }

            return set_error(STATUS_OK);
        }

        ssize_t InStringSequence::skip(size_t count)
        {
            if (pString == NULL)
                return set_error(STATUS_CLOSED);

            size_t avail = pString->length() - nOffset;
            if (count > avail)
                count = avail;
            nOffset    += count;
            return count;
        }

        status_t InStringSequence::close()
        {
            do_close();
            return set_error(STATUS_OK);
        }
    }
} /* namespace lsp */
