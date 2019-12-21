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
            pOS         = NULL;
            nWrapFlags  = 0;
        }

        OutSequence::~OutSequence()
        {
            // Close file descriptor
            if (pOS != NULL)
            {
                flush_buffer_internal(true);

                if (nWrapFlags & WRAP_CLOSE)
                    pOS->close();
                if (nWrapFlags & WRAP_DELETE)
                    delete pOS;
                pOS         = NULL;
            }
            nWrapFlags  = 0;

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
            status_t res = f->wrap(fd, close);
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

            return set_error(STATUS_OK);
        }

        status_t OutSequence::wrap_native(lsp_fhandle_t fd, bool close, const char *charset)
        {
            if (pOS != NULL)
                return set_error(STATUS_BAD_STATE);

            OutFileStream *f = new OutFileStream();
            if (f == NULL)
                return set_error(STATUS_NO_MEM);
            status_t res = f->wrap_native(fd, close);
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

            return set_error(STATUS_OK);
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
            status_t res = f->wrap(fd, flags);
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

            return set_error(STATUS_OK);
        }

        status_t OutSequence::wrap(IOutStream *os, size_t flags, const char *charset)
        {
            if (pOS != NULL)
                return set_error(STATUS_BAD_STATE);
            else if (os == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

            // Initialize decoder
            status_t res = sEncoder.init(charset);
            if (res != STATUS_OK)
            {
                sEncoder.close();
                return set_error(res);
            }

            // Store pointers
            pOS         = os;
            nWrapFlags  = flags;

            return set_error(STATUS_OK);
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
            status_t res = f->open(path, mode);
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

        status_t OutSequence::flush_buffer_internal(bool force)
        {
            // Flush all character data to stream
            ssize_t fetch;
            do
            {
                fetch = sEncoder.fetch(pOS);
            } while (fetch > 0);

            if ((fetch < 0) && (fetch != -STATUS_EOF))
                return set_error(-fetch);

            return set_error((force) ? pOS->flush() : STATUS_OK);
        }

        status_t OutSequence::write(lsp_wchar_t c)
        {
            if (pOS == NULL)
                return set_error(STATUS_CLOSED);

            ssize_t filled = sEncoder.fill(c);
            if (filled > 0)
                return set_error(STATUS_OK);

            status_t res = flush_buffer_internal(false);
            if (res != STATUS_OK)
                return set_error(res);

            filled = sEncoder.fill(c);
            return set_error((filled > 0) ? STATUS_OK : STATUS_UNKNOWN_ERR);
        }

        status_t OutSequence::write(const lsp_wchar_t *c, size_t count)
        {
            if (pOS == NULL)
                return set_error(STATUS_CLOSED);

            size_t written = 0;

            while (written < count)
            {
                // Try to fill data
                ssize_t filled = sEncoder.fill(c, count - written);
                if (filled > 0)
                {
                    c          += filled;
                    written    += filled;
                    continue;
                }

                // Try to fetch data
                ssize_t fetched = sEncoder.fetch(pOS);
                if (fetched > 0)
                    continue;

                // Nothing to do more? Skip any errors if there was data processed
                if (written > 0)
                    break;

                // Analyze errors
                if (filled < 0)
                    return -set_error(-filled);
                else if (fetched < 0)
                    return -set_error(-fetched);

                break;
            }

            return set_error(STATUS_OK);
        }

        status_t OutSequence::write_ascii(const char *s, size_t count)
        {
            if (pOS == NULL)
                return set_error(STATUS_CLOSED);

            size_t written = 0;

            while (written < count)
            {
                // Try to fill data
                ssize_t filled = sEncoder.fill(s, count - written);
                if (filled > 0)
                {
                    s          += filled;
                    written    += filled;
                    continue;
                }

                // Try to fetch data
                ssize_t fetched = sEncoder.fetch(pOS);
                if (fetched > 0)
                    continue;

                // Nothing to do more? Skip any errors if there was data processed
                if (written > 0)
                    break;

                // Analyze errors
                if (filled < 0)
                    return -set_error(-filled);
                else if (fetched < 0)
                    return -set_error(-fetched);

                break;
            }

            return set_error(STATUS_OK);
        }

        status_t OutSequence::flush()
        {
            if (pOS == NULL)
                return set_error(STATUS_CLOSED);

            return flush_buffer_internal(true);
        }
    }

} /* namespace lsp */
