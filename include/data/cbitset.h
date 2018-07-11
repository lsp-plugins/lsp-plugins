/*
 * cbitset.h
 *
 *  Created on: 2 авг. 2017 г.
 *      Author: sadko
 */

#ifndef DATA_CBITSET_H_
#define DATA_CBITSET_H_

#include <core/types.h>

namespace lsp
{
    class cbitset
    {
        protected:
            size_t      nSize;          // Size of set in bits
            size_t      nCapacity;      // Capacity of set in machine words
            size_t      nMask;
            umword_t   *vData;

        public:
            cbitset(bool dfl = false);
            ~cbitset();

        public:
            inline  size_t  size() const     { return nSize; };
            void    fill_all(bool set);
            void    clear_all();
            void    set_all();

            bool    is_set(size_t idx);
            bool    is_unset(size_t idx);

            bool    resize(size_t size);

    };
}

#endif /* DATA_CBITSET_H_ */
