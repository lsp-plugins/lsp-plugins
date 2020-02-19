/*
 * Parameters.cpp
 *
 *  Created on: 19 февр. 2020 г.
 *      Author: sadko
 */

#include <core/calc/Parameters.h>

namespace lsp
{
    namespace calc
    {
        
        Parameters::Parameters()
        {
        }
        
        Parameters::~Parameters()
        {
            // TODO Auto-generated destructor stub
        }

        status_t Parameters::resolve(value_t *value, const char *name, size_t num_indexes, const ssize_t *indexes)
        {
            if (name == NULL)
                return STATUS_BAD_ARGUMENTS;

            LSPString key;
            if (!key.set_utf8(name))
                return STATUS_NO_MEM;

            return resolve(value, &key, num_indexes, indexes);
        }

        Parameters::param_t *Parameters::lookup_by_name(const LSPString *name)
        {
            for (size_t i=0, n=vParams.size(); i<n; ++i)
            {
                param_t *p = vParams.at(i);
                if ((p != NULL) && (p->len >= 0) && (name->equals(p->name, p->len)))
                    return p;
            }
            return NULL;
        }

        ssize_t Parameters::lookup_idx_by_name(const LSPString *name)
        {
            for (size_t i=0, n=vParams.size(); i<n; ++i)
            {
                param_t *p = vParams.at(i);
                if ((p != NULL) && (p->len >= 0) && (name->equals(p->name, p->len)))
                    return i;
            }
            return -STATUS_NOT_FOUND;
        }

        Parameters::param_t *Parameters::lookup_by_name(const LSPString *name, size_t *idx)
        {
            for (size_t i=0, n=vParams.size(); i<n; ++i)
            {
                param_t *p = vParams.at(i);
                if ((p != NULL) && (p->len >= 0) && (name->equals(p->name, p->len)))
                {
                    *idx = i;
                    return p;
                }
            }
            return NULL;
        }

        status_t Parameters::resolve(value_t *value, const LSPString *name, size_t num_indexes, const ssize_t *indexes)
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

            // Lookup the list
            param_t *p = lookup_by_name(search);
            if (p == NULL)
                return STATUS_NOT_FOUND;

            if (value != NULL)
                return copy_value(value, &p->value);

            return STATUS_OK;
        }
    
    } /* namespace calc */
} /* namespace lsp */
