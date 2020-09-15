/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 2 авг. 2017 г.
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
