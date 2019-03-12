/*
 * FileReader.cpp
 *
 *  Created on: 16 июн. 2018 г.
 *      Author: sadko
 */

#include <errno.h>
#include <core/io/charset.h>
#include <core/io/StdioFile.h>
#include <core/io/NativeFile.h>
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
            nWrapFlags  = 0;
        }

        FileReader::~FileReader()
        {
            // Close file descriptor
            if (pFD != NULL)
            {
                if (nWrapFlags & WRAP_CLOSE)
                    pFD->close();
                if (nWrapFlags & WRAP_DELETE)
                    delete pFD;
                pFD         = NULL;
            }
            nWrapFlags  = 0;

            // Drop buffer data
            if (bBuf != NULL)
            {
                ::free(bBuf);
                bBuf        = NULL;
                cBuf        = NULL;
            }

            // Close decoder
            sDecoder.close();
        }

        status_t FileReader::close()
        {
            status_t res = STATUS_OK;

            // Close file descriptor
            if (pFD != NULL)
            {
                if (nWrapFlags & WRAP_CLOSE)
                    res = pFD->close();
                if (nWrapFlags & WRAP_DELETE)
                    delete pFD;
                pFD         = NULL;
            }
            nWrapFlags  = 0;

            // Drop buffer data
            if (bBuf != NULL)
            {
                ::free(bBuf);
                bBuf        = NULL;
                cBuf        = NULL;
            }

            // Close decoder
            sDecoder.close();

            // Return result
            return set_error(res);
        }
    
        status_t FileReader::wrap(FILE *fd, bool close, const char *charset)
        {
            if (pFD != NULL)
                return set_error(STATUS_BAD_STATE);
            else if (fd == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

            StdioFile *f = new StdioFile();
            if (f == NULL)
                return set_error(STATUS_NO_MEM);
            status_t res = f->wrap(fd, File::FM_READ, close);
            if (res != STATUS_OK)
            {
                f->close();
                delete f;
                return set_error(res);
            }

            return wrap(f, WRAP_DELETE, charset);
        }

        status_t FileReader::wrap(lsp_fhandle_t fd, bool close, const char *charset)
        {
            if (pFD != NULL)
                return set_error(STATUS_BAD_STATE);

            NativeFile *f = new NativeFile();
            if (f == NULL)
                return set_error(STATUS_NO_MEM);
            status_t res = f->wrap(fd, File::FM_READ, close);
            if (res != STATUS_OK)
            {
                f->close();
                delete f;
                return set_error(res);
            }

            return wrap(f, WRAP_DELETE, charset);
        }

        status_t FileReader::wrap(File *fd, size_t flags, const char *charset)
        {
            if (pFD != NULL)
                return set_error(STATUS_BAD_STATE);
            else if (fd == NULL)
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

            bBufSize    = 0;
            bBufPos     = 0;
            cBufSize    = 0;
            cBufPos     = 0;

            // Initialize decoder
            status_t res = sDecoder.init(charset);
            if (res != STATUS_OK)
            {
                sDecoder.close();
                ::free(bBuf);
                bBuf        = NULL;
                cBuf        = NULL;
                return set_error(res);
            }

            // Store pointers
            pFD         = fd;
            nWrapFlags  = flags;

            return STATUS_OK;
        }

        status_t FileReader::open(const char *path, const char *charset)
        {
            if (pFD != NULL)
                return set_error(STATUS_BAD_STATE);
            else if (path == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

            LSPString tmp;
            if (!tmp.set_utf8(path))
                return set_error(STATUS_NO_MEM);
            return open(&tmp, charset);
        }

        status_t FileReader::open(const LSPString *path, const char *charset)
        {
            if (pFD != NULL)
                return set_error(STATUS_BAD_STATE);
            else if (path == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

            NativeFile *f = new NativeFile();
            if (f == NULL)
                return set_error(STATUS_NO_MEM);

            status_t res = f->open(path, File::FM_READ);
            if (res != STATUS_OK)
            {
                f->close();
                return set_error(res);
            }

            return wrap(f, WRAP_CLOSE | WRAP_DELETE, charset);
        }

        status_t FileReader::open(const Path *path, const char *charset)
        {
            return open(path->as_string(), charset);
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
                ssize_t nbytes      = pFD->read(&bBuf[bBufSize], BBUF_SIZE - bBufSize);
                if ((nbytes <= 0) && (left <= 0))
                    return set_error((nbytes < 0) ? -nbytes : STATUS_EOF);
                else if (nbytes > 0)
                    bBufSize       += nbytes;

                left        = bBufSize - bBufPos;
            }

            // Do the conversion
            size_t c_left   = (CBUF_SIZE - cBufSize) * sizeof(lsp_wchar_t);
            size_t xb_left  = left;
            size_t xc_left  = c_left;

            void *inbuf         = &bBuf[bBufPos];
            lsp_wchar_t *outbuf = &cBuf[cBufSize];
            ssize_t nconv       = sDecoder.decode(&outbuf, &xc_left, &inbuf, &xb_left);
            if (nconv < 0)
                return set_error(-nconv);

            // Update state of buffers
            cBufSize       += (c_left - xc_left) / sizeof(lsp_wchar_t);
            bBufPos        += (left - xb_left);

            return set_error((cBufSize > cBufPos) ? STATUS_OK : STATUS_EOF);
        }

        ssize_t FileReader::read(lsp_wchar_t *dst, size_t count)
        {
            if (pFD == NULL)
                return -set_error(STATUS_CLOSED);

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
                return -set_error(STATUS_CLOSED);

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
                return set_error(STATUS_CLOSED);

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
                            return set_error(STATUS_NO_MEM);
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
                            return set_error(STATUS_NO_MEM);
                    }

                    // Update buffer state
                    cBufPos     = end;

                    // Break the loop because line was completed
                    s->take(&sLine);
                    return set_error(STATUS_OK);
                }
            }

            // Check force flag
            if ((force) && (sLine.length() > 0))
            {
                s->take(&sLine);
                return set_error(STATUS_OK);
            }

            return set_error(STATUS_EOF);
        }

        ssize_t FileReader::skip(size_t count)
        {
            sLine.clear();
            return Reader::skip(count);
        }

    }
} /* namespace lsp */
