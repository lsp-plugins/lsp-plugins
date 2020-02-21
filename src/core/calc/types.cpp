/*
 * types.cpp
 *
 *  Created on: 19 сент. 2019 г.
 *      Author: sadko
 */

#include <core/io/InStringSequence.h>
#include <core/calc/types.h>
#include <core/calc/Tokenizer.h>

namespace lsp
{
    namespace calc
    {
        void init_value(value_t *dst)
        {
            dst->type       = VT_UNDEF;
            dst->v_str      = NULL;
        }

        inline void destroy_value_internal(value_t *value)
        {
            if ((value->type == VT_STRING) && (value->v_str != NULL))
            {
                delete value->v_str;
                value->v_str        = NULL;
            }
        }

        void set_value_null(value_t *dst)
        {
            destroy_value_internal(dst);
            dst->type       = VT_NULL;
        }

        void set_value_undef(value_t *dst)
        {
            destroy_value_internal(dst);
            dst->type       = VT_UNDEF;
        }

        void set_value_int(value_t *dst, ssize_t value)
        {
            destroy_value_internal(dst);
            dst->type       = VT_INT;
            dst->v_int      = value;
        }

        void set_value_float(value_t *dst, double value)
        {
            destroy_value_internal(dst);
            dst->type       = VT_FLOAT;
            dst->v_float    = value;
        }

        void set_value_bool(value_t *dst, bool value)
        {
            destroy_value_internal(dst);
            dst->type       = VT_BOOL;
            dst->v_bool     = value;
        }

