/*
 * evaluator.cpp
 *
 *  Created on: 18 сент. 2019 г.
 *      Author: sadko
 */

#include <math.h>
#include <core/calc/parser.h>
#include <core/calc/evaluator.h>

namespace lsp
{
    namespace calc
    {
        #define INT_OP(eval_name, oper) \
            status_t eval_name(value_t *value, const expr_t *expr, eval_env_t *env) \
            { \
                status_t res = expr->calc.left->eval(value, expr->calc.left, env); \
                if (res != STATUS_OK) \
                    return res; \
                \
                cast_int(value); \
                if (value->type == VT_UNDEF) \
                    return STATUS_OK; \
                else if (value->type == VT_NULL) \
                { \
                    value->type = VT_UNDEF; \
                    return STATUS_OK; \
                } \
                \
                value_t right; \
                init_value(&right); \
                res = expr->calc.right->eval(&right, expr->calc.right, env); \
                if (res != STATUS_OK) \
                { \
                    destroy_value(&right); \
                    destroy_value(value); \
                    return res; \
                } \
                \
                cast_int(&right); \
                switch (right.type) \
                { \
                    case VT_INT: value->v_int = value->v_int oper right.v_int; break; \
                    case VT_NULL: value->type = VT_UNDEF; break; \
                    case VT_UNDEF: break; \
                    default: res = STATUS_BAD_TYPE; break; \
                } \
                \
                if (res != STATUS_OK) \
                    destroy_value(value); \
                destroy_value(&right); \
                \
                return res; \
            }

        status_t eval_add(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = expr->calc.left->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;

            cast_numeric(value);
            if (value->type == VT_UNDEF)
                return STATUS_OK;
            else if (value->type == VT_NULL)
            {
                value->type = VT_UNDEF;
                return STATUS_OK;
            }

            value_t right;
            init_value(&right);

            res = expr->calc.right->eval(&right, expr->calc.right, env);
            if (res != STATUS_OK)
            {
                destroy_value(&right);
                destroy_value(value);
                return res;
            }

            cast_numeric(&right);

            switch (right.type)
            {
                case VT_INT:
                    if (value->type == VT_INT)
                        value->v_int    = value->v_int + right.v_int;
                    else
                        value->v_float  = value->v_float + right.v_int;
                    break;
                case VT_FLOAT:
                    if (value->type == VT_INT)
                        value->v_float  = value->v_int + right.v_float;
                    else
                        value->v_float  = value->v_float + right.v_float;
                    value->type = VT_FLOAT;
                    break;
                case VT_NULL:
                    value->type = VT_UNDEF;
                    break;
                case VT_UNDEF: break;
                default: res = STATUS_BAD_TYPE; break;
            }

            if (res != STATUS_OK)
                destroy_value(value);
            destroy_value(&right);

            return res;
        }

        status_t eval_sub(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = expr->calc.left->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;

            cast_numeric(value);
            if (value->type == VT_UNDEF)
                return STATUS_OK;
            else if (value->type == VT_NULL)
            {
                value->type = VT_UNDEF;
                return STATUS_OK;
            }

            value_t right;
            init_value(&right);
            res = expr->calc.right->eval(&right, expr->calc.right, env);
            if (res != STATUS_OK)
            {
                destroy_value(&right);
                destroy_value(value);
                return res;
            }

            cast_numeric(&right);

            switch (right.type)
            {
                case VT_INT:
                    if (value->type == VT_INT)
                        value->v_int    = value->v_int - right.v_int;
                    else
                        value->v_float  = value->v_float - double(right.v_int);
                    break;
                case VT_FLOAT:
                    if (value->type == VT_INT)
                        value->v_float  = double(value->v_int) - right.v_float;
                    else
                        value->v_float  = value->v_float - right.v_float;
                    value->type = VT_FLOAT;
                    break;
                case VT_NULL:
                    value->type = VT_UNDEF;
                    break;
                case VT_UNDEF: break;
                default: res = STATUS_BAD_TYPE; break;
            }

            if (res != STATUS_OK)
                destroy_value(value);
            destroy_value(&right);

            return res;
        }

