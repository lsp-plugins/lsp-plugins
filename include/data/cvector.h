/*
 * cvector.h
 *
 *  Created on: 03 дек. 2015 г.
 *      Author: sadko
 */

#ifndef DATA_CVECTOR_H_
#define DATA_CVECTOR_H_

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define CVECTOR_GROW        16

namespace lsp
{
    class basic_vector
    {
        protected:
            void      **pvItems;
            size_t      nCapacity;
            size_t      nItems;

        protected:
            inline bool add_item(const void *ptr)
            {
                if (nItems >= nCapacity)
                {
                    void *data      = ::realloc(pvItems, sizeof(void *) * (nCapacity + CVECTOR_GROW));
                    if (data == NULL)
                        return false;

                    pvItems         = static_cast<void **>(data);
                    nCapacity      += CVECTOR_GROW;
                }

                pvItems[nItems++]   = const_cast<void *>(ptr);
                return true;
            }

            inline bool add_unique(const void *ptr)
            {
                for (size_t i=0; i<nItems; ++i)
                    if (pvItems[i] == ptr)
                        return true;
                return add_item(ptr);
            }

            inline bool insert_item(const void *ptr, size_t idx)
            {
                if (nItems >= nCapacity)
                {
                    void *data      = ::realloc(pvItems, sizeof(void *) * (nCapacity + CVECTOR_GROW));
                    if (data == NULL)
                        return false;

                    pvItems         = static_cast<void **>(data);
                    nCapacity      += CVECTOR_GROW;
                }

                if (idx >= nItems)
                {
                    if (idx > nItems)
                        return false;
                    pvItems[nItems] = const_cast<void *>(ptr);
                }
                else
                {
                    ::memmove(&pvItems[idx+1], &pvItems[idx], (nItems - idx) * sizeof(void *));
                    pvItems[idx]    = const_cast<void *>(ptr);
                }
                ++nItems;
                return true;
            }

            inline void *get_item(size_t index) const
            {
                return (index < nItems) ? const_cast<void *>(pvItems[index]) : NULL;
            }

            inline bool set_item(size_t index, void *ptr)
            {
                if (index >= nItems)
                    return false;
                pvItems[index] = ptr;
                return true;
            }

            inline bool do_remove(size_t i, bool fast)
            {
                if (i < (--nItems))
                {
                    if (fast)
                        pvItems[i]  = pvItems[nItems];
                    else
                        ::memmove(&pvItems[i], &pvItems[i+1], (nItems - i) * sizeof(void *));
                }

                pvItems[nItems] = NULL;
                return true;
            }

            inline bool remove_item(const void *item, bool fast)
            {
                for (size_t i=0; i<nItems; ++i)
                {
                    if (pvItems[i] == item)
                        return do_remove(i, fast);
                }
                return false;
            }

            inline bool remove_items(size_t first, size_t count)
            {
                size_t last = first + count;
                if (last == nItems)
                {
                    nItems  = first;
                    return true;
                }
                else if (last > nItems)
                    return false;

                ::memmove(&pvItems[first], &pvItems[last], (nItems - last) * sizeof(void *));
                nItems  -= count;
                return true;
            }

            inline bool remove_item(size_t index, bool fast)
            {
                if (index >= nItems)
                    return false;
                return do_remove(index, fast);
            }

            inline void do_swap_data(basic_vector *src)
            {
                void **ptr      = src->pvItems;
                size_t cap      = src->nCapacity;
                size_t n        = src->nItems;

                src->pvItems    = pvItems;
                src->nCapacity  = nCapacity;
                src->nItems     = nItems;

                pvItems         = ptr;
                nCapacity       = cap;
                nItems          = n;
            }

            inline void do_take_from(basic_vector *src)
            {
                flush();

                pvItems         = src->pvItems;
                nCapacity       = src->nCapacity;
                nItems          = src->nItems;

                src->pvItems    = NULL;
                src->nCapacity  = 0;
                src->nItems     = 0;
            }

            inline bool do_copy_from(const basic_vector *src)
            {
                if (nCapacity < src->nItems)
                {
                    size_t cap      = ((src->nItems + CVECTOR_GROW - 1) / CVECTOR_GROW) * CVECTOR_GROW;
                    void *data      = ::realloc(pvItems, sizeof(void *) * (cap));
                    if (data == NULL)
                        return false;

                    pvItems         = static_cast<void **>(data);
                    nCapacity       = cap;
                }
                ::memcpy(pvItems, src->pvItems, sizeof(void *) * src->nItems);
                nItems          = src->nItems;
                return true;
            }

            inline bool add_all(const void *const *src, size_t count)
            {
                size_t n = nItems + count;
                if (nCapacity < n)
                {
                    size_t cap      = ((n + CVECTOR_GROW - 1) / CVECTOR_GROW) * CVECTOR_GROW;
                    void *data      = ::realloc(pvItems, sizeof(void *) * (cap));
                    if (data == NULL)
                        return false;

                    pvItems         = static_cast<void **>(data);
                    nCapacity       = cap;
                }

                ::memcpy(&pvItems[nItems], src, sizeof(void *) * count);
                nItems          = n;
                return true;
            }

            inline ssize_t do_index_of(const void *ptr) const
            {
                for (size_t i=0; i<nItems; ++i)
                {
                    if (pvItems[i] == ptr)
                        return i;
                }
                return -1;
            }

