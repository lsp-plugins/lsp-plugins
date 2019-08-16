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
                 * @param mime_type MIME type of the data
                 * @return status of operation
                 */
                virtual status_t open(const char *mime_type);

                /**
                 * Write amount of bytes to the sink
                 * @param buf buffer to write
                 * @param count number of bytes to write
                 */
                virtual status_t write(const void *buf, size_t count);

                /**
                 * Commit: the data has been successfully committed
                 */
                virtual status_t commit();

                /**
                 * Abort: there was an error while data has been transferred
                 * @param code the error code
                 */
                virtual status_t abort(status_t code);
        };
    
    } /* namespace ws */
} /* namespace lsp */

#endif /* UI_WS_IDATASINK_H_ */