        status_t eval_mul(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = expr->calc.left->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;

            cast_numeric(value);
            if (value->type == VT_UNDEF)
                return STATUS_OK;
            else if (value->type == VT_NULL)
            {
                value->type = VT_UNDEF;
                return STATUS_OK;
            }

            value_t right;
            init_value(&right);
            res = expr->calc.right->eval(&right, expr->calc.right, env);
            if (res != STATUS_OK)
            {
                destroy_value(&right);
                destroy_value(value);
                return res;
            }

            cast_numeric(&right);

            switch (right.type)
            {
                case VT_INT:
                    if (value->type == VT_INT)
                        value->v_int    = value->v_int * right.v_int;
                    else
                        value->v_float  = value->v_float * double(right.v_int);
                    break;
                case VT_FLOAT:
                    if (value->type == VT_INT)
                        value->v_float  = double(value->v_int) * right.v_float;
                    else
                        value->v_float  = value->v_float * right.v_float;
                    value->type = VT_FLOAT;
                    break;
                case VT_NULL:
                    value->type = VT_UNDEF;
                    break;
                case VT_UNDEF: break;
                default: res = STATUS_BAD_TYPE; break;
            }

            if (res != STATUS_OK)
                destroy_value(value);
            destroy_value(&right);

            return res;
        }

        status_t eval_div(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = expr->calc.left->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;

            cast_numeric(value);
            if (value->type == VT_UNDEF)
                return STATUS_OK;
            else if (value->type == VT_NULL)
            {
                value->type = VT_UNDEF;
                return STATUS_OK;
            }

            value_t right;
            init_value(&right);
            res = expr->calc.right->eval(&right, expr->calc.right, env);
            if (res != STATUS_OK)
            {
                destroy_value(&right);
                destroy_value(value);
                return res;
            }

            cast_numeric(&right);

            switch (right.type)
            {
                case VT_INT:
                    if (value->type == VT_INT)
                    {
                        if (right.v_int != 0)
                            value->v_int    = value->v_int / right.v_int;
                        else
                            value->type     = VT_UNDEF;
                    }
                    else
                        value->v_float  = value->v_float / double(right.v_int);
                    break;
                case VT_FLOAT:
                    if (value->type == VT_INT)
                        value->v_float  = double(value->v_int) / right.v_float;
                    else
                        value->v_float  = value->v_float / right.v_float;
                    value->type = VT_FLOAT;
                    break;
                case VT_NULL:
                    value->type = VT_UNDEF;
                    break;
                case VT_UNDEF: break;
                default: res = STATUS_BAD_TYPE; break;
            }

            if (res != STATUS_OK)
                destroy_value(value);
            destroy_value(&right);

            return res;
        }

        INT_OP(eval_iadd, + );
        INT_OP(eval_isub, - );
        INT_OP(eval_imul, * );
        INT_OP(eval_idiv, / );
        INT_OP(eval_bit_or, | );
        INT_OP(eval_bit_and, & );
        INT_OP(eval_bit_xor, ^ );

        status_t eval_imod(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = expr->calc.left->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;

            cast_int(value);
            if (value->type == VT_UNDEF)
                return STATUS_OK;
            else if (value->type == VT_NULL)
            {
                value->type = VT_UNDEF;
                return STATUS_OK;
            }

            value_t right;
            init_value(&right);
            res = expr->calc.right->eval(&right, expr->calc.right, env);
            if (res != STATUS_OK)
            {
                destroy_value(&right);
                destroy_value(value);
                return res;
            }

            cast_int(&right);
            switch (right.type)
            {
                case VT_INT:
                    if (right.v_int != 0)
                        value->v_int = value->v_int % right.v_int;
                    else
                        value->type  = VT_UNDEF;
                    break;
                case VT_NULL: value->type = VT_UNDEF; break;
                case VT_UNDEF: break;
                default: res = STATUS_BAD_TYPE; break;
            }

            if (res != STATUS_OK)
                destroy_value(value);
            destroy_value(&right);

            return res;
        }

