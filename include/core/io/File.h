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
#include <core/LSPString.h>
#include <core/io/Path.h>
#include <core/stdlib/stdio.h>

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

                    FM_READWRITE = FM_READ | FM_WRITE,
                    FM_WRITE_NEW = FM_CREATE | FM_WRITE | FM_TRUNC,
                    FM_READWRITE_NEW = FM_CREATE | FM_READ | FM_WRITE | FM_TRUNC
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
                 * @return number of bytes read or negative status of operation,
                 *   on end of file -STATUS_EOF is returned
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
                 * Write binary file.
                 * The implementation should write the most possible amount
                 * of bytes before exit. Such behaviour will simplify the caller's
                 * implementation.
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
                 * Get file attributes
                 * @param attr file attributes
                 * @return
                 */
                virtual status_t stat(fattr_t *attr);

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

            public:

                /**
                 * Obtain file information, do not follow symlinks
                 * @param path UTF-8 encoded path to file
                 * @param attr structure to return file attributes
                 * @return status of operation
                 */
                static status_t stat(const char *path, fattr_t *attr);

                /**
                 * Obtain file information, do not follow symlinks
                 * @param path path to file
                 * @param attr structure to return file attributes
                 * @return status of operation
                 */
                static status_t stat(const LSPString *path, fattr_t *attr);

                /**
                 * Obtain file information, do not follow symlinks
                 * @param path path to file
                 * @param attr structure to return file attributes
                 * @return status of operation
                 */
                static status_t stat(const Path *path, fattr_t *attr);

                /**
                 * Obtain file information
                 * @param fd native file descriptor
                 * @param attr structure to return file attributes
                 * @return status of operation
                 */
                static status_t stat(lsp_fhandle_t fd, fattr_t *attr);

                /**
                 * Obtain file information
                 * @param fd stdio file descriptor
                 * @param attr structure to return file attributes
                 * @return status of operation
                 */
                static status_t stat(FILE *fd, fattr_t *attr);

                /**
                 * Obtain file information, follow symlinks
                 * @param path UTF-8 encoded path to file
                 * @param attr structure to return file attributes
                 * @return status of operation
                 */
                static status_t sym_stat(const char *path, fattr_t *attr);

                /**
                 * Obtain file information, follow symlinks
                 * @param path path to file
                 * @param attr structure to return file attributes
                 * @return status of operation
                 */
                static status_t sym_stat(const LSPString *path, fattr_t *attr);

                /**
                 * Obtain file information, follow symlinks
                 * @param path path to file
                 * @param attr structure to return file attributes
                 * @return status of operation
                 */
                static status_t sym_stat(const Path *path, fattr_t *attr);

                /**
                 * Delete file
                 * @param path path to file
                 * @return status of operation
                 */
                static status_t remove(const char *path);

                /**
                 * Delete file
                 * @param path path to file
                 * @return status of operation
                 */
                static status_t remove(const LSPString *path);

                /**
                 * Delete file
                 * @param path path to file
                 * @return status of operation
                 */
                static status_t remove(const Path *path);
        };
    
    } /* namespace io */
} /* namespace lsp */

#endif /* INCLUDE_CORE_IO_FILE_H_ */
