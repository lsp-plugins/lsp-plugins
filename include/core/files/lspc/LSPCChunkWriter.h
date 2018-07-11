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
        protected:
            friend class LSPCFile;

        protected:
            size_t              nChunksOut;

        protected:
            status_t            do_flush(bool force);

        protected:
            LSPCChunkWriter(LSPCResource *fd, uint32_t magic);

        public:
            virtual ~LSPCChunkWriter();

        public:
            virtual status_t    write(const void *buf, size_t count);

            virtual status_t    flush();

            virtual status_t    close();


    };

} /* namespace lsp */

#endif /* CORE_FILES_LSPC_LSPCCHUNKWRITER_H_ */
