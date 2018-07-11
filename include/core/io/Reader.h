/*
 * Reader.h
 *
 *  Created on: 14 июн. 2018 г.
 *      Author: sadko
 */

#ifndef CORE_IO_READER_H_
#define CORE_IO_READER_H_

#include <core/types.h>
#include <core/status.h>
#include <core/LSPString.h>
#include <iconv.h>

namespace lsp
{
    namespace io
    {
        class Reader
        {
            public:
                Reader();
                virtual ~Reader();

            public:
                /**
                 * Read amount of characters
                 * @param dst target buffer to read
                 * @param count number of characters to read
                 * @return actual number of characters read or negative value on end of stream
                 */
                virtual ssize_t     read(lsp_wchar_t *dst, size_t count);

                /**
                 * Read single character
                 * @return code of single character or negative value on end of stream
                 */
                virtual int         read();

                /**
                 * Read single line
                 * @param s string to store value
                 * @return status of operation
                 */
                virtual status_t    read_line(LSPString *s, bool force = false);

                /**
                 * Skip amount of characters
                 * @param count number of characters to skip
                 * @return number of skipped characters
                 */
                virtual ssize_t     skip(size_t count);

                /**
                 * Return last error code
                 * @return last error code, STATUS_OK on success
                 */
                virtual status_t    error();

                /**
                 * Close input stream
                 * @return status of operation
                 */
                virtual status_t    close();
        };
    }
    
} /* namespace lsp */

#endif /* CORE_IO_READER_H_ */
