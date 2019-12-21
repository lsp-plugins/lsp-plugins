/*
 * IOutStream.h
 *
 *  Created on: 14 нояб. 2017 г.
 *      Author: sadko
 */

#ifndef CORE_IO_IOUTSTREAM_H_
#define CORE_IO_IOUTSTREAM_H_

#include <core/types.h>
#include <core/status.h>

namespace lsp
{
    namespace io
    {
        class IOutStream
        {
            private:
                IOutStream & operator = (const IOutStream &);

            protected:
                status_t        nErrorCode;

            protected:
                inline status_t set_error(status_t error) { return nErrorCode = error; }

            public:
                explicit IOutStream();
                virtual ~IOutStream();

            public:
                /** Get last I/O error code
                 *
                 * @return last I/O error code
                 */
                inline status_t     last_error() const { return nErrorCode; };

                /** The current read position
                 *
                 * @return current read position or error code
                 */
                virtual wssize_t    position();

                /** Write the data to output stream.
                 * The implementation should write the most possible amount
                 * of bytes before exit. Such behaviour will simplify the caller's
                 * implementation.
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

                /**
                 * Flush buffers to underlying storage
                 * @return status of operation
                 */
                virtual status_t    flush();

                /** Close the clip data stream
                 *
                 * @return status of operation
                 */
                virtual status_t    close();
        };
    
    } /* namespace ws */
} /* namespace lsp */

#endif /* CORE_IO_IOUTSTREAM_H_ */
