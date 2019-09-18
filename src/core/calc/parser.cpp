/*
 * parser.cpp
 *
 *  Created on: 18 сент. 2019 г.
 *      Author: sadko
 */

#include <core/calc/parser.h>

namespace lsp
{
    namespace calc
    {
        void parse_destroy(expr_t *expr)
        {
            // TODO
        }

        status_t parse_func(expr_t **expr, Tokenizer *t, size_t flags)
        {
            // TODO
            return STATUS_OK;
        }

        status_t parse_sign(expr_t **expr, Tokenizer *t, size_t flags)
        {
            // Check token
            token_t tok = t->get_token(flags);

            // Parse right part
            status_t res;
            expr_t *right   = NULL;

            switch(tok)
            {
                case TT_ADD:
                case TT_SUB:
                    res = parse_sign(&right, t, flags);
                    break;
                default:
                    res = parse_func(&right, t, flags);
                    break;
            }
            if (res != STATUS_OK)
                return res;

            // Create binding between left and right
            expr_t *bind        = reinterpret_cast<expr_t *>(::malloc(sizeof(expr_t)));
            if (bind == NULL)
            {
                parse_destroy(right);
                return STATUS_NO_MEM;
            }
            switch (tok)
            {
                case TT_ADD:            bind->eval  = eval_nsign; break;
                case TT_SUB:            bind->eval  = eval_psign; break;
                default:                bind->eval  = NULL; break;
            }
            bind->sCalc.pLeft   = right;
            bind->sCalc.pRight  = NULL;
            bind->sCalc.pCond   = NULL;
            return STATUS_OK;
        }

        status_t parse_not(expr_t **expr, Tokenizer *t, size_t flags)
        {
            // Check token
            token_t tok = t->get_token(flags);

            // Parse right part
            expr_t *right   = NULL;
            status_t res    = ((tok == TT_NOT) || (tok == TT_BNOT)) ?
                parse_not(&right, t, TF_GET) :
                parse_sign(&right, t, TF_NONE);
            if (res != STATUS_OK)
                return res;
            else if ((tok != TT_NOT) && (tok != TT_BNOT))
            {
                *expr       = right;
                return res;
            }

            // Create binding between left and right
            expr_t *bind        = reinterpret_cast<expr_t *>(::malloc(sizeof(expr_t)));
            if (bind == NULL)
            {
                parse_destroy(right);
                return STATUS_NO_MEM;
            }
            bind->eval          = eval_not;
            bind->sCalc.pLeft   = right;
            bind->sCalc.pRight  = NULL;
            bind->sCalc.pCond   = NULL;
            return STATUS_OK;
        }

        status_t parse_power(expr_t **expr, Tokenizer *t, size_t flags)
        {
            expr_t *left = NULL, *right = NULL;

            // Parse left part
            status_t res = parse_not(&left, t, flags);
            if (res != STATUS_OK)
                return res;

            // Check token
            token_t tok = t->get_token(TF_NONE);
            if (tok != TT_POW)
            {
                *expr   = left;
                return STATUS_OK;
            }

            // Parse right part
            res = parse_power(&right, t, TF_GET);
            if (res != STATUS_OK)
            {
                parse_destroy(left);
                return res;
            }

            // Create binding between left and right
            expr_t *bind        = reinterpret_cast<expr_t *>(::malloc(sizeof(expr_t)));
            if (bind == NULL)
            {
                parse_destroy(left);
                parse_destroy(right);
                return STATUS_NO_MEM;
            }
            bind->eval          = eval_power;
            bind->sCalc.pLeft   = left;
            bind->sCalc.pRight  = right;
            bind->sCalc.pCond   = NULL;
            return STATUS_OK;
        }

        status_t parse_muldiv(expr_t **expr, Tokenizer *t, size_t flags)
        {
            expr_t *left = NULL, *right = NULL;

            // Parse left part
            status_t res = parse_power(&left, t, flags);
            if (res != STATUS_OK)
                return res;

            // Check token
            token_t tok = t->get_token(TF_NONE);
            switch (tok)
            {
                case TT_MUL:
                case TT_DIV:
                case TT_IMUL:
                case TT_IDIV:
                case TT_FMOD:
                case TT_IMOD:
                    break;
                default:
                    *expr = left;
                    return res;
            }

            // Parse right part
            res = parse_muldiv(&right, t, TF_GET);
            if (res != STATUS_OK)
            {
                parse_destroy(left);
                return res;
            }

            // Create binding between left and right
            expr_t *bind        = reinterpret_cast<expr_t *>(::malloc(sizeof(expr_t)));
            if (bind == NULL)
            {
                parse_destroy(left);
                parse_destroy(right);
                return STATUS_NO_MEM;
            }
            switch (tok)
            {
                case TT_MUL:            bind->eval  = eval_mul; break;
                case TT_DIV:            bind->eval  = eval_div; break;
                case TT_IMUL:           bind->eval  = eval_imul; break;
                case TT_IDIV:           bind->eval  = eval_idiv; break;
                case TT_FMOD:           bind->eval  = eval_fmod; break;
                case TT_IMOD:           bind->eval  = eval_imod; break;
                default:                bind->eval  = NULL; break;
            }
            bind->sCalc.pLeft   = left;
            bind->sCalc.pRight  = right;
            bind->sCalc.pCond   = NULL;
            return STATUS_OK;
        }

