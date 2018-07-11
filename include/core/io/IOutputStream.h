/*
 * IOutputStream.h
 *
 *  Created on: 14 нояб. 2017 г.
 *      Author: sadko
 */

#ifndef CORE_IO_IOUTPUTSTREAM_H_
#define CORE_IO_IOUTPUTSTREAM_H_

#include <core/types.h>
#include <core/status.h>

namespace lsp
{
    namespace io
    {
        class IOutputStream
        {
            protected:
                status_t        nError;

            public:
                IOutputStream();
                virtual ~IOutputStream();

            public:
                /** Get last I/O error code
                 *
                 * @return last I/O error code
                 */
                inline status_t     error_code() const { return nError; };

                /** The current read position
                 *
                 * @return current read position or error code
                 */
                virtual wssize_t    position();

                /** Write the data to output stream
                 *
                 * @param buf buffer to write
                 * @param count number of bytes
                 * @return number of bytes actually written
                 */
                virtual ssize_t     write(const void *buf, size_t count);

                /** Seek the stream to the specified position from the beginning
                 *
                 * @param position the specified position
                 * @return real position or negative value on error
                 */
                virtual wssize_t    seek(wsize_t position);

                /** Close the clip data stream
                 *
                 * @return status of operation
                 */
                virtual status_t    close();
        };
    
    } /* namespace ws */
} /* namespace lsp */

#endif /* CORE_IO_IOUTPUTSTREAM_H_ */
