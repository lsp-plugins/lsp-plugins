/*
 * LSPDataSink.h
 *
 *  Created on: 22 авг. 2019 г.
 *      Author: sadko
 */

#ifndef UI_TK_UTIL_LSPDATASINK_H_
#define UI_TK_UTIL_LSPDATASINK_H_

#include <core/io/OutMemoryStream.h>

namespace lsp
{
    namespace tk
    {
        class LSPTextDataSink: public IDataSink
        {
            private:
                ssize_t                 nMimeType;
                io::OutMemoryStream     sOut;

            public:
                explicit LSPTextDataSink();
                virtual ~LSPTextDataSink();

            public:
                virtual ssize_t     open(const char * const *mime_types);

                virtual status_t    write(const void *buf, size_t count);

                virtual status_t    close(status_t code);

                /**
                 * Callback, is called on the close() method has been called
                 * @param code completion code
                 * @param data the actual data received
                 * @return status of operation
                 */
                virtual status_t    on_complete(status_t code, const LSPString *data);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_UTIL_LSPDATASINK_H_ */
