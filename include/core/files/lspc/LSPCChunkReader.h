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
            size_t              nUnread;            // Number of bytes still not read from chunk
            size_t              nBufTail;           // Buffer tail
            wsize_t             nFileOff;           // File read offset

        protected:
            LSPCChunkReader(LSPCResource *fd, uint32_t magic, uint32_t uid);

        public:
            virtual ~LSPCChunkReader();

        public:
            virtual ssize_t     read(void *buf, size_t count);

            virtual ssize_t     skip(size_t count);
    };

} /* namespace lsp */

#endif /* CORE_FILES_LSPC_LSPCCHUNKREADER_H_ */
