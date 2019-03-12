/*
 * LSPClipboard.cpp
 *
 *  Created on: 14 нояб. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>
#include <locale.h>

namespace lsp
{
    namespace tk
    {
        //-------------------------------------------------------------------------------
        // LSPInputStream
        LSPClipboard::LSPInputStream::LSPInputStream(LSPClipboard *cb)
        {
            pCB         = cb;
            bClosed     = false;
            nChunk      = 0;
            nOffset     = 0;
        }

        LSPClipboard::LSPInputStream::~LSPInputStream()
        {
            pCB         = NULL;
            bClosed     = true;
            nChunk      = 0;
            nOffset     = 0;
        }

        wssize_t LSPClipboard::LSPInputStream::avail()
        {
            if (bClosed)
                return - (nErrorCode = STATUS_CLOSED);
            wsize_t tail    = (pCB->nUsedChunks > 1) ? (pCB->nUsedChunks - 1) * CHUNK_SIZE + pCB->nLastChunkSize : pCB->nLastChunkSize;
            wsize_t pos     = nChunk * CHUNK_SIZE + nOffset;
            return tail - pos;
        }

        wssize_t LSPClipboard::LSPInputStream::position()
        {
            if (bClosed)
                return - (nErrorCode = STATUS_CLOSED);
            return nChunk * CHUNK_SIZE + nOffset;
        }

        ssize_t LSPClipboard::LSPInputStream::read(void *dst, size_t count)
        {
            if (bClosed)
                return - (nErrorCode = STATUS_CLOSED);

            size_t tot_read = 0;
            uint8_t *ptr = static_cast<uint8_t *>(dst);

            while (count > 0)
            {
                if (nChunk < (pCB->nUsedChunks-1))
                {
                    // Not the last chunk
                    size_t to_read      = CHUNK_SIZE - nOffset;
                    if (to_read > count)
                        to_read             = count;
                    memcpy(ptr, &pCB->vChunks[nChunk][nOffset], to_read);

                    // Update positions
                    nOffset            += to_read;
                    tot_read           += to_read;
                    ptr                += to_read;
                    count              -= to_read;

                    // Need to move to the next chunk?
                    if (nOffset >= CHUNK_SIZE)
                    {
                        nChunk              ++;
                        nOffset             = 0;
                    }
                }
                else
                {
                    // Last chunk
                    size_t to_read      = pCB->nLastChunkSize - nOffset;
                    if (to_read > count)
                        to_read             = count;
                    memcpy(dst, &pCB->vChunks[nChunk][nOffset], to_read);

                    // There is nothing more to read
                    tot_read           += to_read;
                    nOffset            += to_read;
                    count               = 0;
                }
            }

            nErrorCode      = STATUS_OK;
            return tot_read;
        }

        wssize_t LSPClipboard::LSPInputStream::seek(wsize_t position)
        {
            if (bClosed)
                return nErrorCode = STATUS_CLOSED;

            size_t chunk_id     = position / CHUNK_SIZE;
            if (chunk_id >= pCB->nUsedChunks)
                chunk_id            = (pCB->nUsedChunks > 0) ? pCB->nUsedChunks-1 : 0;

            size_t chunk_off    = position % CHUNK_SIZE;
            if (chunk_id == (pCB->nUsedChunks - 1))
            {
                if (chunk_off > pCB->nLastChunkSize)
                    chunk_off   = pCB->nLastChunkSize;
            }

            nChunk          = chunk_id;
            nOffset         = chunk_off;
            nErrorCode          = STATUS_OK;

            return nChunk * CHUNK_SIZE + nOffset;
        }

        status_t LSPClipboard::LSPInputStream::close()
        {
            if (bClosed)
                return nErrorCode = STATUS_CLOSED;

            bClosed         = true;
            if ((--pCB->nReferences) <= 0)
            {
                pCB->destroy_data();
                delete pCB;
                pCB = NULL;
            }

            return nErrorCode = STATUS_OK;
        }

        //-------------------------------------------------------------------------------
        // LSPOutputStream
        LSPClipboard::LSPOutputStream::LSPOutputStream(LSPClipboard *cb)
        {
            pCB         = cb;
            bClosed     = false;
            nChunk      = 0;
            nOffset     = 0;
        }

        LSPClipboard::LSPOutputStream::~LSPOutputStream()
        {
            pCB         = NULL;
            bClosed     = true;
            nChunk      = 0;
            nOffset     = 0;
        }

        wssize_t LSPClipboard::LSPOutputStream::position()
        {
            if (bClosed)
                return - set_error(STATUS_CLOSED);
            return nChunk * CHUNK_SIZE + nOffset;
        }

        ssize_t LSPClipboard::LSPOutputStream::write(const void *buf, size_t count)
        {
            if (bClosed)
                return - set_error(STATUS_CLOSED);
            else if (count <= 0)
                return count;
            else if (pCB->nTotalChunks <= 0)
            {
                // There is no data, need to allocate chunk map
                uint8_t **ccnk      = static_cast<uint8_t **>(malloc(sizeof(uint8_t *) * CHUNK_EXT));
                if (ccnk == NULL)
                    return - set_error(STATUS_NO_MEM);
                pCB->nTotalChunks   = CHUNK_EXT;
                pCB->vChunks        = ccnk;

                // Now we need to allocate new chunk
                uint8_t *chunk      = static_cast<uint8_t *>(malloc(sizeof(uint8_t) * CHUNK_SIZE));
                if (chunk == NULL)
                    return - set_error(STATUS_NO_MEM);

                pCB->vChunks[pCB->nUsedChunks++]    = chunk;
            }

            size_t tot_written = 0;
            const uint8_t *ptr = static_cast<const uint8_t *>(buf);

            while (count > 0)
            {
                // Not the last chunk
                size_t to_write     = CHUNK_SIZE - nOffset;
                if (to_write > count)
                    to_write            = count;
                memcpy(&pCB->vChunks[nChunk][nOffset], ptr, to_write);

                // Update positions
                nOffset            += to_write;
                tot_written        += to_write;
                ptr                += to_write;
                count              -= to_write;

                // Move read pointer
                if (nChunk < (pCB->nUsedChunks-1))
                {
                    // Need to move to the next chunk?
                    if (nOffset >= CHUNK_SIZE)
                    {
                        nChunk              ++;
                        nOffset             = 0;
                    }
                }
                else
                {
                    // Update chunk size
                    if (pCB->nLastChunkSize < nOffset)
                        pCB->nLastChunkSize = nOffset;

                    // Need to allocate new chunk ?
                    if (pCB->nLastChunkSize >= CHUNK_SIZE)
                    {
                        // First check that we have enough space in vChunks
                        if (pCB->nUsedChunks >= pCB->nTotalChunks)
                        {
                            size_t new_chunks   = pCB->nTotalChunks + CHUNK_EXT;
                            uint8_t **ccnk      = static_cast<uint8_t **>(realloc(pCB->vChunks, sizeof(uint8_t *) * new_chunks));
                            if (ccnk == NULL)
                            {
                                if (tot_written > 0)
                                    break;
                                return - set_error(STATUS_NO_MEM);
                            }
                            pCB->nTotalChunks   = new_chunks;
                            pCB->vChunks        = ccnk;
                        }

                        // Now we need to allocate new chunk
                        uint8_t *chunk      = static_cast<uint8_t *>(malloc(sizeof(uint8_t) * CHUNK_SIZE));
                        if (chunk == NULL)
                        {
                            if (tot_written > 0)
                                break;
                            return - set_error(STATUS_NO_MEM);
                        }

                        // Store pointer to the chunk and update position
                        pCB->vChunks[pCB->nUsedChunks++]    = chunk;
                        pCB->nLastChunkSize                 = 0;
                        nChunk                              ++;
                        nOffset                             = 0;
                    }
                } // else
            } // while

            return tot_written;
        }

        wssize_t LSPClipboard::LSPOutputStream::seek(wsize_t position)
        {
            if (bClosed)
                return set_error(STATUS_CLOSED);

            size_t chunk_id     = position / CHUNK_SIZE;
            if (chunk_id >= pCB->nUsedChunks)
                chunk_id            = (pCB->nUsedChunks > 0) ? pCB->nUsedChunks-1 : 0;

            size_t chunk_off    = position % CHUNK_SIZE;
            if (chunk_id == (pCB->nUsedChunks - 1))
            {
                if (chunk_off > pCB->nLastChunkSize)
                    chunk_off   = pCB->nLastChunkSize;
            }

            nChunk          = chunk_id;
            nOffset         = chunk_off;

            set_error(STATUS_OK);
            return nChunk * CHUNK_SIZE + nOffset;
        }

        status_t LSPClipboard::LSPOutputStream::close()
        {
            if (bClosed)
                return set_error(STATUS_CLOSED);

            bClosed         = true;
            if ((--pCB->nReferences) <= 0)
            {
                pCB->destroy_data();
                delete pCB;
                pCB = NULL;
            }

            return set_error(STATUS_OK);
        }

        //-------------------------------------------------------------------------------
        // LSPClipboard implementation
        LSPClipboard::LSPClipboard()
        {
            vChunks         = NULL;
            nTotalChunks    = 0;
            nUsedChunks     = 0;
            nLastChunkSize  = 0;
            nReferences     = 1;
            nError          = STATUS_OK;
            sCType          = NULL;
            bClosed         = false;
        }
        
        LSPClipboard::~LSPClipboard()
        {
            destroy_data();
            nError      = STATUS_OK;
        }

        void LSPClipboard::destroy_data()
        {
            if (vChunks != NULL)
            {
                for (size_t i=0; i<nUsedChunks; ++i)
                {
                    uint8_t *ptr    = vChunks[i];
                    if (ptr != NULL)
                        free(ptr);
                }
                free(vChunks);
                vChunks     = NULL;
            }

            if (sCType != NULL)
            {
                free(sCType);
                sCType = NULL;
            }

            nTotalChunks    = 0;
            nUsedChunks     = 0;
            nLastChunkSize  = 0;
        }

        status_t LSPClipboard::close()
        {
            if (bClosed)
                return nError = STATUS_CLOSED;

            nError      = STATUS_OK;
            bClosed     = true;

            if ((--nReferences) <= 0)
            {
                destroy_data();
                delete this;
            }

            return STATUS_OK;
        }

        io::IInputStream *LSPClipboard::read(const char *ctype)
        {
            // Check arguments and state
            if (ctype == NULL)
            {
                nError = STATUS_BAD_ARGUMENTS;
                return NULL;
            }
            else if (sCType == NULL)
            {
                nError = STATUS_BAD_STATE;
                return NULL;
            }

            // Check content type
            if (strcmp(ctype, sCType) != 0)
            {
                nError = STATUS_BAD_FORMAT;
                return NULL;
            }

            // Create stream
            io::IInputStream *strm = new LSPInputStream(this);
            if (strm == NULL)
            {
                nError      = STATUS_NO_MEM;
                return NULL;
            }

            // Increment number of references
            nReferences++;

            // Return the created stream
            return strm;
        }

        io::IOutputStream *LSPClipboard::write(const char *ctype)
        {
            // Data can be written only once
            if (sCType != NULL)
            {
                nError = STATUS_BAD_STATE;
                return NULL;
            }
            else if (ctype == NULL)
            {
                nError = STATUS_BAD_ARGUMENTS;
                return NULL;
            }

            // Allocate memory for charset and content type
            sCType      = lsp_strdup(ctype);
            if (sCType == NULL)
            {
                nError      = STATUS_NO_MEM;
                return NULL;
            }

            // Create stream
            io::IOutputStream *strm = new LSPOutputStream(this);
            if (strm == NULL)
            {
                lsp_free(sCType);
                sCType      = NULL;
                nError      = STATUS_NO_MEM;
                return NULL;
            }

            // Increment number of references
            nReferences++;

            // Return the created stream
            return strm;
        }

        size_t LSPClipboard::size() const
        {
            return (nUsedChunks > 1) ? (nUsedChunks-1) * CHUNK_SIZE + nLastChunkSize : nLastChunkSize;
        }

        size_t LSPClipboard::capacity() const
        {
            return nUsedChunks * CHUNK_SIZE;
        }

    } /* namespace tk */
} /* namespace lsp */
