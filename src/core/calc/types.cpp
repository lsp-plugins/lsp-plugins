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

        status_t cast_int(ssize_t *dst, const value_t *v)
        {
            switch (v->type)
            {
                case VT_INT:    *dst = v->v_int; break;
                case VT_FLOAT:  *dst = ssize_t(v->v_float); break;
                case VT_BOOL:   *dst = (v->v_bool) ? 1 : 0; break;
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
                        default: return STATUS_BAD_FORMAT;
                    }

                    if (t.get_token(TF_GET) != TT_EOF)
                        return STATUS_BAD_FORMAT;
                    *dst    = ivalue;
                    break;
                }
                default:
                    return STATUS_BAD_TYPE;
            }

            return STATUS_OK;
        }

        status_t cast_float(float *dst, const value_t *v)
        {
            switch (v->type)
            {
                case VT_INT:    *dst = v->v_int; break;
                case VT_FLOAT:  *dst = v->v_float; break;
                case VT_BOOL:   *dst = (v->v_bool) ? 1.0 : 0.0; break;
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
                        default: return STATUS_BAD_FORMAT;
                    }

                    if (t.get_token(TF_GET) != TT_EOF)
                        return STATUS_BAD_FORMAT;
                    *dst    = fvalue;
                    break;
                }
                default:
                    return STATUS_BAD_TYPE;
            }

            return STATUS_OK;
        }

        status_t cast_bool(bool *dst, const value_t *v)
        {
            switch (v->type)
            {
                case VT_INT:    *dst = v->v_int != 0; break;
                case VT_FLOAT:  *dst = (v->v_float >= 0.5f) || (v->v_float <= -0.5f); break;
                case VT_BOOL:   *dst = v->v_bool; break;
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
                        default: return STATUS_BAD_FORMAT;
                    }

                    if (t.get_token(TF_GET) != TT_EOF)
                        return STATUS_BAD_FORMAT;
                    *dst    = bvalue;
                    break;
                }
                default:
                    return STATUS_BAD_TYPE;
            }

            return STATUS_OK;
        }

        status_t cast_string(LSPString *dst, const value_t *v)
        {
            switch (v->type)
            {
                case VT_INT:
                    if (!dst->fmt_ascii("%ld", long(v->v_int)))
                        return STATUS_NO_MEM;
                    break;
                case VT_FLOAT:
                    if (!dst->fmt_ascii("%f", double(v->v_float)))
                        return STATUS_NO_MEM;
                    break;
                case VT_BOOL:
                    if (!dst->set_ascii((v->v_bool) ? "true" : "false"))
                        return STATUS_NO_MEM;
                    break;
                case VT_STRING:
                    if (!dst->set(v->v_str))
                        return STATUS_NO_MEM;
                    break;
                default:
                    return STATUS_BAD_TYPE;
            }

            return STATUS_OK;
        }
    }
}


