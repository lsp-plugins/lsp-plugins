/*
 * IInSequence.h
 *
 *  Created on: 14 июн. 2018 г.
 *      Author: sadko
 */

#ifndef CORE_IO_IINSEQUENCE_H_
#define CORE_IO_IINSEQUENCE_H_

#include <core/types.h>
#include <core/status.h>
#include <core/LSPString.h>
#include <iconv.h>

namespace lsp
{
    namespace io
    {
        class IInSequence
        {
            protected:
                status_t        nErrorCode;

            protected:
                inline status_t set_error(status_t error) { return nErrorCode = error; }

            private:
                IInSequence & operator = (const IInSequence &);

            public:
                explicit IInSequence();
                virtual ~IInSequence();

            public:
                /**
                 * Return last error code
                 * @return last error code
                 */
                inline status_t last_error() const  { return nErrorCode; };

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
                virtual lsp_swchar_t    read();

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
                 * Close input stream
                 * @return status of operation
                 */
                virtual status_t    close();
        };
    }
    
} /* namespace lsp */

#endif /* CORE_IO_IINSEQUENCE_H_ */
