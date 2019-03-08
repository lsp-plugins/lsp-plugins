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
            nCodePage       = UINT(-1);
            vBuffer         = NULL;
            pBufTail        = NULL;
            pBufHead        = NULL;
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
            vBuffer     = malloc(sizeof(WCHAR) * DATA_BUFSIZE);
            if (vBuffer == NULL)
                return STATUS_NO_MEM;

            pBufHead    = vBuffer;
            pBufTail    = vBuffer;
            nCodePage   = cp;
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
            nCodePage   = UINT(-1);
            if (vBuffer != NULL)
            {
                free(vBuffer);

                vBuffer     = NULL;
                pBufHead    = NULL;
                pBufTail    = NULL;
            }
#else
            if (hIconv != iconv_t(-1))
            {
                iconv_close(hIconv);
                hIconv      = iconv_t(-1);
            }
#endif /* PLATFORM_WINDOWS */
        }

        ssize_t CharsetDecoder::decode(lsp_wchar_t **outbuf, size_t *outleft, const void **inbuf, size_t *inleft)
        {
            size_t nconv;

#if defined(PLATFORM_WINDOWS)
            lsp_utf32_t cp;

            const CHAR *xinbuf  = reinterpret_cast<const CHAR *>(*inbuf);
            lsp_wchar_t *xoutbuf= *outbuf;
            size_t xinleft      = *inleft;
            size_t xoutleft     = *outleft;
            nconv               = 0;

            while (xoutleft > 0)
            {
                size_t nbuf     = pBufTail - pBufHead;
                cp              = read_utf16_streaming(pBufTail, &nbuf, false);

                if (cp == LSP_UTF32_EOF)
                {
                    // Move the buffer data from the tail to the head (if there is one)
                    if (nbuf > 0)
                    {
                        ::memmove(vBuffer, pBufHead, nbuf * sizeof(WCHAR));
                        pBufHead    = vBuffer;
                        pBufTail    = &vBuffer[nbuf];
                    }

                    // Fill the rest space with converted UTF-16 data
                    // Each input character can take up to 2 UTF-16 characters, prevent from buffer overflows
                    // We can manipulate only with input buffer size because otherwise we will
                    // get a conversion error from dump MultiByteToWideChar routine
                    size_t amount   = nbuf >> 2;
                    if (amount > xinleft)
                        amount          = xinleft;
                    if (amount <= 0)
                        break;

                    ssize_t nchars  = MultiByteToWideChar(nCodePage, 0,
                            xinbuf, amount,
                            pBufTail, vBuffer + DATA_BUFSIZE - pBufTail);
                    if (nchars == 0)
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

                    // If function meets invalid sequence, it replaces the code point with such magic value
                    // We should know if function has failed
                    if (pBufTail[nchars-1] == 0xfffd)
                        --nchars;

                    // Estimate number of bytes decoded (yep, this is dumb but no way...)
                    ssize_t nbytes  = WideCharToMultiByte(nCodePage, 0, pBufTail, nchars, NULL, 0, 0, 0);
                    if (nbytes <= 0)
                    {
                        if (nconv > 0)
                            break;
                        return -STATUS_IO_ERROR;
                    }

                    // Update pointers and data
                    pBufTail       += nchars;
                    xinbuf         += nbytes;
                    xinleft        -= nbytes;
                }
                else
                {
                    // Store the code point to the output buffer
                    pBufHead        = &vBuffer[nbuf];
                    xoutleft       -= 1;
                    *(xoutbuf++)    = cp;
                    nconv          += 1;
                }
            }

            // Update pointers and values
            *outbuf             = xoutbuf;
            *outleft            = xoutleft;
            *inbuf              = reinterpret_cast<const void *>(xinbuf);
            *inleft             = xinleft;
#else
            char *xinbuf        = const_cast<char *>(reinterpret_cast<const char *>(*inbuf));
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
            *inbuf              = reinterpret_cast<const void *>(xinbuf);
            *inleft             = xinleft;
#endif /* PLATFORM_WINDOWS */

            return nconv;
        }

    } /* namespace io */
} /* namespace lsp */
