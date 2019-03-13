/*
 * File.h
 *
 *  Created on: 6 мар. 2019 г.
 *      Author: sadko
 */

#ifndef INCLUDE_CORE_IO_FILE_H_
#define INCLUDE_CORE_IO_FILE_H_

#include <core/types.h>
#include <core/status.h>

namespace lsp
{
    namespace io
    {
        /**
         * Binary file interface, allows to perform read/write access
         * to the file on local file system
         */
        class File
        {
            public:
                enum seek_t {
                    FSK_SET,
                    FSK_CUR,
                    FSK_END
                };

                enum mode_t {
                    FM_READ     = 1 << 0,       // Open for reading
                    FM_WRITE    = 1 << 1,       // Open for writing
                    FM_CREATE   = 1 << 2,       // Create file if not exists
                    FM_TRUNC    = 1 << 3,       // Truncate file
                    FM_DIRECT   = 1 << 4,       // Do not use buffered input/output if possible
                };

            protected:
                status_t    nErrorCode;

            protected:
                inline status_t set_error(status_t error) { return nErrorCode = error; }

            private:
                File & operator = (const File &);       // Deny copying

            public:
                explicit File();
                virtual ~File();
                
            public:
                /**
                 * Read binary file
                 * @param dst target buffer to perform read
                 * @param count amount of bytes to read
                 * @return number of bytes read or negative status of operation
                 */
                virtual ssize_t read(void *dst, size_t count);

                /**
                 * Perform positioned read of binary file
                 * @param pos offset in bytes relative to the beginning of the file
                 * @param dst target buffer to perform read
                 * @param count amount of bytes to read
                 * @return number of bytes read or status of operation
                 */
                virtual ssize_t pread(wsize_t pos, void *dst, size_t count);

                /**
                 * Write binary file
                 * @param dst source buffer to perform write
                 * @param count number of bytes to write
                 * @return status of operation
                 */
                virtual ssize_t write(const void *src, size_t count);

                /**
                 * Perform positioned write of binary file
                 * @param pos offset in bytes relative to the beginning of the file
                 * @param dst source buffer to perform write
                 * @param count number of bytes to write
                 * @return status of operation
                 */
                virtual ssize_t pwrite(wsize_t pos, const void *src, size_t count);

                /**
                 * Perform seek to the specified position
                 * @param pos position to perform seek
                 * @param type seek type
                 * @return status of operation
                 */
                virtual status_t seek(wssize_t pos, size_t type);

                /**
                 * Obtain current file's position
                 * @return current file's position or negative error code
                 */
                virtual wssize_t position();

                /**
                 * Obtain current file's size
                 * @return current file's size or negative error code
                 */
                virtual wssize_t size();

                /**
                 * Truncate the file
                 * @param length the final file length
                 * @return status of operation
                 */
                virtual status_t truncate(wsize_t length);

                /**
                 * Return last error code
                 * @return last error code
                 */
                inline status_t last_error() const  { return nErrorCode; };

                /**
                 * Return true if last read operations reached end of file
                 * @return true if last read operations reached end of file
                 */
                inline bool eof() const { return nErrorCode == STATUS_EOF; };

                /**
                 * Flush usespace file buffer
                 * @return status of operation
                 */
                virtual status_t flush();

                /**
                 * Sync file with the underlying storage
                 * @return status of operation
                 */
                virtual status_t sync();

                /**
                 * Close file
                 * @return status of operation
                 */
                virtual status_t close();
        };
    
    } /* namespace io */
} /* namespace lsp */

#endif /* INCLUDE_CORE_IO_FILE_H_ */
