/*
 * object_stream.h
 *
 *  Created on: 28 авг. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_JAVA_OBJECT_STREAM_H_
#define CORE_FILES_JAVA_OBJECT_STREAM_H_

#include <core/status.h>
#include <core/LSPString.h>
#include <core/io/Path.h>
#include <core/io/IInStream.h>
#include <core/files/java/IObjectStreamHandler.h>

namespace lsp
{
    namespace java
    {
        /**
         * Parse object stream
         * @param file location of the file
         * @param handler object stream handler
         * @return status of operation
         */
        status_t    parse_object_stream(const char *file, IObjectStreamHandler *handler);

        /**
         * Parse object stream
         * @param file location of the file
         * @param handler object stream handler
         * @return status of operation
         */
        status_t    parse_object_stream(const LSPString *file, IObjectStreamHandler *handler);

        /**
         * Parse object stream
         * @param file location of the file
         * @param handler object stream handler
         * @return status of operation
         */
        status_t    parse_object_stream(const io::Path *file, IObjectStreamHandler *handler);

        /**
         * Parse object stream
         * @param buf data contents
         * @param size size of data
         * @param handler object stream handler
         * @return status of operation
         */
        status_t    parse_object_stream(const void *buf, size_t count, IObjectStreamHandler *handler);

        /**
         * Parse object stream
         * @param is input stream
         * @param handler object stream handler
         * @return status of operation
         */
        status_t    parse_object_stream(io::IInStream *is, IObjectStreamHandler *handler);
    }
}

#endif /* CORE_FILES_JAVA_OBJECT_STREAM_H_ */
