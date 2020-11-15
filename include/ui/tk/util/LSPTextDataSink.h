/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 22 авг. 2019 г.
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

#ifndef UI_TK_UTIL_LSPDATASINK_H_
#define UI_TK_UTIL_LSPDATASINK_H_

#include <core/io/OutMemoryStream.h>

namespace lsp
{
    namespace tk
    {
        class LSPTextDataSink: public IDataSink
        {
            private:
                ssize_t                 nMimeType;
                io::OutMemoryStream     sOut;

            public:
                explicit LSPTextDataSink();
                virtual ~LSPTextDataSink();

            public:
                virtual ssize_t     open(const char * const *mime_types);

                virtual status_t    write(const void *buf, size_t count);

                virtual status_t    close(status_t code);

                /**
                 * Callback, is called on the close() method has been called
                 * @param code completion code
                 * @param data the actual data received
                 * @return status of operation
                 */
                virtual status_t    on_complete(status_t code, const LSPString *data);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_UTIL_LSPDATASINK_H_ */
