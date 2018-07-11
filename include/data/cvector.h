/*
 * cvector.h
 *
 *  Created on: 03 дек. 2015 г.
 *      Author: sadko
 */

#ifndef DATA_CVECTOR_H_
#define DATA_CVECTOR_H_

#include <stddef.h>

namespace lsp
{
    class basic_vector
    {
        private:
            void      **pvItems;
            size_t      nCapacity;
            size_t      nItems;

        protected:
            inline bool add_item(void *ptr)
            {
                if (nItems >= nCapacity)
                {
                    void **ptrs = new void *[nCapacity + 16];
                    if (ptrs == NULL)
                        return false;
                    for (size_t i=0; i<nItems; ++i)
                        ptrs[i]         = pvItems[i];

                    delete [] pvItems;
                    pvItems         = ptrs;
                    nCapacity      += 16;
                }

                pvItems[nItems++]   = ptr;
                return true;
            }

            inline void *get_item(size_t index)
            {
                return (index < nItems) ? pvItems[index] : NULL;
            }

        public:
            explicit inline basic_vector()
            {
                pvItems      = NULL;
                nCapacity   = 0;
                nItems      = 0;
            }

            inline ~basic_vector()
            {
                if (pvItems != NULL)
                {
                    delete [] pvItems;
                    pvItems      = NULL;
                }
                nCapacity   = 0;
                nItems      = 0;
            }

            inline size_t size() const  { return nItems; }

            inline size_t capacity() const { return nCapacity; }

            inline void clear() { nItems = 0; }
    };

    // Generalize pointers with templates
    template <class T>
        class cvector: public basic_vector
        {
            public:
                inline bool add(T *item) { return basic_vector::add_item(item); }
                inline T *get(size_t index) { return reinterpret_cast<T *>(basic_vector::get_item(index)); }

                inline T *operator[](size_t index) { return reinterpret_cast<T *>(basic_vector::get_item(index)); }
        };

}



#endif /* DATA_CVECTOR_H_ */
