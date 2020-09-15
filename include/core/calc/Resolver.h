/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 16 сент. 2019 г.
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

#ifndef CORE_CALC_RESOLVER_H_
#define CORE_CALC_RESOLVER_H_

#include <core/calc/types.h>

namespace lsp
{
    namespace calc
    {
        /**
         * This is a class that resolves the actual value by the variable name.
         * By default, it resolves all values as NULLs
         *
         */
        class Resolver
        {
            private:
                Resolver & operator = (const Resolver &);

            public:
                explicit Resolver();
                virtual ~Resolver();

            public:
                /**
                 * Resolve array variable by name and indexes
                 * @param value pointer to value to store the data
                 * @param name array variable name
                 * @param num_indexes number of indexes in array
                 * @param indexes pointer to array containing all index values
                 * @return status of operation
                 */
                virtual status_t resolve(value_t *value, const char *name, size_t num_indexes = 0, const ssize_t *indexes = NULL);

                /**
                 * Resolve array variable by name and indexes
                 * @param value pointer to value to store the data
                 * @param name array variable name
                 * @param num_indexes number of indexes in array
                 * @param indexes pointer to array containing all index values
                 * @return status of operation
                 */
                virtual status_t resolve(value_t *value, const LSPString *name, size_t num_indexes = 0, const ssize_t *indexes = NULL);
        };
    
    } /* namespace calc */
} /* namespace lsp */

#endif /* CORE_CALC_RESOLVER_H_ */
