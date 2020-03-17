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

        /**
         * Initialize value with VT_UNDEF type
         * @param dst destination value
         */
        void        init_value(value_t *dst);

        /**
         * Initialize value with another value by copying contents
         * @param dst destination value
         * @param src source value to take data from
         * @return status of operation
         */
        status_t    init_value(value_t *dst, const value_t *src);

        /**
         * Copy value. Frees previously used value if it was set
         * @param dst destination value to perform copy
         * @param src source value to take data from
         * @return status of operation
         */
        status_t    copy_value(value_t *dst, const value_t *src);

        /**
         * Set value to NULL
         * @param dst target to set
         */
        void        set_value_null(value_t *dst);

        /**
         * Set value to UNDEF
         * @param dst target to set
         */
        void        set_value_undef(value_t *dst);

        /**
         * Set value to integer
         * @param dst target to set
         * @param value integer value to set
         */
        void        set_value_int(value_t *dst, ssize_t value);

        /**
         * Set value to floating point
         * @param dst target to set
         * @param value floating-point value to set
         */
        void        set_value_float(value_t *dst, double value);

        /**
         * Set value to boolean
         * @param dst target to set
         * @param value boolean value to set
         */
        void        set_value_bool(value_t *dst, bool value);

        /**
         * Set value to boolean
         * @param dst target to set
         * @param value string value to set
         */
        status_t    set_value_string(value_t *dst, LSPString *value);

        /**
         * Destroy value and all internal contents associated with it
         * The value remains valid and available for further operations but set to UNDEF
         * @param value value to destroy
         */
        void        destroy_value(value_t *value);

        status_t    fetch_int(ssize_t *dst, const value_t *v);
        status_t    fetch_float(float *dst, const value_t *v);
        status_t    fetch_bool(bool *dst, const value_t *v);
        status_t    fetch_string(LSPString *dst, const value_t *v);

        status_t    cast_value(value_t *v, value_type_t type);

        status_t    cast_int(value_t *v);
        status_t    cast_float(value_t *v);
        status_t    cast_bool(value_t *v);
        status_t    cast_string(value_t *v);
        status_t    cast_string_ext(value_t *v);

        status_t    cast_numeric(value_t *v);
    }
}

#endif /* CORE_CALC_TYPES_H_ */
