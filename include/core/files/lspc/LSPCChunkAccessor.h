/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 14 янв. 2018 г.
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

#ifndef CORE_FILES_LSPC_LSPCCHUNKACCESSOR_H_
#define CORE_FILES_LSPC_LSPCCHUNKACCESSOR_H_

#include <core/types.h>
#include <core/status.h>

namespace lsp
{
    typedef struct LSPCResource
    {
        lsp_fhandle_t   fd;             // File handle
        size_t          refs;           // Number of references
        size_t          bufsize;        // Default buffer size
        uint32_t        chunk_id;       // Chunk identifier allocator
        wsize_t         length;         // Length of the output file

        status_t        acquire();
        status_t        release();
        status_t        allocate(uint32_t *id);
        status_t        write(const void *buf, size_t count);
        ssize_t         read(wsize_t pos, void *buf, size_t count);
    } LSPCResource;

    class LSPCChunkAccessor
    {
        private:
            LSPCChunkAccessor & operator = (const LSPCChunkAccessor &);

        protected:
            friend class LSPCFile;

        protected:
            enum const_t
            {
                MIN_BUF_SIZE        = 0x1000
            };

        protected:
            uint8_t        *pBuffer;        // Pointer to the buffer
            size_t          nBufSize;       // Size of the buffer
            size_t          nBufPos;        // Current buffer position
            LSPCResource   *pFile;          // Pointer to the shared file resource
            uint32_t        nMagic;         // Magic number as size of chunk
            status_t        nErrorCode;     // Last error code
            uint32_t        nUID;           // Unique chunk identifier

        protected:
            inline status_t set_error(status_t err)     { return nErrorCode = err; }
            status_t        do_close();

        protected:
            explicit LSPCChunkAccessor(LSPCResource *fd, uint32_t magic);

        public:
            virtual ~LSPCChunkAccessor();

        public:
            inline size_t       buffer_size() const     { return nBufSize;      }
            inline status_t     last_error() const      { return nErrorCode;    }
            inline uint32_t     unique_id() const       { return nUID;          }
            inline uint32_t     magic() const           { return nMagic;        }

        public:
            /** Close chunk accessor
             *
             * @return status of operation
             */
            virtual status_t    close();
    };

} /* namespace lsp */

#endif /* CORE_FILES_LSPC_LSPCCHUNKACCESSOR_H_ */
