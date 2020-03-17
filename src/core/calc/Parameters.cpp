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

        void Parameters::modified()
        {
        }

        Parameters *Parameters::clone() const
        {
            Parameters *res = new Parameters();
            if (res == NULL)
                return NULL;
            status_t status = res->set(this);
            if (status == STATUS_OK)
                return res;
            delete res;
            return NULL;
        }

        void Parameters::swap(Parameters *src)
        {
            vParams.swap_data(&src->vParams);
            src->modified();
            this->modified();
        }

        void Parameters::clear()
        {
            destroy_params(vParams);
            modified();
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

        ssize_t Parameters::get_index(const LSPString *name) const
        {
            for (size_t i=0, n=vParams.size(); i<n; ++i)
            {
                param_t *p = vParams.at(i);
                if ((p != NULL) && (p->len >= 0) && (name->equals(p->name, p->len)))
                    return i;
            }
            return -STATUS_NOT_FOUND;
        }

        ssize_t Parameters::get_index(const char *name) const
        {
            LSPString tmp;
            if (!tmp.set_utf8(name))
                return STATUS_NO_MEM;
            return get_index(&tmp);
        }

        status_t Parameters::get_name(size_t index, LSPString *name) const
        {
            param_t *p = vParams.get(index);
            if (p == NULL)
                return STATUS_INVALID_VALUE;
            else if (p->len < 0)
                return STATUS_NULL;

            return (name->set(p->name, p->len)) ? STATUS_OK : STATUS_NO_MEM;
        }

        ssize_t Parameters::get_type(size_t index) const
        {
            param_t *p = vParams.get(index);
            return (p != NULL) ? p->value.type : -STATUS_INVALID_VALUE;
        }

        ssize_t Parameters::get_type(const char *name) const
        {
            LSPString tmp;
            if (!tmp.set_utf8(name))
                return STATUS_NO_MEM;
            return get_type(&tmp);
        }

        ssize_t Parameters::get_type(const LSPString *name) const
        {
            param_t *p = const_cast<Parameters *>(this)->lookup_by_name(name);
            return (p != NULL) ? p->value.type : -STATUS_NOT_FOUND;
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
                destroy(params.at(i));
            params.flush();
        }

        void Parameters::destroy(param_t *p)
        {
            if (p == NULL)
                return;
            destroy_value(&p->value);
            ::free(p);
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
                ::memcpy(p->name, name, len * sizeof(lsp_wchar_t));
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
                init_value(&p->value, &src->value);
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
                if ((p == NULL) || (!slice.add(p)))
                {
                    destroy_params(slice);
                    return STATUS_NO_MEM;
                }
            }

            // Swap parameters and destroy old data
            vParams.swap_data(&slice);
            destroy_params(slice);
            modified();
            return STATUS_OK;
        }

        status_t Parameters::insert(size_t index, const Parameters *p, ssize_t first, ssize_t last)
        {
            if ((first < 0) || (first > last) || (last > ssize_t(p->vParams.size())))
                return STATUS_INVALID_VALUE;
            if (index > vParams.size())
                return STATUS_OVERFLOW;

            // Perform a copy
            cvector<param_t> slice;
            param_t * const *vp = vParams.get_array();
            if (!slice.add_all(&vp[0], index))
                return STATUS_NO_MEM;

            // Clone parameters and append to the current tail
            for (ssize_t i=first; i < last; ++i)
            {
                param_t *cp = clone(p->vParams.at(i));
                if ((cp == NULL) || (!slice.add(cp)))
                {
                    // Destroy cloned parameters
                    size_t count = slice.size() - index;
                    for (size_t j=0; j<count; ++j)
                        destroy(slice.at(j));
                    return STATUS_NO_MEM;
                }
            }

            // Append parameters
            if (!slice.add_all(&vp[index], vParams.size() - index))
            {
                // Destroy cloned parameters
                size_t count = slice.size() - index;
                for (size_t j=0; j<count; ++j)
                    destroy(slice.at(j));
                return STATUS_NO_MEM;
            }

            // Clean temporary parameters, swap parameters and destroy old data
            vParams.swap_data(&slice);
            modified();
            return STATUS_OK;
        }

        status_t Parameters::add(const Parameters *p, ssize_t first, ssize_t last)
        {
            if ((first < 0) || (first > last) || (last > ssize_t(p->vParams.size())))
                return STATUS_INVALID_VALUE;

            // Clone parameters and append to the tail
            for (ssize_t i=first; i < last; ++i)
            {
                param_t *cp = clone(p->vParams.at(i));
                if ((cp == NULL) || (!vParams.add(cp)))
                {
                    // Destroy cloned parameters
                    size_t count = vParams.size() - first;
                    for (size_t j=0; j<count; ++j)
                        destroy(vParams.at(j));
                    vParams.remove_n(first, count);
                    return STATUS_NO_MEM;
                }
            }

            modified();
            return STATUS_OK;
        }

        status_t Parameters::add(const LSPString *name, const value_t *value)
        {
            if (name == NULL)
                return add(value);

            param_t *p = allocate(name);
            if (p == NULL)
                return STATUS_NO_MEM;

            status_t res = init_value(&p->value, value);
            if (res == STATUS_OK)
            {
                if (vParams.add(p))
                {
                    modified();
                    return STATUS_OK;
                }
                res = STATUS_NO_MEM;
            }

            destroy(p);
            return res;
        }

        status_t Parameters::add(const char *name, const value_t *value)
        {
            if (name == NULL)
                return add(value);

            LSPString tmp;
            if (!tmp.set_utf8(name))
                return STATUS_NO_MEM;
            return add(&tmp, value);
        }

        status_t Parameters::add(const value_t *value)
        {
            param_t *p = allocate();
            if (p == NULL)
                return STATUS_NO_MEM;

            status_t res = init_value(&p->value, value);
            if (res == STATUS_OK)
            {
                if (vParams.add(p))
                {
                    modified();
                    return STATUS_OK;
                }
                res = STATUS_NO_MEM;
            }

            destroy(p);
            return res;
        }

        status_t Parameters::add_int(const char *name, ssize_t value)
        {
            value_t v;
            v.type      = VT_INT;
            v.v_int     = value;
            return add(name, &v);
        }

        status_t Parameters::add_float(const char *name, double value)
        {
            value_t v;
            v.type      = VT_FLOAT;
            v.v_float   = value;
            return add(name, &v);
        }

        status_t Parameters::add_bool(const char *name, bool value)
        {
            value_t v;
            v.type      = VT_BOOL;
            v.v_bool    = value;
            return add(name, &v);
        }

        status_t Parameters::add_cstring(const char *name, const char *value)
        {
            if (value == NULL)
                return add_null(name);

            LSPString s;
            if (!s.set_utf8(value))
                return STATUS_NO_MEM;

            value_t v;
            v.type      = VT_STRING;
            v.v_str     = &s;
            return add(name, &v);
        }

        status_t Parameters::add_string(const char *name, const LSPString *value)
        {
            if (value == NULL)
                return add_null(name);

            value_t v;
            v.type      = VT_STRING;
            v.v_str     = const_cast<LSPString *>(value);
            return add(name, &v);
        }

        status_t Parameters::add_null(const char *name)
        {
            value_t v;
            v.type      = VT_NULL;
            v.v_str     = NULL;
            return add(name, &v);
        }

        status_t Parameters::add_undef(const char *name)
        {
            value_t v;
            v.type      = VT_UNDEF;
            v.v_str     = NULL;
            return add(name, &v);
        }

        status_t Parameters::add_int(const LSPString *name, ssize_t value)
        {
            value_t v;
            v.type      = VT_INT;
            v.v_int     = value;
            return add(name, &v);
        }

        status_t Parameters::add_float(const LSPString *name, double value)
        {
            value_t v;
            v.type      = VT_FLOAT;
            v.v_float   = value;
            return add(name, &v);
        }

        status_t Parameters::add_bool(const LSPString *name, bool value)
        {
            value_t v;
            v.type      = VT_BOOL;
            v.v_bool    = value;
            return add(name, &v);
        }

        status_t Parameters::add_cstring(const LSPString *name, const char *value)
        {
            if (value == NULL)
                return add_null(name);

            LSPString s;
            if (!s.set_utf8(value))
                return STATUS_NO_MEM;

            value_t v;
            v.type      = VT_STRING;
            v.v_str     = &s;
            return add(name, &v);
        }

        status_t Parameters::add_string(const LSPString *name, const LSPString *value)
        {
            if (value == NULL)
                return add_null(name);

            value_t v;
            v.type      = VT_STRING;
            v.v_str     = const_cast<LSPString *>(value);
            return add(name, &v);
        }

        status_t Parameters::add_null(const LSPString *name)
        {
            value_t v;
            v.type      = VT_NULL;
            v.v_str     = NULL;
            return add(name, &v);
        }

        status_t Parameters::add_undef(const LSPString *name)
        {
            value_t v;
            v.type      = VT_UNDEF;
            v.v_str     = NULL;
            return add(name, &v);
        }

        status_t Parameters::add_int(ssize_t value)
        {
            value_t v;
            v.type      = VT_INT;
            v.v_int     = value;
            return add(&v);
        }

        status_t Parameters::add_float(double value)
        {
            value_t v;
            v.type      = VT_FLOAT;
            v.v_float   = value;
            return add(&v);
        }

        status_t Parameters::add_bool(bool value)
        {
            value_t v;
            v.type      = VT_BOOL;
            v.v_bool    = value;
            return add(&v);
        }

        status_t Parameters::add_cstring(const char *value)
        {
            if (value == NULL)
                return add_null();

            LSPString s;
            if (!s.set_utf8(value))
                return STATUS_NO_MEM;

            value_t v;
            v.type      = VT_STRING;
            v.v_str     = &s;
            return add(&v);
        }

        status_t Parameters::add_string(const LSPString *value)
        {
            if (value == NULL)
                return add_null();

            value_t v;
            v.type      = VT_STRING;
            v.v_str     = const_cast<LSPString *>(value);
            return add(&v);
        }

        status_t Parameters::add_null()
        {
            value_t v;
            v.type      = VT_NULL;
            v.v_str     = NULL;
            return add(&v);
        }

        status_t Parameters::add_undef()
        {
            value_t v;
            v.type      = VT_UNDEF;
            v.v_str     = NULL;
            return add(&v);
        }

        status_t Parameters::insert(size_t index, const char *name, const value_t *value)
        {
            if (name == NULL)
                return insert(index, value);

            LSPString tmp;
            if (!tmp.set_utf8(name))
                return STATUS_NO_MEM;
            return insert(index, &tmp, value);
        }

        status_t Parameters::insert(size_t index, const LSPString *name, const value_t *value)
        {
            if (name == NULL)
                return insert(index, value);
            else if (index > vParams.size())
                return STATUS_INVALID_VALUE;

            param_t *p = allocate(name);
            if (p == NULL)
                return STATUS_NO_MEM;

            status_t res = init_value(&p->value, value);
            if (res == STATUS_OK)
            {
                if (vParams.insert(p, index))
                {
                    modified();
                    return STATUS_OK;
                }
                res = STATUS_NO_MEM;
            }

            destroy(p);
            return res;
        }

        status_t Parameters::insert(size_t index, const value_t *value)
        {
            if (index > vParams.size())
                return STATUS_INVALID_VALUE;

            param_t *p = allocate();
            if (p == NULL)
                return STATUS_NO_MEM;

            status_t res = init_value(&p->value, value);
            if (res == STATUS_OK)
            {
                if (vParams.insert(p, index))
                {
                    modified();
                    return STATUS_OK;
                }
                res = STATUS_NO_MEM;
            }

            destroy(p);
            return res;
        }

        status_t Parameters::insert_int(size_t index, const char *name, ssize_t value)
        {
            value_t v;
            v.type      = VT_INT;
            v.v_int     = value;
            return insert(index, name, &v);
        }

        status_t Parameters::insert_float(size_t index, const char *name, double value)
        {
            value_t v;
            v.type      = VT_FLOAT;
            v.v_float   = value;
            return insert(index, name, &v);
        }

        status_t Parameters::insert_bool(size_t index, const char *name, bool value)
        {
            value_t v;
            v.type      = VT_BOOL;
            v.v_bool    = value;
            return insert(index, name, &v);
        }

        status_t Parameters::insert_string(size_t index, const char *name, const char *value)
        {
            if (value == NULL)
                return insert_null(index, name);

            LSPString s;
            if (!s.set_utf8(value))
                return STATUS_NO_MEM;

            value_t v;
            v.type      = VT_STRING;
            v.v_str     = &s;
            return insert(index, name, &v);
        }

        status_t Parameters::insert_string(size_t index, const char *name, const LSPString *value)
        {
            if (value == NULL)
                return insert_null(index, name);

            value_t v;
            v.type      = VT_STRING;
            v.v_str     = const_cast<LSPString *>(value);
            return insert(index, name, &v);
        }

        status_t Parameters::insert_null(size_t index, const char *name)
        {
            value_t v;
            v.type      = VT_NULL;
            v.v_str     = NULL;
            return insert(index, name, &v);
        }

        status_t Parameters::insert_undef(size_t index, const char *name)
        {
            value_t v;
            v.type      = VT_UNDEF;
            v.v_str     = NULL;
            return insert(index, name, &v);
        }

        status_t Parameters::insert_int(size_t index, const LSPString *name, ssize_t value)
        {
            value_t v;
            v.type      = VT_INT;
            v.v_int     = value;
            return insert(index, name, &v);
        }

        status_t Parameters::insert_float(size_t index, const LSPString *name, double value)
        {
            value_t v;
            v.type      = VT_FLOAT;
            v.v_float   = value;
            return insert(index, name, &v);
        }

        status_t Parameters::insert_bool(size_t index, const LSPString *name, bool value)
        {
            value_t v;
            v.type      = VT_BOOL;
            v.v_bool    = value;
            return insert(index, name, &v);
        }

        status_t Parameters::insert_string(size_t index, const LSPString *name, const char *value)
        {
            if (value == NULL)
                return insert_null(index, name);

            LSPString s;
            if (!s.set_utf8(value))
                return STATUS_NO_MEM;

            value_t v;
            v.type      = VT_STRING;
            v.v_str     = &s;
            return insert(index, name, &v);
        }

        status_t Parameters::insert_string(size_t index, const LSPString *name, const LSPString *value)
        {
            if (value == NULL)
                return insert_null(index, name);

            value_t v;
            v.type      = VT_STRING;
            v.v_str     = const_cast<LSPString *>(value);
            return insert(index, name, &v);
        }

        status_t Parameters::insert_null(size_t index, const LSPString *name)
        {
            value_t v;
            v.type      = VT_NULL;
            v.v_str     = NULL;
            return insert(index, name, &v);
        }

        status_t Parameters::insert_undef(size_t index, const LSPString *name)
        {
            value_t v;
            v.type      = VT_UNDEF;
            v.v_str     = NULL;
            return insert(index, name, &v);
        }

        status_t Parameters::insert_int(size_t index, ssize_t value)
        {
            value_t v;
            v.type      = VT_INT;
            v.v_int     = value;
            return insert(index, &v);
        }

        status_t Parameters::insert_float(size_t index, double value)
        {
            value_t v;
            v.type      = VT_FLOAT;
            v.v_float   = value;
            return insert(index, &v);
        }

        status_t Parameters::insert_bool(size_t index, bool value)
        {
            value_t v;
            v.type      = VT_BOOL;
            v.v_bool    = value;
            return insert(index, &v);
        }

        status_t Parameters::insert_cstring(size_t index, const char *value)
        {
            if (value == NULL)
                return insert_null(index);

            LSPString s;
            if (!s.set_utf8(value))
                return STATUS_NO_MEM;

            value_t v;
            v.type      = VT_STRING;
            v.v_str     = &s;
            return insert(index, &v);
        }

        status_t Parameters::insert_string(size_t index, const LSPString *value)
        {
            if (value == NULL)
                return insert_null(index);

            value_t v;
            v.type      = VT_STRING;
            v.v_str     = const_cast<LSPString *>(value);
            return insert(index, &v);
        }

        status_t Parameters::insert_null(size_t index)
        {
            value_t v;
            v.type      = VT_NULL;
            v.v_str     = NULL;
            return insert(index, &v);
        }

        status_t Parameters::insert_undef(size_t index)
        {
            value_t v;
            v.type      = VT_UNDEF;
            v.v_str     = NULL;
            return insert(index, &v);
        }

        status_t Parameters::get(size_t index, value_t *value) const
        {
            param_t *v = vParams.get(index);
            if (v == NULL)
                return STATUS_INVALID_VALUE;
            return (value != NULL) ? copy_value(value, &v->value) : STATUS_OK;
        }

        status_t Parameters::get(const char *name, value_t *value) const
        {
            LSPString tmp;
            if (!tmp.set_utf8(name))
                return STATUS_NO_MEM;

            return get(&tmp, value);
        }

        status_t Parameters::get(const LSPString *name, value_t *value) const
        {
            param_t *v = const_cast<Parameters *>(this)->lookup_by_name(name);
            if (v == NULL)
                return STATUS_NOT_FOUND;
            return (value != NULL) ? copy_value(value, &v->value) : STATUS_OK;
        }

        status_t Parameters::get_int(size_t index, ssize_t *value) const
        {
            param_t *v = vParams.get(index);
            if (v == NULL)
                return STATUS_INVALID_VALUE;
            else if (v->value.type != VT_INT)
                return STATUS_BAD_TYPE;
            else if (value != NULL)
                *value = v->value.v_int;
            return STATUS_OK;
        }

        status_t Parameters::get_float(size_t index, double *value) const
        {
            param_t *v = vParams.get(index);
            if (v == NULL)
                return STATUS_INVALID_VALUE;
            else if (v->value.type != VT_FLOAT)
                return STATUS_BAD_TYPE;
            else if (value != NULL)
                *value = v->value.v_float;
            return STATUS_OK;
        }

        status_t Parameters::get_bool(size_t index, bool *value) const
        {
            param_t *v = vParams.get(index);
            if (v == NULL)
                return STATUS_INVALID_VALUE;
            else if (v->value.type != VT_BOOL)
                return STATUS_BAD_TYPE;
            else if (value != NULL)
                *value = v->value.v_bool;
            return STATUS_OK;
        }

        status_t Parameters::get_string(size_t index, LSPString *value) const
        {
            param_t *v = vParams.get(index);
            if (v == NULL)
                return STATUS_INVALID_VALUE;
            else if (v->value.type != VT_STRING)
                return STATUS_BAD_TYPE;
            else if (value != NULL)
            {
                if (!value->set(v->value.v_str))
                    return STATUS_NO_MEM;
            }
            return STATUS_OK;
        }

        status_t Parameters::get_null(size_t index) const
        {
            param_t *v = vParams.get(index);
            if (v == NULL)
                return STATUS_INVALID_VALUE;
            else if (v->value.type != VT_NULL)
                return STATUS_BAD_TYPE;
            return STATUS_OK;
        }

        status_t Parameters::get_undef(size_t index) const
        {
            param_t *v = vParams.get(index);
            if (v == NULL)
                return STATUS_INVALID_VALUE;
            else if (v->value.type != VT_UNDEF)
                return STATUS_BAD_TYPE;
            return STATUS_OK;
        }

        status_t Parameters::get_int(const char *name, ssize_t *value) const
        {
            if (name == NULL)
                return STATUS_INVALID_VALUE;
            LSPString tmp;
            if (!tmp.set_utf8(name))
                return STATUS_NO_MEM;

            return get_int(&tmp, value);
        }

        status_t Parameters::get_float(const char *name, double *value) const
        {
            if (name == NULL)
                return STATUS_INVALID_VALUE;
            LSPString tmp;
            if (!tmp.set_utf8(name))
                return STATUS_NO_MEM;

            return get_float(&tmp, value);
        }

        status_t Parameters::get_bool(const char *name, bool *value) const
        {
            if (name == NULL)
                return STATUS_INVALID_VALUE;
            LSPString tmp;
            if (!tmp.set_utf8(name))
                return STATUS_NO_MEM;

            return get_bool(&tmp, value);
        }

        status_t Parameters::get_string(const char *name, LSPString *value) const
        {
            if (name == NULL)
                return STATUS_INVALID_VALUE;
            LSPString tmp;
            if (!tmp.set_utf8(name))
                return STATUS_NO_MEM;

            return get_string(&tmp, value);
        }

        status_t Parameters::get_null(const char *name) const
        {
            if (name == NULL)
                return STATUS_INVALID_VALUE;
            LSPString tmp;
            if (!tmp.set_utf8(name))
                return STATUS_NO_MEM;

            return get_null(&tmp);
        }

        status_t Parameters::get_undef(const char *name) const
        {
            if (name == NULL)
                return STATUS_INVALID_VALUE;
            LSPString tmp;
            if (!tmp.set_utf8(name))
                return STATUS_NO_MEM;

            return get_undef(&tmp);
        }

        status_t Parameters::get_int(const LSPString *name, ssize_t *value) const
        {
            if (name == NULL)
                return STATUS_INVALID_VALUE;
            param_t *v = const_cast<Parameters *>(this)->lookup_by_name(name);
            if (v == NULL)
                return STATUS_NOT_FOUND;
            else if (v->value.type != VT_INT)
                return STATUS_BAD_TYPE;
            else if (value != NULL)
                *value = v->value.v_int;

            return STATUS_OK;
        }

        status_t Parameters::get_float(const LSPString *name, double *value) const
        {
            if (name == NULL)
                return STATUS_INVALID_VALUE;
            param_t *v = const_cast<Parameters *>(this)->lookup_by_name(name);
            if (v == NULL)
                return STATUS_NOT_FOUND;
            else if (v->value.type != VT_FLOAT)
                return STATUS_BAD_TYPE;
            else if (value != NULL)
                *value = v->value.v_float;

            return STATUS_OK;
        }

        status_t Parameters::get_bool(const LSPString *name, bool *value) const
        {
            if (name == NULL)
                return STATUS_INVALID_VALUE;
            param_t *v = const_cast<Parameters *>(this)->lookup_by_name(name);
            if (v == NULL)
                return STATUS_NOT_FOUND;
            else if (v->value.type != VT_BOOL)
                return STATUS_BAD_TYPE;
            else if (value != NULL)
                *value = v->value.v_bool;

            return STATUS_OK;
        }

        status_t Parameters::get_string(const LSPString *name, LSPString *value) const
        {
            if (name == NULL)
                return STATUS_INVALID_VALUE;
            param_t *v = const_cast<Parameters *>(this)->lookup_by_name(name);
            if (v == NULL)
                return STATUS_NOT_FOUND;
            else if (v->value.type != VT_STRING)
                return STATUS_BAD_TYPE;
            else if (value != NULL)
            {
                if (!value->set(v->value.v_str))
                    return STATUS_NO_MEM;
            }

            return STATUS_OK;
        }

        status_t Parameters::get_null(const LSPString *name) const
        {
            if (name == NULL)
                return STATUS_INVALID_VALUE;
            param_t *v = const_cast<Parameters *>(this)->lookup_by_name(name);
            if (v == NULL)
                return STATUS_NOT_FOUND;
            else if (v->value.type != VT_NULL)
                return STATUS_BAD_TYPE;

            return STATUS_OK;
        }

        status_t Parameters::get_undef(const LSPString *name) const
        {
            if (name == NULL)
                return STATUS_INVALID_VALUE;
            param_t *v = const_cast<Parameters *>(this)->lookup_by_name(name);
            if (v == NULL)
                return STATUS_NOT_FOUND;
            else if (v->value.type != VT_UNDEF)
                return STATUS_BAD_TYPE;

            return STATUS_OK;
        }

        status_t Parameters::as_value(size_t index, value_t *value, value_type_t type) const
        {
            const param_t *v = const_cast<Parameters *>(this)->vParams.get(index);
            if (v == NULL)
                return STATUS_INVALID_VALUE;

            value_t tmp;
            status_t res = init_value(&tmp, &v->value);
            if (res == STATUS_OK)
            {
                if ((res = cast_value(&tmp, type)) == STATUS_OK)
                    res = (tmp.type == type) ? copy_value(value, &tmp) : STATUS_BAD_TYPE;
            }

            destroy_value(&tmp);
            return res;
        }

        status_t Parameters::as_value(const LSPString *name, value_t *value, value_type_t type) const
        {
            if (name == NULL)
                return STATUS_INVALID_VALUE;

            param_t *v = const_cast<Parameters *>(this)->lookup_by_name(name);
            if (v == NULL)
                return STATUS_NOT_FOUND;

            value_t tmp;
            status_t res = init_value(&tmp, &v->value);
            if (res == STATUS_OK)
            {
                if ((res = cast_value(&tmp, type)) == STATUS_OK)
                    res = (tmp.type == type) ? copy_value(value, &tmp) : STATUS_BAD_TYPE;
            }

            destroy_value(&tmp);
            return res;
        }

        status_t Parameters::as_value(const char *name, value_t *value, value_type_t type) const
        {
            if (name == NULL)
                return STATUS_INVALID_VALUE;

            LSPString tmp;
            if (!tmp.set_utf8(name))
                return STATUS_NO_MEM;

            return as_value(&tmp, value, type);
        }

        status_t Parameters::as_int(size_t index, ssize_t *value) const
        {
            value_t v;
            init_value(&v);
            status_t res = as_value(index, &v, VT_INT);
            if (res == STATUS_OK)
                *value  = v.v_int;
            destroy_value(&v);
            return res;
        }

        status_t Parameters::as_float(size_t index, double *value) const
        {
            value_t v;
            init_value(&v);
            status_t res = as_value(index, &v, VT_FLOAT);
            if (res == STATUS_OK)
                *value  = v.v_float;
            destroy_value(&v);
            return res;
        }

        status_t Parameters::as_bool(size_t index, bool *value) const
        {
            value_t v;
            init_value(&v);
            status_t res = as_value(index, &v, VT_BOOL);
            if (res == STATUS_OK)
                *value  = v.v_bool;
            destroy_value(&v);
            return res;
        }

        status_t Parameters::as_string(size_t index, LSPString *value) const
        {
            value_t v;
            init_value(&v);
            status_t res = as_value(index, &v, VT_STRING);
            if (res == STATUS_OK)
                res = (value->set(v.v_str)) ? STATUS_OK : STATUS_NO_MEM;
            destroy_value(&v);
            return res;
        }

        status_t Parameters::as_null(size_t index) const
        {
            value_t v;
            init_value(&v);
            status_t res = as_value(index, &v, VT_NULL);
            destroy_value(&v);
            return res;
        }

        status_t Parameters::as_undef(size_t index) const
        {
            value_t v;
            init_value(&v);
            status_t res = as_value(index, &v, VT_UNDEF);
            destroy_value(&v);
            return res;
        }

        status_t Parameters::as_int(const char *name, ssize_t *value) const
        {
            value_t v;
            init_value(&v);
            status_t res = as_value(name, &v, VT_INT);
            if (res == STATUS_OK)
                *value  = v.v_int;
            destroy_value(&v);
            return res;
        }

        status_t Parameters::as_float(const char *name, double *value) const
        {
            value_t v;
            init_value(&v);
            status_t res = as_value(name, &v, VT_FLOAT);
            if (res == STATUS_OK)
                *value  = v.v_float;
            destroy_value(&v);
            return res;
        }

        status_t Parameters::as_bool(const char *name, bool *value) const
        {
            value_t v;
            init_value(&v);
            status_t res = as_value(name, &v, VT_BOOL);
            if (res == STATUS_OK)
                *value  = v.v_bool;
            destroy_value(&v);
            return res;
        }

        status_t Parameters::as_string(const char *name, LSPString *value) const
        {
            value_t v;
            init_value(&v);
            status_t res = as_value(name, &v, VT_STRING);
            if (res == STATUS_OK)
                res = (value->set(v.v_str)) ? STATUS_OK : STATUS_NO_MEM;
            destroy_value(&v);
            return res;
        }

        status_t Parameters::as_null(const char *name) const
        {
            value_t v;
            init_value(&v);
            status_t res = as_value(name, &v, VT_NULL);
            destroy_value(&v);
            return res;
        }

        status_t Parameters::as_undef(const char *name) const
        {
            value_t v;
            init_value(&v);
            status_t res = as_value(name, &v, VT_UNDEF);
            destroy_value(&v);
            return res;
        }

        status_t Parameters::as_int(const LSPString *name, ssize_t *value) const
        {
            value_t v;
            init_value(&v);
            status_t res = as_value(name, &v, VT_INT);
            if (res == STATUS_OK)
                *value  = v.v_int;
            destroy_value(&v);
            return res;
        }

        status_t Parameters::as_float(const LSPString *name, double *value) const
        {
            value_t v;
            init_value(&v);
            status_t res = as_value(name, &v, VT_FLOAT);
            if (res == STATUS_OK)
                *value  = v.v_float;
            destroy_value(&v);
            return res;
        }

        status_t Parameters::as_bool(const LSPString *name, bool *value) const
        {
            value_t v;
            init_value(&v);
            status_t res = as_value(name, &v, VT_BOOL);
            if (res == STATUS_OK)
                *value  = v.v_bool;
            destroy_value(&v);
            return res;
        }

        status_t Parameters::as_string(const LSPString *name, LSPString *value) const
        {
            value_t v;
            init_value(&v);
            status_t res = as_value(name, &v, VT_STRING);
            if (res == STATUS_OK)
                res = (value->set(v.v_str)) ? STATUS_OK : STATUS_NO_MEM;
            destroy_value(&v);
            return res;
        }

        status_t Parameters::as_null(const LSPString *name) const
        {
            value_t v;
            init_value(&v);
            status_t res = as_value(name, &v, VT_NULL);
            destroy_value(&v);
            return res;
        }

        status_t Parameters::as_undef(const LSPString *name) const
        {
            value_t v;
            init_value(&v);
            status_t res = as_value(name, &v, VT_UNDEF);
            destroy_value(&v);
            return res;
        }

        status_t Parameters::set(const char *name, const value_t *value)
        {
            if (name == NULL)
                return STATUS_INVALID_VALUE;

            LSPString tmp;
            if (!tmp.set_utf8(name))
                return STATUS_NO_MEM;

            return set(&tmp, value);
        }

        status_t Parameters::set(const LSPString *name, const value_t *value)
        {
            param_t *v = lookup_by_name(name);
            if (v == NULL)
                return add(name, value);

            status_t res = copy_value(&v->value, value);
            if (res == STATUS_OK)
                modified();
            return res;
        }

        status_t Parameters::set(size_t index, const value_t *value)
        {
            param_t *v = vParams.get(index);
            if (v == NULL)
                return STATUS_INVALID_VALUE;

            status_t res = copy_value(&v->value, value);
            if (res == STATUS_OK)
                modified();
            return res;
        }

        status_t Parameters::set_int(const char *name, ssize_t value)
        {
            value_t v;
            v.type      = VT_INT;
            v.v_int     = value;
            return set(name, &v);
        }

        status_t Parameters::set_float(const char *name, double value)
        {
            value_t v;
            v.type      = VT_FLOAT;
            v.v_float   = value;
            return set(name, &v);
        }

        status_t Parameters::set_bool(const char *name, bool value)
        {
            value_t v;
            v.type      = VT_BOOL;
            v.v_bool    = value;
            return set(name, &v);
        }

        status_t Parameters::set_cstring(const char *name, const char *value)
        {
            if (value == NULL)
                return set_null(name);

            LSPString tmp;
            if (!tmp.set_utf8(value))
                return STATUS_NO_MEM;

            value_t v;
            v.type      = VT_STRING;
            v.v_str     = &tmp;
            return set(name, &v);
        }

        status_t Parameters::set_string(const char *name, const LSPString *value)
        {
            value_t v;
            v.type      = VT_STRING;
            v.v_str     = const_cast<LSPString *>(value);
            return set(name, &v);
        }

        status_t Parameters::set_null(const char *name)
        {
            value_t v;
            v.type      = VT_NULL;
            v.v_str     = NULL;
            return set(name, &v);
        }

        status_t Parameters::set_undef(const char *name)
        {
            value_t v;
            v.type      = VT_UNDEF;
            v.v_str     = NULL;
            return set(name, &v);
        }

        status_t Parameters::set_int(const LSPString *name, ssize_t value)
        {
            value_t v;
            v.type      = VT_INT;
            v.v_int     = value;
            return set(name, &v);
        }

        status_t Parameters::set_float(const LSPString *name, double value)
        {
            value_t v;
            v.type      = VT_FLOAT;
            v.v_float   = value;
            return set(name, &v);
        }

        status_t Parameters::set_bool(const LSPString *name, bool value)
        {
            value_t v;
            v.type      = VT_BOOL;
            v.v_bool    = value;
            return set(name, &v);
        }

        status_t Parameters::set_cstring(const LSPString *name, const char *value)
        {
            if (value == NULL)
                return set_null(name);

            LSPString tmp;
            if (!tmp.set_utf8(value))
                return STATUS_NO_MEM;

            value_t v;
            v.type      = VT_STRING;
            v.v_str     = &tmp;
            return set(name, &v);
        }

        status_t Parameters::set_string(const LSPString *name, const LSPString *value)
        {
            value_t v;
            v.type      = VT_STRING;
            v.v_str     = const_cast<LSPString *>(value);
            return set(name, &v);
        }

        status_t Parameters::set_null(const LSPString *name)
        {
            value_t v;
            v.type      = VT_NULL;
            v.v_str     = NULL;
            return set(name, &v);
        }

        status_t Parameters::set_undef(const LSPString *name)
        {
            value_t v;
            v.type      = VT_UNDEF;
            v.v_str     = NULL;
            return set(name, &v);
        }

        status_t Parameters::set_int(size_t index, ssize_t value)
        {
            value_t v;
            v.type      = VT_INT;
            v.v_int     = value;
            return set(index, &v);
        }

        status_t Parameters::set_float(size_t index, double value)
        {
            value_t v;
            v.type      = VT_FLOAT;
            v.v_float   = value;
            return set(index, &v);
        }

        status_t Parameters::set_bool(size_t index, bool value)
        {
            value_t v;
            v.type      = VT_BOOL;
            v.v_bool    = value;
            return set(index, &v);
        }

        status_t Parameters::set_cstring(size_t index, const char *value)
        {
            if (value == NULL)
                return set_null(index);

            LSPString tmp;
            if (!tmp.set_utf8(value))
                return STATUS_NO_MEM;

            value_t v;
            v.type      = VT_STRING;
            v.v_str     = &tmp;
            return set(index, &v);
        }

        status_t Parameters::set_string(size_t index, const LSPString *value)
        {
            value_t v;
            v.type      = VT_STRING;
            v.v_str     = const_cast<LSPString *>(value);
            return set(index, &v);
        }

        status_t Parameters::set_null(size_t index)
        {
            value_t v;
            v.type      = VT_NULL;
            v.v_str     = NULL;
            return set(index, &v);
        }

        status_t Parameters::set_undef(size_t index)
        {
            value_t v;
            v.type      = VT_UNDEF;
            v.v_str     = NULL;
            return set(index, &v);
        }

        status_t Parameters::remove(size_t index, value_t *value)
        {
            param_t *v  = vParams.get(index);
            if (v == NULL)
                return STATUS_INVALID_VALUE;
            if (value != NULL)
            {
                status_t res = copy_value(value, &v->value);
                if (res != STATUS_OK)
                    return res;
            }

            vParams.remove(index);
            destroy(v);
            modified();
            return STATUS_OK;
        }

        status_t Parameters::remove_value(size_t index, value_type_t type, value_t *value)
        {
            param_t *v  = vParams.get(index);
            if (v == NULL)
                return STATUS_INVALID_VALUE;
            else if (v->value.type != type)
                return STATUS_BAD_TYPE;
            if (value != NULL)
            {
                status_t res = copy_value(value, &v->value);
                if (res != STATUS_OK)
                    return res;
            }

            vParams.remove(index);
            destroy(v);
            modified();
            return STATUS_OK;
        }

        status_t Parameters::remove(const char *name, value_t *value)
        {
            if (name == NULL)
                return STATUS_INVALID_VALUE;

            LSPString tmp;
            if (!tmp.set_utf8(name))
                return STATUS_NO_MEM;

            return remove(&tmp, value);
        }

        status_t Parameters::remove_value(const char *name, value_type_t type, value_t *value)
        {
            if (name == NULL)
                return STATUS_INVALID_VALUE;

            LSPString tmp;
            if (!tmp.set_utf8(name))
                return STATUS_NO_MEM;

            return remove_value(&tmp, type, value);
        }

        status_t Parameters::remove(const LSPString *name, value_t *value)
        {
            if (name == NULL)
                return STATUS_INVALID_VALUE;

            size_t index;
            param_t *v = lookup_by_name(name, &index);
            if (v == NULL)
                return STATUS_NOT_FOUND;
            if (value != NULL)
            {
                status_t res = copy_value(value, &v->value);
                if (res != STATUS_OK)
                    return res;
            }

            vParams.remove(index);
            destroy(v);
            modified();
            return STATUS_OK;
        }

        status_t Parameters::remove_value(const LSPString *name, value_type_t type, value_t *value)
        {
            if (name == NULL)
                return STATUS_INVALID_VALUE;

            size_t index;
            param_t *v = lookup_by_name(name, &index);
            if (v == NULL)
                return STATUS_NOT_FOUND;
            else if (v->value.type != type)
                return STATUS_BAD_TYPE;
            if (value != NULL)
            {
                status_t res = copy_value(value, &v->value);
                if (res != STATUS_OK)
                    return res;
            }

            vParams.remove(index);
            destroy(v);
            modified();
            return STATUS_OK;
        }

        status_t Parameters::remove(ssize_t first, ssize_t last)
        {
            if ((first < 0) || (last > ssize_t(vParams.size())))
                return STATUS_INVALID_VALUE;

            ssize_t count = last - first;
            if (count < 0)
                return STATUS_INVALID_VALUE;

            for (ssize_t i=first; i<last; ++i)
                destroy(vParams.at(i));

            bool success = vParams.remove_n(first, count);
            if (success)
                modified();
            return (success) ? STATUS_OK : STATUS_CORRUPTED;
        }

        status_t Parameters::drop_value(size_t index, value_type_t type, param_t **out)
        {
            param_t *v = vParams.get(index);
            if (v == NULL)
                return STATUS_INVALID_VALUE;
            else if (v->value.type != type)
                return STATUS_BAD_TYPE;

            vParams.remove(index);
            *out = v;
            modified();
            return STATUS_OK;
        }

        status_t Parameters::drop_value(const char *name, value_type_t type, param_t **out)
        {
            if (name == NULL)
                return STATUS_INVALID_VALUE;

            LSPString tmp;
            if (!tmp.set_utf8(name))
                return STATUS_NO_MEM;

            return drop_value(&tmp, type, out);
        }

        status_t Parameters::drop_value(const LSPString *name, value_type_t type, param_t **out)
        {
            size_t index;
            param_t *v = lookup_by_name(name, &index);
            if (v == NULL)
                return STATUS_NOT_FOUND;
            else if (v->value.type != type)
                return STATUS_BAD_TYPE;

            vParams.remove(index);
            *out = v;
            modified();
            return STATUS_OK;
        }

        status_t Parameters::remove_int(size_t index, ssize_t *value)
        {
            param_t *pv;
            status_t res = drop_value(index, VT_INT, &pv);
            if (res != STATUS_OK)
                return res;
            if (value != NULL)
                *value  = pv->value.v_int;
            destroy(pv);
            return STATUS_OK;
        }

        status_t Parameters::remove_float(size_t index, double *value)
        {
            param_t *pv;
            status_t res = drop_value(index, VT_FLOAT, &pv);
            if (res != STATUS_OK)
                return res;
            if (value != NULL)
                *value  = pv->value.v_float;
            destroy(pv);
            return STATUS_OK;
        }

        status_t Parameters::remove_bool(size_t index, bool *value)
        {
            param_t *pv;
            status_t res = drop_value(index, VT_BOOL, &pv);
            if (res != STATUS_OK)
                return res;
            if (value != NULL)
                *value  = pv->value.v_bool;
            destroy(pv);
            return STATUS_OK;
        }

        status_t Parameters::remove_string(size_t index, LSPString *value)
        {
            param_t *pv;
            status_t res = drop_value(index, VT_STRING, &pv);
            if (res != STATUS_OK)
                return res;
            if (value != NULL)
                value->swap(pv->value.v_str);
            destroy(pv);
            return STATUS_OK;
        }

        status_t Parameters::remove_null(size_t index)
        {
            param_t *pv;
            status_t res = drop_value(index, VT_NULL, &pv);
            if (res != STATUS_OK)
                return res;
            destroy(pv);
            return STATUS_OK;
        }

        status_t Parameters::remove_undef(size_t index)
        {
            param_t *pv;
            status_t res = drop_value(index, VT_UNDEF, &pv);
            if (res != STATUS_OK)
                return res;
            destroy(pv);
            return STATUS_OK;
        }



        status_t Parameters::remove_int(const char *name, ssize_t *value)
        {
            param_t *pv;
            status_t res = drop_value(name, VT_INT, &pv);
            if (res != STATUS_OK)
                return res;
            if (value != NULL)
                *value  = pv->value.v_int;
            destroy(pv);
            return STATUS_OK;
        }

        status_t Parameters::remove_float(const char *name, double *value)
        {
            param_t *pv;
            status_t res = drop_value(name, VT_FLOAT, &pv);
            if (res != STATUS_OK)
                return res;
            if (value != NULL)
                *value  = pv->value.v_float;
            destroy(pv);
            return STATUS_OK;
        }

        status_t Parameters::remove_bool(const char *name, bool *value)
        {
            param_t *pv;
            status_t res = drop_value(name, VT_BOOL, &pv);
            if (res != STATUS_OK)
                return res;
            if (value != NULL)
                *value  = pv->value.v_bool;
            destroy(pv);
            return STATUS_OK;
        }

        status_t Parameters::remove_string(const char *name, LSPString *value)
        {
            param_t *pv;
            status_t res = drop_value(name, VT_STRING, &pv);
            if (res != STATUS_OK)
                return res;
            if (value != NULL)
                value->swap(pv->value.v_str);
            destroy(pv);
            return STATUS_OK;
        }

        status_t Parameters::remove_null(const char *name)
        {
            param_t *pv;
            status_t res = drop_value(name, VT_NULL, &pv);
            if (res != STATUS_OK)
                return res;
            destroy(pv);
            return STATUS_OK;
        }

        status_t Parameters::remove_undef(const char *name)
        {
            param_t *pv;
            status_t res = drop_value(name, VT_UNDEF, &pv);
            if (res != STATUS_OK)
                return res;
            destroy(pv);
            return STATUS_OK;
        }




        status_t Parameters::remove_int(const LSPString *name, ssize_t *value)
        {
            param_t *pv;
            status_t res = drop_value(name, VT_INT, &pv);
            if (res != STATUS_OK)
                return res;
            if (value != NULL)
                *value  = pv->value.v_int;
            destroy(pv);
            return STATUS_OK;
        }

        status_t Parameters::remove_float(const LSPString *name, double *value)
        {
            param_t *pv;
            status_t res = drop_value(name, VT_FLOAT, &pv);
            if (res != STATUS_OK)
                return res;
            if (value != NULL)
                *value  = pv->value.v_float;
            destroy(pv);
            return STATUS_OK;
        }

        status_t Parameters::remove_bool(const LSPString *name, bool *value)
        {
            param_t *pv;
            status_t res = drop_value(name, VT_BOOL, &pv);
            if (res != STATUS_OK)
                return res;
            if (value != NULL)
                *value  = pv->value.v_bool;
            destroy(pv);
            return STATUS_OK;
        }

        status_t Parameters::remove_string(const LSPString *name, LSPString *value)
        {
            param_t *pv;
            status_t res = drop_value(name, VT_STRING, &pv);
            if (res != STATUS_OK)
                return res;
            if (value != NULL)
                value->swap(pv->value.v_str);
            destroy(pv);
            return STATUS_OK;
        }

        status_t Parameters::remove_null(const LSPString *name)
        {
            param_t *pv;
            status_t res = drop_value(name, VT_NULL, &pv);
            if (res != STATUS_OK)
                return res;
            destroy(pv);
            return STATUS_OK;
        }

        status_t Parameters::remove_undef(const LSPString *name)
        {
            param_t *pv;
            status_t res = drop_value(name, VT_UNDEF, &pv);
            if (res != STATUS_OK)
                return res;
            destroy(pv);
            return STATUS_OK;
        }

    } /* namespace calc */
} /* namespace lsp */
