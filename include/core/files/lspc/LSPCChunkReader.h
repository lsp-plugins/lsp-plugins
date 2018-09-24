/*
 * LSPChunkReader.h
 *
 *  Created on: 14 янв. 2018 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_LSPC_LSPCCHUNKREADER_H_
#define CORE_FILES_LSPC_LSPCCHUNKREADER_H_

#include <core/files/lspc/LSPCChunkAccessor.h>

namespace lsp
{
    
    class LSPCChunkReader: public LSPCChunkAccessor
    {
        protected:
            friend class LSPCFile;

        protected:
            uint32_t            nUnread;            // Number of bytes still not read from chunk
            size_t              nBufTail;           // Buffer tail
            wsize_t             nFileOff;           // File read offset
            bool                bLast;

        protected:
            LSPCChunkReader(LSPCResource *fd, uint32_t magic, uint32_t uid);

        public:
            virtual ~LSPCChunkReader();

        public:
            /**
             * Read chunk data header from LSPC chunk. Header should contain lspc_header_t at
             * the beginning.
             * @param hdr the header data to store
             * @param size the size of header, should be at least sizeof(lspc_header_t)
             * @return status of operation or error code (negative)
             */
            virtual ssize_t     read_header(void *hdr, size_t size);

            /**
             * Read regular data from LSPC chunk.
             * @param buf the pointer to store data
             * @param count number of bytes to read
             * @return status of operation or error code (negative)
             */
            virtual ssize_t     read(void *buf, size_t count);

            /**
             * Skip some amount of data
             * @param count amount of bytes to skip
             * @return number of skipped bytes or error code (negative)
             */
            virtual ssize_t     skip(size_t count);
    };

} /* namespace lsp */

#endif /* CORE_FILES_LSPC_LSPCCHUNKREADER_H_ */
