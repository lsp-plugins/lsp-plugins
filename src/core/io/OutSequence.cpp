/*
 * FileWriter.cpp
 *
 *  Created on: 18 июн. 2018 г.
 *      Author: sadko
 */

#include <errno.h>
#include <core/io/charset.h>
#include <core/io/StdioFile.h>
#include <core/io/NativeFile.h>
#include <core/io/OutFileStream.h>
#include <core/io/OutSequence.h>

#define CBUF_SIZE        0x1000
#define BBUF_SIZE        0x4000

namespace lsp
{
    namespace io
    {
        OutSequence::OutSequence()
        {
            bBuf        = NULL;
            cBuf        = NULL;
            bBufPos     = 0;
            cBufPos     = 0;
            pOS         = NULL;
            nWrapFlags  = 0;
        }

        OutSequence::~OutSequence()
        {
            // Close file descriptor
            if (pOS != NULL)
            {
                flush_buffer(true);


                if (nWrapFlags & WRAP_CLOSE)
                    pOS->close();
                if (nWrapFlags & WRAP_DELETE)
                    delete pOS;
                pOS         = NULL;
            }
            nWrapFlags  = 0;

            // Drop buffer data
            if (bBuf != NULL)
            {
                ::free(bBuf);
                bBuf        = NULL;
                cBuf        = NULL;
            }

            // Close encoder
            sEncoder.close();
        }

        status_t OutSequence::close()
        {
            status_t res = STATUS_OK, tres;

            // Close file descriptor
            if (pOS != NULL)
            {
                // Flush buffers
                res = flush();

                // Perform close
                if (nWrapFlags & WRAP_CLOSE)
                {
                    tres = pOS->close();
                    if (res == STATUS_OK)
                        res = tres;
                }
                if (nWrapFlags & WRAP_DELETE)
                    delete pOS;
                pOS         = NULL;
            }
            nWrapFlags  = 0;

            // Drop buffer data
            if (bBuf != NULL)
            {
                ::free(bBuf);
                bBuf        = NULL;
                cBuf        = NULL;
            }

            // Close encoder
            sEncoder.close();

            // Return result
            return set_error(res);
        }
    
