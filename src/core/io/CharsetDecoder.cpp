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
            bBuffer         = NULL;
            bBufHead        = NULL;
            bBufTail        = NULL;
            cBuffer         = NULL;
            cBufHead        = NULL;
            cBufTail        = NULL;

#if defined(PLATFORM_WINDOWS)
            xBuffer         = NULL;
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
            nCodePage       = cp;
#else
            if (hIconv != iconv_t(-1))
                return STATUS_BAD_STATE;

            iconv_t handle = init_iconv_to_wchar_t(charset);
            if (handle == iconv_t(-1))
                return STATUS_BAD_LOCALE;
            hIconv      = handle;
#endif /* PLATFORM_WINDOWS */

            // Allocate buffer
            uint8_t *buf= reinterpret_cast<uint8_t *>(::malloc(
                        DATA_BUFSIZE    // The byte buffer size
                        + sizeof(lsp_wchar_t) * DATA_BUFSIZE * 2 // The temporary buffer size
#if defined(PLATFORM_WINDOWS)
                        + sizeof(lsp_utf16_t) * DATA_BUFSIZE * 2
#endif /* PLATFORM_WINDOWS */
                    ));
            if (buf == NULL)
            {
                close();
                return STATUS_NO_MEM;
            }

            bBuffer         = buf;
            bBufHead        = bBuffer;
            bBufTail        = bBuffer;
            buf            += DATA_BUFSIZE;
            cBuffer         = reinterpret_cast<lsp_wchar_t *>(buf);
            cBufHead        = cBuffer;
            cBufTail        = cBuffer;

#if defined(PLATFORM_WINDOWS)
            buf            += sizeof(lsp_wchar_t) * DATA_BUFSIZE * 2;
            xBuffer         = reinterpret_cast<lsp_utf16_t *>(buf);
#endif /* PLATFORM_WINDOWS */

            return STATUS_OK;
        }

        void CharsetDecoder::close()
        {
            if (bBuffer != NULL)
            {
                free(bBuffer);

                bBuffer         = NULL;
                bBufHead        = NULL;
                bBufTail        = NULL;
                cBuffer         = NULL;
                cBufHead        = NULL;
                cBufTail        = NULL;
            }

#ifdef PLATFORM_WINDOWS
            xBuffer     = NULL;
            nCodePage   = UINT(-1);
#else
            if (hIconv != iconv_t(-1))
            {
                ::iconv_close(hIconv);
                hIconv      = iconv_t(-1);
            }
#endif /* PLATFORM_WINDOWS */
        }
