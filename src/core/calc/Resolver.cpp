/*
 * Resolver.cpp
 *
 *  Created on: 16 сент. 2019 г.
 *      Author: sadko
 */

#include <core/calc/Resolver.h>

namespace lsp
{
    namespace calc
    {
        
        Resolver::Resolver()
        {
        }
        
        Resolver::~Resolver()
        {
        }
    
        status_t Resolver::resolve(value_t *value, const char *name)
        {
            value->type     = VT_NULL;
            return STATUS_OK;
        }

        status_t Resolver::resolve(value_t *value, const char *name, size_t num_indexes, const size_t *indexes)
        {
            value->type     = VT_NULL;
            return STATUS_OK;
        }

    } /* namespace calc */
} /* namespace lsp */
