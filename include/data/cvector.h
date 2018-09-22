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
                    void *data      = realloc(pvItems, sizeof(void *) * (nCapacity + CVECTOR_GROW));
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
                    void *data      = realloc(pvItems, sizeof(void *) * (nCapacity + CVECTOR_GROW));
                    if (data == NULL)
                        return false;

                    pvItems         = static_cast<void **>(data);
                    nCapacity      += CVECTOR_GROW;
                }

                if (idx >= nItems)
                {
                    if (idx > nItems)
                        return false;
                    pvItems[nItems++]   = const_cast<void *>(ptr);
                }
                else
                {
                    memmove(&pvItems[idx+1], &pvItems[idx], (nItems - idx) * sizeof(void *));
                    pvItems[idx]    = const_cast<void *>(ptr);
                }
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
                        memmove(&pvItems[i], &pvItems[i+1], (nItems - i) * sizeof(void *));
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

            inline ssize_t do_index_of(const void *ptr)
            {
                for (size_t i=0; i<nItems; ++i)
                {
                    if (pvItems[i] == ptr)
                        return i;
                }
                return -1;
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
                    free(pvItems);
                    pvItems      = NULL;
                }
                nCapacity   = 0;
                nItems      = 0;
            }
    };

    // Generalize pointers with templates
    template <class T>
        class cvector: public basic_vector
        {
            public:
                inline bool add(T *item) { return basic_vector::add_item(item); }

                inline bool add_unique(T *item) { return basic_vector::add_unique(item); }

                inline bool insert(T *item, size_t index) { return basic_vector::insert_item(item, index); }

                inline T *get(size_t index) const { return reinterpret_cast<T *>(basic_vector::get_item(index)); }

                inline bool set(size_t index, T *item) { return basic_vector::set_item(index, item); }

                inline bool remove(const T *item, bool fast = false) { return basic_vector::remove_item(item, fast); }

                inline bool remove(size_t index, bool fast = false) { return basic_vector::remove_item(index, fast); };

                inline T *operator[](size_t index) { return reinterpret_cast<T *>(basic_vector::get_item(index)); }

                inline T *at(size_t index) { return reinterpret_cast<T *>(pvItems[index]); }

                inline T **get_array() { return (nItems > 0) ? reinterpret_cast<T **>(pvItems) : NULL; }

                inline void swap_data(cvector<T> *src) { do_swap_data(src); }

                inline void take_from(cvector<T> *src) { do_take_from(src); }

                inline ssize_t index_of(const T *item) { return do_index_of(item); }
        };

}



#endif /* DATA_CVECTOR_H_ */
