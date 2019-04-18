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
            size_t      nChunks;        // Number of chunks in vChunks array
            size_t      nShift;         // Chunk identifier shift
            size_t      nMask;          // Chunk item mask
            size_t      nSizeOf;        // Size of record (in bytes)
            size_t      nAllocated;     // Number of allocated items
            uint8_t   **vChunks;        // List of all chunks
            uint8_t    *pCurr;          // Current chunk
            size_t      nLeft;          // Number of left items

        protected:
            uint8_t    *get_chunk(size_t id);
            void       *do_alloc();
            void        do_clear();
            ssize_t     do_ialloc(void **p);
            void       *do_get(size_t idx);
            void        do_destroy();
            size_t      do_alloc_n(void **ptr, size_t n);
            void        do_swap(BasicAllocator3D *alloc);
            bool        do_validate(const void *ptr) const;
            ssize_t     calc_index_of(const void *ptr) const;

        public:
            explicit BasicAllocator3D(size_t sz_of, size_t c_size);
            ~BasicAllocator3D();
    };

    template <class T>
        class Allocator3D: public BasicAllocator3D
        {
            public:
                /**
                 * Constructor
                 * @param csize chunk size, will be rounded to be power of 2
                 */
                explicit Allocator3D(size_t csize): BasicAllocator3D(sizeof(T), csize) {}

            public:
                /**
                 * Allocate single item
                 * @return pointer to allocated single item or NULL
                 */
                inline T *alloc() { return reinterpret_cast<T *>(do_alloc()); }

                /**
                 * Allocate single item
                 * @return pointer to allocated single item or negative error status
                 */
                inline ssize_t ialloc(T **dst) { return do_ialloc(reinterpret_cast<void **>(dst)); }

                /**
                 * Allocate single item and initialize with value
                 * @param src value to initialize
                 * @return pointer to allocated item or NULL
                 */
                inline T *alloc(const T *src)
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
                inline ssize_t ialloc(T **dst, const T *src) {
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
                inline T *alloc(const T &src)
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
                inline ssize_t ialloc(T **dst, const T &src) {
                    size_t res = do_ialloc(reinterpret_cast<void **>(dst));
                    if (*dst != NULL)
                        **dst = src;
                    return res;
                }

                /**
                 * Allocate set of items
                 * @param retval pointer to store pointers to allocated elements
                 * @param n number of elements to allocate
                 * @return actual number of allocated items
                 */
                inline size_t alloc_n(T **retval, size_t n) { return do_alloc_n(reinterpret_cast<void **>(retval), n); }

                /**
                 * Get number of allocated items
                 * @return number of allocated items
                 */
                inline size_t size() const { return nAllocated; }

                /**
                 * Get number of elements per one chunk
                 * @return number of elements per one chunk
                 */
                inline size_t chunk_size() const { return 1 << nShift; }

                /**
                 * Get number of allocated chunks
                 * @return number of allocated chunks
                 */
                inline size_t chunks() const { return nChunks; }

                /**
                 * Get chunk pointer
                 * @param id chunk identifier
                 * @return chunk pointer
                 */
                inline T *chunk(size_t id) { return (id < nChunks) ? reinterpret_cast<T *>(vChunks[id]) : NULL; }

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

                /**
                 * Swap internal contents with another allocator
                 * @param src allocator to perform swapping
                 */
                inline void swap(Allocator3D<T> *src) { do_swap(src); };

                /** Drop all allocated data
                 *
                 */
                inline void destroy() { do_destroy(); };

                /** Drop all allocated data (similar to destroy)
                 *
                 */
                inline void flush() { do_destroy(); };

                /** Drop all allocated data (currently similar to destroy)
                 *
                 */
                inline void clear() { do_clear(); };

                /**
                 * Ensure that the specified pointer is right pointer, NULL pointers
                 * also return positive result
                 * @param ptr pointer
                 * @return true if pointer is right and belongs to this allocator
                 */
                inline bool validate(const void *ptr) const { return do_validate(ptr); };

                /**
                 * Get index of the item in allocator
                 * @param ptr pointer to the item
                 * @return index of the item or negative value on error
                 */
                inline ssize_t index_of(const void *ptr) const { return calc_index_of(ptr); }
        };
}

#endif /* INCLUDE_CORE_3D_ALLOCATOR3D_H_ */
