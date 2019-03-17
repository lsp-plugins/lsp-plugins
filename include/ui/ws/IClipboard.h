/*
 * IClipboard.h
 *
 *  Created on: 14 нояб. 2017 г.
 *      Author: sadko
 */

#ifndef UI_WS_ICLIPBOARD_H_
#define UI_WS_ICLIPBOARD_H_

#include <core/io/IInStream.h>

namespace lsp
{
    namespace ws
    {
        /** This class implements the clipboard data stream factory
         *
         */
        class IClipboard
        {
            protected:
                status_t        nError;
                ssize_t         nReferences;

            public:
                explicit IClipboard();
                virtual ~IClipboard();

            public:
                /** Get last error code
                 *
                 * @return last error code
                 */
                inline status_t             error_code() const { return nError; };

                /** Increments number of references to the clipboard object
                 *
                 * @return status of operation
                 */
                virtual status_t            acquire();

                /** Closes the clipboard data and forces the clipboard to destroy object if possible
                 *
                 * @return status of operation
                 */
                virtual status_t            close();

                /** Get clipboard content for reading
                 *
                 * @param ctype content type
                 * @return pointer to the opened clipboard stream or NULL if content type or character set is not supported
                 */
                virtual io::IInStream   *read(const char *ctype);

                /** Get number of supported converison targets
                 *
                 * @return number of supported conversion types
                 */
                virtual size_t              targets();

                /** Get name of the supported target
                 *
                 * @param i the index of target
                 * @return status of operation
                 */
                virtual const char         *target(size_t i);

        };
    
    } /* namespace ws */
} /* namespace lsp */

#endif /* UI_WS_ICLIPBOARD_H_ */
