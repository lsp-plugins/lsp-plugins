/*
 * Variables.cpp
 *
 *  Created on: 19 сент. 2019 г.
 *      Author: sadko
 */

#include <core/calc/types.h>
#include <core/calc/Variables.h>

namespace lsp
{
    namespace calc
    {
        Variables::Variables()
        {
            pResolver       = NULL;
        }
        
        Variables::Variables(Resolver *r)
        {
            pResolver       = r;
        }
        
        Variables::~Variables()
        {
            clear();
        }
    
        status_t Variables::set_int(const char *name, ssize_t value)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPString key;
            if (!key.set_utf8(name))
                return STATUS_NO_MEM;

            value_t v;
            v.type      = VT_INT;
            v.v_int     = value;
            return set(&key, &v);
        }

        status_t Variables::set_float(const char *name, double value)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPString key;
            if (!key.set_utf8(name))
                return STATUS_NO_MEM;

            value_t v;
            v.type      = VT_FLOAT;
            v.v_float   = value;
            return set(&key, &v);
        }

        status_t Variables::set_bool(const char *name, bool value)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPString key;
            if (!key.set_utf8(name))
                return STATUS_NO_MEM;

            value_t v;
            v.type      = VT_BOOL;
            v.v_bool    = value;
            return set(&key, &v);
        }

        status_t Variables::set_string(const char *name, const char *value, const char *charset)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;

            LSPString key;
            if (!key.set_utf8(name))
                return STATUS_NO_MEM;

            value_t v;
            LSPString tmp;

            if (value != NULL)
            {
                if (!tmp.set_native(value, charset))
                {
                    delete v.v_str;
                    return STATUS_NO_MEM;
                }
                v.type      = VT_STRING;
                v.v_str     = &tmp;
            }
            else
            {
                v.type      = VT_NULL;
                v.v_str     = NULL;
            }

            return set(&key, &v);
        }

        status_t Variables::set_string(const char *name, const LSPString *value)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;

            LSPString key;
            if (!key.set_utf8(name))
                return STATUS_NO_MEM;

            value_t v;
            if (value != NULL)
            {
                v.type      = VT_STRING;
                v.v_str     = const_cast<LSPString *>(value);
            }
            else
            {
                v.type      = VT_NULL;
                v.v_str     = NULL;
            }

            return set(&key, &v);
        }

        status_t Variables::set_null(const char *name)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPString key;
            if (!key.set_utf8(name))
                return STATUS_NO_MEM;

            value_t v;
            v.type          = VT_NULL;
            v.v_str         = NULL;
            return set(&key, &v);
        }

        status_t Variables::set_int(const LSPString *name, ssize_t value)
        {
            value_t v;
            v.type      = VT_INT;
            v.v_int     = value;
            return set(name, &v);
        }

        status_t Variables::set_float(const LSPString *name, double value)
        {
            value_t v;
            v.type      = VT_FLOAT;
            v.v_float   = value;
            return set(name, &v);
        }

        status_t Variables::set_bool(const LSPString *name, bool value)
        {
            value_t v;
            v.type      = VT_BOOL;
            v.v_bool    = value;
            return set(name, &v);
        }

        status_t Variables::set_string(const LSPString *name, const char *value, const char *charset)
        {
            value_t v;
            LSPString tmp;

            if (value != NULL)
            {
                if (!tmp.set_native(value, charset))
                {
                    delete v.v_str;
                    return STATUS_NO_MEM;
                }
                v.type      = VT_STRING;
                v.v_str     = &tmp;
            }
            else
            {
                v.type      = VT_NULL;
                v.v_str     = NULL;
            }

            return set(name, &v);
        }

        status_t Variables::set_string(const LSPString *name, const LSPString *value)
        {
            value_t v;
            v.type      = VT_STRING;
            v.v_str     = const_cast<LSPString *>(value);
            return set(name, &v);
        }

        status_t Variables::set_null(const LSPString *name)
        {
            value_t v;
            v.type          = VT_NULL;
            v.v_str         = NULL;
            return set(name, &v);
        }

        status_t Variables::set(const char *name, const value_t *value)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPString key;
            if (!key.set_utf8(name))
                return STATUS_NO_MEM;

            return set(&key, value);
        }

        status_t Variables::unset(const char *name, value_t *value)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPString key;
            if (!key.set_utf8(name))
                return STATUS_NO_MEM;

            return unset(&key, value);
        }

        status_t Variables::resolve(value_t *value, const char *name, size_t num_indexes, const ssize_t *indexes)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;

            LSPString key;
            if (!key.set_utf8(name))
                return STATUS_NO_MEM;

            return resolve(value, &key, num_indexes, indexes);
        }

        status_t Variables::resolve(value_t *value, const LSPString *name, size_t num_indexes, const ssize_t *indexes)
        {
            // Need to form indexes?
            LSPString tmp;
            const LSPString *search;

            if (num_indexes > 0)
            {
                if (!tmp.set(name))
                    return STATUS_NO_MEM;
                for (size_t i=0; i<num_indexes; ++i)
                {
                    if (!tmp.fmt_append_ascii("_%ld", long(indexes[i])))
                        return STATUS_NO_MEM;
                }
                search = &tmp;
            }
            else
                search = name;

            // Lookup the cache
            for (size_t i=0, n=vVars.size(); i<n; ++i)
            {
                variable_t *var = vVars.at(i);
                if ((var != NULL) && (var->name.equals(search)))
                {
                    if (value != NULL)
                        return copy_value(value, &var->value);
                    return STATUS_OK;
                }
            }

            // No Resolver?
            if (pResolver == NULL)
                return STATUS_NOT_FOUND;

            // Resolve from underlying resolver
            value_t v;
            init_value(&v);
            status_t res = pResolver->resolve(&v, name, num_indexes, indexes);
            if (res != STATUS_OK)
                return res;

            // Save variable to cache
            res = add(search, &v);
            if ((res == STATUS_OK) && (value != NULL))
                res = copy_value(value, &v);

            destroy_value(&v);
            return res;
        }

        status_t Variables::add(const LSPString *name, const value_t *value)
        {
            variable_t *var = new variable_t;
            if (!var->name.set(name))
                return STATUS_NO_MEM;

            init_value(&var->value);
            status_t res = copy_value(&var->value, value);
            if (res == STATUS_OK)
                res = (vVars.add(var)) ? STATUS_OK : STATUS_NO_MEM;
            if (res == STATUS_OK)
                return res;

            destroy_value(&var->value);
            delete var;
            return res;
        }

        status_t Variables::set(const LSPString *name, const value_t *value)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;

            // Lookup for existing data
            for (size_t i=0, n=vVars.size(); i<n; ++i)
            {
                variable_t *var = vVars.at(i);
                if (var->name.equals(name))
                {
                    destroy_value(&var->value);
                    return copy_value(&var->value, value);
                }
            }

            // Add non-existing value
            return add(name, value);
        }

        status_t Variables::unset(const LSPString *name, value_t *value)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;

            // Lookup for data
            for (size_t i=0, n=vVars.size(); i<n; ++i)
            {
                variable_t *var = vVars.at(i);
                if (var->name.equals(name))
                {
                    vVars.remove(i, true);
                    destroy_value(&var->value);
                    delete var;
                }
            }

            return STATUS_OK;
        }

        void Variables::clear()
        {
            for (size_t i=0, n=vVars.size(); i<n; ++i)
            {
                variable_t *var = vVars.at(i);
                if (var != NULL)
                {
                    destroy_value(&var->value);
                    delete var;
                }
            }
            vVars.flush();
        }

    } /* namespace calc */
} /* namespace lsp */
