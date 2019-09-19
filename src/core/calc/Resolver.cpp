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
    
        status_t Resolver::resolve(value_t *value, const char *name, size_t num_indexes, const ssize_t *indexes)
        {
            value->type     = VT_NULL;
            return STATUS_OK;
        }

        status_t Resolver::resolve(value_t *value, const LSPString *name, size_t num_indexes, const ssize_t *indexes)
        {
            return resolve(value, name->get_utf8(), num_indexes, indexes);
        }

    } /* namespace calc */
} /* namespace lsp */
