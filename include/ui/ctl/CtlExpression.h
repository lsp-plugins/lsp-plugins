/*
 * CtlExpression.h
 *
 *  Created on: 23 июн. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLEXPRESSION_H_
#define UI_CTL_CTLEXPRESSION_H_

#include <core/calc/Expression.h>
#include <core/calc/Variables.h>

namespace lsp
{
    namespace ctl
    {
        enum
        {
            EXPR_FLAGS_MULTIPLE = calc::Expression::FLAG_MULTIPLE,
            EXPR_FLAGS_STRING   = calc::Expression::FLAG_STRING
        };

        class CtlExpression: public CtlPortListener
        {
            private:
                CtlExpression & operator = (const CtlExpression &);

            protected:
                class CtlResolver: public CtlPortResolver
                {
                    protected:
                        CtlExpression *pExpr;

                    public:
                        explicit inline CtlResolver(CtlExpression *expr) { pExpr = expr; }

                    public:
                        virtual status_t on_resolved(const LSPString *name, CtlPort *p);

                        virtual status_t resolve(calc::value_t *value, const char *name, size_t num_indexes, const ssize_t *indexes);

                        virtual status_t resolve(calc::value_t *value, const LSPString *name, size_t num_indexes, const ssize_t *indexes);
                };

            protected:
                calc::Expression    sExpr;
                calc::Variables     sVars;
                calc::Parameters    sParams;
                CtlResolver         sResolver;
                CtlRegistry        *pCtl;
                CtlPortListener    *pListener;
                cvector<CtlPort>    vDependencies;
                #ifdef LSP_TRACE
                LSPString           sText;
                #endif /* LSP_TRACE */

            protected:
                void            do_destroy();
                void            drop_dependencies();
                status_t        on_resolved(const LSPString *name, CtlPort *p);

            public:
                explicit CtlExpression();
                virtual ~CtlExpression();

            public:
                virtual void    notify(CtlPort *port);

            public:
                void            init(CtlRegistry *ctl, CtlPortListener *listener);
                void            destroy();

                inline calc::Parameters *params()   { return &sParams; };

                float           evaluate();
                float           evaluate(size_t idx);
                inline size_t   results()               { return sExpr.results(); }
                float           result(size_t idx);
                bool            parse(const char *expr, size_t flags = calc::Expression::FLAG_NONE);
                bool            parse(const LSPString *expr, size_t flags = calc::Expression::FLAG_NONE);
                bool            parse(io::IInSequence *expr, size_t flags = calc::Expression::FLAG_NONE);
                inline bool     valid() const               { return sExpr.valid(); };
                inline bool     depends(CtlPort *port) const { return vDependencies.index_of(port) >= 0; }

                #ifdef LSP_TRACE
                inline const char *text() const         { return sText.get_utf8(); }
                #endif /* LSP_TRACE */
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_CTL_CTLEXPRESSION_H_ */
