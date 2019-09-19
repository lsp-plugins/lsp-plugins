/*
 * types.cpp
 *
 *  Created on: 19 сент. 2019 г.
 *      Author: sadko
 */

#include <core/calc/types.h>

namespace lsp
{
    namespace calc
    {
        status_t copy_value(value_t *dst, const value_t *src)
        {
            if ((src->type == VT_STRING) && (src->v_str != NULL))
            {
                LSPString *copy = src->v_str->clone();
                if (copy == NULL)
                    return STATUS_NO_MEM;

                dst->type       = VT_STRING;
                dst->v_str      = copy;
            }
            else
                *dst        = *src;
            return STATUS_OK;
        }

        void destroy_value(value_t *value)
        {
            if ((value->type == VT_STRING) && (value->v_str != NULL))
            {
                delete value->v_str;
                value->v_str        = NULL;
            }
            value->type     = VT_UNDEF;
        }
    }
}


