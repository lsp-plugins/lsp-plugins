/*
 * IDataSink.h
 *
 *  Created on: 16 авг. 2019 г.
 *      Author: sadko
 */

#ifndef UI_WS_IDATASINK_H_
#define UI_WS_IDATASINK_H_

namespace lsp
{
    namespace ws
    {
        /**
         * Data sink interface for asynchronous receive of binary data
         */
        class IDataSink
        {
            protected:
                ssize_t     nReferences;

            public:
                /**
                 * Create data sink with specified MIME type
                 * @param mime MIME type of the data sink
                 */
                explicit IDataSink();

                /**
                 * Destroy data sink
                 */
                virtual ~IDataSink();

            public:
                /**
                 * Open sink for writing
                 * @param mime_types NULL-terminated list of available MIME type of the data
                 * @return index of selected MIME type or negative error code
                 */
                virtual ssize_t     open(const char * const *mime_types);

                /**
                 * Write amount of bytes to the sink
                 * @param buf buffer to write
                 * @param count number of bytes to write
                 * @return status of operation
                 */
                virtual status_t    write(const void *buf, size_t count);

                /**
                 * Close sink
                 * @param code the data transfer completion code
                 */
                virtual status_t    close(status_t code);

                /**
                 * Acquire data sink for usage
                 * @return number of references
                 */
                size_t              acquire();

                /**
                 * Get number of references to the data sink
                 * @return number of references to the data sink
                 */
                inline size_t       references() const { return nReferences; };

                /**
                 * Release data source
                 * @return number of references to the data source
                 */
                size_t              release();
        };
    
    } /* namespace ws */
} /* namespace lsp */

#endif /* UI_WS_IDATASINK_H_ */
