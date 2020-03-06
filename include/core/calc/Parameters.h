/*
 * Parameters.h
 *
 *  Created on: 19 февр. 2020 г.
 *      Author: sadko
 */

#ifndef CORE_CALC_PARAMETERS_H_
#define CORE_CALC_PARAMETERS_H_

#include <data/cvector.h>
#include <core/calc/Resolver.h>

namespace lsp
{
    namespace calc
    {
        /**
         * Implements list of optionally named parameters which then can be used for string formatting.
         * Parameters may have duplicate names. From set of named parameters with the same name the parameter
         * with the least index will be visible by the name.
         */
        class Parameters: public Resolver
        {
            private:
                Parameters & operator = (const Parameters &);

            protected:
                typedef struct param_t
                {
                    value_t         value;
                    ssize_t         len;
                    lsp_wchar_t     name[];
                } param_t;

            protected:
                cvector<param_t>    vParams;

            protected:
                param_t            *lookup_by_name(const LSPString *name);
                param_t            *lookup_by_name(const LSPString *name, size_t *idx);
                static param_t     *allocate();
                static param_t     *allocate(const lsp_wchar_t *name, ssize_t len);
                static param_t     *clone(const param_t *src);
                static void         destroy(param_t *p);
                inline static param_t *allocate(const LSPString *name) { return allocate(name->characters(), name->length()); };

                static void         destroy_params(cvector<param_t> &params);

                status_t            drop_value(size_t index, value_type_t type, param_t **out);
                status_t            drop_value(const char *name, value_type_t type, param_t **out);
                status_t            drop_value(const LSPString *name, value_type_t type, param_t **out);

            protected:
                /**
                 * This callback is called when the collection becomes modified.
                 * Method can be overridden to trigger different events like parent
                 * collection update.
                 */
                virtual void        modified();

            public:
                explicit Parameters();
                virtual ~Parameters();

            public:
                virtual status_t    resolve(value_t *value, const char *name, size_t num_indexes = 0, const ssize_t *indexes = NULL);

                virtual status_t    resolve(value_t *value, const LSPString *name, size_t num_indexes = 0, const ssize_t *indexes = NULL);

            public:
                inline size_t       size() const            { return vParams.size(); }
                void                swap(Parameters *src);
                void                clear();
                Parameters         *clone() const;

                // Adding: add parameter to the end of list.
                status_t            add_int(const char *name, ssize_t value);
                status_t            add_float(const char *name, double value);
                status_t            add_bool(const char *name, bool value);
                status_t            add_cstring(const char *name, const char *value);
                status_t            add_string(const char *name, const LSPString *value);
                status_t            add_null(const char *name);
                status_t            add_undef(const char *name);
                status_t            add(const char *name, const value_t *value);

                status_t            add_int(const LSPString *name, ssize_t value);
                status_t            add_float(const LSPString *name, double value);
                status_t            add_bool(const LSPString *name, bool value);
                status_t            add_cstring(const LSPString *name, const char *value);
                status_t            add_string(const LSPString *name, const LSPString *value);
                status_t            add_null(const LSPString *name);
                status_t            add_undef(const LSPString *name);
                status_t            add(const LSPString *name, const value_t *value);

                status_t            add_int(ssize_t value);
                status_t            add_float(double value);
                status_t            add_bool(bool value);
                status_t            add_cstring(const char *value);
                status_t            add_string(const LSPString *value);
                status_t            add_null();
                status_t            add_undef();
                status_t            add(const value_t *value);

                status_t            add(const Parameters *p, ssize_t first = 0, ssize_t last = -1);

                // Inserting
                status_t            insert_int(size_t index, const char *name, ssize_t value);
                status_t            insert_float(size_t index, const char *name, double value);
                status_t            insert_bool(size_t index, const char *name, bool value);
                status_t            insert_string(size_t index, const char *name, const char *value);
                status_t            insert_string(size_t index, const char *name, const LSPString *value);
                status_t            insert_null(size_t index, const char *name);
                status_t            insert_undef(size_t index, const char *name);
                status_t            insert(size_t index, const char *name, const value_t *value);

                status_t            insert_int(size_t index, const LSPString *name, ssize_t value);
                status_t            insert_float(size_t index, const LSPString *name, double value);
                status_t            insert_bool(size_t index, const LSPString *name, bool value);
                status_t            insert_string(size_t index, const LSPString *name, const char *value);
                status_t            insert_string(size_t index, const LSPString *name, const LSPString *value);
                status_t            insert_null(size_t index, const LSPString *name);
                status_t            insert_undef(size_t index, const LSPString *name);
                status_t            insert(size_t index, const LSPString *name, const value_t *value);

                status_t            insert_int(size_t index, ssize_t value);
                status_t            insert_float(size_t index, double value);
                status_t            insert_bool(size_t index, bool value);
                status_t            insert_cstring(size_t index, const char *value);
                status_t            insert_string(size_t index, const LSPString *value);
                status_t            insert_null(size_t index);
                status_t            insert_undef(size_t index);
                status_t            insert(size_t index, const value_t *value);

                status_t            insert(size_t index, const Parameters *p, ssize_t first = 0, ssize_t last = -1);

                // Getting
                status_t            get_int(size_t index, ssize_t *value) const;
                status_t            get_float(size_t index, double *value) const;
                status_t            get_bool(size_t index, bool *value) const;
                status_t            get_string(size_t index, LSPString *value) const;
                status_t            get_null(size_t index) const;
                status_t            get_undef(size_t index) const;
                status_t            get(size_t index, value_t *value) const;

