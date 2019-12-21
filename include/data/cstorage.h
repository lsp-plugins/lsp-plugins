/*
 * cstorage.h
 *
 *  Created on: 9 апр. 2017 г.
 *      Author: sadko
 */

#ifndef DATA_CSTORAGE_H_
#define DATA_CSTORAGE_H_

#include <core/types.h>
#include <core/alloc.h>
#include <string.h>

namespace lsp
{
    class basic_storage
    {
        protected:
            uint8_t    *vItems;
            size_t      nCapacity;
            size_t      nItems;
            size_t      nSizeOf;

        protected:
            bool realloc_capacity(size_t capacity)
            {
                if (capacity < 32)
                    capacity        = 32;

                // Do aligned (re)allocation
                uint8_t *ptr    = lsp_trealloc(uint8_t, vItems, nSizeOf * capacity);
                if (ptr == NULL)
                    return false;

                // Update pointer and capacity
                vItems          = ptr;
                nCapacity       = capacity;
                return true;
            }

            uint8_t *alloc_items(size_t n)
            {
                if ((nItems + n) > nCapacity)
                {
                    size_t dn = nCapacity + n;
                    if (!realloc_capacity(dn + (dn >> 1)))
                        return NULL;
                }

                uint8_t    *ptr = &vItems[nItems * nSizeOf];
                nItems         += n;
                return ptr;
            }

            uint8_t *alloc_item()
            {
                if (nItems >= nCapacity)
                {
                    if (!realloc_capacity(nCapacity + (nCapacity >> 1)))
                        return NULL;
                }

                return &vItems[(nItems++)*nSizeOf];
            }

            uint8_t *insert_items(size_t index, size_t n)
            {
                if ((index < 0) || (index > nItems))
                    return NULL;
                if ((nItems + n) > nCapacity)
                {
                    size_t dn = nCapacity + n;
                    if (!realloc_capacity(dn + (dn >> 1)))
                        return NULL;
                }
                uint8_t *res = &vItems[index * nSizeOf];
                if (index < nItems)
                    ::memmove(&res[n*nSizeOf], res, (nItems - index)*nSizeOf);
                nItems += n;
                return res;
            }

            uint8_t *at(size_t index)
            {
                return &vItems[index * nSizeOf];
            }

            uint8_t *c_at(size_t index)
            {
                return &vItems[(index % nItems) * nSizeOf];
            }

            uint8_t *get_item(size_t index)
            {
                return (index < nItems) ? &vItems[index * nSizeOf] : NULL;
            }

            uint8_t *c_get_item(size_t index)
            {
                if (nItems == 0)
                    return NULL;

                return &vItems[(index % nItems) * nSizeOf];
            }

            uint8_t *first()
            {
                return (nItems > 0) ? vItems : NULL;
            }

            uint8_t *last()
            {
                return (nItems > 0) ? &vItems[(nItems-1)*nSizeOf] : NULL;
            }

            uint8_t *pop_last()
            {
                return (nItems > 0) ? &vItems[(--nItems)*nSizeOf] : NULL;
            }

            inline void do_swap_data(basic_storage *src)
            {
                uint8_t *_vItems    = src->vItems;
                size_t  _nCapacity  = src->nCapacity;
                size_t  _nItems     = src->nItems;
                size_t  _nSizeOf    = src->nSizeOf;

                src->vItems     = vItems;
                src->nCapacity  = nCapacity;
                src->nItems     = nItems;
                src->nSizeOf    = nSizeOf;

                vItems          = _vItems;
                nCapacity       = _nCapacity;
                nItems          = _nItems;
                nSizeOf         = _nSizeOf;
            }

        public:
            inline basic_storage(size_t sz)
            {
                vItems      = NULL;
                nCapacity   = 0;
                nItems      = 0;
                nSizeOf     = sz;
            }

            inline ~basic_storage()
            {
                flush();
            }

            void flush()
            {
                if (vItems != NULL)
                {
                    lsp_free(vItems);
                    vItems      = NULL;
                }
                nCapacity   = 0;
                nItems      = 0;
            }

            inline bool remove(size_t idx)
            {
                if (idx >= nItems)
                    return false;
                if (idx != (--nItems))
                    ::memmove(&vItems[idx * nSizeOf], &vItems[(idx+1)*nSizeOf], (nItems-idx) * nSizeOf);
                return true;
            }

            inline bool remove(const void *ptr)
            {
                ssize_t off = reinterpret_cast<const uint8_t *>(ptr) - vItems;
                if ((off < 0) || ((off % nSizeOf) != 0))
                    return false;
                return remove(off / nSizeOf);
            }

            inline bool remove_n(size_t idx, size_t n)
            {
                size_t last = idx + n;
                if ((last + n) > nItems)
                    return false;
                if ((last + n) != nItems)
                    ::memmove(&vItems[idx * nSizeOf], &vItems[last * nSizeOf], (nItems - last) * nSizeOf);
                nItems     -= n;
                return true;
            }

            inline size_t size() const  { return nItems; }

            inline size_t capacity() const { return nCapacity; }

            inline void clear() { nItems = 0; }

    };

    template <class T>
        class cstorage: public basic_storage
        {
            private:
                cstorage(const cstorage<T> &src);                               // Disable copying
                cstorage<T> & operator = (const cstorage<T> & src);             // Disable copying

