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

#ifndef CORE_IO_STRINWRITER_H_
#define CORE_IO_STRINWRITER_H_

#include <core/io/IOutSequence.h>

namespace lsp
{
    namespace io
    {
        class OutStringSequence: public IOutSequence
        {
            private:
                LSPString  *pOut;
                bool        bDelete;

            private:
                OutStringSequence & operator = (const OutStringSequence &);

            public:
                explicit OutStringSequence();
                explicit OutStringSequence(LSPString *out, bool del = false);
                virtual ~OutStringSequence();
    
            public:
                status_t            wrap(LSPString *out, bool del);

                virtual status_t    write(lsp_wchar_t c);

                virtual status_t    write(const lsp_wchar_t *c, size_t count);

                virtual status_t    write_ascii(const char *s);

                virtual status_t    write_ascii(const char *s, size_t count);

                virtual status_t    writeln_ascii(const char *s);

                virtual status_t    write(const LSPString *s);

                virtual status_t    write(const LSPString *s, ssize_t first);

                virtual status_t    write(const LSPString *s, ssize_t first, ssize_t last);

                virtual status_t    flush();

                virtual status_t    close();
        };
    }
} /* namespace lsp */

#endif /* CORE_IO_STRINWRITER_H_ */
