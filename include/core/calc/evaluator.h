/*
 * evaluator.h
 *
 *  Created on: 18 сент. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_CALC_EVALUATOR_H_
#define CORE_CALC_EVALUATOR_H_

#include <core/calc/types.h>
#include <core/calc/Resolver.h>

namespace lsp
{
    namespace calc
    {
        struct expr_t;

        typedef Resolver       eval_env_t;

        typedef status_t (* evaluator_t)(value_t *value, const expr_t *expr, eval_env_t *env);


        status_t eval_ternary(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_xor(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_or(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_and(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_bit_xor(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_bit_or(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_bit_and(value_t *value, const expr_t *expr, eval_env_t *env);

        status_t eval_cmp(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_cmp_eq(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_cmp_ne(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_cmp_lt(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_cmp_gt(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_cmp_le(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_cmp_ge(value_t *value, const expr_t *expr, eval_env_t *env);

        status_t eval_icmp(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_icmp_eq(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_icmp_ne(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_icmp_lt(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_icmp_gt(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_icmp_le(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_icmp_ge(value_t *value, const expr_t *expr, eval_env_t *env);

        status_t eval_add(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_sub(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_iadd(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_isub(value_t *value, const expr_t *expr, eval_env_t *env);

        status_t eval_mul(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_div(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_imul(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_idiv(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_fmod(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_imod(value_t *value, const expr_t *expr, eval_env_t *env);

        status_t eval_power(value_t *value, const expr_t *expr, eval_env_t *env);

        status_t eval_strcat(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_strrep(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_strupper(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_strlower(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_strlen(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_strrev(value_t *value, const expr_t *expr, eval_env_t *env);

        status_t eval_int_cast(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_float_cast(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_string_cast(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_bool_cast(value_t *value, const expr_t *expr, eval_env_t *env);

        status_t eval_not(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_neg(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_psign(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_nsign(value_t *value, const expr_t *expr, eval_env_t *env);

        status_t eval_exists(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_db(value_t *value, const expr_t *expr, eval_env_t *env);

        status_t eval_resolve(value_t *value, const expr_t *expr, eval_env_t *env);
        status_t eval_value(value_t *value, const expr_t *expr, eval_env_t *env);
    }
}

#endif /* CORE_CALC_EVALUATOR_H_ */