        status_t eval_fmod(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = expr->calc.left->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;

            cast_float(value);
            if (value->type == VT_UNDEF)
                return STATUS_OK;
            else if (value->type == VT_NULL)
            {
                value->type = VT_UNDEF;
                return STATUS_OK;
            }

            value_t right;
            init_value(&right);
            res = expr->calc.right->eval(&right, expr->calc.right, env);
            if (res != STATUS_OK)
            {
                destroy_value(&right);
                destroy_value(value);
                return res;
            }

            cast_float(&right);
            switch (right.type)
            {
                case VT_FLOAT: value->v_float = fmod(value->v_float, right.v_float); break;
                case VT_NULL: value->type = VT_UNDEF; break;
                case VT_UNDEF: break;
                default: res = STATUS_BAD_TYPE; break;
            }

            if (res != STATUS_OK)
                destroy_value(value);
            destroy_value(&right);

            return res;
        }

        status_t eval_xor(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            // Test left argument
            status_t res = expr->calc.left->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;

            value_t right;
            init_value(&right);
            res = cast_bool(value);
            if (res == STATUS_OK)
                res = expr->calc.right->eval(&right, expr->calc.right, env);
            if (res != STATUS_OK)
            {
                destroy_value(&right);
                destroy_value(value);
                return res;
            }

            // Test right argument
            res = cast_bool(&right);
            if (res == STATUS_OK)
                value->v_bool = !(value->v_bool == right.v_bool);
            else
                destroy_value(value);
            destroy_value(&right);

            return res;
        }

        status_t eval_or(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            // Test left argument
            status_t res = expr->calc.left->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;

            res = cast_bool(value);
            if (res != STATUS_OK)
            {
                destroy_value(value);
                return res;
            }
            else if (value->v_bool)
                return STATUS_OK;

            // Test right argument
            destroy_value(value);
            res = expr->calc.right->eval(value, expr->calc.right, env);
            if (res != STATUS_OK)
                return res;

            res = cast_bool(value);
            if (res != STATUS_OK)
                destroy_value(value);

            return res;
        }

        status_t eval_and(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            // Test left argument
            status_t res = expr->calc.left->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;

            res = cast_bool(value);
            if (res != STATUS_OK)
            {
                destroy_value(value);
                return res;
            }
            else if (!value->v_bool)
                return STATUS_OK;

            // Test right argument
            destroy_value(value);
            res = expr->calc.right->eval(value, expr->calc.right, env);
            if (res != STATUS_OK)
                return res;

            res = cast_bool(value);
            if (res != STATUS_OK)
                destroy_value(value);

            return res;
        }