        status_t OutSequence::wrap(FILE *fd, bool close, const char *charset)
        {
            if (pOS != NULL)
                return set_error(STATUS_BAD_STATE);
            else if (fd == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

            OutFileStream *f = new OutFileStream();
            if (f == NULL)
                return set_error(STATUS_NO_MEM);
            status_t res = f->wrap(fd, close, charset);
            if (res != STATUS_OK)
            {
                f->close();
                delete f;
                return set_error(res);
            }

            res = wrap(f, WRAP_CLOSE | WRAP_DELETE, charset);
            if (res != STATUS_OK)
            {
                f->close();
                delete f;
            }

            return STATUS_OK;
        }

        status_t OutSequence::wrap(lsp_fhandle_t fd, bool close, const char *charset)
        {
            if (pOS != NULL)
                return set_error(STATUS_BAD_STATE);

            OutFileStream *f = new OutFileStream();
            if (f == NULL)
                return set_error(STATUS_NO_MEM);
            status_t res = f->wrap(fd, close, charset);
            if (res != STATUS_OK)
            {
                f->close();
                delete f;
                return set_error(res);
            }

            res = wrap(f, WRAP_CLOSE | WRAP_DELETE, charset);
            if (res != STATUS_OK)
            {
                f->close();
                delete f;
            }

            return STATUS_OK;
        }

        status_t OutSequence::wrap(File *fd, size_t flags, const char *charset)
        {
            if (pOS != NULL)
                return set_error(STATUS_BAD_STATE);
            else if (fd == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

            OutFileStream *f = new OutFileStream();
            if (f == NULL)
                return set_error(STATUS_NO_MEM);
            status_t res = f->wrap(fd, flags, charset);
            if (res != STATUS_OK)
            {
                f->close();
                delete f;
                return set_error(res);
            }

            res = wrap(f, WRAP_CLOSE | WRAP_DELETE, charset);
            if (res != STATUS_OK)
            {
                f->close();
                delete f;
            }

            return STATUS_OK;
        }

        status_t OutSequence::wrap(IOutStream *os, size_t flags, const char *charset)
        {
            if (pOS != NULL)
                return set_error(STATUS_BAD_STATE);
            else if (os == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

            // Allocate buffers
            if (bBuf == NULL)
            {
                uint8_t *ptr    = reinterpret_cast<uint8_t *>(::malloc(
                                CBUF_SIZE * sizeof(lsp_wchar_t) +
                                BBUF_SIZE * sizeof(uint8_t)
                        ));
                if (ptr == NULL)
                    return set_error(STATUS_NO_MEM);
                bBuf            = ptr;
                cBuf            = reinterpret_cast<lsp_wchar_t *>(&bBuf[BBUF_SIZE * sizeof(uint8_t)]);
            }

            bBufPos     = 0;
            cBufPos     = 0;

            // Initialize decoder
            status_t res = sEncoder.init(charset);
            if (res != STATUS_OK)
            {
                sEncoder.close();
                ::free(bBuf);
                bBuf        = NULL;
                cBuf        = NULL;
                return set_error(res);
            }

            // Store pointers
            pOS         = os;
            nWrapFlags  = flags;

            return STATUS_OK;
        }

        status_t OutSequence::open(const char *path, size_t mode, const char *charset)
        {
            if (pOS != NULL)
                return set_error(STATUS_BAD_STATE);
            else if (path == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

            LSPString tmp;
            if (!tmp.set_utf8(path))
                return set_error(STATUS_NO_MEM);
            return open(&tmp, mode, charset);
        }

        status_t OutSequence::open(const LSPString *path, size_t mode, const char *charset)
        {
            if (pOS != NULL)
                return set_error(STATUS_BAD_STATE);
            else if (path == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

            OutFileStream *f = new OutFileStream();
            if (f == NULL)
                return set_error(STATUS_NO_MEM);
            status_t res = f->open(path, mode, charset);
            if (res != STATUS_OK)
            {
                f->close();
                delete f;
                return set_error(res);
            }

            res = wrap(f, WRAP_CLOSE | WRAP_DELETE, charset);
            if (res != STATUS_OK)
            {
                f->close();
                delete f;
                return set_error(res);
            }

            return set_error(STATUS_OK);
        }

        status_t OutSequence::open(const Path *path, size_t mode, const char *charset)
        {
            return open(path->as_string(), mode, charset);
        }

        status_t OutSequence::flush_byte_buffer()
        {
            if (bBufPos <= 0)
                return STATUS_OK;

            for (size_t pos=0; pos < bBufPos; )
            {
                size_t k = bBufPos - pos;
                size_t n = pOS->write(&bBuf[pos], k);
                pos     += n;

                if (n < k)
                    return set_error(STATUS_EOF);
            }

            // Reset byte buffer size
            bBufPos     = 0;

            return STATUS_OK;
        }

        status_t OutSequence::flush_buffer(bool force)
        {
            for (size_t pos=0; pos < cBufPos; )
            {
                if (bBufPos >= (BBUF_SIZE/2))
                {
                    status_t res = flush_byte_buffer();
                    if (res != STATUS_OK)
                        return set_error(res);
                }

                // Do the conversion
                size_t xc_left  = cBufPos - pos;
                size_t xb_left  = BBUF_SIZE - bBufPos;

                lsp_wchar_t *inbuf  = &cBuf[pos];
                void *outbuf        = &bBuf[bBufPos];
                ssize_t nconv       = sEncoder.encode(&outbuf, &xb_left, &inbuf, &xc_left);

                if (nconv < 0)
                    return set_error(-nconv);

                // Update pointers
                bBufPos         = BBUF_SIZE - xb_left;
                pos             = cBufPos - xc_left;
            }

            // Reset character buffer size
            cBufPos     = 0;

            return (force) ? flush_byte_buffer() : STATUS_OK;
        }

        status_t OutSequence::write(lsp_wchar_t c)
        {
            if (pOS == NULL)
                return set_error(STATUS_CLOSED);

            if (cBufPos >= CBUF_SIZE)
            {
                status_t res = flush_buffer(false);
                if (res != STATUS_OK)
                    return res;
            }

            cBuf[cBufPos++] = c;
            return STATUS_OK;
        }

        status_t OutSequence::write(const lsp_wchar_t *c, size_t count)
        {
            if (pOS == NULL)
                return set_error(STATUS_CLOSED);

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

        status_t OutSequence::write_ascii(const char *s)
        {
            if (pOS == NULL)
                return set_error(STATUS_CLOSED);
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
                    cBuf[cBufPos++] = uint8_t(*(s++));
            }

            return STATUS_OK;
        }

        status_t OutSequence::write(const LSPString *s)
        {
            return IOutSequence::write(s);
        }

        status_t OutSequence::write(const LSPString *s, ssize_t first)
        {
            return IOutSequence::write(s, first);
        }

        status_t OutSequence::write(const LSPString *s, ssize_t first, ssize_t last)
        {
            return IOutSequence::write(s, first, last);
        }

        status_t OutSequence::flush()
        {
            if (pOS == NULL)
                return set_error(STATUS_CLOSED);

            // First, flush byte buffer
            status_t res = flush_buffer(true);

            // Seconds, flush underlying storage on success
            if (res == STATUS_OK)
                res     = pOS->flush();

            return set_error(res);
        }
    }

} /* namespace lsp */
