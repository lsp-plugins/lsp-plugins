/*
 * CtlCtlExpression.cpp
 *
 *  Created on: 23 июн. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        
        status_t CtlExpression::CtlResolver::on_resolved(const LSPString *name, CtlPort *p)
        {
            return pExpr->on_resolved(name, p);
        }

        CtlExpression::CtlExpression(): CtlPortListener(),
            sResolver(this)
        {
            pCtl        = NULL;
            pListener   = NULL;
        }

        CtlExpression::~CtlExpression()
        {
            do_destroy();
        }

        void CtlExpression::init(CtlRegistry *ctl, CtlPortListener *listener)
        {
            pCtl        = ctl;
            pListener   = listener;

            // Bind expression stuff
            sResolver.init(ctl);
            sVars.set_resolver(&sResolver);
            sExpr.set_resolver(&sVars);
        }

        void CtlExpression::destroy()
        {
            do_destroy();
        }

        void CtlExpression::do_destroy()
        {
            sExpr.destroy();
            sVars.clear();
            drop_dependencies();
        }

        void CtlExpression::drop_dependencies()
        {
            for (size_t i=0, n=vDependencies.size(); i<n; ++i)
            {
                CtlPort *p = vDependencies.at(i);
                if (p != NULL)
                    p->unbind(this);
            }
            vDependencies.flush();
        }

        void CtlExpression::notify(CtlPort *port)
        {
            if (pListener != NULL)
                pListener->notify(port);
        }

        float CtlExpression::evaluate()
        {
            calc::value_t value;

            sVars.clear();
            drop_dependencies();
            status_t res = sExpr.evaluate(&value);
            if (res != STATUS_OK)
                return 0.0f;

            float fval;
            calc::cast_float(&value);
            fval = (value.type == calc::VT_FLOAT) ? value.v_float : 0.0f;
            calc::destroy_value(&value);
            return fval;
        }

        float CtlExpression::evaluate(size_t idx)
        {
            calc::value_t value;

            sVars.clear();
            drop_dependencies();
            status_t res = sExpr.evaluate(idx, &value);
            if (res != STATUS_OK)
                return 0.0f;

            float fval;
            calc::cast_float(&value);
            fval = (value.type == calc::VT_FLOAT) ? value.v_float : 0.0f;
            calc::destroy_value(&value);
            return fval;
        }

        float CtlExpression::result(size_t idx)
        {
            calc::value_t value;
            status_t res = sExpr.result(&value, idx);
            if (res != STATUS_OK)
                return 0.0f;

            float fval;
            calc::cast_float(&value);
            fval = (value.type == calc::VT_FLOAT) ? value.v_float : 0.0f;
            calc::destroy_value(&value);
            return fval;
        }

        bool CtlExpression::parse(const char *expr, size_t flags)
        {
            LSPString tmp;
            if (!tmp.set_utf8(expr))
                return false;
            if (sExpr.parse(&tmp, flags) != STATUS_OK)
                return false;
            return evaluate() == STATUS_OK;
        }

        bool CtlExpression::parse(const LSPString *expr, size_t flags)
        {
            if (sExpr.parse(expr, flags) != STATUS_OK)
                return false;
            return evaluate() == STATUS_OK;
        }

        bool CtlExpression::parse(io::IInSequence *expr, size_t flags)
        {
            if (sExpr.parse(expr, flags) != STATUS_OK)
                return false;
            return evaluate() == STATUS_OK;
        }

        status_t CtlExpression::on_resolved(const LSPString *name, CtlPort *p)
        {
            // Already subscribed?
            if (vDependencies.index_of(p) >= 0)
                return STATUS_OK;

            if (!vDependencies.add(p))
                return STATUS_NO_MEM;
            p->bind(this);
            return STATUS_OK;
        }

    } /* namespace tk */
} /* namespace lsp */
