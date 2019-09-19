/*
 * evaluator.cpp
 *
 *  Created on: 18 сент. 2019 г.
 *      Author: sadko
 */

#include <core/calc/parser.h>
#include <core/calc/evaluator.h>

namespace lsp
{
    namespace calc
    {
        status_t eval_ternary(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_xor(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_or(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_and(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_bit_xor(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_bit_or(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_bit_and(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_cmp(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_cmp_eq(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_cmp_ne(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_cmp_lt(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_cmp_gt(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_cmp_le(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_cmp_ge(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_icmp(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_icmp_eq(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_icmp_ne(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_icmp_lt(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_icmp_gt(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_icmp_le(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_icmp_ge(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_add(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_sub(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_iadd(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_isub(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_mul(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_div(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_imul(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_idiv(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_fmod(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_imod(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_power(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_not(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_psign(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_nsign(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_exists(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_db(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t eval_resolve(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            status_t res;

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
            ssize_t *indexes = reinterpret_cast<ssize_t *>(::malloc(expr->resolve.count));
            if (indexes == NULL)
                return STATUS_NO_MEM;

            value_t tmp;
            for (size_t i=0; i<expr->resolve.count; ++i)
            {
                expr_t *e = expr->resolve.items[i];

                // Evaluate and store index
                res = e->eval(&tmp, e, env);
                if (res == STATUS_OK)
                {
                    res = cast_int(&indexes[i], &tmp);
                    destroy_value(&tmp);
                }

                // All is OK?
                if (res != STATUS_OK)
                {
                    ::free(indexes);
                    return res;
                }
            }

            // Now we can resolve values
            res = env->resolve(value, expr->resolve.name, expr->resolve.count, indexes);
            ::free(indexes);

            return res;
        }

        status_t eval_value(value_t *value, const expr_t *expr, eval_env_t *env)
        {
            return copy_value(value, &expr->value);
        }
    }
}