            inline bool pop_last(void **ptr)
            {
                if (nItems <= 0)
                    return false;

                void *p = pvItems[--nItems];
                if (ptr != NULL)
                    *ptr = p;
                pvItems[nItems] = NULL; // Replace with NULL
                return true;
            }

            inline bool pop()
            {
                if (nItems <= 0)
                    return false;

                pvItems[--nItems] = NULL;
                return true;
            }

        public:
            explicit inline basic_vector()
            {
                pvItems     = NULL;
                nCapacity   = 0;
                nItems      = 0;
            }

            inline ~basic_vector()
            {
                flush();
            }

            inline size_t size() const  { return nItems; }

            inline size_t capacity() const { return nCapacity; }

            inline void clear() { nItems = 0; }

            inline bool swap(size_t a, size_t b)
            {
                if ((a >= nItems) || (b >= nItems))
                    return false;

                void *ptr   = pvItems[a];
                pvItems[a]  = pvItems[b];
                pvItems[b]  = ptr;
                return true;
            }

            inline void swap_unsafe(size_t a, size_t b)
            {
                void *ptr   = pvItems[a];
                pvItems[a]  = pvItems[b];
                pvItems[b]  = ptr;
            }

            void flush()
            {
                if (pvItems != NULL)
                {
                    ::free(pvItems);
                    pvItems      = NULL;
                }
                nCapacity   = 0;
                nItems      = 0;
            }

            inline bool move(size_t a, size_t b)
            {
                if ((a >= nItems) || (b >= nItems))
                    return false;
                else if (a == b)
                    return true;

                void *ptr   = pvItems[a];
                if (a < b)
                    ::memmove(&pvItems[a], &pvItems[a+1], (b-a) * sizeof(void *));
                else
                    ::memmove(&pvItems[b+1], &pvItems[b], (a-b) * sizeof(void *));
                pvItems[b]  = ptr;

                return true;
            }
    };

    // Generalize pointers with templates
    template <class T>
        class cvector: public basic_vector
        {
            private:
                cvector(const cvector<T> &src);                         // Disable copying
                cvector<T> & operator = (const cvector<T> & src);       // Disable copying

            public:
                explicit inline cvector() {}

            public:
                inline T **get_array() { return (nItems > 0) ? reinterpret_cast<T **>(pvItems) : NULL; }

                inline const T * const *get_const_array() const { return (nItems > 0) ? reinterpret_cast<const T * const *>(pvItems) : NULL; }

                inline bool add(T *item) { return basic_vector::add_item(item); }

                inline bool add_all(const T * const *items, size_t count) {
                    union {
                        const T * const *titems;
                        const void * const *vitems;
                    } x;
                    x.titems = items;
                    return basic_vector::add_all(x.vitems, count);
                }

                inline bool add_all(const cvector<T> *items) {
                    union {
                        const T * const *titems;
                        const void * const *vitems;
                    } x;
                    x.titems = items->get_const_array();
                    return basic_vector::add_all(x.vitems, items->size());
                }

                inline bool push(T *item) { return basic_vector::add_item(item); }

                inline bool pop() { return basic_vector::pop(); }

                inline bool pop(T **item)
                {
                    void *ptr;
                    if (!basic_vector::pop_last(&ptr))
                        return false;
                    *item = reinterpret_cast<T *>(ptr);
                    return true;
                }

                inline bool pop_last(T **item)
                {
                    void *ptr;
                    if (!basic_vector::pop_last(&ptr))
                        return false;
                    *item = reinterpret_cast<T *>(ptr);
                    return true;
                }

                inline T *last() { return (nItems > 0) ? reinterpret_cast<T *>(pvItems[nItems-1]) : NULL; }

                inline T *last() const { return (nItems > 0) ? reinterpret_cast<const T *>(pvItems[nItems-1]) : NULL; }

                inline T *first() { return (nItems > 0) ? reinterpret_cast<T *>(pvItems[0]) : NULL; }

                inline T *first() const { return (nItems > 0) ? reinterpret_cast<const T *>(pvItems[0]) : NULL; }

                inline bool add_unique(T *item) { return basic_vector::add_unique(item); }

                inline bool insert(T *item, size_t index) { return basic_vector::insert_item(item, index); }

                inline T *get(size_t index) const { return reinterpret_cast<T *>(basic_vector::get_item(index)); }

                inline bool set(size_t index, T *item) { return basic_vector::set_item(index, item); }

                inline bool remove(const T *item, bool fast = false) { return basic_vector::remove_item(item, fast); }

                inline bool remove(size_t index, bool fast = false) { return basic_vector::remove_item(index, fast); };

                inline bool remove_n(size_t index, size_t count) { return basic_vector::remove_items(index, count); };

                inline T *operator[](size_t index) { return reinterpret_cast<T *>(basic_vector::get_item(index)); }

                inline T *at(size_t index) const { return reinterpret_cast<T *>(pvItems[index]); }

                inline T **release()
                {
                    if (nItems <= 0)
                    {
                        flush();
                        return NULL;
                    }
                    T **res     = (nItems > 0) ? reinterpret_cast<T **>(pvItems) : NULL;
                    pvItems     = NULL;
                    nCapacity   = 0;
                    nItems      = 0;
                    return res;
                }

                inline void swap_data(cvector<T> *src) { do_swap_data(src); }

                inline void take_from(cvector<T> *src) { do_take_from(src); }

                inline ssize_t index_of(const T *item) const { return do_index_of(item); }

                inline bool copy_from(const cvector<T> *src) { return do_copy_from(src); }
        };

}



#endif /* DATA_CVECTOR_H_ */
