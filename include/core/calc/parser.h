/*
 * parser.h
 *
 *  Created on: 18 сент. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_CALC_PARSER_H_
#define CORE_CALC_PARSER_H_

#include <core/LSPString.h>
#include <core/calc/types.h>
#include <core/calc/evaluator.h>
#include <core/calc/Tokenizer.h>

namespace lsp
{
    namespace calc
    {
        enum operation_t
        {
            OP_LOAD,
            OP_TERNARY,

            // Floating-point operations
            OP_ADD,
            OP_SUB,
            OP_SIGN,
            OP_MUL,
            OP_DIV,

            // Integer operations
            OP_IADD,
            OP_ISUB,
            OP_IMUL,
            OP_POWER,
            OP_IDIV,
            OP_MOD,

            // Logical operations
            OP_AND,
            OP_OR,
            OP_NOT,
            OP_XOR,

            // Bitwise operations
            OP_BAND,
            OP_BOR,
            OP_BNOT,
            OP_BXOR,

            // Floating-point comparisons
            OP_LESS,
            OP_GREATER,
            OP_LESS_EQ,
            OP_GREATER_EQ,
            OP_NOT_EQ,
            OP_EQ,

            // Integer comparisons
            OP_ILESS,
            OP_IGREATER,
            OP_ILESS_EQ,
            OP_IGREATER_EQ,
            OP_INOT_EQ,
            OP_IEQ
        };

        typedef struct expr_t
        {
            evaluator_t     eval;
            union
            {
                struct
                {
                    expr_t     *pLeft;
                    expr_t     *pRight;
                    expr_t     *pCond;
                } sCalc;

                value_t     sValue;
            };
        } expr_t;

        void parse_destroy(expr_t *expr);

        status_t parse_ternary(expr_t **expr, Tokenizer *t, size_t flags);
        status_t parse_or(expr_t **expr, Tokenizer *t, size_t flags);
        status_t parse_xor(expr_t **expr, Tokenizer *t, size_t flags);
        status_t parse_and(expr_t **expr, Tokenizer *t, size_t flags);
        status_t parse_bit_or(expr_t **expr, Tokenizer *t, size_t flags);
        status_t parse_bit_xor(expr_t **expr, Tokenizer *t, size_t flags);
        status_t parse_bit_and(expr_t **expr, Tokenizer *t, size_t flags);
        status_t parse_cmp_eq(expr_t **expr, Tokenizer *t, size_t flags);
        status_t parse_cmp_rel(expr_t **expr, Tokenizer *t, size_t flags);
        status_t parse_addsub(expr_t **expr, Tokenizer *t, size_t flags);
        status_t parse_muldiv(expr_t **expr, Tokenizer *t, size_t flags);
        status_t parse_power(expr_t **expr, Tokenizer *t, size_t flags);
        status_t parse_not(expr_t **expr, Tokenizer *t, size_t flags);
        status_t parse_sign(expr_t **expr, Tokenizer *t, size_t flags);
        status_t parse_func(expr_t **expr, Tokenizer *t, size_t flags);

        status_t parse_expression(expr_t **expr, Tokenizer *t, size_t flags);
    }
}

#endif /* CORE_CALC_PARSER_H_ */
