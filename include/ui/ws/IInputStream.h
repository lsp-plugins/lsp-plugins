/*
 * IInputStream.h
 *
 *  Created on: 14 нояб. 2017 г.
 *      Author: sadko
 */

#ifndef UI_IINPUTSTREAM_H_
#define UI_IINPUTSTREAM_H_

namespace lsp
{
    namespace ws
    {
        
        /** This is data stream available to be read from clipboard
         *
         */
        class IInputStream
        {
            protected:
                status_t        nError;

            public:
                explicit IInputStream();
                virtual ~IInputStream();

            public:
                /** Get last I/O error code
                 *
                 * @return last I/O error code
                 */
                inline status_t     error_code() const { return nError; };

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

#endif /* UI_IINPUTSTREAM_H_ */
