/*
 * FileReader.cpp
 *
 *  Created on: 16 июн. 2018 г.
 *      Author: sadko
 */

#include <errno.h>
#include <core/io/charset.h>
#include <core/io/FileReader.h>

#define CBUF_SIZE        0x1000
#define BBUF_SIZE        0x4000

// Values for tests
//#define CBUF_SIZE       64
//#define BBUF_SIZE       4 * CBUF_SIZE

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
            hIconv      = iconv_t(-1);
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
            if (hIconv != iconv_t(-1))
            {
                iconv_close(hIconv);
                hIconv      = iconv_t(-1);
            }
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

            hIconv      = init_iconv_to_wchar_t(charset);
            if (hIconv == iconv_t(-1))
            {
                do_destroy();
                return STATUS_BAD_LOCALE;
            }

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
            do_destroy();

            FILE *fd        = fopen(path, "r");
            if (fd == NULL)
                return nError = STATUS_IO_ERROR;

            status_t res = initialize(fd, charset, true);
            if (res != STATUS_OK)
                fclose(fd);
            return res;
        }

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

            // Read until buffer is fully filled with data
            while (cBufSize < CBUF_SIZE)
            {
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
                    bBufSize   += fread(&bBuf[bBufSize], sizeof(uint8_t), BBUF_SIZE - bBufSize, pFD);
                    left        = bBufSize - bBufPos;
                }

                // Prepare to byte-to-character conversion
                if (left <= 0)
                    break;
                size_t c_left   = (CBUF_SIZE - cBufSize) * sizeof(lsp_wchar_t);

                // Do the conversion
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
            }

            return nError = STATUS_OK;
        }

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
                    return res;

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
