/*
 * FileReader.cpp
 *
 *  Created on: 16 июн. 2018 г.
 *      Author: sadko
 */

#include <errno.h>
#include <core/io/charset.h>
#include <core/io/FileReader.h>

#if 1
    #if defined(PLATFORM_WINDOWS)
        // Character buffer should have enough space to decode characters
        #define CBUF_SIZE        0x4000
        #define BBUF_SIZE        0x1000
    #else
        // We economy of I/O operations
        #define CBUF_SIZE        0x1000
        #define BBUF_SIZE        0x4000
    #endif /* PLATFORM_WINDOWS */
#else
    // Values for tests
    #if defined(PLATFORM_WINDOWS)
        #define BBUF_SIZE       16
        #define CBUF_SIZE       (BBUF_SIZE * 4)
    #else
        #define CBUF_SIZE       32
        #define BBUF_SIZE       (CBUF_SIZE * 4)
    #endif
#endif

namespace lsp
{
    namespace io
    {
        FileReader::FileReader()
        {
            bBuf        = NULL;
            cBuf        = NULL;
            bBufSize    = 0;
            bBufPos     = 0;
            cBufSize    = 0;
            cBufPos     = 0;
            pFD         = NULL;
            nError      = STATUS_OK;
            bClose      = false;
            #if defined(PLATFORM_WINDOWS)
                nCodePage   = UINT(-1);
            #else
                hIconv      = iconv_t(-1);
            #endif /* PLATFORM_WINDOWS */
        }

        FileReader::~FileReader()
        {
            do_destroy();
        }
    
