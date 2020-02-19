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
            destroy_params(vParams);
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

        void Parameters::destroy_params(cvector<param_t> &params)
        {
            for (size_t i=0; i<params.size(); ++i)
            {
                param_t *p = params.at(i);
                if (p != NULL)
                {
                    destroy_value(&p->value);
                    ::free(p);
                }
            }
            params.flush();
        }

        Parameters::param_t *Parameters::allocate()
        {
            param_t *p = reinterpret_cast<param_t *>(::malloc(ALIGN_SIZE(sizeof(param_t), DEFAULT_ALIGN)));
            if (p != NULL)
            {
                init_value(&p->value);
                p->len = -1;
            }
            return p;
        }

        Parameters::param_t *Parameters::allocate(const lsp_wchar_t *name, ssize_t len)
        {
            size_t to_alloc = sizeof(param_t) + len * sizeof(lsp_wchar_t);

            param_t *p = reinterpret_cast<param_t *>(::malloc(ALIGN_SIZE(to_alloc, DEFAULT_ALIGN)));
            if (p != NULL)
            {
                init_value(&p->value);
                p->len = len;
            }
            return p;
        }

        Parameters::param_t *Parameters::clone(const param_t *src)
        {
            size_t len = (src->len < 0) ? 0 : src->len;
            size_t to_alloc = sizeof(param_t) + len * sizeof(lsp_wchar_t);

            param_t *p = reinterpret_cast<param_t *>(::malloc(ALIGN_SIZE(to_alloc, DEFAULT_ALIGN)));
            if (p != NULL)
            {
                copy_value(&p->value, &src->value);
                p->len = src->len;
                ::memcpy(p->name, src->name, len * sizeof(lsp_wchar_t));
            }

            return p;
        }

        status_t Parameters::set(const Parameters *p, ssize_t first, ssize_t last)
        {
            const cvector<param_t> &vp = p->vParams;

            // Check ranges
            if (first < 0)
                return STATUS_UNDERFLOW;
            if (last < 0)
            {
                last = vp.size();
                if (first > last)
                    return STATUS_OVERFLOW;
            }
            else
            {
                if (last > ssize_t(vp.size()))
                    return STATUS_OVERFLOW;
                if (first > last)
                    return STATUS_INVALID_VALUE;
            }

            // Perform a copy
            cvector<param_t> slice;
            for ( ; first < last; ++first)
            {
                param_t *p = clone(vp.at(first));
                if (p == NULL)
                {
                    destroy_params(slice);
                    return STATUS_NO_MEM;
                }
            }

            // Swap parameters and destroy old data
            vParams.swap_data(&slice);
            destroy_params(slice);
            return STATUS_OK;
        }

        status_t Parameters::insert(size_t index, const Parameters *p, ssize_t first, ssize_t last)
        {
            if (index > vParams.size())
                return STATUS_OVERFLOW;

            // Initialize temporary parameters
            Parameters tmp;
            status_t res = tmp.add(p, first, last);
            if (res != STATUS_OK)
                return res;

            // Perform a copy
            cvector<param_t> slice;
            param_t * const *vp = vParams.get_array();
            if (!slice.add_all(&vp[0], index))
                return STATUS_NO_MEM;
            if (!slice.add_all(&tmp.vParams))
                return STATUS_NO_MEM;
            if (!slice.add_all(&vp[index], index - vParams.size()))
                return STATUS_NO_MEM;

            // Clean temporary parameters, swap parameters and destroy old data
            tmp.vParams.flush();
            vParams.swap_data(&slice);
            destroy_params(slice);
            return STATUS_OK;
        }

        status_t Parameters::add(const Parameters *p, ssize_t first, ssize_t last)
        {
            return insert(vParams.size(), p, first, last);
        }
    
    } /* namespace calc */
} /* namespace lsp */