        status_t parse_addsub(expr_t **expr, Tokenizer *t, size_t flags)
        {
            expr_t *left = NULL, *right = NULL;

            // Parse left part
            status_t res = parse_muldiv(&left, t, flags);
            if (res != STATUS_OK)
                return res;

            // Check token
            token_t tok = t->get_token(TF_NONE);
            switch (tok)
            {
                case TT_ADD:
                case TT_SUB:
                case TT_ADDSYM:
                case TT_SUBSYM:
                case TT_IADD:
                case TT_ISUB:
                    break;
                default:
                    *expr = left;
                    return res;
            }

            // Parse right part
            res = parse_addsub(&right, t, TF_GET);
            if (res != STATUS_OK)
            {
                parse_destroy(left);
                return res;
            }

            // Create binding between left and right
            expr_t *bind        = reinterpret_cast<expr_t *>(::malloc(sizeof(expr_t)));
            if (bind == NULL)
            {
                parse_destroy(left);
                parse_destroy(right);
                return STATUS_NO_MEM;
            }
            switch (tok)
            {
                case TT_ADD:            bind->eval  = eval_add; break;
                case TT_SUB:            bind->eval  = eval_sub; break;
                case TT_ADDSYM:         bind->eval  = eval_add; break;
                case TT_SUBSYM:         bind->eval  = eval_sub; break;
                case TT_IADD:           bind->eval  = eval_iadd; break;
                case TT_ISUB:           bind->eval  = eval_isub; break;
                default:                bind->eval  = NULL; break;
            }
            bind->sCalc.pLeft   = left;
            bind->sCalc.pRight  = right;
            bind->sCalc.pCond   = NULL;
            return STATUS_OK;
        }

        status_t parse_cmp_rel(expr_t **expr, Tokenizer *t, size_t flags)
        {
            expr_t *left = NULL, *right = NULL;

            // Parse left part
            status_t res = parse_addsub(&left, t, flags);
            if (res != STATUS_OK)
                return res;

            // Check token
            token_t tok = t->get_token(TF_NONE);
            switch (tok)
            {
                case TT_LESS:
                case TT_GREATER:
                case TT_LESS_EQ:
                case TT_GREATER_EQ:
                case TT_ILESS:
                case TT_IGREATER:
                case TT_ILESS_EQ:
                case TT_IGREATER_EQ:
                    break;
                default:
                    *expr = left;
                    return res;
            }

            // Parse right part
            res = parse_cmp_rel(&right, t, TF_GET);
            if (res != STATUS_OK)
            {
                parse_destroy(left);
                return res;
            }

            // Create binding between left and right
            expr_t *bind        = reinterpret_cast<expr_t *>(::malloc(sizeof(expr_t)));
            if (bind == NULL)
            {
                parse_destroy(left);
                parse_destroy(right);
                return STATUS_NO_MEM;
            }
            switch (tok)
            {
                case TT_LESS:           bind->eval  = eval_cmp_lt; break;
                case TT_GREATER:        bind->eval  = eval_cmp_gt; break;
                case TT_LESS_EQ:        bind->eval  = eval_cmp_le; break;
                case TT_GREATER_EQ:     bind->eval  = eval_cmp_ge; break;
                case TT_ILESS:          bind->eval  = eval_icmp_lt; break;
                case TT_IGREATER:       bind->eval  = eval_icmp_gt; break;
                case TT_ILESS_EQ:       bind->eval  = eval_icmp_le; break;
                case TT_IGREATER_EQ:    bind->eval  = eval_icmp_ge; break;
                default:                bind->eval  = NULL; break;
            }
            bind->sCalc.pLeft   = left;
            bind->sCalc.pRight  = right;
            bind->sCalc.pCond   = NULL;
            return STATUS_OK;
        }

