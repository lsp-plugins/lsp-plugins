/*
 * CharsetDecoder.h
 *
 *  Created on: 8 мар. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_IO_CHARSETDECODER_H_
#define CORE_IO_CHARSETDECODER_H_

#include <core/status.h>
#include <core/io/charset.h>

namespace lsp
{
    namespace io
    {
        /**
         * Character set decoder, from any character set to UTF-32
         */
        class CharsetDecoder
        {
            protected:
#if defined(PLATFORM_WINDOWS)
                UINT            nCodePage;      // Code page
                WCHAR          *vBuffer;        // Temporary buffer for storing UTF-16 code points
                WCHAR          *pBufTail;       // End of buffer
                WCHAR          *pBufHead;       // Head of buffer
#else
                iconv_t         hIconv;         // iconv handle
#endif /* PLATFORM_WINDOWS */

            private:
                CharsetDecoder & operator = (const CharsetDecoder &);   // Deny copying

            public:
                explicit CharsetDecoder();
                ~CharsetDecoder();

            public:
                /**
                 * Initialize character-set decoder
                 * @param charset character set name, NULL for default native character set
                 * @return status of operation
                 */
                status_t    init(const char *charset = NULL);

                /**
                 * Close decoder and free all resources
                 */
                void        close();

                /**
                 * Decode characters from input buffer
                 * @param outbuf pointer to output buffer to store data
                 * @param outleft number of code points left unread in output buffer
                 * @param inbuf pointer to the input buffer
                 * @param inleft number of bytes left unread in input buffer
                 * @return number of code points decoded or negative error code
                 */
                ssize_t     decode(lsp_wchar_t **outbuf, size_t *outleft, const void **inbuf, size_t *inleft);
        };
    
    } /* namespace io */
} /* namespace lsp */

#endif /* CORE_IO_CHARSETDECODER_H_ */
