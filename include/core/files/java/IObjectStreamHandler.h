/*
 * IObjectStreamHandler.h
 *
 *  Created on: 28 авг. 2019 г.
 *      Author: sadko
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