        status_t parse_cmp_eq(expr_t **expr, Tokenizer *t, size_t flags)
        {
            expr_t *left = NULL, *right = NULL;

            // Parse left part
            status_t res = parse_cmp_rel(&left, t, flags);
            if (res != STATUS_OK)
                return res;

            // Check token
            token_t tok = t->get_token(TF_NONE);
            switch (tok)
            {
                case TT_NOT_EQ:
                case TT_EQ:
                case TT_CMP:
                case TT_INOT_EQ:
                case TT_IEQ:
                case TT_ICMP:
                    break;
                default:
                    *expr = left;
                    return res;
            }

            // Parse right part
            res = parse_cmp_eq(&right, t, TF_GET);
            if (res != STATUS_OK)
            {
                parse_destroy(left);
                return res;
            }

            // Create binding between left and right
            expr_t *bind        = reinterpret_cast<expr_t *>(::malloc(sizeof(expr_t)));
            if (bind == NULL)
            {
                parse_destroy(left);
                parse_destroy(right);
                return STATUS_NO_MEM;
            }
            switch (tok)
            {
                case TT_NOT_EQ:     bind->eval  = eval_cmp_ne; break;
                case TT_EQ:         bind->eval  = eval_cmp_eq; break;
                case TT_CMP:        bind->eval  = eval_cmp; break;
                case TT_INOT_EQ:    bind->eval  = eval_icmp_ne; break;
                case TT_IEQ:        bind->eval  = eval_icmp_eq; break;
                case TT_ICMP:       bind->eval  = eval_icmp; break;
                default:            bind->eval  = NULL; break;
            }
            bind->sCalc.pLeft   = left;
            bind->sCalc.pRight  = right;
            bind->sCalc.pCond   = NULL;
            return STATUS_OK;
        }

        status_t parse_bit_and(expr_t **expr, Tokenizer *t, size_t flags)
        {
            expr_t *left = NULL, *right = NULL;

            // Parse left part
            status_t res = parse_cmp_eq(&left, t, flags);
            if (res != STATUS_OK)
                return res;

            // Check token
            token_t tok = t->get_token(TF_NONE);
            if (tok != TT_BAND)
            {
                *expr   = left;
                return STATUS_OK;
            }

            // Parse right part
            res = parse_bit_and(&right, t, TF_GET);
            if (res != STATUS_OK)
            {
                parse_destroy(left);
                return res;
            }

            // Create binding between left and right
            expr_t *bind        = reinterpret_cast<expr_t *>(::malloc(sizeof(expr_t)));
            if (bind == NULL)
            {
                parse_destroy(left);
                parse_destroy(right);
                return STATUS_NO_MEM;
            }
            bind->eval          = eval_bit_and;
            bind->sCalc.pLeft   = left;
            bind->sCalc.pRight  = right;
            bind->sCalc.pCond   = NULL;
            return STATUS_OK;
        }

        status_t parse_bit_xor(expr_t **expr, Tokenizer *t, size_t flags)
        {
            expr_t *left = NULL, *right = NULL;

            // Parse left part
            status_t res = parse_bit_and(&left, t, flags);
            if (res != STATUS_OK)
                return res;

            // Check token
            token_t tok = t->get_token(TF_NONE);
            if (tok != TT_BXOR)
            {
                *expr   = left;
                return STATUS_OK;
            }

            // Parse right part
            res = parse_bit_xor(&right, t, TF_GET);
            if (res != STATUS_OK)
            {
                parse_destroy(left);
                return res;
            }

            // Create binding between left and right
            expr_t *bind        = reinterpret_cast<expr_t *>(::malloc(sizeof(expr_t)));
            if (bind == NULL)
            {
                parse_destroy(left);
                parse_destroy(right);
                return STATUS_NO_MEM;
            }
            bind->eval          = eval_bit_xor;
            bind->sCalc.pLeft   = left;
            bind->sCalc.pRight  = right;
            bind->sCalc.pCond   = NULL;
            return STATUS_OK;
        }

        status_t parse_bit_or(expr_t **expr, Tokenizer *t, size_t flags)
        {
            expr_t *left = NULL, *right = NULL;

            // Parse left part
            status_t res = parse_bit_xor(&left, t, flags);
            if (res != STATUS_OK)
                return res;

            // Check token
            token_t tok = t->get_token(TF_NONE);
            if (tok != TT_BOR)
            {
                *expr   = left;
                return STATUS_OK;
            }

            // Parse right part
            res = parse_bit_or(&right, t, TF_GET);
            if (res != STATUS_OK)
            {
                parse_destroy(left);
                return res;
            }

            // Create binding between left and right
            expr_t *bind        = reinterpret_cast<expr_t *>(::malloc(sizeof(expr_t)));
            if (bind == NULL)
            {
                parse_destroy(left);
                parse_destroy(right);
                return STATUS_NO_MEM;
            }
            bind->eval          = eval_bit_or;
            bind->sCalc.pLeft   = left;
            bind->sCalc.pRight  = right;
            bind->sCalc.pCond   = NULL;
            return STATUS_OK;
        }

