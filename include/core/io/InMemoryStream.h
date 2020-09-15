/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 21 авг. 2019 г.
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

#ifndef CORE_IO_INMEMORYSTREAM_H_
#define CORE_IO_INMEMORYSTREAM_H_

#include <core/io/IInStream.h>

namespace lsp
{
    namespace io
    {
        
        class InMemoryStream: public IInStream
        {
            protected:
                uint8_t        *pData;
                size_t          nOffset;
                size_t          nSize;
                lsp_memdrop_t  enDrop;

            public:
                explicit InMemoryStream();

                /**
                 * Create memory input stream
                 * @param data associated buffer
                 * @param size size of buffer
                 */
                explicit InMemoryStream(const void *data, size_t size);

                /**
                 * Create memory input stream
                 * @param data associated buffer
                 * @param size size of buffer
                 * @param drop the method how to drop data on close()
                 */
                explicit InMemoryStream(void *data, size_t size, lsp_memdrop_t drop);

                virtual ~InMemoryStream();

            public:
                /**
                 * Wrap the memory buffer, drop previous buffer using specified mechanism
                 * @param data associated buffer
                 * @param size size of buffer
                 * @param drop the method how to drop data on close()
                 */
                void wrap(void *data, size_t size, lsp_memdrop_t drop);

                /**
                 * Wrap the memory buffer, drop previous buffer using specified mechanism
                 * @param data associated buffer
                 * @param size size of buffer
                 * @param drop the method how to drop data on close()
                 */
                void wrap(const void *data, size_t size);

                /**
                 * Get the memory contents
                 * @return memory contents
                 */
                inline const uint8_t *data() const { return pData; }

                /**
                 * Drop data using specified drop mechanism
                 * @return true if data has been dropped
                 */
                bool drop(lsp_memdrop_t drop);

                /**
                 * Drop data using default mechanism specified in constructor
                 * @return true if data has been dropped
                 */
                inline bool drop() { return drop(enDrop); };

            public:

                virtual wssize_t    avail();

                virtual wssize_t    position();

                virtual ssize_t     read(void *dst, size_t count);

                virtual wssize_t    seek(wsize_t position);

                virtual wssize_t    skip(wsize_t amount);

                virtual status_t    close();
        };
    
    } /* namespace io */
} /* namespace lsp */

#endif /* CORE_IO_INMEMORYSTREAM_H_ */
