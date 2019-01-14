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
        protected:
            size_t      nCapacity;      // Capacity ov vChunks array
            size_t      nChunks;        // Number of chunks in vChunks array
            size_t      nChunkCapacity; // Capacity of chunk (in elements)
            size_t      nChunkSize;     // Current size of chunk
            size_t      nSizeOf;        // Size of record (in bytes)
            size_t      nAllocated;     // Number of allocated items
            uint8_t    *pCurrChunk;     // Current chunk
            uint8_t   **vChunks;        // List of all chunks

        protected:
            void        allocate_new_chunk();
            void       *do_alloc();
            ssize_t     do_ialloc(void **p);
            void       *do_get(size_t idx);
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
                 * @return pointer to allocated single item or NULL
                 */
                T *alloc() { return reinterpret_cast<T *>(do_alloc()); }

                /**
                 * Allocate single item
                 * @return pointer to allocated single item or NULL
                 */
                ssize_t ialloc(T **dst) { return do_ialloc(reinterpret_cast<void **>(dst)); }

                /**
                 * Allocate single item and initialize with value
                 * @param src value to initialize
                 * @return pointer to allocated item or NULL
                 */
                T *alloc(const T *src)
                {
                    T *res = reinterpret_cast<T *>(do_alloc());
                    if (res != NULL)
                        *res = *src;
                    return res;
                }

                /**
                 * Allocate single item
                 * @return pointer to allocated single item or NULL
                 */
                ssize_t ialloc(T **dst, const T *src) {
                    size_t res = do_ialloc(reinterpret_cast<void **>(dst));
                    if (*dst != NULL)
                        **dst = *src;
                    return res;
                }

                /**
                 * Allocate single item and initialize with value
                 * @param src value to initialize
                 * @return pointer to allocated item or NULL
                 */
                T *alloc(const T &src)
                {
                    T *res = reinterpret_cast<T *>(do_alloc());
                    if (res != NULL)
                        *res = src;
                    return res;
                }

                /**
                 * Allocate single item
                 * @return pointer to allocated single item or NULL
                 */
                ssize_t ialloc(T **dst, const T &src) {
                    size_t res = do_ialloc(reinterpret_cast<void **>(dst));
                    if (*dst != NULL)
                        **dst = src;
                    return res;
                }

                /**
                 * Allocate set of items
                 * @param retval pointer to store results
                 * @param n number of elements to allocate
                 * @return actual number of allocated items
                 */
                size_t alloc_n(T **retval, size_t n) { return do_alloc_n(retval, n); }

                /**
                 * Get number of allocated items
                 * @return number of allocated items
                 */
                inline size_t size() const
                {
                    return nAllocated;
                }

                /**
                 * Get element at specified index
                 * @param idx element at specified index
                 * @return element at specified index or NULL if index is invalid
                 */
                inline T *get(size_t idx) { return reinterpret_cast<T *>(do_get(idx)); }

                /**
                 * Get element at specified index
                 * @param idx element at specified index
                 * @return element at specified index or NULL if index is invalid
                 */
                inline T * operator[] (size_t idx) { return do_get(idx); }

                /** Drop all allocated data
                 *
                 */
                void destroy() { do_destroy(); };
        };
}

#endif /* INCLUDE_CORE_3D_ALLOCATOR3D_H_ */
