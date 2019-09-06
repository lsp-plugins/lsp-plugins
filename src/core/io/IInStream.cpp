/*
 * IClipStream.cpp
 *
 *  Created on: 14 нояб. 2017 г.
 *      Author: sadko
 */

#include <core/io/IInStream.h>
#include <core/io/IOutStream.h>
#include <core/status.h>

namespace lsp
{
    namespace io
    {
        
        IInStream::IInStream()
        {
            nErrorCode      = STATUS_OK;
        }
        
        IInStream::~IInStream()
        {
            nErrorCode      = STATUS_OK;
        }
    
        wssize_t IInStream::avail()
        {
            return - set_error(STATUS_NOT_IMPLEMENTED);
        }

        wssize_t IInStream::position()
        {
            return - set_error(STATUS_NOT_IMPLEMENTED);
        }

        ssize_t IInStream::read(void *dst, size_t count)
        {
            return - set_error(STATUS_NOT_IMPLEMENTED);
        }

        ssize_t IInStream::read_byte()
        {
            uint8_t byte;
            ssize_t nread = read(&byte, sizeof(byte));
            if (nread > 0)
                return byte;
            return (nread != 0) ? nread : -STATUS_EOF;
        }

        ssize_t IInStream::read_fully(void *dst, size_t count)
        {
            uint8_t *ptr    = reinterpret_cast<uint8_t *>(dst);
            size_t left     = count;
            while (left > 0)
            {
                ssize_t act_read = read(ptr, left);
                if (act_read < 0)
                {
                    if (left > count)
                        break;
                    else
                        return act_read;
                }

                left   -= act_read;
                ptr    += act_read;
            }

            return count - left;
        }

        status_t IInStream::read_block(void *dst, size_t count)
        {
            if (dst == NULL)
                return set_error(STATUS_BAD_ARGUMENTS);
            else if (count == 0)
                return set_error(STATUS_OK);

            ssize_t read = read_fully(dst, count);
            if (read < 0)
                return -read;

            return set_error((size_t(read) == count) ? STATUS_OK : STATUS_EOF);
        }

        wssize_t IInStream::seek(wsize_t position)
        {
            return - set_error(STATUS_NOT_IMPLEMENTED);
        }

        wssize_t IInStream::skip(wsize_t amount)
        {
            uint8_t skip_buf[0x1000];

            wsize_t initial = amount;
            while (amount > 0)
            {
                ssize_t n = read(skip_buf, (amount > sizeof(skip_buf)) ? sizeof(skip_buf) : amount);
                if (n < 0)
                {
                    if (initial > amount)
                        break;
                    return n;
                }
                amount -= n;
            }

            return initial - amount;
        }

        wssize_t IInStream::sink(IOutStream *os, size_t buf_size)
        {
            if ((os == NULL) || (buf_size < 1))
                return -set_error(STATUS_BAD_ARGUMENTS);

            uint8_t *buf = reinterpret_cast<uint8_t *>(::malloc(buf_size));
            if (buf == NULL)
                return STATUS_NO_MEM;

            wssize_t count = 0;
            while (true)
            {
                // Read data
                ssize_t nread = read(buf, buf_size);
                if (nread < 0)
                {
                    if (nread == -STATUS_EOF)
                    {
                        set_error(STATUS_OK);
                        return count;
                    }

                    set_error(-nread);
                    return nread;
                }
                count += nread;

                // Write data
                ssize_t off = 0;
                while (off < nread)
                {
                    ssize_t nwritten = os->write(&buf[off], nread-off);
                    if (nwritten < 0)
                    {
                        set_error(-nwritten);
                        return nwritten;
                    }
                    off    += nwritten;
                }
            }
        }


        status_t IInStream::close()
        {
            return set_error(nErrorCode);
        }

    } /* namespace ws */
} /* namespace lsp */
