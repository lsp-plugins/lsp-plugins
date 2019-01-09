/*
 * InputStreamReader.cpp
 *
 *  Created on: 26 июн. 2018 г.
 *      Author: sadko
 */

#include <core/io/InputStreamReader.h>
#include <core/io/charset.h>
#include <errno.h>

#define CBUF_SIZE        0x1000
#define BBUF_SIZE        0x4000

namespace lsp
{
    namespace io
    {
        InputStreamReader::InputStreamReader()
        {
            bBuf        = NULL;
            cBuf        = NULL;
            bBufSize    = 0;
            bBufPos     = 0;
            cBufSize    = 0;
            cBufPos     = 0;
            pIS         = NULL;
            nError      = STATUS_OK;
            bClose      = false;
            #if defined(PLATFORM_WINDOWS)
                nCodePage   = UINT(-1);
            #else
                hIconv      = iconv_t(-1);
            #endif /* PLATFORM_WINDOWS */
        }

        InputStreamReader::~InputStreamReader()
        {
            do_destroy();
        }

        void InputStreamReader::do_destroy()
        {
            if ((bClose) && (pIS != NULL))
            {
                pIS->close();
                pIS         = NULL;
            }
            if (bBuf != NULL)
                free(bBuf);
            #if defined(PLATFORM_WINDOWS)
                nCodePage   = UINT(-1);
            #else
                if (hIconv != iconv_t(-1))
                {
                    iconv_close(hIconv);
                    hIconv      = iconv_t(-1);
                }
            #endif /* PLATFORM_WINDOWS */
            bBuf        = NULL;
            cBuf        = NULL;
            bClose      = false;
        }

        status_t InputStreamReader::init_buffers()
        {
            if (bBuf == NULL)
            {
                uint8_t *ptr    = reinterpret_cast<uint8_t *>(malloc(
                                CBUF_SIZE * sizeof(lsp_wchar_t) +
                                BBUF_SIZE * sizeof(uint8_t)
                        ));
                if (ptr == NULL)
                    return nError = STATUS_NO_MEM;
                bBuf            = ptr;
                cBuf            = reinterpret_cast<lsp_wchar_t *>(&bBuf[BBUF_SIZE * sizeof(uint8_t)]);
            }

            bBufSize    = 0;
            bBufPos     = 0;
            cBufSize    = 0;
            cBufPos     = 0;

            return nError   = STATUS_OK;
        }

        status_t InputStreamReader::initialize(IInputStream *is, const char *charset, bool close)
        {
            status_t res= init_buffers();
            if (res != STATUS_OK)
            {
                do_destroy();
                return res;
            }

            #if defined(PLATFORM_WINDOWS)
                ssize_t cp  = codepage_from_name(charset);
                if (cp < 0)
                {
                    do_destroy();
                    return STATUS_BAD_LOCALE;
                }
                nCodePage   = cp;
            #else
                hIconv      = init_iconv_from_wchar_t(charset);
                if (hIconv == iconv_t(-1))
                {
                    do_destroy();
                    return STATUS_BAD_LOCALE;
                }
            #endif

            pIS         = is;
            bClose      = close;
            return STATUS_OK;
        }

        status_t InputStreamReader::attach(IInputStream *is, const char *charset)
        {
            do_destroy();
            return initialize(is, charset, false);
        }

        status_t InputStreamReader::open(IInputStream *is, const char *charset)
        {
            do_destroy();
            return initialize(is, charset, true);
        }

        status_t InputStreamReader::fill_char_buf()
        {
            // If there is data at the tail of buffer, move it to beginning
            ssize_t left    = cBufSize - cBufPos;
            if (left > 0)
            {
                memmove(cBuf, &cBuf[cBufSize], left * sizeof(lsp_wchar_t));
                cBufSize        = left;
            }
            else
                cBufSize        = 0;
            cBufPos         = 0;

            // Read until buffer is fully filled with data
            while (cBufSize < CBUF_SIZE)
            {
                left    = bBufSize - bBufPos;
                if (left <= (CBUF_SIZE/2))
                {
                    // Ensure that there is data in byte buffer, move it to beginning
                    if (left > 0)
                    {
                        memmove(bBuf, &bBuf[bBufPos], left * sizeof(uint8_t));
                        bBufSize        = left;
                    }
                    else
                        bBufSize        = 0;
                    bBufPos     = 0;

                    // Try to additionally read data
                    ssize_t res = pIS->read(&bBuf[bBufSize], BBUF_SIZE - bBufSize);
                    if (res < 0)
                        return nError = - res;

                    bBufSize   += res;
                    left        = bBufSize - bBufPos;
                }

                // Prepare to byte-to-character conversion
                if (left <= 0)
                    break;
                size_t c_left   = (CBUF_SIZE - cBufSize) * sizeof(lsp_wchar_t);

                // Do the conversion
                size_t xb_left  = left;
                size_t xc_left  = c_left;

                #if defined(PLATFORM_WINDOWS)
                    CHAR *inbuf     = reinterpret_cast<CHAR *>(&bBuf[bBufPos]);
                    WCHAR *outbuf   = reinterpret_cast<WCHAR *>(&cBuf[cBufSize]);

                    ssize_t nchars  = MultiByteToWideChar(nCodePage, 0, inbuf, xb_left, outbuf, c_left);
                    if (nchars == 0)
                    {
                        switch (GetLastError())
                        {
                            case ERROR_INSUFFICIENT_BUFFER:
                                return STATUS_NO_MEM;
                            case ERROR_INVALID_FLAGS:
                            case ERROR_INVALID_PARAMETER:
                                return STATUS_BAD_STATE;
                            case ERROR_NO_UNICODE_TRANSLATION:
                                return STATUS_BAD_LOCALE;
                            default:
                                return STATUS_UNKNOWN_ERR;
                        }
                    }

                    // If function meets invalid sequence, it replaces the codepoint with such magic value
                    // We should know if function has failed
                    if (outbuf[nchars-1] == 0xfffd)
                        --nchars;

                    // Estimate number of bytes decoded (yep, this is dumb but no way...)
                    ssize_t nbytes = WideCharToMultiByte(nCodePage, 0, outbuf, nchars, NULL, 0, 0, 0);
                    if ((nbytes <= 0) || (nbytes > ssize_t(xb_left)))
                        return STATUS_IO_ERROR;

                    xc_left        -= nchars;
                    xb_left        -= nbytes;
                #else
                    char *inbuf     = reinterpret_cast<char *>(&bBuf[bBufPos]);
                    char *outbuf    = reinterpret_cast<char *>(&cBuf[cBufSize]);
                    size_t nchars    = iconv(hIconv, &inbuf, &xb_left, &outbuf, &xc_left);

                    if (nchars == size_t(-1))
                    {
                        int code = errno;
                        switch (code)
                        {
                            case E2BIG:
                            case EINVAL:
                                break;
                            default:
                                return nError = STATUS_BAD_FORMAT;
                        }
                    }
                #endif /* PLATFORM_WINDOWS */

                // Update state of buffers
                cBufSize       += (c_left - xc_left) / sizeof(lsp_wchar_t);
                bBufPos        += (left - xb_left);
            }

            return nError = STATUS_OK;
        }

