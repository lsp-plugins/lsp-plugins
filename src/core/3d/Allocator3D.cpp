/*
 * Allocator3D.cpp
 *
 *  Created on: 14 янв. 2019 г.
 *      Author: sadko
 */

#include <core/3d/Allocator3D.h>

namespace lsp
{
    BasicAllocator3D::BasicAllocator3D(size_t sz_of, size_t c_size)
    {
        nCapacity       = 0;
        nChunks         = 0;
        nChunkCapacity  = c_size;
        nChunkSize      = c_size;
        nSizeOf         = sz_of;
        nAllocated      = 0;
        pCurrChunk      = NULL;
        vChunks         = NULL;
    }

    BasicAllocator3D::~BasicAllocator3D()
    {
        do_destroy();
    }

    void BasicAllocator3D::allocate_new_chunk()
    {
        // Try to allocate chunk data
        uint8_t *chunk = reinterpret_cast<uint8_t *>(::malloc(nChunkCapacity * nSizeOf));
        if (chunk == NULL)
            return;

        // Register chunk data
        if (nChunks >= nCapacity)
        {
            uint8_t **nc    = reinterpret_cast<uint8_t **>(::realloc(vChunks, sizeof(uint8_t *) * (nCapacity + 16)));
            if (nc == NULL)
            {
                free(chunk);
                return;
            }
            vChunks         = nc;
            nCapacity      += 16;
        }

        // Store chunk and reset it's size
        vChunks[nChunks++]  = chunk;
        pCurrChunk          = chunk;
        nChunkSize          = 0;
    }

    void *BasicAllocator3D::do_get(size_t idx)
    {
        if (idx >= nAllocated)
            return NULL;

        uint8_t *chunk      = vChunks[idx / nChunkCapacity];
        return &chunk[nSizeOf * (idx % nChunkCapacity)];
    }

    void *BasicAllocator3D::do_alloc()
    {
        if (nChunkSize >= nChunkCapacity)
        {
            allocate_new_chunk();
            if (nChunkSize >= nChunkCapacity)
                return NULL;
        }
        ++nAllocated;
        return &pCurrChunk[nSizeOf * (nChunkSize++)];
    }

    ssize_t BasicAllocator3D::do_ialloc(void **p)
    {
        if (nChunkSize >= nChunkCapacity)
        {
            allocate_new_chunk();
            if (nChunkSize >= nChunkCapacity)
                return -1;
        }
        ssize_t index = nAllocated++;
        *p = &pCurrChunk[nSizeOf * (nChunkSize++)];
        return index;
    }

    void BasicAllocator3D::do_destroy()
    {
        if (vChunks != NULL)
        {
            for (size_t i=0; i<nChunks; ++i)
            {
                uint8_t *c = vChunks[i];
                if (c != NULL)
                {
                    ::free(c);
                    vChunks[i] = NULL;
                }
            }

            ::free(vChunks);
            vChunks = NULL;
        }

        nAllocated      = 0;
        nCapacity       = 0;
        nChunks         = 0;
        nChunkSize      = nChunkCapacity;
        pCurrChunk      = NULL;
    }

    void BasicAllocator3D::do_swap(BasicAllocator3D *src)
    {
        size_t tnCapacity       = nCapacity;
        size_t tnChunks         = nChunks;
        size_t tnChunkCapacity  = nChunkCapacity;
        size_t tnChunkSize      = nChunkSize;
        size_t tnSizeOf         = nSizeOf;
        size_t tnAllocated      = nAllocated;
        uint8_t *tpCurrChunk    = pCurrChunk;
        uint8_t **tvChunks      = vChunks;

        nCapacity               = src->nCapacity;
        nChunks                 = src->nChunks;
        nChunkCapacity          = src->nChunkCapacity;
        nChunkSize              = src->nChunkSize;
        nSizeOf                 = src->nSizeOf;
        nAllocated              = src->nAllocated;
        pCurrChunk              = src->pCurrChunk;
        vChunks                 = src->vChunks;

        src->nCapacity          = tnCapacity;
        src->nChunks            = tnChunks;
        src->nChunkCapacity     = tnChunkCapacity;
        src->nChunkSize         = tnChunkSize;
        src->nSizeOf            = tnSizeOf;
        src->nAllocated         = tnAllocated;
        src->pCurrChunk         = tpCurrChunk;
        src->vChunks            = tvChunks;
    }

    bool BasicAllocator3D::do_validate(const void *ptr) const
    {
        if (ptr == NULL)
            return true;

        const uint8_t *uptr     = reinterpret_cast<const uint8_t *>(ptr);
        ssize_t csize           = nChunkCapacity * nSizeOf;

        for (size_t i=0; i<nChunks; ++i)
        {
            if (vChunks[i] == NULL)
                continue;
            ssize_t delta           = uptr - vChunks[i];
            if ((delta < 0) || (delta >= csize))
                continue;
            return (delta % nSizeOf) == 0;
        }

        return false;
    }

    ssize_t BasicAllocator3D::calc_index_of(const void *ptr) const
    {
        if (ptr == NULL)
            return -1;

        const uint8_t *uptr     = reinterpret_cast<const uint8_t *>(ptr);
        ssize_t csize           = nChunkCapacity * nSizeOf;
        ssize_t offset          = 0;

        for (size_t i=0; i<nChunks; ++i, offset += nChunkCapacity)
        {
            if (vChunks[i] == NULL)
                continue;
            ssize_t delta           = uptr - vChunks[i];
            if ((delta < 0) || (delta >= csize))
                continue;
            if ((delta % nSizeOf) != 0)
                return -1;
            return offset + delta / nSizeOf;
        }

        return -1;
    }
}

