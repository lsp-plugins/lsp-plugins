/*
 * Expression.cpp
 *
 *  Created on: 16 сент. 2019 г.
 *      Author: sadko
 */

#include <core/io/InStringSequence.h>
#include <core/calc/parser.h>
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
                    res     = evaluate(&r->result, r->expr);
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
                res     = evaluate(&r->result, r->expr);
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

        status_t Expression::parse(io::IInSequence *seq, size_t flags)
        {
            Tokenizer t(seq);
            status_t res = STATUS_OK;

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

            if (res != STATUS_OK)
                destroy_all_data();

            return res;
        }

        status_t Expression::evaluate(value_t *result, expr_t *expr)
        {
            // TODO
            return STATUS_OK;
        }

    } /* namespace calc */
} /* namespace lsp */
