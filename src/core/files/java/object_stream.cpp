/*
 * object_stream.cpp
 *
 *  Created on: 28 авг. 2019 г.
 *      Author: sadko
 */

#include <core/io/InFileStream.h>
#include <core/io/InMemoryStream.h>
#include <core/files/java/object_stream.h>

namespace lsp
{
    namespace java
    {
        status_t    parse_object_stream(const char *file, IObjectStreamHandler *handler)
        {
            io::InFileStream is;
            status_t res = is.open(file);
            if (res != STATUS_OK)
            {
                is.close();
                return res;
            }
            res = parse_object_stream(&is, handler);
            if (res != STATUS_OK)
            {
                is.close();
                return res;
            }

            return is.close();
        }

        status_t    parse_object_stream(const LSPString *file, IObjectStreamHandler *handler)
        {
            io::InFileStream is;
            status_t res = is.open(file);
            if (res != STATUS_OK)
            {
                is.close();
                return res;
            }
            res = parse_object_stream(&is, handler);
            if (res != STATUS_OK)
            {
                is.close();
                return res;
            }

            return is.close();
        }

        status_t    parse_object_stream(const io::Path *file, IObjectStreamHandler *handler)
        {
            io::InFileStream is;
            status_t res = is.open(file);
            if (res != STATUS_OK)
            {
                is.close();
                return res;
            }
            res = parse_object_stream(&is, handler);
            if (res != STATUS_OK)
            {
                is.close();
                return res;
            }

            return is.close();
        }

        status_t    parse_object_stream(const void *buf, size_t count, IObjectStreamHandler *handler)
        {
            io::InMemoryStream is;
        }

        /**
         * Parse object stream
         * @param is input stream
         * @param handler object stream handler
         * @return status of operation
         */
        status_t    parse_object_stream(io::IInStream *is, IObjectStreamHandler *handler);
    }
}


