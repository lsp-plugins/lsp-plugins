/*
 * types.h
 *
 *  Created on: 16 сент. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_CALC_TYPES_H_
#define CORE_CALC_TYPES_H_

#include <common/types.h>
#include <core/LSPString.h>

namespace lsp
{
    namespace calc
    {
        enum value_type_t
        {
            VT_UNDEF,       // Value is undefined
            VT_NULL,        // Value is defined but is NULL

            VT_INT,         // Value is of integer type
            VT_FLOAT,       // Value is of float type
            VT_STRING,      // Value is of string type
            VT_BOOL         // Value is of boolean type
        };

        typedef struct value_t
        {
            value_type_t    type;
            union
            {
                ssize_t         v_int;
                double          v_float;
                bool            v_bool;
                LSPString      *v_str;
            };
        } value_t;

        void        init_value(value_t *dst);
        status_t    copy_value(value_t *dst, const value_t *src);
        void        destroy_value(value_t *value);

        status_t    fetch_int(ssize_t *dst, const value_t *v);
        status_t    fetch_float(float *dst, const value_t *v);
        status_t    fetch_bool(bool *dst, const value_t *v);
        status_t    fetch_string(LSPString *dst, const value_t *v);

        status_t    cast_int(value_t *v);
        status_t    cast_float(value_t *v);
        status_t    cast_bool(value_t *v);
        status_t    cast_string(value_t *v);
        status_t    cast_string_ext(value_t *v);

        status_t    cast_numeric(value_t *v);
    }
}

#endif /* CORE_CALC_TYPES_H_ */
