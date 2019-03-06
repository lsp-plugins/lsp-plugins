/*
 * File.h
 *
 *  Created on: 6 мар. 2019 г.
 *      Author: sadko
 */

#ifndef INCLUDE_CORE_IO_IFILE_H_
#define INCLUDE_CORE_IO_IFILE_H_

#include <core/types.h>
#include <core/status.h>

namespace lsp
{
    namespace io
    {
        /**
         * Binary file interface
         */
        class IFile
        {
            public:
                enum seek_t {
                    FSK_SET,
                    FSK_CURR,
                    FSK_END
                };

            private:
                IFile & operator = (const IFile &);       // Deny copying

            protected:
                status_t    nErrorCode;

                inline status_t set_error(status_t error) { return nErrorCode = error; }

            public:
                explicit IFile();
                virtual ~IFile();
                
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
                virtual status_t write(const void *src, size_t count);

                /**
                 * Perform positioned write of binary file
                 * @param pos offset in bytes relative to the beginning of the file
                 * @param dst source buffer to perform write
                 * @param count number of bytes to write
                 * @return status of operation
                 */
                virtual status_t write(wsize_t pos, const void *src, size_t count);

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
                 * Flush file buffer to underlying storage
                 * @return status of operation
                 */
                virtual status_t flush();

                /**
                 * Close file
                 * @return status of operation
                 */
                virtual status_t close();
        };
    
    } /* namespace io */
} /* namespace lsp */

#endif /* INCLUDE_CORE_IO_IFILE_H_ */
