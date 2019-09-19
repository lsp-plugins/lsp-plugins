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
//                case VT_STRING: // TODO
//                {
//                    break;
//                }
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
//                case VT_STRING: // TODO
//                {
//                    break;
//                }
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
                case VT_FLOAT:  *dst = (v->v_float > 0.5f) || (v->v_float < -0.5f); break;
                case VT_BOOL:   *dst = v->v_bool; break;
//                case VT_STRING: // TODO
//                {
//                    break;
//                }
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
                    if (!dst->fmt_ascii("%f", long(v->v_float)))
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


