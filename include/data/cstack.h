/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 28 мар. 2016 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef DATA_CSTACK_H_
#define DATA_CSTACK_H_

#include <stddef.h>

namespace lsp
{
    class basic_stack
    {
        private:
            void      **pvItems;
            size_t      nCapacity;
            size_t      nItems;

        protected:
            inline bool push(const void *ptr)
            {
                if (nItems >= nCapacity)
                {
                    void **ptrs = new void *[nCapacity + 16];
                    if (ptrs == NULL)
                        return false;
                    if (pvItems != NULL)
                    {
                        for (size_t i=0; i<nItems; ++i)
                            ptrs[i]         = pvItems[i];
                        delete [] pvItems;
                    }
                    pvItems         = ptrs;
                    nCapacity      += 16;
                }

                pvItems[nItems++]   = const_cast<void *>(ptr);
                return true;
            }

            inline void *pop()
            {
                if (nItems <= 0)
                    return NULL;
                void *result        = pvItems[--nItems];
                pvItems[nItems]     = NULL;
                return result;
            }

            inline void *get_item(size_t index)
            {
                return (index < nItems) ? pvItems[index] : NULL;
            }

            inline void *top()
            {
                return (nItems > 0) ? pvItems[nItems - 1] : NULL;
            }

        public:
            explicit inline basic_stack()
            {
                pvItems     = NULL;
                nCapacity   = 0;
                nItems      = 0;
            }

            inline ~basic_stack()
            {
                flush();
            }

            inline size_t size() const  { return nItems; }

            inline size_t capacity() const { return nCapacity; }

            inline void clear() { nItems = 0; }

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
        class cstack: public basic_stack
        {
            private:
                cstack(const cstack<T> &src);                           // Disable copying
                cstack<T> & operator = (const cstack<T> & src);         // Disable copying

            public:
                explicit cstack() {}

            public:
                inline bool push(T *item)   { return basic_stack::push(item); }
                inline T *pop()             { return reinterpret_cast<T *>(basic_stack::pop());  }
                inline T *top()             { return reinterpret_cast<T *>(basic_stack::top()); }

                inline T *get(size_t index) { return reinterpret_cast<T *>(basic_stack::get_item(index)); }
                inline T *operator[](size_t index) { return reinterpret_cast<T *>(basic_stack::get_item(index)); }

                inline T **get_array() { return (nItems > 0) ? reinterpret_cast<T **>(pvItems) : NULL; }
        };

}



#endif /* DATA_CSTACK_H_ */