        status_t eval_cmp(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            // Fetch left argument and test for UNDEF
            status_t res = expr->calc.left->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;

            // Fetch right argument and test for UNDEF
            value_t right;
            init_value(&right);
            res = expr->calc.right->eval(&right, expr->calc.right, env);
            if (res != STATUS_OK)
            {
                destroy_value(&right);
                destroy_value(value);
                return res;
            }

            if (value->type == VT_UNDEF)
            {
                value->type     = VT_INT;
                value->v_int    = (right.type == VT_UNDEF) ? 0 : -1;
                destroy_value(&right);
                return STATUS_OK;
            }
            else if (right.type == VT_UNDEF)
            {
                value->type     = VT_INT;
                value->v_int    = 1;
                destroy_value(&right);
                return STATUS_OK;
            }

            // Prevent from NULLs
            if (value->type == VT_NULL)
            {
                value->type     = VT_INT;
                value->v_int    = (right.type == VT_NULL) ? 0 : -1;
                destroy_value(&right);
                return STATUS_OK;
            }
            else if (right.type == VT_NULL)
            {
                value->type     = VT_INT;
                value->v_int    = 1;
                destroy_value(&right);
                return STATUS_OK;
            }

            // Perform comparison matrix
            switch (value->type)
            {
                case VT_INT:
                {
                    switch (right.type)
                    {
                        case VT_INT:
                            value->type     = VT_INT;
                            value->v_int    =
                                    (value->v_int < right.v_int) ? -1 :
                                    (value->v_int > right.v_int) ? 1 : 0;
                            break;
                        case VT_FLOAT:
                            value->type     = VT_INT;
                            value->v_int    =
                                    (double(value->v_int) < right.v_float) ? -1 :
                                    (double(value->v_int) > right.v_float) ? 1 : 0;
                            break;
                        case VT_BOOL:
                        {
                            ssize_t ivalue  = (value->v_bool) ? 1 : 0;
                            value->type     = VT_INT;
                            value->v_int    =
                                    (value->v_int < ivalue) ? -1 :
                                    (value->v_int > ivalue) ? 1 : 0;
                            break;
                        }
                        case VT_STRING:
                        {
                            res = cast_string(value);
                            if (res == STATUS_OK)
                            {
                                ssize_t ivalue  = value->v_str->compare_to(right.v_str);
                                destroy_value(value);
                                value->type     = VT_INT;
                                value->v_int    = ivalue;
                            }
                            break;
                        }
                        default:
                            res = STATUS_BAD_TYPE;
                            break;
                    } // switch (int)
                    break;
                }
                case VT_FLOAT:
                {
                    switch (right.type)
                    {
                        case VT_INT:
                            value->type     = VT_INT;
                            value->v_int    =
                                    (value->v_float < right.v_int) ? -1 :
                                    (value->v_float > right.v_int) ? 1 : 0;
                            break;
                        case VT_FLOAT:
                            value->type     = VT_INT;
                            value->v_int    =
                                    (value->v_float < right.v_float) ? -1 :
                                    (value->v_float > right.v_float) ? 1 : 0;
                            break;
                        case VT_BOOL:
                        {
                            ssize_t ivalue  = (value->v_bool) ? 1 : 0;
                            value->type     = VT_INT;
                            value->v_int    =
                                    (value->v_float < ivalue) ? -1 :
                                    (value->v_float > ivalue) ? 1 : 0;
                            break;
                        }
                        case VT_STRING:
                        {
                            res = cast_string(value);
                            if (res == STATUS_OK)
                            {
                                ssize_t ivalue  = value->v_str->compare_to(right.v_str);
                                destroy_value(value);
                                value->type     = VT_INT;
                                value->v_int    = ivalue;
                            }
                            break;
                        }
                        default:
                            res = STATUS_BAD_TYPE;
                            break;
                    } // switch (float)
                    break;
                }

                case VT_BOOL:
                {
                    ssize_t xvalue = (value->v_bool) ? 1 : 0;
                    switch (right.type)
                    {
                        case VT_INT:
                            value->type     = VT_INT;
                            value->v_int    =
                                    (xvalue < right.v_int) ? -1 :
                                    (xvalue > right.v_int) ? 1 : 0;
                            break;
                        case VT_FLOAT:
                            value->type     = VT_INT;
                            value->v_int    =
                                    (xvalue < right.v_float) ? -1 :
                                    (xvalue > right.v_float) ? 1 : 0;
                            break;
                        case VT_BOOL:
                        {
                            ssize_t ivalue  = (value->v_bool) ? 1 : 0;
                            value->type     = VT_INT;
                            value->v_int    =
                                    (xvalue < ivalue) ? -1 :
                                    (xvalue > ivalue) ? 1 : 0;
                            break;
                        }
                        case VT_STRING:
                        {
                            res = cast_string(value);
                            if (res == STATUS_OK)
                            {
                                ssize_t ivalue  = value->v_str->compare_to(right.v_str);
                                destroy_value(value);
                                value->type     = VT_INT;
                                value->v_int    = ivalue;
                            }
                            break;
                        }
                        default:
                            res = STATUS_BAD_TYPE;
                            break;
                    } // switch (float)
                    break;
                }

                case VT_STRING:
                {
                    res = cast_string(&right);
                    if (res == STATUS_OK)
                    {
                        ssize_t ivalue  = value->v_str->compare_to(right.v_str);
                        destroy_value(value);
                        value->type     = VT_INT;
                        value->v_int    = ivalue;
                    }
                    break;
                }

                default:
                    res = STATUS_BAD_TYPE;
                    break;
            }

            if (res != STATUS_OK)
                destroy_value(value);
            destroy_value(&right);

            return res;
        }