        void FileReader::do_destroy()
        {
            if ((bClose) && (pFD != NULL))
            {
                fclose(pFD);
                pFD         = NULL;
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

        status_t FileReader::init_buffers()
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

        status_t FileReader::initialize(FILE *fd, const char *charset, bool close)
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
                hIconv      = init_iconv_to_wchar_t(charset);
                if (hIconv == iconv_t(-1))
                {
                    do_destroy();
                    return STATUS_BAD_LOCALE;
                }
            #endif /* PLATFORM_WINDOWS */

            pFD         = fd;
            bClose      = close;
            return STATUS_OK;
        }

        status_t FileReader::attach(FILE *fd, const char *charset)
        {
            do_destroy();
            return initialize(fd, charset, false);
        }

        status_t FileReader::open(FILE *fd, const char *charset)
        {
            do_destroy();
            return initialize(fd, charset, true);
        }

        status_t FileReader::open(const char *path, const char *charset)
        {
            LSPString tmp;
            if (!tmp.set_utf8(path))
                return STATUS_NO_MEM;
            return open(&tmp, charset);
        }

        status_t FileReader::open(const LSPString *path, const char *charset)
        {
            do_destroy();

            #if defined(PLATFORM_WINDOWS)
                FILE *fd        = fopen(path->get_utf8(), "rb");
            #else
                FILE *fd        = fopen(path->get_utf8(), "r");
            #endif /* PLATFORM_WINDOWS */
            if (fd == NULL)
                return nError = STATUS_IO_ERROR;

            status_t res = initialize(fd, charset, true);
            if (res != STATUS_OK)
                fclose(fd);
            return res;
        }

        status_t FileReader::open(const Path *path, const char *charset)
        {
            return open(path->as_string(), charset);
        }

#if defined(PLATFORM_WINDOWS)
        status_t FileReader::fill_char_buf()
        {
            // Move memory buffers
            if (bBufPos > 0)
            {
                bBufSize   -= bBufPos;
                if (bBufSize > 0)
                    ::memmove(bBuf, &bBuf[bBufPos], bBufSize);
                bBufPos     = 0;
            }
            if (cBufPos > 0)
            {
                cBufSize   -= cBufPos;
                if (cBufSize > 0)
                    ::memmove(cBuf, &cBuf[cBufPos], cBufSize * sizeof(lsp_wchar_t));
                cBufPos     = 0;
            }

            // Try to read new portion of data into buffer
            size_t nbytes   = fread(&bBuf[bBufSize], sizeof(uint8_t), BBUF_SIZE - bBufSize, pFD);
            if ((nbytes <= 0) && (bBufPos >= bBufSize))
                return nError = STATUS_EOF;
            else if (nbytes > 0)
                bBufSize       += nbytes;

            // Do the conversion
            CHAR *inbuf     = reinterpret_cast<CHAR *>(&bBuf[bBufPos]);
            WCHAR *outbuf   = reinterpret_cast<WCHAR *>(&cBuf[cBufSize]);

            ssize_t nchars  = MultiByteToWideChar(nCodePage, 0, inbuf, bBufSize-bBufPos, outbuf, CBUF_SIZE - cBufSize);
            if (nchars == 0)
            {
                switch (GetLastError())
                {
                    case ERROR_INSUFFICIENT_BUFFER:
                        return nError = STATUS_NO_MEM;
                    case ERROR_INVALID_FLAGS:
                    case ERROR_INVALID_PARAMETER:
                        return nError = STATUS_BAD_STATE;
                    case ERROR_NO_UNICODE_TRANSLATION:
                        return nError = STATUS_BAD_LOCALE;
                    default:
                        return nError = STATUS_UNKNOWN_ERR;
                }
            }

            // If function meets invalid sequence, it replaces the code point with such magic value
            // We should know if function has failed
            if (outbuf[nchars-1] == 0xfffd)
                --nchars;

            // Estimate number of bytes decoded (yep, this is dumb but no way...)
            nbytes = WideCharToMultiByte(nCodePage, 0, outbuf, nchars, NULL, 0, 0, 0);
            if ((nbytes <= 0) || (nbytes > (bBufSize - bBufPos)))
                return nError = STATUS_IO_ERROR;

            // Update state of buffers
            cBufSize       += nchars;
            bBufPos        += nbytes;

            return nError = STATUS_OK;
        }
#else
        status_t FileReader::fill_char_buf()
        {
            // If there is data at the tail of buffer, move it to beginning
            ssize_t left    = cBufSize - cBufPos;
            if (left > 0)
            {
                ::memmove(cBuf, &cBuf[cBufSize], left * sizeof(lsp_wchar_t));
                cBufSize        = left;
            }
            else
                cBufSize        = 0;
            cBufPos         = 0;

            // Try to additionally fill byte buffer with data
            left    = bBufSize - bBufPos;
            if (left <= (CBUF_SIZE/2))
            {
                // Ensure that there is data in byte buffer, move it to beginning
                if (left > 0)
                {
                    ::memmove(bBuf, &bBuf[bBufPos], left * sizeof(uint8_t));
                    bBufSize        = left;
                }
                else
                    bBufSize        = 0;
                bBufPos     = 0;

                // Try to additionally read data
                size_t nbytes       = fread(&bBuf[bBufSize], sizeof(uint8_t), BBUF_SIZE - bBufSize, pFD);
                if ((nbytes <= 0) && (left <= 0))
                    return nError = STATUS_EOF;
                else if (nbytes > 0)
                    bBufSize       += nbytes;

                left        = bBufSize - bBufPos;
            }

            // Do the conversion
            size_t c_left   = (CBUF_SIZE - cBufSize) * sizeof(lsp_wchar_t);
            size_t xb_left  = left;
            size_t xc_left  = c_left;

            char *inbuf     = reinterpret_cast<char *>(&bBuf[bBufPos]);
            char *outbuf    = reinterpret_cast<char *>(&cBuf[cBufSize]);
            size_t nconv    = iconv(hIconv, &inbuf, &xb_left, &outbuf, &xc_left);

            if (nconv == size_t(-1))
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

            // Update state of buffers
            cBufSize       += (c_left - xc_left) / sizeof(lsp_wchar_t);
            bBufPos        += (left - xb_left);

            return nError = (cBufSize > cBufPos) ? STATUS_OK : STATUS_EOF;
        }
#endif /* PLATFORM_WINDOWS */

        ssize_t FileReader::read(lsp_wchar_t *dst, size_t count)
        {
            if (pFD == NULL)
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
                        return (n_read > 0) ? n_read : -res;

                    // Ensure that there is data in character buffer
                    n_copy = cBufSize - cBufPos;
                    if (n_copy <= 0)
                        break;
                }

                // Check limits
                if (n_copy > ssize_t(count))
                    n_copy = count;

                // Copy data from character buffer and update pointers
                ::memcpy(dst, &cBuf[cBufPos], n_copy * sizeof(lsp_wchar_t));
                cBufPos    += n_copy;
                dst        += n_copy;
                n_read     += n_copy;
                count      -= n_copy;
            }

            return n_read;
        }

        int FileReader::read()
        {
            if (pFD == NULL)
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
                    return -res;

                // Ensure that there is data in character buffer
                if (cBufPos >= cBufSize)
                    return -1;
            }
            return cBuf[cBufPos++];
        }

        status_t FileReader::read_line(LSPString *s, bool force)
        {
            if (pFD == NULL)
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

        ssize_t FileReader::skip(size_t count)
        {
            sLine.clear();
            return Reader::skip(count);
        }

        status_t FileReader::error()
        {
            return nError;
        }

        status_t FileReader::close()
        {
            do_destroy();
            return STATUS_OK;
        }
    }
} /* namespace lsp */
