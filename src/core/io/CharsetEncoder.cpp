/*
 * CharsetEncoder.cpp
 *
 *  Created on: 8 мар. 2019 г.
 *      Author: sadko
 */

#include <core/io/CharsetEncoder.h>
#include <errno.h>

#define DATA_BUFSIZE    0x1000

namespace lsp
{
    namespace io
    {
        
        CharsetEncoder::CharsetEncoder()
        {
#if defined(PLATFORM_WINDOWS)
            bBuffer         = NULL;
            bBufHead        = NULL;
            bBufTail        = NULL;

            cBuffer         = NULL;
            cBufHead        = NULL;
            cBufTail        = NULL;

            nCodePage       = UINT(-1);
#else
            hIconv          = iconv_t(-1);
#endif /* PLATFORM_WINDOWS */
        }
        
        CharsetEncoder::~CharsetEncoder()
        {
            close();
        }

        status_t CharsetEncoder::init(const char *charset)
        {
#if defined(PLATFORM_WINDOWS)
            if (nCodePage != UINT(-1))
                return STATUS_BAD_STATE;

            ssize_t cp  = codepage_from_name(charset);
            if (cp < 0)
                return STATUS_BAD_LOCALE;

            // Allocate buffer
            uint8_t *buf= reinterpret_cast<uint8_t *>(::malloc(
                        sizeof(lsp_utf16_t) * DATA_BUFSIZE +
                        sizeof(lsp_utf32_t) * DATA_BUFSIZE
                    );
            if (buf == NULL)
                return STATUS_NO_MEM;

            bBuffer         = buf;
            bBufHead        = bBuffer;
            bBufTail        = bBuffer;

            cBuffer         = reinterpret_cast<lsp_utf16_t *>(&buf[sizeof(lsp_utf32_t) * DATA_BUFSIZE]);
            cBufHead        = cBuffer;
            cBufTail        = cBuffer;

            nCodePage       = cp;
#else
            if (hIconv != iconv_t(-1))
                return STATUS_BAD_STATE;

            iconv_t handle = init_iconv_from_wchar_t(charset);
            if (handle == iconv_t(-1))
                return STATUS_BAD_LOCALE;
            hIconv      = handle;
#endif /* PLATFORM_WINDOWS */

            return STATUS_OK;
        }

        void CharsetEncoder::close()
        {
#if defined(PLATFORM_WINDOWS)
            nCodePage   = UINT(-1);
            if (bBuffer != NULL)
            {
                free(bBuffer);

                bBuffer     = NULL;
                bBufHead    = NULL;
                bBufTail    = NULL;

                cBuffer     = NULL;
                cBufHead    = NULL;
                cBufTail    = NULL;
            }
#else
            if (hIconv != iconv_t(-1))
            {
                iconv_close(hIconv);
                hIconv      = iconv_t(-1);
            }
#endif /* PLATFORM_WINDOWS */
        }

        ssize_t CharsetEncoder::encode(void **outbuf, size_t *outleft, const lsp_wchar_t **inbuf, size_t *inleft)
        {
            size_t nconv;
#if defined(PLATFORM_WINDOWS)
            const lsp_wchar_t *xinbuf    = *inbuf;
            uint8_t *xoutbuf    = reinterpret_cast<uint8_t *>(*outbuf);
            size_t xinleft      = *inleft;
            size_t xoutleft     = *outleft;

            while (xoutleft > 0)
            {
                // Is there a data in byte buffer?
                size_t nbuf     = bBufTail - bBufHead;
                if (nbuf > 0)
                {
                    if (nbuf > xoutleft)
                        nbuf        = xoutleft;
                    ::memcpy(xoutbuf, bBufHead, nbuf);

                    nconv          += nbuf;
                    xoutbuf        += nbuf;
                    xoutleft       -= nbuf;
                    bBufHead       += nbuf;
                    continue;
                }

                // Is there a data in character buffer?
                // Byte buffer is guaranteed to be empty
                nbuf            = cBufTail - cBufHead;
                if (nbuf > 0)
                {
                    // Perform UTF-16 -> native encoding
                    size_t bytes    = WideCharToMultiByte(nCodePage, 0, bBuffer, DATA_BUFSIZE*sizeof(lsp_wchar_t), cBufHead, nbuf, 0, FALSE);
                    if (bytes == 0)
                    {
                        if (nconv > 0)
                            break;

                        switch (GetLastError())
                        {
                            case ERROR_INSUFFICIENT_BUFFER:
                                return -STATUS_NO_MEM;
                            case ERROR_INVALID_FLAGS:
                            case ERROR_INVALID_PARAMETER:
                                return -STATUS_BAD_STATE;
                            case ERROR_NO_UNICODE_TRANSLATION:
                                return -STATUS_BAD_LOCALE;
                            default:
                                return -STATUS_UNKNOWN_ERR;
                        }
                    }

                    // Update byte buffer state
                    bBufHead    = bBuffer;
                    bBufTail    = &bBuffer[bytes];
                    continue;
                }

                // Perform UTF-32 -> UTF-16 encoding
                // Character and byte buffers are guaranteed to be empty
                size_t ndst = DATA_BUFSIZE;
                nbuf        = utf32_to_utf16(cBuffer, &ndst, inbuf, &xinleft, false);
                if (nbuf <= 0)
                    break;

                // Update buffer state
                bBufHead    = bBuffer;
                bBufTail    = bBuffer;
                cBufHead    = cBuffer;
                cBufTail    = &cBuffer[DATA_BUFSIZE - ndst];
            }

            *outbuf             = xoutbuf;
            *outleft            = xoutleft;
            *inbuf              = reinterpret_cast<lsp_wchar_t *>(xinbuf);
            *inleft             = xinleft;
#else
            char *xinbuf        = const_cast<char *>(reinterpret_cast<const char *>(*inbuf));
            char *xoutbuf       = reinterpret_cast<char *>(*outbuf);
            size_t xinleft      = *inleft * sizeof(lsp_wchar_t);
            size_t xoutleft     = *outleft;

            // Perform conversion
            nconv               = ::iconv(hIconv, &xinbuf, &xinleft, &xoutbuf, &xoutleft);
            if (nconv == size_t(-1))
            {
                int code = errno;
                switch (code)
                {
                    case E2BIG:
                    case EINVAL:
                        break;
                    default:
                        return -STATUS_BAD_FORMAT;
                }
            }

            // Update pointers and values
            *outbuf             = xoutbuf;
            *outleft            = xoutleft;
            *inbuf              = reinterpret_cast<lsp_wchar_t *>(xinbuf);
            *inleft             = xinleft / sizeof(lsp_wchar_t);
#endif /* PLATFORM_WINDOWS */

            return nconv;
        }
    
    } /* namespace io */
} /* namespace lsp */
