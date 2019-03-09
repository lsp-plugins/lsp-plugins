/*
 * CharsetDecoder.cpp
 *
 *  Created on: 8 мар. 2019 г.
 *      Author: sadko
 */

#include <core/io/CharsetDecoder.h>
#include <errno.h>

#define DATA_BUFSIZE   0x1000

namespace lsp
{
    namespace io
    {
        
        CharsetDecoder::CharsetDecoder()
        {
#if defined(PLATFORM_WINDOWS)
            cBuffer         = NULL;
            cBufHead        = NULL;
            cBufTail        = NULL;

            nCodePage       = UINT(-1);
#else
            hIconv          = iconv_t(-1);
#endif /* PLATFORM_WINDOWS */
        }
        
        CharsetDecoder::~CharsetDecoder()
        {
            close();
        }
    
        status_t CharsetDecoder::init(const char *charset)
        {
#if defined(PLATFORM_WINDOWS)
            if (nCodePage != UINT(-1))
                return STATUS_BAD_STATE;

            ssize_t cp  = codepage_from_name(charset);
            if (cp < 0)
                return STATUS_BAD_LOCALE;

            // Allocate buffer
            uint8_t *buf= reinterpret_cast<uint8_t *>(::malloc(
                        sizeof(lsp_utf16_t) * DATA_BUFSIZE * 4
                    ));
            if (buf == NULL)
                return STATUS_NO_MEM;

            cBuffer         = reinterpret_cast<lsp_utf16_t *>(buf);
            cBufHead        = cBuffer;
            cBufTail        = cBuffer;
            nCodePage       = cp;
#else
            if (hIconv != iconv_t(-1))
                return STATUS_BAD_STATE;

            iconv_t handle = init_iconv_to_wchar_t(charset);
            if (handle == iconv_t(-1))
                return STATUS_BAD_LOCALE;
            hIconv      = handle;
#endif /* PLATFORM_WINDOWS */

            return STATUS_OK;
        }

        void CharsetDecoder::close()
        {
#if defined(PLATFORM_WINDOWS)
            if (cBuffer != NULL)
            {
                free(cBuffer);

                cBuffer     = NULL;
                cBufHead    = NULL;
                cBufTail    = NULL;
            }

            nCodePage   = UINT(-1);
#else
            if (hIconv != iconv_t(-1))
            {
                iconv_close(hIconv);
                hIconv      = iconv_t(-1);
            }
#endif /* PLATFORM_WINDOWS */
        }

        ssize_t CharsetDecoder::decode(lsp_wchar_t **outbuf, size_t *outleft, void **inbuf, size_t *inleft)
        {
            size_t nconv;

#if defined(PLATFORM_WINDOWS)
            CHAR *xinbuf        = reinterpret_cast<CHAR *>(*inbuf);
            lsp_wchar_t *xoutbuf= *outbuf;
            size_t xinleft      = *inleft;
            size_t xoutleft     = *outleft;
            nconv               = 0;

            while (xoutleft > 0)
            {
                // Is there a data in wchar_t buffer?
                size_t nbuf     = cBufTail - cBufHead;
                if (nbuf > 0)
                {
                    size_t nsrc = nbuf;
                    nbuf        = utf16_to_utf32(xoutbuf, &xoutleft, cBufHead, &nsrc, false);
                    if (nbuf <= 0)
                        break;

                    nconv          += nbuf;
                    xoutbuf        += nbuf;
                    cBufHead       += nbuf;
                    continue;
                }

                // Fill the rest space with converted UTF-16 data
                // Each input character can take up to 2 UTF-16 characters, prevent from buffer overflows
                // We can manipulate only with input buffer size because otherwise we will
                // get a conversion error from dump MultiByteToWideChar routine
                // character buffer is guaranteed to be empty
                size_t xinamount    = (xinleft > DATA_BUFSIZE) ? DATA_BUFSIZE : xinleft;

                ssize_t nchars      = multibyte_to_widechar(nCodePage, xinbuf, xinamount, cBuffer, DATA_BUFSIZE*4);
                if (nchars <= 0)
                {
                    if (nconv > 0)
                        break;
                    return nchars;
                }

                // If function meets invalid sequence, it replaces the code point with such magic value
                // We should know if function has failed
                if (cBuffer[nchars-1] == 0xfffd)
                    --nchars;

                // Estimate number of bytes decoded (yep, this is dumb but no way...)
                ssize_t nbytes  = WideCharToMultiByte(nCodePage, 0, cBuffer, nchars, NULL, 0, 0, 0);
                if (nbytes <= 0)
                {
                    if (nconv > 0)
                        break;
                    return -STATUS_IO_ERROR;
                }

                // Update pointers and data
                cBufHead        = cBuffer;
                cBufTail        = &cBuffer[nchars];
                xinbuf         += nbytes;
                xinleft        -= nbytes;
            }

            // Update pointers and values
            *outbuf             = xoutbuf;
            *outleft            = xoutleft;
            *inbuf              = reinterpret_cast<void *>(xinbuf);
            *inleft             = xinleft;
#else
            char *xinbuf        = reinterpret_cast<char *>(*inbuf);
            char *xoutbuf       = reinterpret_cast<char *>(*outbuf);
            size_t xinleft      = *inleft;
            size_t xoutleft     = *outleft * sizeof(lsp_wchar_t);

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
            *outbuf             = reinterpret_cast<lsp_wchar_t *>(xoutbuf);
            *outleft            = xoutleft / sizeof(lsp_wchar_t);
            *inbuf              = reinterpret_cast<void *>(xinbuf);
            *inleft             = xinleft;
#endif /* PLATFORM_WINDOWS */

            return nconv;
        }

    } /* namespace io */
} /* namespace lsp */
