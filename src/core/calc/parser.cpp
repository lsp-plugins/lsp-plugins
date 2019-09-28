/*
 * parser.cpp
 *
 *  Created on: 18 сент. 2019 г.
 *      Author: sadko
 */

#include <core/calc/parser.h>
#include <data/cvector.h>

namespace lsp
{
    namespace calc
    {
        void parse_destroy(expr_t *expr)
        {
            if (expr == NULL)
                return;

            expr->eval      = NULL;
            switch (expr->type)
            {
                case ET_VALUE:
                    if ((expr->value.type == VT_STRING) && (expr->value.v_str != NULL))
                    {
                        delete expr->value.v_str;
                        expr->value.v_str = NULL;
                    }
                    break;
                case ET_RESOLVE:
                    if (expr->resolve.items != NULL)
                    {
                        for (size_t i=0, n=expr->resolve.count; i<n; ++i)
                            parse_destroy(expr->resolve.items[i]);
                        ::free(expr->resolve.items);
                        expr->resolve.items     = NULL;
                    }
                    if (expr->resolve.name != NULL)
                    {
                        delete expr->resolve.name;
                        expr->resolve.name  = NULL;
                    }
                    break;
                case ET_CALC:
                    parse_destroy(expr->calc.left);
                    expr->calc.left      = NULL;
                    parse_destroy(expr->calc.right);
                    expr->calc.right     = NULL;
                    parse_destroy(expr->calc.cond);
                    expr->calc.cond     = NULL;
                    break;

                default:
                    break;
            }

            // Free the expression
            ::free(expr);
        }

        expr_t *parse_create_expr()
        {
            return reinterpret_cast<expr_t *>(::malloc(sizeof(expr_t)));
        }

        void drop_indexes(cvector<expr_t> *indexes)
        {
            for (size_t i=0, n=indexes->size(); i<n; ++i)
                parse_destroy(indexes->at(i));
            indexes->flush();
        }

        status_t parse_identifier(expr_t **expr, Tokenizer *t, size_t flags)
        {
            // Get identifier
            expr_t *bind = NULL;
            token_t tok = t->get_token(flags);
            if ((tok != TT_IDENTIFIER) && (!(flags & TF_BAREWORD)))
                return STATUS_BAD_TOKEN;

            LSPString *id   = t->text_value()->clone();
            if (id == NULL)
                return STATUS_NO_MEM;

            // Lookup for indexes
            cvector<expr_t> indexes;
            while ((tok = t->get_token(TF_GET | TF_XSIGN)) == TT_LQBRACE)
            {
                // Brace, analyze next token
                tok = t->get_token(TF_GET);
                if (tok == TT_BAREWORD)
                {
                    // Store the bareword
                    LSPString *name = t->text_value()->clone();
                    if (name == NULL)
                    {
                        drop_indexes(&indexes);
                        delete id;
                        return STATUS_NO_MEM;
                    }

                    // Next token should be TT_RQBRACE
                    tok = t->get_token(TF_GET);
                    if (tok != TT_RQBRACE)
                    {
                        drop_indexes(&indexes);
                        delete id;
                        return STATUS_BAD_TOKEN;
                    }

                    // Create new expression
                    bind = parse_create_expr();
                    if (bind == NULL)
                    {
                        drop_indexes(&indexes);
                        delete name;
                        delete id;
                        return STATUS_NO_MEM;
                    }

                    // Simple variable get
                    bind->eval          = eval_resolve;
                    bind->type          = ET_RESOLVE;
                    bind->resolve.name  = name;
                    bind->resolve.count = 0;
                    bind->resolve.items = NULL;
                }
                else
                {
                    // Parse the expression in square brackets
                    status_t res = parse_expression(&bind, t, TF_NONE); // Token already has been taken
                    if (res != STATUS_OK)
                    {
                        drop_indexes(&indexes);
                        delete id;
                        return res;
                    }

                    // Next token should be TT_RQBRACE
                    tok = t->get_token(TF_NONE);
                    if (tok != TT_RQBRACE)
                    {
                        parse_destroy(bind);
                        drop_indexes(&indexes);
                        delete id;
                        return STATUS_BAD_TOKEN;
                    }
                }

                // Add binding to list of indexes
                if (!indexes.add(bind))
                {
                    parse_destroy(bind);
                    drop_indexes(&indexes);
                    delete id;
                    return STATUS_NO_MEM;
                }
            } // while

            // Create expression
            bind    = parse_create_expr();
            if (bind == NULL)
            {
                drop_indexes(&indexes);
                delete id;
                return STATUS_NO_MEM;
            }

            bind->eval          = eval_resolve;
            bind->type          = ET_RESOLVE;
            bind->resolve.name  = id;
            bind->resolve.count = indexes.size();
            bind->resolve.items = indexes.release();

            *expr               = bind;
            return STATUS_OK;
        }

