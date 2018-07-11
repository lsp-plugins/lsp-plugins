/*
 * cvector.h
 *
 *  Created on: 03 дек. 2015 г.
 *      Author: sadko
 */

#ifndef DATA_CVECTOR_H_
#define DATA_CVECTOR_H_

#include <stddef.h>

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
                    void **ptrs = new void *[nCapacity + CVECTOR_GROW];
                    if (ptrs == NULL)
                        return false;
                    if (pvItems != NULL)
                    {
                        for (size_t i=0; i<nItems; ++i)
                            ptrs[i]         = pvItems[i];
                        delete [] pvItems;
                    }
                    pvItems         = ptrs;
                    nCapacity      += CVECTOR_GROW;
                }

                pvItems[nItems++]   = const_cast<void *>(ptr);
                return true;
            }

            inline bool insert_item(const void *ptr, size_t idx)
            {
                if (idx >= nItems)
                    return add_item(ptr);

                if (nItems >= nCapacity)
                {
                    void **ptrs = new void *[nCapacity + CVECTOR_GROW];
                    if (ptrs == NULL)
                        return false;
                    if (pvItems != NULL)
                    {
                        for (size_t i=0; i<idx; ++i)
                            ptrs[i]         = pvItems[i];
                        ptrs[idx]       = const_cast<void *>(ptr);
                        for (size_t i=idx; i<nItems; ++i)
                            ptrs[i+1]       = pvItems[i];
                        delete [] pvItems;
                    }
                    pvItems         = ptrs;
                    nCapacity      += CVECTOR_GROW;
                }
                else
                {
                    for (size_t i=nItems++; i>idx; --i)
                        pvItems[i]      = pvItems[i-1];
                    pvItems[idx]    = const_cast<void *>(ptr);
                }
                return true;
            }

            inline void *get_item(size_t index)
            {
                return (index < nItems) ? pvItems[index] : NULL;
            }

            inline bool remove_item(const void *item, bool fast)
            {
                for (size_t i=0; i<nItems; ++i)
                {
                    if (pvItems[i] == item)
                    {
                        --nItems;
                        if (fast)
                        {
                            if (i < nItems)
                                pvItems[i]  = pvItems[nItems];
                        }
                        else
                        {
                            for (size_t j=i; j<nItems; ++j)
                                pvItems[j]  = pvItems[j+1];
                        }
                        pvItems[nItems] = NULL;

                        return true;
                    }
                }
                return false;
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
                    delete [] pvItems;
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

                inline bool insert(T *item, size_t index) { return basic_vector::insert_item(item, index); }

                inline T *get(size_t index) { return reinterpret_cast<T *>(basic_vector::get_item(index)); }

                inline bool remove(const T *item, bool fast = false) { return basic_vector::remove_item(item, fast); }

                inline T *operator[](size_t index) { return reinterpret_cast<T *>(basic_vector::get_item(index)); }

                inline T *at(size_t index) { return reinterpret_cast<T *>(pvItems[index]); }
        };

}



#endif /* DATA_CVECTOR_H_ */