        status_t eval_cmp_eq(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = eval_cmp(value, expr, env);
            if (res != STATUS_OK)
                return res;
            if (value->type == VT_INT)
            {
                value->type     = VT_BOOL;
                value->v_bool   = (value->v_int == 0);
            }

            return res;
        }

        status_t eval_cmp_ne(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = eval_cmp(value, expr, env);
            if (res != STATUS_OK)
                return res;
            if (value->type == VT_INT)
            {
                value->type     = VT_BOOL;
                value->v_bool   = (value->v_int != 0);
            }

            return res;
        }

        status_t eval_cmp_lt(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = eval_cmp(value, expr, env);
            if (res != STATUS_OK)
                return res;
            if (value->type == VT_INT)
            {
                value->type     = VT_BOOL;
                value->v_bool   = (value->v_int < 0);
            }

            return res;
        }

        status_t eval_cmp_gt(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = eval_cmp(value, expr, env);
            if (res != STATUS_OK)
                return res;
            if (value->type == VT_INT)
            {
                value->type     = VT_BOOL;
                value->v_bool   = (value->v_int > 0);
            }

            return res;
        }

        status_t eval_cmp_le(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = eval_cmp(value, expr, env);
            if (res != STATUS_OK)
                return res;
            if (value->type == VT_INT)
            {
                value->type     = VT_BOOL;
                value->v_bool   = (value->v_int <= 0);
            }

            return res;
        }

        status_t eval_cmp_ge(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = eval_cmp(value, expr, env);
            if (res != STATUS_OK)
                return res;
            if (value->type == VT_INT)
            {
                value->type     = VT_BOOL;
                value->v_bool   = (value->v_int >= 0);
            }

            return res;
        }



        status_t eval_icmp(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            // Fetch left argument and test for UNDEF
            status_t res = expr->calc.left->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;

            // Fetch right argument and test for UNDEF
            value_t right;
            init_value(&right);
            res = expr->calc.right->eval(&right, expr->calc.right, env);
            if (res != STATUS_OK)
            {
                destroy_value(&right);
                destroy_value(value);
                return res;
            }

            cast_int(value);
            cast_int(&right);
            if (value->type == VT_UNDEF)
            {
                value->type     = VT_INT;
                value->v_int    = (right.type == VT_UNDEF) ? 0 : -1;
                destroy_value(&right);
                return STATUS_OK;
            }
            else if (right.type == VT_UNDEF)
            {
                value->type     = VT_INT;
                value->v_int    = 1;
                destroy_value(&right);
                return STATUS_OK;
            }

            // Prevent from NULLs
            if (value->type == VT_NULL)
            {
                value->type     = VT_INT;
                value->v_int    = (right.type == VT_NULL) ? 0 : -1;
                destroy_value(&right);
                return STATUS_OK;
            }
            else if (right.type == VT_NULL)
            {
                value->type     = VT_INT;
                value->v_int    = 1;
                destroy_value(&right);
                return STATUS_OK;
            }

            // Perform compare
            value->v_int =
                    (value->v_int < right.v_int) ? -1 :
                    (value->v_int > right.v_int) ? 1 : 0;
            return STATUS_OK;
        }

        status_t eval_icmp_eq(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = eval_icmp(value, expr, env);
            if (res != STATUS_OK)
                return res;
            if (value->type == VT_INT)
            {
                value->type     = VT_BOOL;
                value->v_bool   = (value->v_int == 0);
            }

            return res;
        }