        ssize_t InputStreamReader::read(lsp_wchar_t *dst, size_t count)
        {
            if (pIS == NULL)
            {
                nError = STATUS_CLOSED;
                return -1;
            }

            // Clear line buffer
            sLine.clear();

            size_t n_read = 0;
            while (count > 0)
            {
                ssize_t n_copy = cBufSize - cBufPos;

                // Ensure that there is data in character buffer
                if (n_copy <= 0)
                {
                    // Try to fill character buffer
                    status_t res = fill_char_buf();
                    if (res != STATUS_OK)
                        return res;

                    // Ensure that there is data in character buffer
                    n_copy = cBufSize - cBufPos;
                    if (n_copy <= 0)
                        break;
                }

                // Check limits
                if (n_copy > ssize_t(count))
                    n_copy = count;

                // Copy data from character buffer and update pointers
                memcpy(dst, &cBuf[cBufPos], n_copy * sizeof(lsp_wchar_t));
                cBufPos    += n_copy;
                dst        += n_copy;
                n_read     += n_copy;
                count      -= n_copy;
            }

            return n_read;
        }

        int InputStreamReader::read()
        {
            if (pIS == NULL)
            {
                nError = STATUS_CLOSED;
                return -1;
            }

            // Clear line buffer
            sLine.clear();

            // Ensure that there is data in character buffer
            if (cBufPos >= cBufSize)
            {
                // Try to fill character buffer
                status_t res = fill_char_buf();
                if (res != STATUS_OK)
                    return res;

                // Ensure that there is data in character buffer
                if (cBufPos >= cBufSize)
                    return -1;
            }
            return cBuf[cBufPos++];
        }

        status_t InputStreamReader::read_line(LSPString *s, bool force)
        {
            if (pIS == NULL)
                return nError = STATUS_CLOSED;

            while (true)
            {
                ssize_t n_copy = cBufSize - cBufPos;

                // Ensure that there is data in character buffer
                if (n_copy <= 0)
                {
                    // Try to fill character buffer
                    status_t res = fill_char_buf();
                    if (res != STATUS_OK)
                        return res;

                    // Ensure that there is data in character buffer
                    n_copy = cBufSize - cBufPos;
                    if (n_copy <= 0)
                        break;
                }

                // Scan for end-of-line
                size_t last = cBufPos;
                while (last < cBufSize)
                {
                    if (cBuf[last] == '\n')
                        break;
                    last ++;
                }

                // Analyze scan results
                if (last >= cBufSize)
                {
                    // Not found line termination, just append line
                    if (last > cBufPos)
                    {
                        if (!sLine.append(&cBuf[cBufPos], cBufSize - cBufPos))
                            return nError = STATUS_NO_MEM;
                    }

                    // Clear buffer
                    cBufSize    = 0;
                    cBufPos     = 0;
                }
                else
                {
                    // Found split character, emit string
                    size_t end  = last + 1;
                    if (last > cBufPos)
                    {
                        if (cBuf[last] == '\r')
                            last--;
                    }

                    // Append line with characters
                    if (last > cBufPos)
                    {
                        if (!sLine.append(&cBuf[cBufPos], last - cBufPos))
                            return nError = STATUS_NO_MEM;
                    }

                    // Update buffer state
                    cBufPos     = end;

                    // Break the loop because line was completed
                    s->take(&sLine);
                    return nError = STATUS_OK;
                }
            }

            // Check force flag
            if ((force) && (sLine.length() > 0))
            {
                s->take(&sLine);
                return nError = STATUS_OK;
            }

            nError = STATUS_OK;
            return STATUS_EOF;
        }

        ssize_t InputStreamReader::skip(size_t count)
        {
            sLine.clear();
            return Reader::skip(count);
        }

        status_t InputStreamReader::error()
        {
            return nError;
        }

        status_t InputStreamReader::close()
        {
            do_destroy();
            return STATUS_OK;
        }
    
    } /* namespace io */
} /* namespace lsp */
