/*
 * LSPChunkWriter.h
 *
 *  Created on: 14 янв. 2018 г.
 *      Author: sadko
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
