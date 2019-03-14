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
            bBuffer         = NULL;
            bBufHead        = NULL;
            bBufTail        = NULL;

            cBuffer         = NULL;
            cBufHead        = NULL;
            cBufTail        = NULL;

#if defined(PLATFORM_WINDOWS)
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
            nCodePage       = cp;
#else
            if (hIconv != iconv_t(-1))
                return STATUS_BAD_STATE;

            iconv_t handle = init_iconv_from_wchar_t(charset);
            if (handle == iconv_t(-1))
                return STATUS_BAD_LOCALE;
            hIconv      = handle;
#endif /* PLATFORM_WINDOWS */

            // Allocate buffer
            uint8_t *buf= reinterpret_cast<uint8_t *>(::malloc(
                        sizeof(lsp_utf32_t) * DATA_BUFSIZE * 2 + // Byte buffer size
                        sizeof(lsp_wchar_t) * DATA_BUFSIZE // wchar_t buffer size
                    ));
            if (buf == NULL)
            {
                close();
                return STATUS_NO_MEM;
            }

            bBuffer         = buf;
            bBufHead        = bBuffer;
            bBufTail        = bBuffer;

            cBuffer         = reinterpret_cast<lsp_wchar_t *>(&buf[sizeof(lsp_utf32_t) * DATA_BUFSIZE * 2]);
            cBufHead        = cBuffer;
            cBufTail        = cBuffer;

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

#if 0
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

#endif

        size_t CharsetEncoder::prepare_buffer()
        {
        }

        ssize_t CharsetEncoder::encode_buffer()
        {

        }

        ssize_t CharsetEncoder::fill(lsp_wchar_t ch)
        {
            if (bBuffer == NULL)
                return -STATUS_CLOSED;
            if (cBufTail >= &cBuffer[DATA_BUFSIZE])
                return 0;
            *(cBufTail++)   = ch;
            return 1;
        }

        ssize_t CharsetEncoder::fill(const lsp_wchar_t *buf, size_t count)
        {
            if (bBuffer == NULL)
                return -STATUS_CLOSED;
            else if (buf == NULL)
                return -STATUS_BAD_ARGUMENTS;

            // Is there a space in the buffer for reading?
            size_t bufsz = prepare_buffer();
            if (bufsz <= 0)
                return bufsz;

            if (count > bufsz)
                count   = bufsz;
            ::memcpy(&cBufTail, buf, count * sizeof(lsp_wchar_t));
            bBufTail       += count;
            return count;
        }

        ssize_t CharsetEncoder::fill(const LSPString *in, size_t first, size_t last)
        {
            if (bBuffer == NULL)
                return -STATUS_CLOSED;
            else if ((in == NULL) || (first > last) || (last > in->length()))
                return -STATUS_BAD_ARGUMENTS;

            // Is there a space in the buffer for reading?
            size_t bufsz = prepare_buffer();
            if (bufsz <= 0)
                return bufsz;

            size_t count = last - first;
            const lsp_wchar_t *buf = in->characters();
            if (count > bufsz)
                count   = bufsz;
            ::memcpy(&cBufTail, &buf[first], count * sizeof(lsp_wchar_t));
            bBufTail       += count;
            return count;
        }

        ssize_t CharsetEncoder::fill(IInSequence *in, size_t count = 0)
        {
            if (bBuffer == NULL)
                return -STATUS_CLOSED;
            else if (in == NULL)
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
                ssize_t nread   = in->read(cBufTail, count - read);
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


        ssize_t CharsetEncoder::fetch(void *buf, size_t count)
        {
            if (bBuffer == NULL)
                return -STATUS_CLOSED;
            else if (buf == NULL)
                return -STATUS_BAD_ARGUMENTS;

            // Compute the amount of data to read
            size_t processed = 0;
            if (!count)
                count   = DATA_BUFSIZE*2;

            // Perform read
            uint8_t *dst = reinterpret_cast<uint8_t *>(buf);

            while (processed < count)
            {
                // Perform decoding
                ssize_t nbytes  = encode_buffer();
                if (nbytes <= 0)
                {
                    if (processed > 0)
                        break;
                    return nbytes;
                }

                // Write data to output sequence
                ssize_t to_copy = count - processed;
                if (nbytes > to_copy)
                    nbytes          = to_copy;
                ::memcpy(dst, bBufHead, nbytes);

                // Update state
                bBufHead       += nbytes;
                processed      += nbytes;
                dst            += nbytes;
            }

            return processed;
        }

        ssize_t CharsetEncoder::fetch(File *out, size_t count = 0)
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
                ssize_t nbytes  = encode_buffer();
                if (nbytes <= 0)
                {
                    if (processed > 0)
                        break;
                    return nbytes;
                }

                // Write data to output sequence
                ssize_t to_copy = count - processed;
                if (nbytes > to_copy)
                    nbytes          = to_copy;
                nbytes = out->write(bBufHead, nbytes);
                if (nbytes < 0)
                {
                    if (processed > 0)
                        break;
                    return nbytes;
                }

                // Update state
                bBufHead       += nbytes;
                processed      += nbytes;
            }

            return processed;
        }

        ssize_t CharsetEncoder::fetch(IOutStream *out, size_t count = 0)
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
                ssize_t nbytes  = encode_buffer();
                if (nbytes <= 0)
                {
                    if (processed > 0)
                        break;
                    return nbytes;
                }

                // Write data to output sequence
                ssize_t to_copy = count - processed;
                if (nbytes > to_copy)
                    nbytes          = to_copy;
                nbytes = out->write(bBufHead, nbytes);
                if (nbytes < 0)
                {
                    if (processed > 0)
                        break;
                    return nbytes;
                }

                // Update state
                bBufHead       += nbytes;
                processed      += nbytes;
            }

            return processed;
        }

    } /* namespace io */
} /* namespace lsp */