        status_t eval_icmp_ne(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = eval_icmp(value, expr, env);
            if (res != STATUS_OK)
                return res;
            if (value->type == VT_INT)
            {
                value->type     = VT_BOOL;
                value->v_bool   = (value->v_int != 0);
            }

            return res;
        }

        status_t eval_icmp_lt(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = eval_icmp(value, expr, env);
            if (res != STATUS_OK)
                return res;
            if (value->type == VT_INT)
            {
                value->type     = VT_BOOL;
                value->v_bool   = (value->v_int < 0);
            }

            return res;
        }

        status_t eval_icmp_gt(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = eval_icmp(value, expr, env);
            if (res != STATUS_OK)
                return res;
            if (value->type == VT_INT)
            {
                value->type     = VT_BOOL;
                value->v_bool   = (value->v_int > 0);
            }

            return res;
        }

        status_t eval_icmp_le(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = eval_icmp(value, expr, env);
            if (res != STATUS_OK)
                return res;
            if (value->type == VT_INT)
            {
                value->type     = VT_BOOL;
                value->v_bool   = (value->v_int <= 0);
            }

            return res;
        }

        status_t eval_icmp_ge(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = eval_icmp(value, expr, env);
            if (res != STATUS_OK)
                return res;
            if (value->type == VT_INT)
            {
                value->type     = VT_BOOL;
                value->v_bool   = (value->v_int >= 0);
            }

            return res;
        }

        status_t eval_power(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = expr->calc.left->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;

            value_t right;
            init_value(&right);
            cast_float(value);
            switch (value->type)
            {
                case VT_FLOAT:
                    res = expr->calc.right->eval(&right, expr->calc.right, env);
                    break;
                case VT_NULL:
                    value->type = VT_UNDEF;
                    return STATUS_OK;
                case VT_UNDEF:
                    return STATUS_OK;
                default:
                    res = STATUS_BAD_TYPE;
                    break;
            }

            if (res == STATUS_OK)
            {
                cast_float(&right);
                switch (right.type)
                {
                    case VT_FLOAT:
                        value->v_float  = ::pow(value->v_float, right.v_float);
                        break;
                    case VT_NULL:
                    case VT_UNDEF:
                        value->type = VT_UNDEF;
                        break;
                    default:
                        res = STATUS_BAD_TYPE;
                        break;
                }
            }

            destroy_value(&right);
            if (res != STATUS_OK)
                destroy_value(value);

            return res;
        }

        status_t eval_neg(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res =  expr->calc.left->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;

            if (value->type == VT_STRING)
                cast_numeric(value);

            switch (value->type)
            {
                case VT_INT:
                    value->type     = VT_INT;
                    value->v_int    = ~value->v_int;
                    break;
                case VT_FLOAT:
                    value->type     = VT_FLOAT;
                    value->v_float  = ~ssize_t(value->v_float);
                    break;
                case VT_BOOL:
                    value->type     = VT_BOOL;
                    value->v_bool   = !value->v_bool;
                    break;
                case VT_NULL:
                    value->type     = VT_UNDEF;
                    break;
                case VT_UNDEF:
                    break;
                default:
                    res             = STATUS_BAD_TYPE;
                    break;
            }

            if (res != STATUS_OK)
                destroy_value(value);

            return res;
        }

        status_t eval_not(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res =  expr->calc.left->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;

            cast_bool(value);
            switch (value->type)
            {
                case VT_BOOL:
                    value->v_bool   = !value->v_bool;
                    break;
                case VT_NULL:
                    value->type     = VT_UNDEF;
                    break;
                case VT_UNDEF:
                    break;
                default:
                    res             = STATUS_BAD_TYPE;
                    break;
            }

            if (res != STATUS_OK)
                destroy_value(value);

            return res;
        }

        status_t eval_psign(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return  expr->calc.left->eval(value, expr->calc.left, env);
        }

