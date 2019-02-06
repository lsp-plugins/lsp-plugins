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
//        if (c_size && !(c_size & (c_size - 1)))
//            nShift          = c_size;
//        else
        for (nShift = 0; size_t(1 << nShift) < c_size; nShift ++) {} // Estimate chunk size

        nMask           = (1 << nShift) - 1;

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
            size_t cap      = (id + 0x10) & (~0x0f);
            uint8_t **nc    = reinterpret_cast<uint8_t **>(::realloc(vChunks, sizeof(uint8_t *) * cap));
            if (nc == NULL)
                return NULL;
            // Initialize pointers
            for (size_t i=nChunks; i<cap; ++i)
                nc[nChunks++] = NULL;
            vChunks         = nc;
        }

        // Fetch chunks
        uint8_t *chunk = vChunks[id];
        if (chunk != NULL)
            return chunk;

        // Try to allocate
        chunk = reinterpret_cast<uint8_t *>(::malloc(nSizeOf << nShift));
        if (chunk == NULL)
            return NULL;

        vChunks[id]         = chunk;
        return chunk;
    }

    void *BasicAllocator3D::do_get(size_t idx)
    {
        if (idx >= nAllocated)
            return NULL;
        uint8_t *chunk      = vChunks[idx >> nShift];
        return &chunk[nSizeOf * (idx & nMask)];
    }

    void *BasicAllocator3D::do_alloc()
    {
        ssize_t index   = nAllocated;
        size_t cid      = index >> nShift;

        uint8_t *chunk  = get_chunk(cid);
        if (chunk == NULL)
            return NULL;

        size_t iid      = index & nMask;
        ++nAllocated;
        return &chunk[nSizeOf * iid];
    }

    ssize_t BasicAllocator3D::do_ialloc(void **p)
    {
        ssize_t index   = nAllocated;
        size_t cid      = index >> nShift;

        uint8_t *chunk  = get_chunk(cid);
        if (chunk == NULL)
            return -STATUS_NO_MEM;

        size_t iid      = index & nMask;
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
        swap(nShift, src->nShift);
        swap(nMask, src->nMask);
        swap(nSizeOf, src->nSizeOf);
        swap(nAllocated, src->nAllocated);
        swap(vChunks, src->vChunks);
    }

    bool BasicAllocator3D::do_validate(const void *ptr) const
    {
        if (ptr == NULL)
            return true;

        const uint8_t *uptr     = reinterpret_cast<const uint8_t *>(ptr);
        ssize_t csize           = nSizeOf << nShift;

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

            return ((i << nShift) + delta) < nAllocated;
        }

        return false;
    }

    ssize_t BasicAllocator3D::calc_index_of(const void *ptr) const
    {
        if (ptr == NULL)
            return -1;

        const uint8_t *uptr     = reinterpret_cast<const uint8_t *>(ptr);
        ssize_t csize           = nSizeOf << nShift;
        ssize_t offset          = 0;
        ssize_t chunk_cap       = 1 << nShift;

        for (size_t i=0; i<nChunks; ++i, offset += chunk_cap)
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

