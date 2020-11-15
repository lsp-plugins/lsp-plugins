/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 28 авг. 2019 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
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
