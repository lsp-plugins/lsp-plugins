/*
 * Allocator3D.cpp
 *
 *  Created on: 14 янв. 2019 г.
 *      Author: sadko
 */

#include <core/3d/Allocator3D.h>
#include <core/sugar.h>
#include <core/status.h>

namespace lsp
{
    BasicAllocator3D::BasicAllocator3D(size_t sz_of, size_t c_size)
    {
        nChunks         = 0;
        nChunkCapacity  = c_size;
        nSizeOf         = sz_of;
        nAllocated      = 0;
        vChunks         = NULL;
    }

    BasicAllocator3D::~BasicAllocator3D()
    {
        do_destroy();
    }

    uint8_t *BasicAllocator3D::get_chunk(size_t id)
    {
        // Reallocate chunk index if too small
        if (id >= nChunks)
        {
            uint8_t **nc    = reinterpret_cast<uint8_t **>(::realloc(vChunks, sizeof(uint8_t *) * (nChunks + 16)));
            if (nc == NULL)
                return NULL;
            // Initialize pointers
            for (size_t i=0; i<16; ++i)
                nc[nChunks++] = NULL;
            vChunks         = nc;
        }

        // Fetch chunks
        uint8_t *chunk = vChunks[id];
        if (chunk != NULL)
            return chunk;

        // Try to allocate
        chunk = reinterpret_cast<uint8_t *>(::malloc(nChunkCapacity * nSizeOf));
        if (chunk == NULL)
            return NULL;

        vChunks[id]         = chunk;
        return chunk;
    }

    void *BasicAllocator3D::do_get(size_t idx)
    {
        if (idx >= nAllocated)
            return NULL;

        uint8_t *chunk      = vChunks[idx / nChunkCapacity];
        return (chunk != NULL) ? &chunk[nSizeOf * (idx % nChunkCapacity)] : NULL;
    }

    void *BasicAllocator3D::do_alloc()
    {
        ssize_t index   = nAllocated;
        size_t cid      = index / nChunkCapacity;

        uint8_t *chunk  = get_chunk(cid);
        if (chunk == NULL)
            return NULL;

        size_t iid      = index - (cid * nChunkCapacity);
        ++nAllocated;
        return &chunk[nSizeOf * iid];
    }

    ssize_t BasicAllocator3D::do_ialloc(void **p)
    {
        ssize_t index   = nAllocated;
        size_t cid      = index / nChunkCapacity;

        uint8_t *chunk  = get_chunk(cid);
        if (chunk == NULL)
            return -STATUS_NO_MEM;

        size_t iid      = index - (cid * nChunkCapacity);
        ++nAllocated;
        *p              = &chunk[nSizeOf * iid];
        return iid;
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
        nChunks         = 0;
    }

    void BasicAllocator3D::do_clear()
    {
        nAllocated      = 0;
    }

    void BasicAllocator3D::do_swap(BasicAllocator3D *src)
    {
        swap(nChunks, src->nChunks);
        swap(nChunkCapacity, src->nChunkCapacity);
        swap(nSizeOf, src->nSizeOf);
        swap(nAllocated, src->nAllocated);
        swap(vChunks, src->vChunks);
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
            if ((delta % nSizeOf) != 0)
                return false;
            delta /= nSizeOf;

            return (i*nChunkCapacity + delta) < nAllocated;
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