        status_t eval_nsign(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res =  expr->calc.left->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;

            cast_numeric(value);
            switch (value->type)
            {
                case VT_INT:
                    value->type     = VT_INT;
                    value->v_int    = -value->v_int;
                    break;
                case VT_FLOAT:
                    value->type     = VT_FLOAT;
                    value->v_float  = -value->v_float;
                    break;
                case VT_NULL:
                    value->type     = VT_UNDEF;
                    break;
                case VT_UNDEF:
                    break;
                default:
                    res = STATUS_BAD_TYPE;
                    break;
            }

            if (res != STATUS_OK)
                destroy_value(value);

            return res;
        }

        status_t eval_exists(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res    =  expr->calc.left->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;

            bool exists     = value->type != VT_UNDEF;
            destroy_value(value);

            value->type     = VT_BOOL;
            value->v_bool   = exists;

            return STATUS_OK;
        }

        status_t eval_db(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res =  expr->calc.left->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;

            cast_float(value);
            switch (value->type)
            {
                case VT_FLOAT:
                    value->v_float  = exp(value->v_float * M_LN10 * 0.05);
                    break;
                case VT_NULL:
                    value->type     = VT_UNDEF;
                    break;
                case VT_UNDEF:
                    break;
                default:
                    res = STATUS_BAD_TYPE;
                    break;
            }

            if (res != STATUS_OK)
                destroy_value(value);

            return res;
        }

        status_t eval_resolve(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res;
            if (env == NULL)
            {
                value->type     = VT_UNDEF;
                value->v_str    = NULL;
                return STATUS_OK;
            }

            // No indexes? Do simple stuff
            if (expr->resolve.count <= 0)
            {
                res = env->resolve(value, expr->resolve.name, 0, NULL);
                if (res != STATUS_NOT_FOUND)
                    return res;

                value->type     = VT_UNDEF;
                value->v_str    = NULL;
                return STATUS_OK;
            }

            // Compute index values
            ssize_t *indexes = reinterpret_cast<ssize_t *>(::malloc(expr->resolve.count * sizeof(ssize_t)));
            if (indexes == NULL)
                return STATUS_NO_MEM;

            value_t tmp;
            init_value(&tmp);
            for (size_t i=0; i<expr->resolve.count; ++i)
            {
                expr_t *e = expr->resolve.items[i];

                // Evaluate and store index
                res = e->eval(&tmp, e, env);
                if (res == STATUS_OK)
                {
                    res = cast_int(&tmp);
                    if (res == STATUS_OK)
                        indexes[i] = tmp.v_int;
                    destroy_value(&tmp);
                }

                // All is OK?
                if (res != STATUS_OK)
                {
                    ::free(indexes);
                    destroy_value(&tmp);
                    return res;
                }
            }

            // Now we can resolve values
            res = env->resolve(value, expr->resolve.name, expr->resolve.count, indexes);
            ::free(indexes);
            destroy_value(&tmp);

            return res;
        }

        status_t eval_value(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return copy_value(value, &expr->value);
        }

        status_t eval_ternary(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = expr->calc.cond->eval(value, expr->calc.cond, env);
            if (res != STATUS_OK)
                return res;
            cast_bool(value);
            if ((value->type) != VT_BOOL)
            {
                destroy_value(value);
                return STATUS_OK;
            }

            // Determine which expression to execute
            expr = (value->v_bool) ? expr->calc.left : expr->calc.right;

            destroy_value(value);
            return expr->eval(value, expr, env);
        }

        status_t eval_strcat(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = expr->calc.left->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;
            if ((res = cast_string_ext(value)) != STATUS_OK)
            {
                destroy_value(value);
                return res;
            }

            value_t right;
            init_value(&right);
            res = expr->calc.right->eval(&right, expr->calc.right, env);
            if (res != STATUS_OK)
            {
                destroy_value(&right);
                destroy_value(value);
                return res;
            }
            if ((res = cast_string_ext(&right)) != STATUS_OK)
            {
                destroy_value(value);
                destroy_value(&right);
                return res;
            }

            if (!value->v_str->append(right.v_str))
            {
                destroy_value(value);
                res = STATUS_NO_MEM;
            }
            destroy_value(&right);

            return res;
        }