                status_t            get_int(const char *name, ssize_t *value) const;
                status_t            get_float(const char *name, double *value) const;
                status_t            get_bool(const char *name, bool *value) const;
                status_t            get_string(const char *name, LSPString *value) const;
                status_t            get_null(const char *name) const;
                status_t            get_undef(const char *name) const;
                status_t            get(const char *name, value_t *value) const;

                status_t            get_int(const LSPString *name, ssize_t *value) const;
                status_t            get_float(const LSPString *name, double *value) const;
                status_t            get_bool(const LSPString *name, bool *value) const;
                status_t            get_string(const LSPString *name, LSPString *value) const;
                status_t            get_null(const LSPString *name) const;
                status_t            get_undef(const LSPString *name) const;
                status_t            get(const LSPString *name, value_t *value) const;

                // Getting with cast
                status_t            as_int(size_t index, ssize_t *value) const;
                status_t            as_float(size_t index, double *value) const;
                status_t            as_bool(size_t index, bool *value) const;
                status_t            as_string(size_t index, LSPString *value) const;
                status_t            as_null(size_t index) const;
                status_t            as_undef(size_t index) const;
                status_t            as_value(size_t index, value_t *value, value_type_t type) const;

                status_t            as_int(const char *name, ssize_t *value) const;
                status_t            as_float(const char *name, double *value) const;
                status_t            as_bool(const char *name, bool *value) const;
                status_t            as_string(const char *name, LSPString *value) const;
                status_t            as_null(const char *name) const;
                status_t            as_undef(const char *name) const;
                status_t            as_value(const char *name, value_t *value, value_type_t type) const;

                status_t            as_int(const LSPString *name, ssize_t *value) const;
                status_t            as_float(const LSPString *name, double *value) const;
                status_t            as_bool(const LSPString *name, bool *value) const;
                status_t            as_string(const LSPString *name, LSPString *value) const;
                status_t            as_null(const LSPString *name) const;
                status_t            as_undef(const LSPString *name) const;
                status_t            as_value(const LSPString *name, value_t *value, value_type_t type) const;

                // Setting
                status_t            set_int(const char *name, ssize_t value);
                status_t            set_float(const char *name, double value);
                status_t            set_bool(const char *name, bool value);
                status_t            set_cstring(const char *name, const char *value);
                status_t            set_string(const char *name, const LSPString *value);
                status_t            set_null(const char *name);
                status_t            set_undef(const char *name);
                status_t            set(const char *name, const value_t *value);

                status_t            set_int(const LSPString *name, ssize_t value);
                status_t            set_float(const LSPString *name, double value);
                status_t            set_bool(const LSPString *name, bool value);
                status_t            set_cstring(const LSPString *name, const char *value);
                status_t            set_string(const LSPString *name, const LSPString *value);
                status_t            set_null(const LSPString *name);
                status_t            set_undef(const LSPString *name);
                status_t            set(const LSPString *name, const value_t *value);

                status_t            set_int(size_t index, ssize_t value);
                status_t            set_float(size_t index, double value);
                status_t            set_bool(size_t index, bool value);
                status_t            set_cstring(size_t index, const char *value);
                status_t            set_string(size_t index, const LSPString *value);
                status_t            set_null(size_t index);
                status_t            set_undef(size_t index);
                status_t            set(size_t index, const value_t *value);

                status_t            set(const Parameters *p, ssize_t first = 0, ssize_t last = -1);

                // Removing
                status_t            remove_int(size_t index, ssize_t *value = NULL);
                status_t            remove_float(size_t index, double *value = NULL);
                status_t            remove_bool(size_t index, bool *value = NULL);
                status_t            remove_string(size_t index, LSPString *value = NULL);
                status_t            remove_null(size_t index);
                status_t            remove_undef(size_t index);
                status_t            remove(size_t index, value_t *value = NULL);
                status_t            remove_value(size_t index, value_type_t type, value_t *value);

                status_t            remove_int(const char *name, ssize_t *value = NULL);
                status_t            remove_float(const char *name, double *value = NULL);
                status_t            remove_bool(const char *name, bool *value = NULL);
                status_t            remove_string(const char *name, LSPString *value = NULL);
                status_t            remove_null(const char *name = NULL);
                status_t            remove_undef(const char *name = NULL);
                status_t            remove(const char *name, value_t *value = NULL);
                status_t            remove_value(const char *name, value_type_t type, value_t *value = NULL);

                status_t            remove_int(const LSPString *name, ssize_t *value = NULL);
                status_t            remove_float(const LSPString *name, double *value = NULL);
                status_t            remove_bool(const LSPString *name, bool *value = NULL);
                status_t            remove_string(const LSPString *name, LSPString *value = NULL);
                status_t            remove_null(const LSPString *name = NULL);
                status_t            remove_undef(const LSPString *name = NULL);
                status_t            remove(const LSPString *name, value_t *value = NULL);
                status_t            remove_value(const LSPString *name, value_type_t type, value_t *value = NULL);

                status_t            remove(ssize_t first, ssize_t last);

                // Obtaining type of parameter (negative result on error)
                ssize_t             get_type(size_t index) const;
                ssize_t             get_type(const char *name) const;
                ssize_t             get_type(const LSPString *name) const;

                // Obtaining name of parameter
                status_t            get_name(size_t index, LSPString *name) const;
                ssize_t             get_index(const LSPString *name) const;
                ssize_t             get_index(const char *name) const;

                // Checking for contents
                inline bool         contains(size_t index) const            { return index < vParams.size(); }
                inline bool         contains(const char *name) const        { return get_index(name) >= 0; }
                inline bool         contains(const LSPString *name) const   { return get_index(name) >= 0; }
        };
    
    } /* namespace calc */
} /* namespace lsp */

#endif /* CORE_CALC_PARAMETERS_H_ */