        status_t parse_primary(expr_t **expr, Tokenizer *t, size_t flags)
        {
            token_t tok = t->get_token(flags);
            switch (tok)
            {
                case TT_IDENTIFIER:
                    return parse_identifier(expr, t, TF_NONE);

                case TT_IVALUE:
                {
                    expr_t *bind        = parse_create_expr();
                    if (bind == NULL)
                        return STATUS_NO_MEM;

                    ssize_t ivalue      = t->int_value();

                    bind->eval          = eval_value;
                    bind->type          = ET_VALUE;
                    if (t->get_token(TF_GET | TF_XSIGN) == TT_DB)
                    {
                        bind->value.type    = VT_FLOAT;
                        bind->value.v_float = exp(ivalue * M_LN10 * 0.05);
                        t->get_token(TF_GET | TF_XSIGN);
                    }
                    else
                    {
                        bind->value.type    = VT_INT;
                        bind->value.v_int   = ivalue;
                    }

                    *expr       = bind;
                    break;
                }

                case TT_FVALUE:
                {
                    expr_t *bind        = parse_create_expr();
                    if (bind == NULL)
                        return STATUS_NO_MEM;

                    double fvalue       = t->float_value();

                    bind->eval          = eval_value;
                    bind->type          = ET_VALUE;
                    bind->value.type    = VT_FLOAT;
                    if (t->get_token(TF_GET | TF_XSIGN) == TT_DB)
                    {
                        bind->value.v_float = exp(fvalue * M_LN10 * 0.05);
                        t->get_token(TF_GET | TF_XSIGN);
                    }
                    else
                        bind->value.v_float = fvalue;

                    *expr       = bind;
                    break;
                }

                case TT_STRING:
                case TT_TRUE:
                case TT_FALSE:
                case TT_NULL:
                case TT_UNDEF:
                {
                    expr_t *bind        = parse_create_expr();
                    if (bind == NULL)
                        return STATUS_NO_MEM;

                    bind->eval          = eval_value;
                    bind->type          = ET_VALUE;
                    switch (tok)
                    {
                        case TT_STRING:
                            bind->value.type        = VT_STRING;
                            bind->value.v_str       = t->text_value()->clone();
                            if (bind->value.v_str != NULL)
                                break;
                            ::free(bind);
                            return STATUS_NO_MEM;
                        case TT_TRUE:
                            bind->value.type        = VT_BOOL;
                            bind->value.v_bool      = true;
                            break;
                        case TT_FALSE:
                            bind->value.type        = VT_BOOL;
                            bind->value.v_bool      = false;
                            break;
                        case TT_NULL:
                            bind->value.type        = VT_NULL;
                            break;
                        case TT_UNDEF:
                            bind->value.type        = VT_UNDEF;
                            break;
                        default:
                            break;
                    }

                    *expr   = bind;
                    t->get_token(TF_GET | TF_XSIGN);

                    return STATUS_OK;
                }

                case TT_LBRACE:
                {
                    expr_t *bind = NULL;
                    status_t res = parse_expression(&bind, t, TF_GET);
                    if (res != STATUS_OK)
                        return res;

                    tok = t->get_token(TF_NONE);
                    if (tok == TT_RBRACE)
                    {
                        t->get_token(TF_GET | TF_XSIGN);
                        *expr = bind;
                        return res;
                    }

                    parse_destroy(bind);
                    return STATUS_BAD_TOKEN;
                }

                case TT_EOF:
                    return STATUS_EOF;

                default:
                    return STATUS_BAD_TOKEN;
            }
            return STATUS_OK;
        }

