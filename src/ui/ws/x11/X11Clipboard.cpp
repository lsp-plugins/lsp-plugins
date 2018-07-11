/*
 * X11Clipboard.cpp
 *
 *  Created on: 15 нояб. 2017 г.
 *      Author: sadko
 */

#include <ui/ws/x11/ws.h>

namespace lsp
{
    namespace ws
    {
        namespace x11
        {
            //-----------------------------------------------------------------
            // X11Clipboard::InputStream
            X11Clipboard::InputStream::InputStream(X11Clipboard *cb)
            {
                pCB             = cb;
                bClosed         = false;
                pCurr           = cb->pFirst;
                nOffset         = 0;
                nPosition       = 0;
            }

            X11Clipboard::InputStream::~InputStream()
            {
                pCB             = NULL;
                bClosed         = true;
                pCurr           = NULL;
                nOffset         = 0;
                nPosition       = 0;
            }

            wssize_t X11Clipboard::InputStream::avail()
            {
                if (bClosed)
                    return nError = STATUS_CLOSED;

                return pCB->nAvail - nPosition;
            }

            wssize_t X11Clipboard::InputStream::position()
            {
                if (bClosed)
                    return nError = STATUS_CLOSED;
                return nPosition;
            }

            ssize_t X11Clipboard::InputStream::read(void *dst, size_t count)
            {
                if (bClosed)
                    return nError = STATUS_CLOSED;

                ssize_t tot_read = 0;
                uint8_t *ptr = static_cast<uint8_t *>(dst);

                while (count > 0)
                {
                    if (pCurr == NULL)
                    {
                        nError = STATUS_OK;
                        return tot_read;
                    }

                    size_t to_read  = pCurr->size - nOffset;
                    if (to_read > count)
                        to_read         = count;

                    memcpy(dst, &pCurr->data[nOffset], to_read);
                    ptr        += to_read;
                    nOffset    += to_read;
                    nPosition  += to_read;
                    count      -= to_read;
                    tot_read   += to_read;

                    if (nOffset >= pCurr->size)
                    {
                        pCurr       = pCurr->next;
                        nOffset     = 0;
                    }
                }

                nError = STATUS_OK;
                return tot_read;
            }

            wssize_t X11Clipboard::InputStream::seek(wsize_t position)
            {
                if (bClosed)
                    return nError = STATUS_CLOSED;

                if ((nPosition - nOffset) > position)
                {
                    nPosition       = 0;
                    nOffset         = 0;
                    pCurr           = pCB->pFirst;
                }
                else if (position < nPosition)
                {
                    nOffset        -= (nPosition - position);
                    nPosition       = position;
                    nError          = STATUS_OK;
                    return STATUS_OK;
                }

                wsize_t skip = position - nPosition;
                while (skip > 0)
                {
                    if (pCurr == NULL)
                    {
                        nError = STATUS_OK;
                        return nPosition;
                    }

                    if (skip >= pCurr->size)
                    {
                        pCurr       = pCurr->next;
                        skip       -= pCurr->size;
                        nPosition  += pCurr->size;
                    }
                    else
                    {
                        nPosition  += skip;
                        nOffset     = skip;
                        skip        = 0;
                    }
                }

                return nPosition;
            }

            status_t X11Clipboard::InputStream::close()
            {
                if (bClosed)
                    return nError = STATUS_CLOSED;

                bClosed         = true;
                return nError   = pCB->close();
            }

            //-----------------------------------------------------------------
            // X11Clipboard
            X11Clipboard::X11Clipboard()
            {
                nReferences     = 1;
                nAvail          = 0;
                pFirst          = NULL;
                pLast           = NULL;
                sType           = NULL;
            }
            
            X11Clipboard::~X11Clipboard()
            {
                chunk_t *ptr = pFirst;
                while (ptr != NULL)
                {
                    chunk_t *next   = ptr->next;
                    free(ptr);
                    ptr             = next;
                }

                nReferences     = 0;
                nAvail          = 0;
                pFirst          = NULL;
                pLast           = NULL;

                if (sType != NULL)
                {
                    free(sType);
                    sType           = NULL;
                }
            }

            status_t X11Clipboard::close()
            {
                nError      = STATUS_OK;

                return IClipboard::close();
            }

            IInputStream *X11Clipboard::read(const char *ctype)
            {
                // Content-Type is ignored

                // Create stream
                IInputStream *strm = new InputStream(this);
                if (strm == NULL)
                {
                    nError      = STATUS_NO_MEM;
                    return NULL;
                }

                // Increment number of references
                acquire();

                // Return the created stream
                return strm;
            }

            ssize_t X11Clipboard::append(const void *buf, size_t count)
            {
                chunk_t *c = pLast;
                ssize_t written = 0;
                const uint8_t *ptr  = static_cast<const uint8_t *>(buf);

                while (count > 0)
                {
                    // Prepare chunk to write
                    if ((c == NULL) || (c->size >= c->avail))
                    {
                        size_t alloc = count;
                        if (alloc < CHUNK_SIZE)
                            alloc       = CHUNK_SIZE;

                        chunk_t *n  = static_cast<chunk_t *>(malloc(alloc + sizeof(chunk_t)));
                        if (n == NULL)
                        {
                            if (written > 0)
                                return written;
                            return - (nError = STATUS_NO_MEM);
                        }

                        n->size     = 0;
                        n->avail    = alloc;
                        n->next     = NULL;

                        // Store
                        if (c == NULL)
                            pFirst          = n;
                        else
                            c->next         = n;
                        c               = n;
                        pLast           = n;
                    }

                    // Write data to chunk
                    size_t to_write     = c->avail - c->size;
                    if (to_write > count)
                        to_write            = count;
                    memcpy(&c->data[c->size], ptr, to_write);

                    // Update pointer
                    c->size        += to_write;
                    ptr            += to_write;
                    count          -= to_write;
                    written        += to_write;
                    nAvail         += to_write;
                }

                nError      = STATUS_OK;
                return written;
            }

        } /* namespace x11 */
    } /* namespace ws */
} /* namespace lsp */
