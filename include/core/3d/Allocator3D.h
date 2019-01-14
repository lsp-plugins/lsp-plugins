/*
 * Allocator3D.h
 *
 *  Created on: 14 янв. 2019 г.
 *      Author: sadko
 */

#ifndef INCLUDE_CORE_3D_ALLOCATOR3D_H_
#define INCLUDE_CORE_3D_ALLOCATOR3D_H_

#include <core/types.h>

namespace lsp
{
    /**
     * Fixed-pointer allocator, allocates data grouped into partitions or 'chunks'
     * to avoid huge memory fragmentation
     */
    class BasicAllocator3D
    {
        private:
            size_t      nCapacity;      // Capacity ov vChunks array
            size_t      nChunks;        // Number of chunks in vChunks array
            size_t      nChunkCapacity; // Capacity of chunk (in elements)
            size_t      nChunkSize;     // Current size of chunk
            size_t      nSizeOf;        // Size of record (in bytes)
            uint8_t    *pCurrChunk;     // Current chunk
            uint8_t   **vChunks;        // List of all chunks

        protected:
            void        allocate_new_chunk();
            void       *do_alloc();
            void        do_destroy();
            size_t     *do_alloc_n(void **ptr, size_t n);

        public:
            explicit BasicAllocator3D(size_t sz_of, size_t c_size);
            ~BasicAllocator3D();
    };

    template <class T>
        class Allocator3D: public BasicAllocator3D
        {
            public:
                explicit Allocator3D(size_t csize): BasicAllocator3D(sizeof(T), csize) {}

            public:
                /**
                 * Allocate single item
                 * @return allocated single item
                 */
                T *alloc() { return reinterpret_cast<T *>(do_alloc()); }

                /**
                 * Allocate set of items
                 * @param retval pointer to store results
                 * @param n number of elements to allocate
                 * @return actual number of allocated items
                 */
                size_t alloc_n(T **retval, size_t n) { return do_alloc_n(retval, n); }

                /** Drop all allocated data
                 *
                 */
                void destroy() { do_destroy(); };
        };
}

#endif /* INCLUDE_CORE_3D_ALLOCATOR3D_H_ */
