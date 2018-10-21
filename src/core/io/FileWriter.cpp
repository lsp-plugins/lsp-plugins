/*
 * FileWriter.cpp
 *
 *  Created on: 18 июн. 2018 г.
 *      Author: sadko
 */

#include <errno.h>
#include <core/io/charset.h>
#include <core/io/FileWriter.h>

#define CBUF_SIZE        0x1000
#define BBUF_SIZE        0x4000
// Values for tests
//#define CBUF_SIZE       64
//#define BBUF_SIZE       4 * CBUF_SIZE

namespace lsp
{
    namespace io
    {
        FileWriter::FileWriter()
        {
            bBuf        = NULL;
            cBuf        = NULL;
            bBufPos     = 0;
            cBufPos     = 0;
            pFD         = NULL;
            bClose      = false;
            hIconv      = iconv_t(-1);
        }

        FileWriter::~FileWriter()
        {
            do_destroy();
        }

        void FileWriter::do_destroy()
        {
            if ((bClose) && (pFD != NULL))
            {
                fclose(pFD);
                pFD         = NULL;
            }
            if (bBuf != NULL)
            {
                free(bBuf);
                bBuf        = NULL;
            }
            if (hIconv != iconv_t(-1))
            {
                iconv_close(hIconv);
                hIconv      = iconv_t(-1);
            }
            cBuf        = NULL;
            bClose      = false;
        }
    
        status_t FileWriter::init_buffers()
        {
            if (bBuf == NULL)
            {
                uint8_t *ptr    = reinterpret_cast<uint8_t *>(malloc(
                            BBUF_SIZE * sizeof(uint8_t) +
                            CBUF_SIZE * sizeof(lsp_wchar_t)
                        ));
                if (ptr == NULL)
                    return STATUS_NO_MEM;
                bBuf            = ptr;
                cBuf            = reinterpret_cast<lsp_wchar_t *>(&bBuf[BBUF_SIZE * sizeof(uint8_t)]);
            }

            bBufPos     = 0;
            cBufPos     = 0;

            return STATUS_OK;
        }
    
        status_t FileWriter::initialize(FILE *fd, const char *charset, bool close)
        {
            status_t res= init_buffers();
            if (res != STATUS_OK)
            {
                do_destroy();
                return res;
            }

            hIconv      = init_iconv_from_wchar_t(charset);
            if (hIconv == iconv_t(-1))
            {
                do_destroy();
                return STATUS_BAD_LOCALE;
            }

            pFD         = fd;
            bClose      = close;
            return STATUS_OK;
        }

        status_t FileWriter::attach(FILE *fd, const char *charset)
        {
            do_destroy();
            return initialize(fd, charset, false);
        }

        status_t FileWriter::open(FILE *fd, const char *charset)
        {
            do_destroy();
            return initialize(fd, charset, true);
        }

        status_t FileWriter::open(const char *path, const char *charset)
        {
            do_destroy();

            FILE *fd        = fopen(path, "w");
            if (fd == NULL)
                return STATUS_IO_ERROR;

            status_t res = initialize(fd, charset, true);
            if (res != STATUS_OK)
                fclose(fd);
            return res;
        }

        status_t FileWriter::append(const char *path, const char *charset)
        {
            do_destroy();

            FILE *fd        = fopen(path, "a");
            if (fd == NULL)
                return STATUS_IO_ERROR;

            status_t res = initialize(fd, charset, true);
            if (res != STATUS_OK)
                fclose(fd);
            return res;
        }

        status_t FileWriter::flush_byte_buffer()
        {
            if (bBufPos <= 0)
                return STATUS_OK;

            for (size_t pos=0; pos < bBufPos; )
            {
                size_t k = bBufPos - pos;
                size_t n = fwrite(&bBuf[pos], sizeof(uint8_t), k, pFD);
                pos     += n;

                if (n < k)
                {
                    if (feof(pFD))
                        return STATUS_EOF;
                }
            }

            // Flush underlying device
            fflush(pFD);

            // Reset byte buffer size
            bBufPos     = 0;

            return STATUS_OK;
        }

        status_t FileWriter::flush_buffer(bool force)
        {
            for (size_t pos=0; pos < cBufPos; )
            {
                if (bBufPos >= (BBUF_SIZE/2))
                {
                    status_t res = flush_byte_buffer();
                    if (res != STATUS_OK)
                        return res;
                }

                // Do the conversion
                size_t xc_left  = (cBufPos - pos) * sizeof(lsp_wchar_t);
                size_t xb_left  = BBUF_SIZE - bBufPos;
                char *inbuf     = reinterpret_cast<char *>(&cBuf[pos]);
                char *outbuf    = reinterpret_cast<char *>(&bBuf[bBufPos]);
                size_t nconv    = iconv(hIconv, &inbuf, &xc_left, &outbuf, &xb_left);

                if (nconv == size_t(-1))
                {
                    int code = errno;
                    switch (code)
                    {
                        case E2BIG:
                        case EINVAL:
                            break;
                        default:
                            return STATUS_BAD_FORMAT;
                    }
                }

                // Update pointers
                bBufPos         = BBUF_SIZE - xb_left;
                pos             = cBufPos - xc_left/sizeof(lsp_wchar_t);
            }

            // Reset character buffer size
            cBufPos     = 0;

            return ((force) && (bBufPos > 0)) ? flush_byte_buffer() : STATUS_OK;
        }

        status_t FileWriter::write(lsp_wchar_t c)
        {
            if (pFD == NULL)
                return STATUS_CLOSED;

            if (cBufPos >= CBUF_SIZE)
            {
                status_t res = flush_buffer(false);
                if (res != STATUS_OK)
                    return res;
            }

            cBuf[cBufPos++] = c;
            return STATUS_OK;
        }

        status_t FileWriter::write(const lsp_wchar_t *c, size_t count)
        {
            if (pFD == NULL)
                return STATUS_CLOSED;

            while (count > 0)
            {
                size_t avail = CBUF_SIZE - cBufPos;
                if (avail <= 0)
                {
                    status_t res = flush_buffer(false);
                    if (res != STATUS_OK)
                        return res;
                    avail = CBUF_SIZE;
                }

                if (avail > count)
                    avail = count;

                memcpy(&cBuf[cBufPos], c, avail * sizeof(lsp_wchar_t));
                cBufPos += avail;
                c       += avail;
                count   -= avail;
            }

            return STATUS_OK;
        }

        status_t FileWriter::write_ascii(const char *s)
        {
            if (pFD == NULL)
                return STATUS_CLOSED;
            size_t count = strlen(s);

            while (count > 0)
            {
                size_t avail = CBUF_SIZE - cBufPos;
                if (avail <= 0)
                {
                    status_t res = flush_buffer(false);
                    if (res != STATUS_OK)
                        return res;
                    avail = CBUF_SIZE;
                }

                if (avail > count)
                    avail = count;

                count   -= avail;
                while (avail--)
                    cBuf[cBufPos++] = *(s++);
            }

            return STATUS_OK;
        }

        status_t FileWriter::write(const LSPString *s)
        {
            return Writer::write(s);
        }

        status_t FileWriter::write(const LSPString *s, ssize_t first)
        {
            return Writer::write(s, first);
        }

        status_t FileWriter::write(const LSPString *s, ssize_t first, ssize_t last)
        {
            return Writer::write(s, first, last);
        }

        status_t FileWriter::flush()
        {
            return flush_buffer(true);
        }

        status_t FileWriter::close()
        {
            status_t res = flush_buffer(true);
            do_destroy();
            return res;
        }
    }

} /* namespace lsp */
