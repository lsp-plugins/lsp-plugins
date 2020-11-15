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

#include <core/io/IInSequence.h>

namespace lsp
{
    namespace io
    {
        static lsp_wchar_t skip_buf[0x1000];

        IInSequence::IInSequence()
        {
            nErrorCode      = STATUS_OK;
        }

        IInSequence::~IInSequence()
        {
        }

        ssize_t IInSequence::read(lsp_wchar_t *dst, size_t count)
        {
            return -set_error(STATUS_EOF);
        }

        lsp_swchar_t IInSequence::read()
        {
            return -set_error(STATUS_EOF);
        }

        status_t IInSequence::read_line(LSPString *s, bool force)
        {
            return set_error(STATUS_EOF);
        }

        ssize_t IInSequence::skip(size_t count)
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

        status_t IInSequence::close()
        {
            return set_error(STATUS_OK);
        }

    }
} /* namespace lsp */
