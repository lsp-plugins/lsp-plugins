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
            cBuffer         = NULL;
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
                        sizeof(lsp_utf32_t) * DATA_BUFSIZE
                    ));
            if (buf == NULL)
                return STATUS_NO_MEM;

            cBuffer         = reinterpret_cast<lsp_utf16_t *>(buf);
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
            if (cBuffer != NULL)
            {
                free(cBuffer);
                cBuffer     = NULL;
            }
#else
            if (hIconv != iconv_t(-1))
            {
                iconv_close(hIconv);
                hIconv      = iconv_t(-1);
            }
#endif /* PLATFORM_WINDOWS */
        }

        ssize_t CharsetEncoder::encode(void **outbuf, size_t *outleft, lsp_wchar_t **inbuf, size_t *inleft)
        {
            size_t nconv;
#if defined(PLATFORM_WINDOWS)
            lsp_wchar_t *xinbuf = *inbuf;
            CHAR *xoutbuf       = reinterpret_cast<CHAR *>(*outbuf);
            size_t xinleft      = *inleft;
            size_t xoutleft     = *outleft;
            nconv               = 0;

            while (xoutleft > 0)
            {
                // Perform optimistic UTF-32 -> UTF-16 encoding of the whole buffer
                size_t nsrc     = (xinleft > DATA_BUFSIZE) ? DATA_BUFSIZE : xinleft;
                size_t ndst     = DATA_BUFSIZE;
                ssize_t nchars  = utf32_to_utf16(cBuffer, &ndst, xinbuf, &nsrc, false);
                if (nchars <= 0)
                {
                    if (nconv <= 0)
                        return nchars;
                    break;
                }

                // Perform safe UTF-16 -> native encoding
                size_t xnchars  = nchars;
                ssize_t nbytes  = widechar_to_multibyte(nCodePage, cBuffer, &xnchars, xoutbuf, &xoutleft);
                if (nbytes <= 0)
                {
                    if (nconv <= 0)
                        return nbytes;
                    break;
                }
                nchars         -= xnchars;

                // Update statistics
                xinbuf         += nchars;
                xinleft        -= nchars;
                xoutbuf        += nbytes;
            }

            *outbuf             = xoutbuf;
            *outleft            = xoutleft;
            *inbuf              = reinterpret_cast<lsp_wchar_t *>(xinbuf);
            *inleft             = xinleft;
#else
            char *xinbuf        = reinterpret_cast<char *>(*inbuf);
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
                        nconv   = *inleft - (xinleft / sizeof(lsp_wchar_t));
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