        status_t parse_and(expr_t **expr, Tokenizer *t, size_t flags)
        {
            expr_t *left = NULL, *right = NULL;

            // Parse left part
            status_t res = parse_bit_or(&left, t, flags);
            if (res != STATUS_OK)
                return res;

            // Check token
            token_t tok = t->get_token(TF_NONE);
            if (tok != TT_AND)
            {
                *expr   = left;
                return STATUS_OK;
            }

            // Parse right part
            res = parse_and(&right, t, TF_GET);
            if (res != STATUS_OK)
            {
                parse_destroy(left);
                return res;
            }

            // Create binding between left and right
            expr_t *bind        = reinterpret_cast<expr_t *>(::malloc(sizeof(expr_t)));
            if (bind == NULL)
            {
                parse_destroy(left);
                parse_destroy(right);
                return STATUS_NO_MEM;
            }
            bind->eval          = eval_and;
            bind->sCalc.pLeft   = left;
            bind->sCalc.pRight  = right;
            bind->sCalc.pCond   = NULL;
            return STATUS_OK;
        }

        status_t parse_xor(expr_t **expr, Tokenizer *t, size_t flags)
        {
            expr_t *left = NULL, *right = NULL;

            // Parse left part
            status_t res = parse_and(&left, t, flags);
            if (res != STATUS_OK)
                return res;

            // Check token
            token_t tok = t->get_token(TF_NONE);
            if (tok != TT_XOR)
            {
                *expr = left;
                return res;
            }

            // Parse right part
            res = parse_xor(&right, t, TF_GET);
            if (res != STATUS_OK)
            {
                parse_destroy(left);
                return res;
            }

            // Create binding between left and right
            expr_t *bind        = reinterpret_cast<expr_t *>(::malloc(sizeof(expr_t)));
            if (bind == NULL)
            {
                parse_destroy(left);
                parse_destroy(right);
                return STATUS_NO_MEM;
            }
            bind->eval          = eval_xor;
            bind->sCalc.pLeft   = left;
            bind->sCalc.pRight  = right;
            bind->sCalc.pCond   = NULL;

            return STATUS_OK;
        }

        status_t parse_or(expr_t **expr, Tokenizer *t, size_t flags)
        {
            expr_t *left = NULL, *right = NULL;

            // Parse left part
            status_t res = parse_xor(&left, t, flags);
            if (res != STATUS_OK)
                return res;

            // Check token
            token_t tok = t->get_token(TF_NONE);
            if (tok != TT_OR)
            {
                *expr   = left;
                return STATUS_OK;
            }

            // Parse right part
            res = parse_or(&right, t, TF_GET);
            if (res != STATUS_OK)
            {
                parse_destroy(left);
                return res;
            }

            // Create binding between left and right
            expr_t *bind        = reinterpret_cast<expr_t *>(::malloc(sizeof(expr_t)));
            if (bind == NULL)
            {
                parse_destroy(left);
                parse_destroy(right);
                return STATUS_NO_MEM;
            }
            bind->eval          = eval_or;
            bind->sCalc.pLeft   = left;
            bind->sCalc.pRight  = right;
            bind->sCalc.pCond   = NULL;
            return STATUS_OK;
        }

        status_t parse_ternary(expr_t **expr, Tokenizer *t, size_t flags)
        {
            expr_t *cond = NULL, *left = NULL, *right = NULL;

            // Parse condition part
            status_t res = parse_or(&cond, t, flags);
            if (res != STATUS_OK)
                return res;

            // Check token
            token_t tok     = t->get_token(TF_NONE);
            if (tok != TT_QUESTION)
            {
                *expr   = cond;
                return res;
            }

            // Parse left part
            res = parse_ternary(&left, t, TF_GET);
            if (res != STATUS_OK)
            {
                parse_destroy(cond);
                return res;
            }

            // Check token
            tok = t->get_token(TF_NONE);
            if (tok != TT_COLON)
            {
                parse_destroy(cond);
                return res;
            }

            // Parse right part
            res = parse_ternary(&right, t, TF_GET);
            if (res != STATUS_OK)
            {
                parse_destroy(cond);
                parse_destroy(left);
                return res;
            }

            // Create binding between left and right
            expr_t *bind     = reinterpret_cast<expr_t *>(::malloc(sizeof(expr_t)));
            if (bind != NULL)
            {
                parse_destroy(cond);
                parse_destroy(left);
                parse_destroy(right);
                return STATUS_NO_MEM;
            }
            bind->eval          = eval_ternary;
            bind->sCalc.pLeft   = left;
            bind->sCalc.pRight  = right;
            bind->sCalc.pCond   = cond;
            *expr               = bind;

            return STATUS_OK;
        }

        status_t parse_expression(expr_t **expr, Tokenizer *t, size_t flags)
        {
            return parse_ternary(expr, t, flags);
        }
    }
}


