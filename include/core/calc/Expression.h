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
#include <core/io/IInSequence.h>
#include <core/calc/types.h>
#include <core/calc/Resolver.h>

#include <data/cstorage.h>
#include <data/cvector.h>

namespace lsp
{
    namespace calc
    {
        struct expr_t;
        class Tokenizer;
        
        class Expression
        {
            private:
                Expression & operator = (const Expression &);

            public:
                enum expr_flags
                {
                    FLAG_NONE           = 0,
                    FLAG_MULTIPLE       = 1 << 0,
                    FLAG_STRING         = 1 << 1
                };

            protected:
                typedef struct root_t
                {
                    expr_t         *expr;
                    value_t         result;
                } root_t;

            protected:
                Resolver           *pResolver;
                cstorage<root_t>    vRoots;
                cvector<LSPString>  vDependencies;

            protected:
                void                destroy_all_data();
                status_t            prepend_string(expr_t **expr, const LSPString *str, bool force);
                status_t            parse_substitution(expr_t **expr, Tokenizer *t);
                status_t            parse_regular(io::IInSequence *seq, size_t flags);
                status_t            parse_string(io::IInSequence *seq, size_t flags);
                status_t            post_process();
                status_t            scan_dependencies(expr_t *expr);
                status_t            add_dependency(const LSPString *str);

            public:
                explicit Expression();
                explicit Expression(Resolver *res);
                virtual ~Expression();

                void destroy();

            public:
                /**
                 * Parse the expression
                 * @param expr string containing expression
                 * @param charset character set, may be NULL for default character set
                 * @param flags additional flags
                 * @return status of operation
                 */
                status_t    parse(const char *expr, const char *charset = NULL, size_t flags = FLAG_NONE);

                /**
                 * Parse the expression
                 * @param expr string containing expression
                 * @param flags additional flags
                 * @return status of operation
                 */
                status_t    parse(const LSPString *expr, size_t flags = FLAG_NONE);

                /**
                 * Parse the expression
                 * @param seq character input sequence
                 * @param flags additional flags
                 * @return status of operation
                 */
                status_t    parse(io::IInSequence *seq, size_t flags = FLAG_NONE);

                /**
                 * Check that expression is valid
                 * @return true if the expression is valid
                 */
                inline bool     valid() const { return vRoots.size() > 0; };

                /**
                 * Evaluate all the expressions
                 * @param result pointer to return value of the zero-indexed expression
                 * @return status of operation
                 */
                status_t        evaluate(value_t *result = NULL);

                /**
                 * Evaluate the specific expression
                 * @param index expression index
                 * @param result pointer to return value of the specified expression
                 * @return status of operation
                 */
                status_t        evaluate(size_t idx, value_t *result = NULL);

                /**
                 * Get number of results
                 * @return number of results
                 */
                inline size_t   results() const { return vRoots.size(); };

                /**
                 * Get result of the specific expression
                 * @param result the pointer to store the result
                 * @param idx the result indes
                 * @return status of operation
                 */
                status_t        result(value_t *result, size_t idx);

                /**
                 * Get variable resolver
                 * @return variable resolver
                 */
                inline Resolver *resolver() { return pResolver; }

                /**
                 * Sett variable resolver
                 * @param resolver variable resolver
                 */
                inline void     set_resolver(Resolver *resolver) { pResolver = resolver; }

                /**
                 * Get number of dependencies
                 * @return number of dependencies
                 */
                inline size_t   dependencies() const { return vDependencies.size(); }

                /**
                 * Get dependency
                 * @param idx the index of dependency
                 * @return dependency name or NULL
                 */
                inline const LSPString *dependency(size_t idx) const { return vDependencies.get(idx); }

                /**
                 * Check that expression has dependency
                 * @param str dependency name
                 * @return true if expression has dependency
                 */
                bool            has_dependency(const LSPString *str) const;

                /**
                 * Check that expression has dependency
                 * @param str dependency name
                 * @return true if expression has dependency
                 */
                bool            has_dependency(const char *str) const;

        };
    
    } /* namespace calc */
} /* namespace lsp */

#endif /* CORE_CALC_EXPRESSION_H_ */
