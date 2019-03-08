/*
 * CharsetEncoder.h
 *
 *  Created on: 8 мар. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_IO_CHARSETENCODER_H_
#define CORE_IO_CHARSETENCODER_H_

#include <core/status.h>
#include <core/io/charset.h>

namespace lsp
{
    namespace io
    {
        /**
         * Character set encoder, from UTF-32 to any character set
         */
        class CharsetEncoder
        {
            protected:
#if defined(PLATFORM_WINDOWS)
                uint8_t        *bBuffer;        // Byte buffer
                uint8_t        *bBufHead;       // Byte buffer head
                uint8_t        *bBufTail;       // Byte buffer tail

                lsp_utf16_t    *cBuffer;        // Temporary buffer for storing UTF-16 code points
                lsp_utf16_t    *cBufHead;       // Head of buffer
                lsp_utf16_t    *cBufTail;       // End of buffer

                UINT            nCodePage;      // Code page
#else
                iconv_t         hIconv;         // iconv handle
#endif /* PLATFORM_WINDOWS */

            private:
                CharsetEncoder &operator = (const CharsetEncoder &);    // Prevent from copying

            public:
                explicit CharsetEncoder();
                ~CharsetEncoder();

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
                 * Encode characters to the output buffer
                 * @param outbuf pointer to output buffer to store data
                 * @param outleft number of bytes in output buffer
                 * @param inbuf pointer to the input buffer
                 * @param inleft
                 * @return
                 */
                ssize_t     encode(void **outbuf, size_t *outleft, const lsp_wchar_t **inbuf, size_t *inleft);
        };
    
    } /* namespace io */
} /* namespace lsp */

#endif /* CORE_IO_CHARSETENCODER_H_ */
