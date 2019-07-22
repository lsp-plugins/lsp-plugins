/*
 * cstorage.h
 *
 *  Created on: 9 окт. 2017 г.
 *      Author: sadko
 */

#ifndef DATA_CASTORAGE_H_
#define DATA_CASTORAGE_H_

#include <core/types.h>
#include <core/alloc.h>
#include <string.h>

namespace lsp
{
    class basic_aligned_storage
    {
        protected:
            uint8_t    *vItems;
            size_t      nCapacity;
            size_t      nItems;
            size_t      nSizeOf;
            size_t      nAlign;
            uint8_t    *vData;

        protected:
            bool realloc_capacity(size_t capacity)
            {
                if (capacity < 32)
                    capacity        = 32;

                // Do aligned (re)allocation
                size_t alloc    = nSizeOf * capacity + nAlign;

                uint8_t *ptr    = lsp_trealloc(uint8_t, vData, alloc);
                if (ptr == NULL)
                    return false;
                else if (ptr == vData)
                {
                    nCapacity   = capacity;
                    return true;
                }

                uint8_t *p      = ALIGN_PTR(ptr, nAlign);
                size_t s_diff   = vItems - vData;
                size_t d_diff   = p - ptr;

                if (s_diff != d_diff)
                    ::memmove(&ptr[d_diff], &ptr[s_diff], nSizeOf * nCapacity);

                // Update internal data
                vData           = ptr;
                vItems          = p;
                nCapacity   = capacity;
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
                    ::memmove(&vItems[(index+n) * nSizeOf], res, (nItems - index)*nSizeOf);
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

            inline void do_swap_data(basic_aligned_storage *src)
            {
                uint8_t *_vItems    = src->vItems;
                size_t  _nCapacity  = src->nCapacity;
                size_t  _nItems     = src->nItems;
                size_t  _nSizeOf    = src->nSizeOf;
                size_t  _nAlign     = src->nAlign;
                uint8_t *_vData     = src->vData;

                src->vItems     = vItems;
                src->nCapacity  = nCapacity;
                src->nItems     = nItems;
                src->nSizeOf    = nSizeOf;
                src->nAlign     = nAlign;
                src->vData      = vData;

                vItems          = _vItems;
                nCapacity       = _nCapacity;
                nItems          = _nItems;
                nSizeOf         = _nSizeOf;
                nAlign          = _nAlign;
                vData           = _vData;
            }

        public:
            inline basic_aligned_storage(size_t sz, size_t align)
            {
                vItems      = NULL;
                nCapacity   = 0;
                nItems      = 0;
                nSizeOf     = sz;
                nAlign      = align;
                vData       = NULL;
            }

            inline ~basic_aligned_storage()
            {
                flush();
            }

            void flush()
            {
                if (vData != NULL)
                {
                    lsp_free(vData);
                    vData   = NULL;
                }
                vItems      = NULL;
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

    template <class T, size_t A=DEFAULT_ALIGN>
        class castorage: public basic_aligned_storage
        {
            private:
                castorage(const castorage<T, A> &src);                          // Disable copying
                castorage<T, A> & operator = (const castorage<T, A> & src);     // Disable copying

            public:
                castorage() : basic_aligned_storage(sizeof(T), A) {};
                ~castorage() {};

            public:
                inline T *append() { return reinterpret_cast<T *>(basic_aligned_storage::alloc_item()); }
                inline T *add() { return reinterpret_cast<T *>(basic_aligned_storage::alloc_item()); }

                inline T *append(const T *v)
                {
                    T *dst = reinterpret_cast<T *>(basic_aligned_storage::alloc_item());
                    if (dst != NULL)
                        *dst = *v;
                    return dst;
                }

                inline T *append(const T *v, size_t n)
                {
                    T *dst = reinterpret_cast<T *>(basic_aligned_storage::alloc_item());
                    if (dst != NULL)
                        ::memcpy(dst, v, n*sizeof(T));
                    return dst;
                }

                inline T *append(T v)
                {
                    T *dst = reinterpret_cast<T *>(basic_aligned_storage::alloc_item());
                    if (dst != NULL)
                        *dst = v;
                    return dst;
                }

                inline T *add(const T *v)
                {
                    T *dst = reinterpret_cast<T *>(basic_aligned_storage::alloc_item());
                    if (dst != NULL)
                        *dst = *v;
                    return dst;
                }

                inline T *add(T v)
                {
                    T *dst = reinterpret_cast<T *>(basic_aligned_storage::alloc_item());
                    if (dst != NULL)
                        *dst = v;
                    return dst;
                }

                inline T *get(size_t idx) { return reinterpret_cast<T *>(basic_aligned_storage::get_item(idx)); }

                inline T *c_get(size_t idx) { return reinterpret_cast<T *>(basic_aligned_storage::c_get_item(idx)); }

                inline T *operator[](size_t index) { return reinterpret_cast<T *>(basic_aligned_storage::get_item(index)); }

                inline T *append_n(size_t n) { return (n == 0) ? NULL : reinterpret_cast<T *>(basic_aligned_storage::alloc_items(n)); }

                inline T *at(size_t index) { return reinterpret_cast<T *>(basic_aligned_storage::at(index)); }

                inline T *c_at(size_t index) { return reinterpret_cast<T *>(basic_aligned_storage::c_at(index)); }

                inline T *first() { return reinterpret_cast<T *>(basic_aligned_storage::first()); }

                inline T *get_array() { return reinterpret_cast<T *>(basic_aligned_storage::first()); }

                inline T *last() { return reinterpret_cast<T *>(basic_aligned_storage::last()); }

                inline T *insert(size_t idx) { return reinterpret_cast<T *>(basic_aligned_storage::insert_items(idx, 1)); }

                inline T *insert(size_t idx, const T *v)
                {
                    T *dst = reinterpret_cast<T *>(basic_aligned_storage::insert_items(idx, 1));
                    if (dst != NULL)
                        *dst = *v;
                    return dst;
                }

                inline T *insert_n(size_t idx, size_t n) { return reinterpret_cast<T *>(basic_aligned_storage::insert_items(idx, n)); }

                inline T *insert_n(size_t idx, const T *v, size_t n)
                {
                    T *dst = reinterpret_cast<T *>(basic_aligned_storage::insert_items(idx, n));
                    if (dst != NULL)
                        ::memcpy(dst, v, n*sizeof(T));
                    return dst;
                }

                inline bool remove(size_t idx) { return basic_aligned_storage::remove(idx); }

                inline bool remove(size_t idx, T *dst)
                {
                    void *p = basic_aligned_storage::get_item(idx);
                    if (p == NULL)
                        return false;
                    *dst = *(reinterpret_cast<T *>(p));
                    return basic_aligned_storage::remove(idx);
                }

                inline bool remove_n(size_t idx, size_t n) { return basic_aligned_storage::remove_n(idx, n); }

                inline bool pop(T *dst)
                {
                    T *src = reinterpret_cast<T *>(basic_aligned_storage::pop_last());
                    if (src == NULL)
                        return false;
                    *dst = *src;
                    return true;
                }

                inline bool remove_last()
                {
                    void *ptr = basic_aligned_storage::pop_last();
                    return ptr != NULL;
                }

                inline bool pop()
                {
                    void *ptr = basic_aligned_storage::pop_last();
                    return ptr != NULL;
                }

                inline ssize_t indexof(const T *ptr)
                {
                    T *p        = reinterpret_cast<T *>(vItems);
                    if (p == NULL)
                        return -1;
                    ssize_t idx = ptr - p;
                    return ((idx < 0) || (idx >= nItems)) ? -1 : idx;
                }

                inline void swap(castorage<T, A> *src) { do_swap_data(src); }
        };
}

#endif /* DATA_CASTORAGE_H_ */
