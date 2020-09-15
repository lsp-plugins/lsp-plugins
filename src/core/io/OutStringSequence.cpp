/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 14 июн. 2018 г.
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

#include <core/io/OutStringSequence.h>

namespace lsp
{
    namespace io
    {
        OutStringSequence::OutStringSequence()
        {
            pOut = NULL;
            bDelete = false;
        }

        OutStringSequence::OutStringSequence(LSPString *out, bool del)
        {
            pOut = out;
            bDelete = del;
        }

        OutStringSequence::~OutStringSequence()
        {
            if (pOut == NULL)
                return;

            if (bDelete)
                delete pOut;

            pOut = NULL;
            bDelete = false;
        }

        status_t OutStringSequence::close()
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

        status_t OutStringSequence::wrap(LSPString *out, bool del)
        {
            if (pOut != NULL)
                return set_error(STATUS_BAD_STATE);
            else if (out == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

            pOut        = out;
            bDelete     = del;
            return set_error(STATUS_OK);
        }

        status_t OutStringSequence::write(lsp_wchar_t c)
        {
            if (pOut == NULL)
                return set_error(STATUS_CLOSED);

            return set_error((pOut->append(c)) ? STATUS_OK : STATUS_NO_MEM);
        }

        status_t OutStringSequence::write(const lsp_wchar_t *c, size_t count)
        {
            if (pOut == NULL)
                return set_error(STATUS_CLOSED);

            return set_error((pOut->append(c, count)) ? STATUS_OK : STATUS_NO_MEM);
        }

        status_t OutStringSequence::write_ascii(const char *s)
        {
            if (pOut == NULL)
                return set_error(STATUS_CLOSED);

            return set_error((pOut->append_ascii(s)) ? STATUS_OK : STATUS_NO_MEM);
        }

        status_t OutStringSequence::writeln_ascii(const char *s)
        {
            if (pOut == NULL)
                return set_error(STATUS_CLOSED);
            bool res = pOut->append_ascii(s);
            if (res)
                res = pOut->append('\n');
            return set_error((res) ? STATUS_OK : STATUS_NO_MEM);
        }

        status_t OutStringSequence::write_ascii(const char *s, size_t count)
        {
            if (pOut == NULL)
                return set_error(STATUS_CLOSED);

            return set_error((pOut->append_ascii(s, count)) ? STATUS_OK : STATUS_NO_MEM);
        }

        status_t OutStringSequence::write(const LSPString *s)
        {
            if (pOut == NULL)
                return set_error(STATUS_CLOSED);

            return set_error((pOut->append(s)) ? STATUS_OK : STATUS_NO_MEM);
        }

        status_t OutStringSequence::write(const LSPString *s, ssize_t first)
        {
            if (pOut == NULL)
                return set_error(STATUS_CLOSED);

            return set_error((pOut->append(s, first)) ? STATUS_OK : STATUS_NO_MEM);
        }

        status_t OutStringSequence::write(const LSPString *s, ssize_t first, ssize_t last)
        {
            if (pOut == NULL)
                return set_error(STATUS_CLOSED);

            return set_error((pOut->append(s, first, last)) ? STATUS_OK : STATUS_NO_MEM);
        }

        status_t OutStringSequence::flush()
        {
            return set_error(STATUS_OK);
        }
    }
} /* namespace lsp */
