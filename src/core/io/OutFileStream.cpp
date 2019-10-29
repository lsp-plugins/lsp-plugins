/*
 * OutFileStream.cpp
 *
 *  Created on: 13 мар. 2019 г.
 *      Author: sadko
 */

#include <core/io/StdioFile.h>
#include <core/io/NativeFile.h>
#include <core/io/OutFileStream.h>

namespace lsp
{
    namespace io
    {
        
        OutFileStream::OutFileStream()
        {
            pFD         = NULL;
            nWrapFlags  = 0;
        }
        
        OutFileStream::~OutFileStream()
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
        }

        status_t OutFileStream::close()
        {
            status_t res = STATUS_OK;

            if (pFD != NULL)
            {
                // Perform close
                if (nWrapFlags & WRAP_CLOSE)
                    res = pFD->close();
                if (nWrapFlags & WRAP_DELETE)
                    delete pFD;
                pFD         = NULL;
            }
            nWrapFlags  = 0;

            return set_error(res);
        }
    
        status_t OutFileStream::wrap(FILE *fd, bool close)
        {
            if (pFD != NULL)
                return set_error(STATUS_BAD_STATE);
            else if (fd == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

            StdioFile *f = new StdioFile();
            if (f == NULL)
                return set_error(STATUS_NO_MEM);
            status_t res = f->wrap(fd, File::FM_WRITE, close);
            if (res != STATUS_OK)
            {
                f->close();
                delete f;
                return set_error(res);
            }

            res = wrap(f, WRAP_DELETE);
            if (res != STATUS_OK)
            {
                f->close();
                delete f;
            }
            return set_error(res);
        }

        status_t OutFileStream::wrap_native(lsp_fhandle_t fd, bool close)
        {
            if (pFD != NULL)
                return set_error(STATUS_BAD_STATE);

            NativeFile *f = new NativeFile();
            if (f == NULL)
                return set_error(STATUS_NO_MEM);
            status_t res = f->wrap(fd, File::FM_WRITE, close);
            if (res != STATUS_OK)
            {
                f->close();
                delete f;
                return set_error(res);
            }

            res = wrap(f, WRAP_DELETE);
            if (res != STATUS_OK)
            {
                f->close();
                delete f;
            }
            return set_error(res);
        }

        status_t OutFileStream::wrap(File *fd, size_t flags)
        {
            if (pFD != NULL)
                return set_error(STATUS_BAD_STATE);
            else if (fd == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

            // Store pointers
            pFD         = fd;
            nWrapFlags  = flags;

            return STATUS_OK;
        }

        status_t OutFileStream::open(const char *path, size_t mode)
        {
            if (pFD != NULL)
                return set_error(STATUS_BAD_STATE);
            else if (path == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

            LSPString tmp;
            if (!tmp.set_utf8(path))
                return set_error(STATUS_NO_MEM);
            return open(&tmp, mode);
        }

        status_t OutFileStream::open(const LSPString *path, size_t mode)
        {
            if (pFD != NULL)
                return set_error(STATUS_BAD_STATE);
            else if (path == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);

            NativeFile *f = new NativeFile();
            if (f == NULL)
                return set_error(STATUS_NO_MEM);

            status_t res = f->open(path, mode | File::FM_WRITE);
            if (res != STATUS_OK)
            {
                f->close();
                delete f;
                return set_error(res);
            }

            return wrap(f, WRAP_CLOSE | WRAP_DELETE);
        }

        status_t OutFileStream::open(const Path *path, size_t mode)
        {
            return open(path->as_string(), mode);
        }

        wssize_t OutFileStream::position()
        {
            if (pFD == NULL)
                return set_error(STATUS_CLOSED);
            wssize_t pos = pFD->position();
            set_error((pos < 0) ? status_t(-pos) : STATUS_OK);
            return pos;
        }

        ssize_t OutFileStream::write(const void *buf, size_t count)
        {
            if (pFD == NULL)
                return set_error(STATUS_CLOSED);
            ssize_t res = pFD->write(buf, count);
            set_error((res < 0) ? status_t(-res) : STATUS_OK);
            return res;
        }

        wssize_t OutFileStream::seek(wsize_t position)
        {
            if (pFD == NULL)
                return set_error(STATUS_CLOSED);
            status_t res = pFD->seek(position, File::FSK_SET);
            if (res != STATUS_OK)
                return -set_error(res);
            wssize_t pos = pFD->position();
            set_error((pos < 0) ? status_t(-pos) : STATUS_OK);
            return pos;
        }

        status_t OutFileStream::flush()
        {
            if (pFD == NULL)
                return set_error(STATUS_CLOSED);
            return set_error(pFD->flush());
        }

    } /* namespace io */
} /* namespace lsp */
