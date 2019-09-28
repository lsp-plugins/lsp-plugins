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
        #define CTL_EXPR_TOKEN_LENGTH   128
        
        #define EXPR_FLAGS_MULTIPLE             (1 << 0)

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
                };

            protected:
                calc::Expression    sExpr;
                calc::Variables     sVars;
                CtlResolver         sResolver;
                CtlRegistry        *pCtl;
                CtlPortListener    *pListener;
                cvector<CtlPort>    vDependencies;

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

                float           evaluate();
                float           evaluate(size_t idx);
                inline size_t   results()               { return sExpr.results(); }
                float           result(size_t idx);
                bool            parse(const char *expr, size_t flags = calc::Expression::FLAG_NONE);
                bool            parse(const LSPString *expr, size_t flags = calc::Expression::FLAG_NONE);
                bool            parse(io::IInSequence *expr, size_t flags = calc::Expression::FLAG_NONE);
                inline bool     valid() const           { return sExpr.valid(); };
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_CTL_CTLEXPRESSION_H_ */