#if 0
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
                size_t bufcw        = DATA_BUFSIZE*2;
                size_t xincw        = xinamount;

                ssize_t nchars      = multibyte_to_widechar(nCodePage, xinbuf, &xincw, cBuffer, &bufcw);
                if (nchars <= 0)
                {
                    if (nconv <= 0)
                        return nchars;
                    break;
                }

                // Update pointers and data
                xinamount      -= xincw;
                cBufHead        = cBuffer;
                cBufTail        = &cBuffer[nchars];
                xinbuf         += xinamount;
                xinleft        -= xinamount;
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
                        nconv   = *outleft - (xoutleft/sizeof(lsp_wchar_t));
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
#endif

        size_t CharsetDecoder::prepare_buffer()
        {
            size_t bufsz = bBufTail - bBufHead;
            if (bufsz > (DATA_BUFSIZE >> 1))
                return 0;
            else if (bBufHead != bBuffer)
            {
                if (bufsz > 0)
                    ::memmove(bBuffer, bBufHead, bufsz);

                bBufHead    = bBuffer;
                bBufTail    = &bBuffer[bufsz];
            }
            return DATA_BUFSIZE - bufsz;
        }

        ssize_t CharsetDecoder::decode_buffer()
        {
            // Prepare buffer
            size_t bufsz = cBufTail - cBufHead;
            if (bufsz > DATA_BUFSIZE)
                return bufsz;
            else if (cBufHead != cBuffer)
            {
                if (bufsz > 0)
                    ::memmove(cBuffer, cBufHead, bufsz * sizeof(lsp_wchar_t));

                cBufHead    = cBuffer;
                cBufTail    = &cBuffer[bufsz];
            }

            // Is there any data in byte buffer?
            size_t xinleft      = bBufTail - bBufHead;
            if (!xinleft)
                return bufsz;

            // Now we can surely decode DATA_BUFSIZE characters
#ifdef PLATFORM_WINDOWS
            // Round 1: Perform native -> UTF-16 decoding
            CHAR *xinbuf        = reinterpret_cast<CHAR *>(bBufHead);
            size_t nsrc         = xinleft;
            size_t ndst         = DATA_BUFSIZE*2;
            ssize_t nbytes      = multibyte_to_widechar(nCodePage, xinbuf, &nsrc, xBuffer, &ndst);
            if (nbytes <= 0)
                return nbytes;
            uint8_t *bhead      = &bBufHead[xinleft - nsrc];

            // Round 2: Perform UTF-16 -> UTF-32 decoding
            nsrc                = DATA_BUFSIZE*2 - ndst;
            ndst                = DATA_BUFSIZE;
            ssize_t nchars      = utf16_to_utf32(cBufTail, &ndst, xBuffer, &nsrc, false);
            if (nchars <= 0)
                return nchars;

            bBufHead            = bhead;
            cBufTail           += DATA_BUFSIZE - ndst;
#else
            char *xinbuf        = reinterpret_cast<char *>(bBufHead);
            char *xoutbuf       = reinterpret_cast<char *>(cBufTail);
            bufsz               = DATA_BUFSIZE * sizeof(lsp_wchar_t);

            // Perform conversion
            size_t nconv        = ::iconv(hIconv, &xinbuf, &xinleft, &xoutbuf, &bufsz);
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

            bBufHead            = reinterpret_cast<uint8_t *>(xinbuf);
            cBufTail            = reinterpret_cast<lsp_wchar_t *>(xoutbuf);
#endif
            return cBufTail - cBufHead;
        }

        lsp_swchar_t CharsetDecoder::fetch()
        {
            if (bBuffer == NULL)
                return -STATUS_CLOSED;

            // Is there any data in character buffer
            if (cBufTail > cBufHead)
                return *(cBufHead++);

            ssize_t nchars   = decode_buffer();
            if (nchars > 0)
                return *(cBufHead++);
            return (nchars < 0) ? nchars : -STATUS_EOF;
        }

        ssize_t CharsetDecoder::fetch(lsp_wchar_t *outbuf, size_t count)
        {
            if (bBuffer == NULL)
                return -STATUS_CLOSED;
            else if (outbuf == NULL)
                return -STATUS_BAD_ARGUMENTS;

            // Compute the amount of data to read
            size_t processed = 0;

            // Perform read
            while (processed < count)
            {
                // Perform decoding
                ssize_t nchars   = decode_buffer();
                if (nchars <= 0)
                {
                    if (processed > 0)
                        break;
                    return nchars;
                }

                // Copy data to output buffer
                ssize_t to_copy = count - processed;
                if (nchars > to_copy)
                    nchars          = to_copy;
                ::memcpy(outbuf, cBufHead, nchars * sizeof(lsp_wchar_t));

                // Update state
                cBufHead       += nchars;
                processed      += nchars;
                outbuf         += nchars;
            }

            return processed;
        }

        ssize_t CharsetDecoder::fetch(LSPString *out, size_t count)
        {
            if (bBuffer == NULL)
                return -STATUS_CLOSED;
            else if (out == NULL)
                return -STATUS_BAD_ARGUMENTS;

            // Compute the amount of data to read
            size_t processed = 0;
            if (!count)
                count   = DATA_BUFSIZE*2;

            // Perform read
            while (processed < count)
            {
                // Perform decoding
                ssize_t nchars   = decode_buffer();
                if (nchars <= 0)
                {
                    if (processed > 0)
                        break;
                    return nchars;
                }

                // Copy data to output buffer
                ssize_t to_copy = count - processed;
                if (nchars > to_copy)
                    nchars          = to_copy;
                if (!out->append(cBufHead, nchars))
                    return -STATUS_NO_MEM;

                // Update state
                cBufHead       += nchars;
                processed      += nchars;
            }

            return processed;
        }

        ssize_t CharsetDecoder::fetch(IOutSequence *out, size_t count)
        {
            if (bBuffer == NULL)
                return -STATUS_CLOSED;
            else if (out == NULL)
                return -STATUS_BAD_ARGUMENTS;

            // Compute the amount of data to read
            size_t processed = 0;
            if (!count)
                count   = DATA_BUFSIZE*2;

            // Perform read
            while (processed < count)
            {
                // Perform decoding
                ssize_t nchars   = decode_buffer();
                if (nchars <= 0)
                {
                    if (processed > 0)
                        break;
                    return nchars;
                }

                // Write data to output sequence
                ssize_t to_copy = count - processed;
                if (nchars > to_copy)
                    nchars          = to_copy;
                nchars = out->write(cBufHead, nchars);
                if (nchars < 0)
                {
                    if (processed > 0)
                        break;
                    return nchars;
                }

                // Update state
                cBufHead       += nchars;
                processed      += nchars;
            }

            return processed;
        }

        ssize_t CharsetDecoder::fill(const void *buf, size_t count)
        {
            if (bBuffer == NULL)
                return -STATUS_CLOSED;
            else if (buf == NULL)
                return -STATUS_BAD_ARGUMENTS;

            size_t bufsz = prepare_buffer();
            if (bufsz <= 0)
                return bufsz;

            if (count > bufsz)
                count   = bufsz;
            ::memcpy(&bBufTail, buf, count);
            bBufTail       += count;
            return count;
        }

        ssize_t CharsetDecoder::fill(File *fd, size_t count)
        {
            if (bBuffer == NULL)
                return -STATUS_CLOSED;
            else if (fd == NULL)
                return -STATUS_BAD_ARGUMENTS;

            // Is there a space in the buffer for reading?
            size_t bufsz = prepare_buffer();
            if (bufsz <= 0)
                return bufsz;

            // Compute the amount of data to read
            size_t read = 0;
            if ((!count) || (count > bufsz))
                count   = bufsz;

            // Perform read
            do
            {
                ssize_t nread   = fd->read(bBufTail, count - read);
                if (nread <= 0)
                {
                    if (read > 0) // Ignore error if there is data on the input
                        break;
                    return nread;
                }

                bBufTail       += nread;
                read           += nread;
            }
            while (read < count);

            return read;
        }

        ssize_t CharsetDecoder::fill(IInStream *is, size_t count)
        {
            if (bBuffer == NULL)
                return -STATUS_CLOSED;
            else if (is == NULL)
                return -STATUS_BAD_ARGUMENTS;

            // Is there a space in the buffer for reading?
            size_t bufsz = prepare_buffer();
            if (bufsz <= 0)
                return bufsz;

            // Compute the amount of data to read
            size_t read = 0;
            if ((!count) || (count > bufsz))
                count   = bufsz;

            // Perform read
            do
            {
                ssize_t nread   = is->read(bBufTail, count - read);
                if (nread <= 0)
                {
                    if (read > 0) // Ignore error if there is data on the input
                        break;
                    return nread;
                }

                bBufTail       += nread;
                read           += nread;
            }
            while (read < count);

            return read;
        }
    } /* namespace io */
} /* namespace lsp */