        status_t parse_func(expr_t **expr, Tokenizer *t, size_t flags)
        {
            // Check token
            token_t tok = t->get_token(flags);

            // Parse right part
            status_t res;
            expr_t *right   = NULL;

            switch(tok)
            {
                case TT_EX:
                case TT_DB:
                case TT_SUPR:
                case TT_SLWR:
                case TT_SLEN:
                case TT_SREV:
                case TT_INT:
                case TT_FLOAT:
                case TT_BOOL:
                case TT_STR:
                    res = parse_func(&right, t, TF_GET);
                    break;
                default:
                    return parse_primary(expr, t, TF_NONE);
            }
            if (res != STATUS_OK)
                return res;

            // Create binding between left and right
            expr_t *bind        = parse_create_expr();
            if (bind == NULL)
            {
                parse_destroy(right);
                return STATUS_NO_MEM;
            }
            switch (tok)
            {
                case TT_EX:             bind->eval  = eval_exists; break;
                case TT_DB:             bind->eval  = eval_db; break;
                case TT_SUPR:           bind->eval  = eval_strupper; break;
                case TT_SLWR:           bind->eval  = eval_strlower; break;
                case TT_SLEN:           bind->eval  = eval_strlen; break;
                case TT_SREV:           bind->eval  = eval_strrev; break;
                case TT_INT:            bind->eval  = eval_int_cast; break;
                case TT_FLOAT:          bind->eval  = eval_float_cast; break;
                case TT_BOOL:           bind->eval  = eval_bool_cast; break;
                case TT_STR:            bind->eval  = eval_string_cast; break;
                default:                bind->eval  = NULL; break;
            }
            bind->type          = ET_CALC;
            bind->calc.left     = right;
            bind->calc.right    = NULL;
            bind->calc.cond     = NULL;

            *expr               = bind;
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
                    if ((res = parse_sign(&right, t, TF_GET)) != STATUS_OK)
                        return res;
                    break;
                default:
                    return parse_func(expr, t, TF_NONE);
            }

            // Create binding between left and right
            expr_t *bind        = parse_create_expr();
            if (bind == NULL)
            {
                parse_destroy(right);
                return STATUS_NO_MEM;
            }
            switch (tok)
            {
                case TT_ADD:            bind->eval  = eval_psign; break;
                case TT_SUB:            bind->eval  = eval_nsign; break;
                default:                bind->eval  = NULL; break;
            }
            bind->type          = ET_CALC;
            bind->calc.left     = right;
            bind->calc.right    = NULL;
            bind->calc.cond     = NULL;

            *expr               = bind;
            return STATUS_OK;
        }

        status_t parse_not(expr_t **expr, Tokenizer *t, size_t flags)
        {
            // Check token
            token_t tok = t->get_token(flags);

            // Parse right part
            status_t res;
            expr_t *right   = NULL;
            switch (tok)
            {
                case TT_NOT:
                case TT_BNOT:
                    if ((res = parse_not(&right, t, TF_GET)) != STATUS_OK)
                        return res;
                    break;
                default:
                    return parse_sign(expr, t, TF_NONE);
            }

            // Create binding between left and right
            expr_t *bind        = parse_create_expr();
            if (bind == NULL)
            {
                parse_destroy(right);
                return STATUS_NO_MEM;
            }
            bind->eval          = (tok == TT_NOT) ? eval_not : eval_neg;
            bind->type          = ET_CALC;
            bind->calc.left     = right;
            bind->calc.right    = NULL;
            bind->calc.cond     = NULL;

            *expr               = bind;
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
            expr_t *bind        = parse_create_expr();
            if (bind == NULL)
            {
                parse_destroy(left);
                parse_destroy(right);
                return STATUS_NO_MEM;
            }
            bind->eval          = eval_power;
            bind->type          = ET_CALC;
            bind->calc.left     = left;
            bind->calc.right    = right;
            bind->calc.cond     = NULL;

            *expr               = bind;
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
            expr_t *bind        = parse_create_expr();
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
            bind->type          = ET_CALC;
            bind->calc.left     = left;
            bind->calc.right    = right;
            bind->calc.cond     = NULL;

            *expr               = bind;
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
            expr_t *bind        = parse_create_expr();
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
            bind->type          = ET_CALC;
            bind->calc.left     = left;
            bind->calc.right    = right;
            bind->calc.cond     = NULL;

            *expr               = bind;
            return STATUS_OK;
        }

