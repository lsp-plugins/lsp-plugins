/*
 * Resolver.h
 *
 *  Created on: 16 сент. 2019 г.
 *      Author: sadko
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
