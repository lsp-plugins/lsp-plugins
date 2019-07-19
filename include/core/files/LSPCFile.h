/*
 * LSPCFile.h
 *
 *  Created on: 14 янв. 2018 г.
 *      Author: sadko
 */

#ifndef INCLUDE_CORE_FILES_LSPCFILE_H_
#define INCLUDE_CORE_FILES_LSPCFILE_H_

#include <core/LSPString.h>
#include <core/io/Path.h>
#include <core/files/lspc/LSPCChunkReader.h>
#include <core/files/lspc/LSPCChunkWriter.h>

namespace lsp
{
    class LSPCFile
    {
        protected:
            friend class LSPCAccessor;
            friend class LSPCChunkReader;
            friend class LSPCChunkWriter;

        private:
            LSPCFile & operator = (const LSPCFile &);

        protected:
            LSPCResource       *pFile;      // Shared resource
            bool                bWrite;     // Read/Write mode
            size_t              nHdrSize;   // Size of header

        protected:
            LSPCResource       *create_resource(lsp_fhandle_t fd);

        public:
            explicit LSPCFile();
            virtual ~LSPCFile();

        public:
            /** Open file for reading
             *
             * @param path location of the file
             * @return status of operation
             */
            status_t    open(const char *path);

            /** Open file for reading
             *
             * @param path location of the file
             * @return status of operation
             */
            status_t    open(const LSPString *path);

            /** Open file for reading
             *
             * @param path location of the file
             * @return status of operation
             */
            status_t    open(const io::Path *path);

            /** Open file for writing
             *
             * @param path location of the file
             * @return status of operation
             */
            status_t    create(const char *path);

            /** Open file for reading
             *
             * @param path location of the file
             * @return status of operation
             */
            status_t    create(const LSPString *path);

            /** Open file for reading
             *
             * @param path location of the file
             * @return status of operation
             */
            status_t    create(const io::Path *path);

            /** Close the file
             *
             * @return status of operation
             */
            status_t    close();

        public:
            /** Write chunk
             *
             * @param magic magic number of the chunk type
             * @return pointer to chunk writer
             */
            LSPCChunkWriter     *write_chunk(uint32_t magic);

            /** Read chunk
             *
             * @param magic magic number of the chunk type
             * @return pointer to chunk reader
             */
            LSPCChunkReader     *read_chunk(uint32_t uid);

            /** Read chunk of specific type
             *
             * @param uid unique chunk identifier
             * @param magic magic number of the chunk type
             * @return pointer to chunk reader
             */
            LSPCChunkReader     *read_chunk(uint32_t uid, uint32_t magic);

            /**
             * Find LSPC chunk in file by magic
             * @param magic chunk magic
             * @param id pointer to return chunk number
             * @param start_id start identifier of chunk
             * @return pointer to chunk reader
             */
            LSPCChunkReader     *find_chunk(uint32_t magic, uint32_t *id, uint32_t start_id = 1);

            /**
             * Find LSPC chunk in file by magic
             * @param magic chunk magic
             * @param id pointer to return chunk number
             * @param start_id start identifier of chunk
             * @return pointer to chunk reader
             */
            inline LSPCChunkReader *find_chunk(uint32_t magic, uint32_t start_id = 1) { return find_chunk(magic, NULL, start_id); }
    };

} /* namespace lsp */

#endif /* INCLUDE_CORE_FILES_LSPCFILE_H_ */
