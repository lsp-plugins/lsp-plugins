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

#ifndef CORE_FILES_LSPC_LSPCCHUNKWRITER_H_
#define CORE_FILES_LSPC_LSPCCHUNKWRITER_H_

#include <core/files/lspc/lspc.h>
#include <core/files/lspc/LSPCChunkAccessor.h>

namespace lsp
{
    
    class LSPCChunkWriter: public LSPCChunkAccessor
    {
        private:
            LSPCChunkWriter & operator = (const LSPCChunkWriter &);

        protected:
            friend class LSPCFile;

            enum flush_t
            {
                F_FORCE         = 1 << 0,
                F_LAST          = 1 << 1
            };

        protected:
            size_t              nChunksOut;

        protected:
            status_t            do_flush(size_t flags);

        protected:
            explicit LSPCChunkWriter(LSPCResource *fd, uint32_t magic);

        public:
            virtual ~LSPCChunkWriter();

        public:
            /**
             * Write chunk data header to LSPC chunk.
             * @param buf buffer containing header
             * @return status of operation
             */
            virtual status_t    write_header(const void *buf);

            /**
             * Write data to LSPC chunk
             * @param buf buffer to write
             * @param count number of bytes to write
             * @return status of operation
             */
            virtual status_t    write(const void *buf, size_t count);

            /**
             * Flush all buffers to file
             * @return status of operation
             */
            virtual status_t    flush();

            /**
             * Close chunk writer
             * @return status of operation
             */
            virtual status_t    close();
    };

} /* namespace lsp */

#endif /* CORE_FILES_LSPC_LSPCCHUNKWRITER_H_ */
