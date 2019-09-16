/*
 * Expression.h
 *
 *  Created on: 16 сент. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_CALC_EXPRESSION_H_
#define CORE_CALC_EXPRESSION_H_

#include <core/status.h>
#include <core/LSPString.h>
#include <core/calc/types.h>
#include <core/calc/Resolver.h>
#include <core/io/IInSequence.h>

namespace lsp
{
    namespace calc
    {
        
        class Expression
        {
            private:
                Expression & operator = (const Expression &);

            protected:
                Resolver       *pResolver;

            public:
                explicit Expression(Resolver *res);
                virtual ~Expression();

                void destroy();

            public:
                /**
                 * Parse the expression
                 * @param expr UTF-8 string containing expression
                 * @param flags additional flags
                 * @return status of operation
                 */
                status_t    parse(const char *expr, size_t flags = 0);

                /**
                 * Parse the expression
                 * @param expr string containing expression
                 * @param flags additional flags
                 * @return status of operation
                 */
                status_t    parse(const LSPString *expr, size_t flags = 0);

                /**
                 * Parse the expression
                 * @param seq character input sequence
                 * @param flags additional flags
                 * @return status of operation
                 */
                status_t    parse(const io::IInSequence *seq, size_t flags = 0);

                /**
                 * Check that expression is valid
                 * @return true if the expression is valid
                 */
                bool        valid();

                /**
                 * Evaluate all the expressions
                 * @return status of operation
                 */
                status_t    evaluate();

                /**
                 * Evaluate the specific expression
                 * @return status of operation
                 */
                status_t    evaluate(size_t idx);

                /**
                 * Get number of results
                 * @return number of results
                 */
                size_t      results() const;

                /**
                 * Get result of the specific expression
                 * @param result the pointer to store the result
                 * @param idx the result indes
                 * @return status of operation
                 */
                status_t    result(value_t *result, size_t idx);
        };
    
    } /* namespace calc */
} /* namespace lsp */

#endif /* CORE_CALC_EXPRESSION_H_ */