        status_t init_value(value_t *dst, const value_t *src)
        {
            if (src == NULL)
            {
                dst->type       = VT_NULL;
                dst->v_str      = NULL;
                return STATUS_OK;
            }
            else if ((src->type == VT_STRING) && (src->v_str != NULL))
            {
                dst->type       = VT_UNDEF;
                dst->v_str      = NULL;

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

        status_t set_value_string(value_t *dst, LSPString *value)
        {
            if (value == NULL)
            {
                set_value_null(dst);
                return STATUS_OK;
            }
            else if (dst->type == VT_STRING)
                return (dst->v_str->set(value)) ? STATUS_OK : STATUS_NO_MEM;

            LSPString *copy = value->clone();
            if (copy == NULL)
                return STATUS_NO_MEM;

            destroy_value_internal(dst);
            dst->type       = VT_STRING;
            dst->v_str      = copy;
            return STATUS_OK;
        }

        status_t copy_value(value_t *dst, const value_t *src)
        {
            if (src == NULL)
            {
                set_value_null(dst);
                return STATUS_OK;
            }
            else if ((src->type == VT_STRING) && (src->v_str != NULL))
            {
                LSPString *copy = src->v_str->clone();
                if (copy == NULL)
                    return STATUS_NO_MEM;

                destroy_value_internal(dst);

                dst->type       = VT_STRING;
                dst->v_str      = copy;
            }
            else
            {
                destroy_value_internal(dst);
                *dst        = *src;
            }
            return STATUS_OK;
        }

        void destroy_value(value_t *value)
        {
            destroy_value_internal(value);
            value->type     = VT_UNDEF;
        }

        status_t fetch_int(ssize_t *dst, const value_t *v)
        {
            if (v->type != VT_INT)
                return STATUS_BAD_TYPE;
            *dst = v->v_int;
            return STATUS_OK;
        }

        status_t fetch_float(float *dst, const value_t *v)
        {
            if (v->type != VT_FLOAT)
                return STATUS_BAD_TYPE;
            *dst = v->v_float;
            return STATUS_OK;
        }

        status_t fetch_bool(bool *dst, const value_t *v)
        {
            if (v->type != VT_BOOL)
                return STATUS_BAD_TYPE;
            *dst = v->v_bool;
            return STATUS_OK;
        }

        status_t fetch_string(LSPString *dst, const value_t *v)
        {
            if (v->type != VT_STRING)
                return STATUS_BAD_TYPE;
            return (dst->set(v->v_str)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t cast_value(value_t *v, value_type_t type)
        {
            switch (type)
            {
                case VT_INT: return cast_int(v);
                case VT_FLOAT: return cast_float(v);
                case VT_BOOL: return cast_bool(v);
                case VT_STRING: return cast_string(v);
                case VT_NULL:
                    set_value_null(v);
                    return STATUS_OK;
                case VT_UNDEF:
                    set_value_undef(v);
                    return STATUS_OK;
            }

            return STATUS_BAD_TYPE;
        }

        status_t cast_int(value_t *v)
        {
            switch (v->type)
            {
                case VT_INT:    return STATUS_OK;
                case VT_FLOAT:  v->v_int = ssize_t(v->v_float); break;
                case VT_BOOL:   v->v_int = (v->v_bool) ? 1 : 0; break;
                case VT_STRING:
                {
                    // Parse integer/float number as string and cast to integer
                    io::InStringSequence s(v->v_str);
                    Tokenizer t(&s);
                    ssize_t ivalue;

                    switch (t.get_token(TF_GET))
                    {
                        case TT_IVALUE: ivalue = t.int_value(); break;
                        case TT_FVALUE: ivalue = t.int_value(); break;
                        case TT_TRUE:   ivalue = 1; break;
                        case TT_FALSE:  ivalue = 0; break;
                        default:
                            delete v->v_str;
                            v->type     = VT_UNDEF;
                            return STATUS_OK;
                    }

                    if (t.get_token(TF_GET) != TT_EOF)
                        return STATUS_BAD_FORMAT;
                    delete v->v_str;
                    v->v_int    = ivalue;
                    break;
                }
                case VT_NULL:
                case VT_UNDEF:
                    return STATUS_OK;
                default:
                    return STATUS_BAD_TYPE;
            }

            v->type     = VT_INT;
            return STATUS_OK;
        }

        status_t cast_float(value_t *v)
        {
            switch (v->type)
            {
                case VT_INT:    v->v_float = v->v_int; break;
                case VT_FLOAT:  return STATUS_OK;
                case VT_BOOL:   v->v_float = (v->v_bool) ? 1.0 : 0.0; break;
                case VT_STRING:
                {
                    // Parse integer/float number as string and cast to integer
                    io::InStringSequence s(v->v_str);
                    Tokenizer t(&s);
                    double fvalue;

                    switch (t.get_token(TF_GET))
                    {
                        case TT_IVALUE: fvalue = t.int_value(); break;
                        case TT_FVALUE: fvalue = t.float_value(); break;
                        case TT_TRUE:   fvalue = 1.0; break;
                        case TT_FALSE:  fvalue = 0.0; break;
                        default:
                            delete v->v_str;
                            v->type     = VT_UNDEF;
                            return STATUS_OK;
                    }

                    if (t.get_token(TF_GET) != TT_EOF)
                        return STATUS_BAD_FORMAT;
                    delete v->v_str;
                    v->v_float  = fvalue;
                    break;
                }
                case VT_NULL:
                case VT_UNDEF:
                    return STATUS_OK;
                default:
                    return STATUS_BAD_TYPE;
            }

            v->type     = VT_FLOAT;
            return STATUS_OK;
        }

        status_t cast_bool(value_t *v)
        {
            switch (v->type)
            {
                case VT_INT:    v->v_bool = v->v_int != 0; break;
                case VT_FLOAT:  v->v_bool = (v->v_float >= 0.5f) || (v->v_float <= -0.5f); break;
                case VT_BOOL:   return STATUS_OK;
                case VT_STRING:
                {
                    // Parse integer/float number as string and cast to integer
                    io::InStringSequence s(v->v_str);
                    Tokenizer t(&s);
                    bool bvalue;

                    switch (t.get_token(TF_GET))
                    {
                        case TT_IVALUE: bvalue = t.int_value() != 0; break;
                        case TT_FVALUE: bvalue = (t.float_value() > 0.5f) || (t.float_value() <= -0.5f); break;
                        case TT_TRUE:   bvalue = true; break;
                        case TT_FALSE:  bvalue = false; break;
                        default:
                            delete v->v_str;
                            v->type     = VT_UNDEF;
                            return STATUS_OK;
                    }

                    if (t.get_token(TF_GET) != TT_EOF)
                        return STATUS_BAD_FORMAT;
                    delete v->v_str;
                    v->v_bool   = bvalue;
                    break;
                }
                case VT_NULL:
                case VT_UNDEF:
                    return STATUS_OK;
                default:
                    return STATUS_BAD_TYPE;
            }

            v->type     = VT_BOOL;
            return STATUS_OK;
        }

        status_t cast_string(value_t *v)
        {
            LSPString tmp;

            switch (v->type)
            {
                case VT_INT:
                    if (!tmp.fmt_ascii("%ld", long(v->v_int)))
                        return STATUS_NO_MEM;
                    break;
                case VT_FLOAT:
                    if (!tmp.fmt_ascii("%f", double(v->v_float)))
                        return STATUS_NO_MEM;
                    break;
                case VT_BOOL:
                    if (!tmp.set_ascii((v->v_bool) ? "true" : "false"))
                        return STATUS_NO_MEM;
                    break;
                case VT_STRING:
                    return STATUS_OK;
                case VT_NULL:
                case VT_UNDEF:
                    return STATUS_OK;
                default:
                    return STATUS_BAD_TYPE;
            }

            LSPString *ns = tmp.release();
            if (ns == NULL)
                return STATUS_NO_MEM;

            v->type     = VT_STRING;
            v->v_str    = ns;

            return STATUS_OK;
        }

        status_t cast_string_ext(value_t *v)
        {
            LSPString tmp;

            switch (v->type)
            {
                case VT_INT:
                    if (!tmp.fmt_ascii("%ld", long(v->v_int)))
                        return STATUS_NO_MEM;
                    break;
                case VT_FLOAT:
                    if (!tmp.fmt_ascii("%f", double(v->v_float)))
                        return STATUS_NO_MEM;
                    break;
                case VT_BOOL:
                    if (!tmp.set_ascii((v->v_bool) ? "true" : "false"))
                        return STATUS_NO_MEM;
                    break;
                case VT_STRING:
                    return STATUS_OK;
                case VT_NULL:
                    if (!tmp.set_ascii("null"))
                        return STATUS_NO_MEM;
                    break;
                case VT_UNDEF:
                    if (!tmp.set_ascii("undef"))
                        return STATUS_NO_MEM;
                    break;
                default:
                    return STATUS_BAD_TYPE;
            }

            LSPString *ns = tmp.release();
            if (ns == NULL)
                return STATUS_NO_MEM;

            v->type     = VT_STRING;
            v->v_str    = ns;

            return STATUS_OK;
        }

        status_t cast_numeric(value_t *v)
        {
            switch (v->type)
            {
                case VT_INT:    return STATUS_OK;
                case VT_FLOAT:  return STATUS_OK;
                case VT_BOOL:
                    v->type     = VT_INT;
                    v->v_int    = (v->v_bool) ? 1 : 0;
                    break;
                case VT_STRING:
                {
                    // Parse integer/float number as string and cast to integer
                    value_t xv;
                    io::InStringSequence s(v->v_str);
                    Tokenizer t(&s);

                    switch (t.get_token(TF_GET))
                    {
                        case TT_IVALUE:
                            xv.type     = VT_INT;
                            xv.v_int    = t.int_value();
                            break;
                        case TT_FVALUE:
                            xv.type     = VT_FLOAT;
                            xv.v_float  = t.float_value();
                            break;
                        case TT_TRUE:
                            xv.type     = VT_INT;
                            xv.v_int    = 1;
                            break;
                        case TT_FALSE:
                            xv.type     = VT_INT;
                            xv.v_int    = 0;
                            break;
                        default:
                            delete v->v_str;
                            v->type     = VT_UNDEF;
                            return STATUS_OK;
                    }

                    if (t.get_token(TF_GET) != TT_EOF)
                        return STATUS_BAD_FORMAT;

                    delete v->v_str;
                    *v      = xv;
                    break;
                }
                case VT_NULL:
                case VT_UNDEF:
                    return STATUS_OK;
                default:
                    return STATUS_BAD_TYPE;
            }
            return STATUS_OK;
        }
    }
}


