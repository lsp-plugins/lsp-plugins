/*
 * IInStream.h
 *
 *  Created on: 14 нояб. 2017 г.
 *      Author: sadko
 */

#ifndef CORE_IO_IINSTREAM_H_
#define CORE_IO_IINSTREAM_H_

#include <core/types.h>
#include <core/status.h>

namespace lsp
{
    namespace io
    {
        
        /** This is data stream available to be read from clipboard
         *
         */
        class IInStream
        {
            private:
                IInStream & operator = (const IInStream &);

            protected:
                status_t        nErrorCode;

            protected:
                inline status_t set_error(status_t error) { return nErrorCode = error; }

            public:
                explicit IInStream();
                virtual ~IInStream();

            public:
                /** Get last I/O error code
                 *
                 * @return last I/O error code
                 */
                inline status_t     last_error() const { return nErrorCode; };

                /** The number of bytes available
                 *
                 * @return number of bytes available or negative error code
                 */
                virtual wssize_t    avail();

                /** The current read position
                 *
                 * @return current read position or negative error code
                 */
                virtual wssize_t    position();

                /** Read amount of data
                 *
                 * @param dst target buffer to read data
                 * @param count number of bytes to read
                 * @return number of bytes actually read
                 */
                virtual ssize_t     read(void *dst, size_t count);

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

#endif /* CORE_IO_IINSTREAM_H_ */
