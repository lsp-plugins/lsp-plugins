/*
 * Expression.cpp
 *
 *  Created on: 16 сент. 2019 г.
 *      Author: sadko
 */

#include <core/io/InStringSequence.h>
#include <core/calc/parser.h>
#include <core/calc/evaluator.h>
#include <core/calc/Expression.h>
#include <core/calc/Tokenizer.h>

namespace lsp
{
    namespace calc
    {
        
        Expression::Expression(Resolver *res)
        {
            pResolver       = res;
        }
        
        Expression::~Expression()
        {
            destroy_all_data();
            pResolver       = NULL;
        }

        void Expression::destroy_all_data()
        {
            for (size_t i=0, n=vRoots.size(); i<n; ++i)
            {
                root_t *r = vRoots.at(i);
                if (r->expr != NULL)
                {
                    parse_destroy(r->expr);
                    r->expr = NULL;
                }
                if ((r->result.type == VT_STRING) && (r->result.v_str != NULL))
                {
                    delete r->result.v_str;
                    r->result.v_str = NULL;
                }
            }
            vRoots.flush();
        }

        status_t Expression::result(value_t *result, size_t idx)
        {
            root_t *r = vRoots.get(idx);
            if (r == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (result != NULL)
                *result     = r->result;
            return STATUS_OK;
        }

        status_t Expression::evaluate(value_t *result)
        {
            status_t res = STATUS_BAD_STATE;

            for (size_t i=0, n=vRoots.size(); i<n; ++i)
            {
                root_t *r = vRoots.at(i);
                if (r->expr != NULL)
                    res     = r->expr->eval(&r->result, r->expr, pResolver);
                else
                {
                    r->result.type  = VT_UNDEF;
                    r->result.v_str = NULL;
                    res             = STATUS_OK;
                }

                if (res != STATUS_OK)
                    break;
            }

            // Store the result if ALL is OK
            if ((res == STATUS_OK) && (result != NULL))
                *result     = vRoots.at(0)->result;

            return res;
        }

        status_t Expression::evaluate(size_t idx, value_t *result)
        {
            root_t *r = vRoots.get(idx);
            if (r == NULL)
                return STATUS_BAD_ARGUMENTS;

            status_t res = STATUS_BAD_STATE;
            if (r->expr != NULL)
                res     = r->expr->eval(&r->result, r->expr, pResolver);
            else
            {
                r->result.type  = VT_UNDEF;
                r->result.v_str = NULL;
                res             = STATUS_OK;
            }

            // Store the result if ALL is OK
            if ((res == STATUS_OK) && (result != NULL))
                *result     = r->result;

            return res;
        }
    
        status_t Expression::parse(const char *expr, const char *charset, size_t flags)
        {
            io::InStringSequence is;

            status_t res = is.wrap(expr, "UTF-8");
            if (res == STATUS_OK)
                res = parse(&is, flags);
            if (res == STATUS_OK)
                res = is.close();
            else
                is.close();

            return res;
        }

        status_t Expression::parse(const LSPString *expr, size_t flags)
        {
            io::InStringSequence is;

            status_t res = is.wrap(expr);
            if (res == STATUS_OK)
                res = parse(&is, flags);
            if (res == STATUS_OK)
                res = is.close();
            else
                is.close();

            return res;
        }

        status_t Expression::parse_regular(io::IInSequence *seq, size_t flags)
        {
            status_t res = STATUS_OK;
            Tokenizer t(seq);

            while (true)
            {
                // Allocate placeholder
                root_t *root    = vRoots.add();
                if (root == NULL)
                {
                    res     = STATUS_NO_MEM;
                    break;
                }

                // Parse expression
                root->expr          = NULL;
                root->result.type   = VT_UNDEF;
                root->result.v_str  = NULL;
                res                 = parse_expression(&root->expr, &t, TF_GET);
                if (res != STATUS_OK)
                    break;

                // Analyze next token after expression
                token_t tok     = t.get_token(TF_NONE);
                if (tok != TT_SEMICOLON)
                {
                    res = (tok == TT_EOF) ? STATUS_OK : t.error();
                    break;
                }
                else if (!(flags & FLAG_MULTIPLE))
                    break;
            }

            return res;
        }

        status_t Expression::prepend_string(expr_t **expr, const LSPString *str, bool force)
        {
            // Do we really need to prepend?
            if ((str->length() <= 0) && (!force))
                return STATUS_OK;

            // What to do with expression?
            expr_t *right = parse_create_expr();
            if (right == NULL)
                return STATUS_NO_MEM;

            right->type         = ET_VALUE;
            right->eval         = eval_value;
            right->value.type   = VT_STRING;
            right->value.v_str  = str->clone();

            if (right->value.v_str == NULL)
            {
                parse_destroy(right);
                return STATUS_NO_MEM;
            }

            // Do we need to immediately return?
            if (*expr == NULL)
            {
                *expr = right;
                return STATUS_OK;
            }

            // Need to create middle expression
            expr_t *middle      = parse_create_expr();
            if (middle == NULL)
            {
                parse_destroy(right);
                return STATUS_NO_MEM;
            }

            middle->type        = ET_CALC;
            middle->eval        = eval_strcat;
            middle->calc.left   = *expr;
            middle->calc.right  = right;
            *expr               = middle;

            return STATUS_OK;
        }

        status_t Expression::parse_substitution(expr_t **expr, Tokenizer *t)
        {
            expr_t *right = NULL;
            token_t tok   = t->get_token(TF_GET);

            // Parse expression
            status_t res = (tok == TT_BAREWORD) ?
                parse_identifier(&right, t, TF_BAREWORD) :
                parse_expression(&right, t, TF_NONE);

            if (res != STATUS_OK)
                return res;

            // Analyze next token after expression
            tok         = t->get_token(TF_NONE);
            if (tok != TT_RCBRACE)
                res = (tok == TT_EOF) ? STATUS_EOF : STATUS_BAD_TOKEN;

            if (res == STATUS_OK)
                *expr       = right;

            return res;
        }

        status_t Expression::parse_string(io::IInSequence *seq, size_t flags)
        {
            // Create expression
            LSPString tmp;
            Tokenizer t(seq);
            status_t res        = STATUS_OK;
            expr_t *expr        = NULL;
            bool predicate      = false;

            while (true)
            {
                lsp_swchar_t wc = seq->read();
                if (predicate)
                {
                    predicate = false;
                    if (wc < 0)
                    {
                        res = (wc == STATUS_EOF) ? STATUS_OK : -wc;
                        break;
                    }
                    else if (wc != '{')
                    {
                        if (wc != '$')
                            res = (tmp.append('$')) ? STATUS_OK : STATUS_NO_MEM;

                        if (res == STATUS_OK)
                            res = (tmp.append(wc)) ? STATUS_OK : STATUS_NO_MEM;
                        if (res != STATUS_OK)
                            break;
                    }
                    else // Sub-expression
                    {
                        // Prepend string if it is defined
                        res = prepend_string(&expr, &tmp, false);
                        if (res != STATUS_OK)
                            break;
                        tmp.clear();

                        // Parse expression
                        expr_t *right = NULL;
                        res = parse_substitution(&right, &t);
                        if (res != STATUS_OK)
                            break;

                        // What to do with expression?
                        if (expr != NULL)
                        {
                            expr_t *middle = parse_create_expr();
                            if (middle == NULL)
                            {
                                parse_destroy(right);
                                res = STATUS_NO_MEM;
                                break;
                            }
                            middle->type        = ET_CALC;
                            middle->eval        = eval_strcat;
                            middle->calc.left   = expr;
                            middle->calc.right  = right;
                            expr                = middle;
                        }
                        else
                            expr    = right;
                    }
                }
                else
                {
                    // Analyze result
                    if (wc < 0)
                    {
                        res = (wc == STATUS_EOF) ? STATUS_OK : -wc;
                        break;
                    }
                    else if (wc == '$')
                    {
                        predicate = true;
                        continue;
                    }
                    else if (!tmp.append(wc))  // Append character to string
                    {
                        res = STATUS_NO_MEM;
                        break;
                    }
                }
            }

            // Need to prepend string?
            if (res == STATUS_OK)
                res = prepend_string(&expr, &tmp, expr == NULL);

            // Analyze result
            if (res != STATUS_OK)
            {
                parse_destroy(expr);
                return res;
            }

            // Allocate placeholder
            root_t *root    = vRoots.add();
            if (root == NULL)
            {
                parse_destroy(expr);
                res     = STATUS_NO_MEM;
            }
            else
            {
                root->expr          = expr;
                root->result.type   = VT_UNDEF;
                root->result.v_str  = NULL;
            }

            return res;
        }

        status_t Expression::parse(io::IInSequence *seq, size_t flags)
        {
            status_t res = STATUS_OK;

            if (flags & FLAG_STRING)
                res = parse_string(seq, flags & (~FLAG_STRING));
            else
                res = parse_regular(seq, flags);

            if (res != STATUS_OK)
                destroy_all_data();

            return res;
        }

    } /* namespace calc */
} /* namespace lsp */
