/*
 * FileWriter.cpp
 *
 *  Created on: 18 июн. 2018 г.
 *      Author: sadko
 */

#include <errno.h>
#include <core/io/charset.h>
#include <core/io/FileWriter.h>

#if 1
    #define CBUF_SIZE        0x1000
    #define BBUF_SIZE        0x4000
#else
    #define CBUF_SIZE       32
    #define BBUF_SIZE       (CBUF_SIZE * 4)
#endif

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
            #if defined(PLATFORM_WINDOWS)
                nCodePage   = UINT(-1);
            #else
                hIconv      = iconv_t(-1);
            #endif
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
            #if defined(PLATFORM_WINDOWS)
                nCodePage   = UINT(-1);
            #else
                if (hIconv != iconv_t(-1))
                {
                    iconv_close(hIconv);
                    hIconv      = iconv_t(-1);
                }
            #endif /* PLATFORM_WINDOWS */
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
            #endif /* PLATFORM_WINDOWS */

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

            #if defined(PLATFORM_WINDOWS)
                FILE *fd        = fopen(path, "wb");
            #else
                FILE *fd        = fopen(path, "w");
            #endif /* PLATFORM_WINDOWS */
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

#if defined(PLATFORM_WINDOWS)
        status_t FileWriter::flush_buffer(bool force)
        {
            status_t res = flush_byte_buffer();
            if (res != STATUS_OK)
                return res;
            if (cBufPos <= 0)
                return STATUS_OK;

            WCHAR *inbuf    = reinterpret_cast<WCHAR *>(cBuf);
            CHAR *outbuf    = reinterpret_cast<CHAR *>(bBuf);
            size_t bytes    = WideCharToMultiByte(nCodePage, 0, inbuf, cBufPos, outbuf, BBUF_SIZE-bBufPos, 0, FALSE);

            if (bytes == 0)
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

            bBufPos        += bytes;
            cBufPos         = 0;

            return (force) ? flush_byte_buffer() : STATUS_OK;
        }
#else
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

            return (force) ? flush_byte_buffer() : STATUS_OK;
        }
#endif /* PLATFORM_WINDOWS */

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

                ::memcpy(&cBuf[cBufPos], c, avail * sizeof(lsp_wchar_t));
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