        status_t parse_strrep(expr_t **expr, Tokenizer *t, size_t flags)
        {
            expr_t *left = NULL, *right = NULL;

            // Parse left part
            status_t res = parse_addsub(&left, t, flags);
            if (res != STATUS_OK)
                return res;

            // Check token
            token_t tok = t->get_token(TF_NONE);
            if (tok != TT_SREP)
            {
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
            expr_t *bind        = parse_create_expr();
            if (bind == NULL)
            {
                parse_destroy(left);
                parse_destroy(right);
                return STATUS_NO_MEM;
            }

            bind->type          = ET_CALC;
            bind->eval          = eval_strrep;
            bind->calc.left     = left;
            bind->calc.right    = right;
            bind->calc.cond     = NULL;

            *expr               = bind;
            return STATUS_OK;
        }

        status_t parse_strcat(expr_t **expr, Tokenizer *t, size_t flags)
        {
            expr_t *left = NULL, *right = NULL;

            // Parse left part
            status_t res = parse_strrep(&left, t, flags);
            if (res != STATUS_OK)
                return res;

            // Check token
            token_t tok = t->get_token(TF_NONE);
            if (tok != TT_SCAT)
            {
                *expr = left;
                return res;
            }

            // Parse right part
            res = parse_strcat(&right, t, TF_GET);
            if (res != STATUS_OK)
            {
                parse_destroy(left);
                return res;
            }

            // Create binding between left and right
            expr_t *bind        = parse_create_expr();
            if (bind == NULL)
            {
                parse_destroy(left);
                parse_destroy(right);
                return STATUS_NO_MEM;
            }

            bind->type          = ET_CALC;
            bind->eval          = eval_strcat;
            bind->calc.left     = left;
            bind->calc.right    = right;
            bind->calc.cond     = NULL;

            *expr               = bind;
            return STATUS_OK;
        }

        status_t parse_cmp_rel(expr_t **expr, Tokenizer *t, size_t flags)
        {
            expr_t *left = NULL, *right = NULL;

            // Parse left part
            status_t res = parse_strcat(&left, t, flags);
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
            expr_t *bind        = parse_create_expr();
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
            bind->type          = ET_CALC;
            bind->calc.left     = left;
            bind->calc.right    = right;
            bind->calc.cond     = NULL;

            *expr               = bind;
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
            expr_t *bind        = parse_create_expr();
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
            bind->type          = ET_CALC;
            bind->calc.left     = left;
            bind->calc.right    = right;
            bind->calc.cond     = NULL;

            *expr               = bind;
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
            expr_t *bind        = parse_create_expr();
            if (bind == NULL)
            {
                parse_destroy(left);
                parse_destroy(right);
                return STATUS_NO_MEM;
            }
            bind->eval          = eval_bit_and;
            bind->type          = ET_CALC;
            bind->calc.left     = left;
            bind->calc.right    = right;
            bind->calc.cond     = NULL;

            *expr               = bind;
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
            expr_t *bind        = parse_create_expr();
            if (bind == NULL)
            {
                parse_destroy(left);
                parse_destroy(right);
                return STATUS_NO_MEM;
            }
            bind->eval          = eval_bit_xor;
            bind->type          = ET_CALC;
            bind->calc.left     = left;
            bind->calc.right    = right;
            bind->calc.cond     = NULL;

            *expr               = bind;
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
            expr_t *bind        = parse_create_expr();
            if (bind == NULL)
            {
                parse_destroy(left);
                parse_destroy(right);
                return STATUS_NO_MEM;
            }
            bind->eval          = eval_bit_or;
            bind->type          = ET_CALC;
            bind->calc.left     = left;
            bind->calc.right    = right;
            bind->calc.cond     = NULL;

            *expr               = bind;
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
            expr_t *bind        = parse_create_expr();
            if (bind == NULL)
            {
                parse_destroy(left);
                parse_destroy(right);
                return STATUS_NO_MEM;
            }
            bind->eval          = eval_and;
            bind->type          = ET_CALC;
            bind->calc.left     = left;
            bind->calc.right    = right;
            bind->calc.cond     = NULL;

            *expr               = bind;
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
            expr_t *bind        = parse_create_expr();
            if (bind == NULL)
            {
                parse_destroy(left);
                parse_destroy(right);
                return STATUS_NO_MEM;
            }
            bind->eval          = eval_xor;
            bind->type          = ET_CALC;
            bind->calc.left     = left;
            bind->calc.right    = right;
            bind->calc.cond     = NULL;

            *expr               = bind;
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
            expr_t *bind        = parse_create_expr();
            if (bind == NULL)
            {
                parse_destroy(left);
                parse_destroy(right);
                return STATUS_NO_MEM;
            }
            bind->eval          = eval_or;
            bind->type          = ET_CALC;
            bind->calc.left     = left;
            bind->calc.right    = right;
            bind->calc.cond     = NULL;

            *expr               = bind;
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
            expr_t *bind     = parse_create_expr();
            if (bind == NULL)
            {
                parse_destroy(cond);
                parse_destroy(left);
                parse_destroy(right);
                return STATUS_NO_MEM;
            }
            bind->eval          = eval_ternary;
            bind->type          = ET_CALC;
            bind->calc.left     = left;
            bind->calc.right    = right;
            bind->calc.cond     = cond;

            *expr               = bind;
            return STATUS_OK;
        }

        status_t parse_expression(expr_t **expr, Tokenizer *t, size_t flags)
        {
            return parse_ternary(expr, t, flags);
        }
    }
}