            public:
                explicit cstorage() : basic_storage(sizeof(T)) {};
                ~cstorage() {};

            public:
                inline T *append() { return reinterpret_cast<T *>(basic_storage::alloc_item()); }
                inline T *add() { return reinterpret_cast<T *>(basic_storage::alloc_item()); }

                inline T *append(const T *v)
                {
                    T *dst = reinterpret_cast<T *>(basic_storage::alloc_item());
                    if (dst != NULL)
                        *dst = *v;
                    return dst;
                }

                inline T *push(const T *v)
                {
                    T *dst = reinterpret_cast<T *>(basic_storage::alloc_item());
                    if (dst != NULL)
                        *dst = *v;
                    return dst;
                }

                inline T *push() { return reinterpret_cast<T *>(basic_storage::alloc_item()); }

                inline T *append(const T *v, size_t n)
                {
                    T *dst = reinterpret_cast<T *>(basic_storage::alloc_items(n));
                    if (dst != NULL)
                        ::memcpy(dst, v, n*sizeof(T));
                    return dst;
                }

                inline T *append(T v)
                {
                    T *dst = reinterpret_cast<T *>(basic_storage::alloc_item());
                    if (dst != NULL)
                        *dst = v;
                    return dst;
                }

                inline T *add(const T *v)
                {
                    T *dst = reinterpret_cast<T *>(basic_storage::alloc_item());
                    if (dst != NULL)
                        *dst = *v;
                    return dst;
                }

                inline T *add(T v)
                {
                    T *dst = reinterpret_cast<T *>(basic_storage::alloc_item());
                    if (dst != NULL)
                        *dst = v;
                    return dst;
                }

                inline T *get(size_t idx) { return reinterpret_cast<T *>(basic_storage::get_item(idx)); }

                inline T *c_get(size_t idx) { return reinterpret_cast<T *>(basic_storage::c_get_item(idx)); }

                inline T *operator[](size_t index) { return reinterpret_cast<T *>(basic_storage::get_item(index)); }

                inline T *append_n(size_t n) { return (n == 0) ? NULL : reinterpret_cast<T *>(basic_storage::alloc_items(n)); }

                inline T *at(size_t index) { return reinterpret_cast<T *>(basic_storage::at(index)); }

                inline T *c_at(size_t index) { return reinterpret_cast<T *>(basic_storage::c_at(index)); }

                inline T *first() { return reinterpret_cast<T *>(basic_storage::first()); }

                inline T *get_array() { return reinterpret_cast<T *>(basic_storage::first()); }

                inline const T *get_array() const { return const_cast< cstorage<T> *>(this)->first(); }

                inline T *last() { return reinterpret_cast<T *>(basic_storage::last()); }

                inline T *insert(size_t idx) { return reinterpret_cast<T *>(basic_storage::insert_items(idx, 1)); }

                inline T *insert(size_t idx, const T *v)
                {
                    T *dst = reinterpret_cast<T *>(basic_storage::insert_items(idx, 1));
                    if (dst != NULL)
                        *dst = *v;
                    return dst;
                }

                inline T *insert_n(size_t idx, size_t n) { return reinterpret_cast<T *>(basic_storage::insert_items(idx, n)); }

                inline T *insert_n(size_t idx, const T *v, size_t n)
                {
                    T *dst = reinterpret_cast<T *>(basic_storage::insert_items(idx, n));
                    if (dst != NULL)
                        ::memcpy(dst, v, n*sizeof(T));
                    return dst;
                }

                inline bool remove(size_t idx) { return basic_storage::remove(idx); }

                inline bool remove(const T *ptr) { return basic_storage::remove(ptr); }

                inline bool remove(size_t idx, T *dst)
                {
                    void *p = basic_storage::get_item(idx);
                    if (p == NULL)
                        return false;
                    *dst = *(reinterpret_cast<T *>(p));
                    return basic_storage::remove(idx);
                }

                inline bool remove_n(size_t idx, size_t n) { return basic_storage::remove_n(idx, n); }

                inline bool pop(T *dst)
                {
                    T *src = reinterpret_cast<T *>(basic_storage::pop_last());
                    if (src == NULL)
                        return false;
                    *dst = *src;
                    return true;
                }

                inline bool remove_last()
                {
                    void *ptr = basic_storage::pop_last();
                    return ptr != NULL;
                }

                inline bool pop()
                {
                    void *ptr = basic_storage::pop_last();
                    return ptr != NULL;
                }

                inline ssize_t indexof(const T *ptr)
                {
                    T *p        = reinterpret_cast<T *>(vItems);
                    if (p == NULL)
                        return -1;
                    ssize_t idx = ptr - p;
                    return ((idx < 0) || (idx >= ssize_t(nItems))) ? -1 : idx;
                }

                inline void swap(cstorage<T> *src) { do_swap_data(src); }

                inline bool add_all(const T *src, size_t count) {
                    if (count <= 0)
                        return true;
                    T *ptr = append_n(count);
                    ::memcpy(ptr, src, count * nSizeOf);
                    return true;
                }

                inline bool add_all(const cstorage<T> *src) {
                    if (src->nItems <= 0)
                        return true;
                    T *ptr = append_n(src->nItems);
                    ::memcpy(ptr, src->vItems, src->nItems * nSizeOf);
                    return true;
                }
        };
}

#endif /* DATA_CSTORAGE_H_ */