        status_t eval_strrep(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = expr->calc.left->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;
            if ((res = cast_string_ext(value)) != STATUS_OK)
            {
                destroy_value(value);
                return res;
            }

            value_t right;
            init_value(&right);
            res = expr->calc.right->eval(&right, expr->calc.right, env);
            if (res != STATUS_OK)
            {
                destroy_value(&right);
                destroy_value(value);
                return res;
            }
            cast_int(&right);
            if ((right.type == VT_NULL) || (right.type == VT_UNDEF) || (right.v_int < 0))
            {
                destroy_value(&right);
                destroy_value(value);
                return STATUS_OK;
            }

            // Perform string repeat
            LSPString tmp;
            tmp.swap(value->v_str);
            size_t x = right.v_int;
            while (x)
            {
                if (x & 1)
                {
                    if (!value->v_str->append(&tmp))
                    {
                        res = STATUS_NO_MEM;
                        break;
                    }
                }
                if (x >>= 1)
                {
                    if (!tmp.append(&tmp))
                    {
                        res = STATUS_NO_MEM;
                        break;
                    }
                }
            }

            if (res != STATUS_OK)
                destroy_value(value);
            destroy_value(&right);

            return res;
        }

        status_t eval_strupper(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = expr->calc.left->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;
            cast_string(value);
            switch (value->type)
            {
                case VT_STRING:
                    value->v_str->toupper();
                    break;
                case VT_NULL:
                    value->type     = VT_UNDEF;
                    break;
                case VT_UNDEF:
                    break;
                default:
                    res = STATUS_BAD_TYPE;
                    break;
            }

            if (res != STATUS_OK)
                destroy_value(value);

            return res;
        }

        status_t eval_strlower(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = expr->calc.left->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;
            cast_string(value);
            switch (value->type)
            {
                case VT_STRING:
                    value->v_str->tolower();
                    break;
                case VT_NULL:
                    value->type     = VT_UNDEF;
                    break;
                case VT_UNDEF:
                    break;
                default:
                    res = STATUS_BAD_TYPE;
                    break;
            }

            if (res != STATUS_OK)
                destroy_value(value);

            return res;
        }

        status_t eval_strlen(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = expr->calc.left->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;
            cast_string(value);
            switch (value->type)
            {
                case VT_STRING:
                {
                    int len         = value->v_str->length();
                    destroy_value(value);
                    value->type     = VT_INT;
                    value->v_int    = len;
                    break;
                }
                case VT_NULL:
                    value->type     = VT_UNDEF;
                    break;
                case VT_UNDEF:
                    break;
                default:
                    res = STATUS_BAD_TYPE;
                    break;
            }

            if (res != STATUS_OK)
                destroy_value(value);

            return res;
        }

        status_t eval_strrev(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = expr->calc.left->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;
            cast_string(value);
            switch (value->type)
            {
                case VT_STRING:
                {
                    value->v_str->reverse();
                    break;
                }
                case VT_NULL:
                    value->type     = VT_UNDEF;
                    break;
                case VT_UNDEF:
                    break;
                default:
                    res = STATUS_BAD_TYPE;
                    break;
            }

            if (res != STATUS_OK)
                destroy_value(value);

            return res;
        }

        status_t eval_int_cast(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = expr->calc.left->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;

            res = cast_int(value);
            if (res != STATUS_OK)
                destroy_value(value);

            return res;
        }

        status_t eval_float_cast(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = expr->calc.left->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;

            res = cast_float(value);
            if (res != STATUS_OK)
                destroy_value(value);

            return res;
        }

        status_t eval_string_cast(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = expr->calc.left->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;

            res = cast_string(value);
            if (res != STATUS_OK)
                destroy_value(value);

            return res;
        }

        status_t eval_bool_cast(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res = expr->calc.left->eval(value, expr->calc.left, env);
            if (res != STATUS_OK)
                return res;

            res = cast_bool(value);
            if (res != STATUS_OK)
                destroy_value(value);

            return res;
        }
    }
}


