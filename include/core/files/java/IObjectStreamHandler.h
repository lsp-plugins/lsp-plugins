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

#ifndef CORE_FILES_JAVA_IOBJECTSTREAMHANDLER_H_
#define CORE_FILES_JAVA_IOBJECTSTREAMHANDLER_H_

#include <common/types.h>
#include <core/status.h>
#include <core/LSPString.h>

namespace lsp
{
    namespace java
    {
        class IObjectStreamHandler
        {
            public:
                explicit IObjectStreamHandler();
                virtual ~IObjectStreamHandler();

            public:
                /**
                 * Handle start of the JAVA stream
                 * @param version stream version
                 * @return status of operation
                 */
                virtual status_t handle_start(size_t version);

                /**
                 * Handle end of stream
                 * @param status of parser
                 * @return status of operation
                 */
                virtual status_t handle_end();

                /**
                 * Close the processing handler
                 * @param res status of parsing
                 * @return status of operation
                 */
                virtual status_t close(status_t res);

                /**
                 * Handle reset
                 * @return status status of operation
                 */
                virtual status_t handle_reset();

                /**
                 * Handle NULL reference
                 * @return status status of operation
                 */
                virtual status_t handle_null();

                /**
                 * Handle reference
                 * @param object_num reference number
                 * @return status of operation
                 */
                virtual status_t handle_reference(size_t ref_num);

                virtual status_t handle_block_data(const void *buf, size_t size);

                virtual status_t handle_end_block_data();

                virtual status_t handle_string(const LSPString *str, bool unshared);
        };
    }

} /* namespace lsp */

#endif /* CORE_FILES_JAVA_OBJECTSTREAMHANDLER_H_ */
