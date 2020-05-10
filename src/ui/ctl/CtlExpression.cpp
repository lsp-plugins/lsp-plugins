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

        status_t CtlExpression::CtlResolver::resolve(calc::value_t *value, const char *name, size_t num_indexes, const ssize_t *indexes)
        {
            status_t res = pExpr->sParams.resolve(value, name, num_indexes, indexes);
            if (res != STATUS_OK)
                res     = CtlPortResolver::resolve(value, name, num_indexes, indexes);
            return res;
        }

        status_t CtlExpression::CtlResolver::resolve(calc::value_t *value, const LSPString *name, size_t num_indexes, const ssize_t *indexes)
        {
            status_t res = pExpr->sParams.resolve(value, name, num_indexes, indexes);
            if (res != STATUS_OK)
                res     = CtlPortResolver::resolve(value, name, num_indexes, indexes);
            return res;
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
            vDependencies.clear();
        }

        void CtlExpression::notify(CtlPort *port)
        {
            if (!depends(port))
                return;
            if (pListener == NULL)
                return;
            pListener->notify(port);
        }

        float CtlExpression::evaluate()
        {
            calc::value_t value;
            calc::init_value(&value);

            sVars.clear();
            drop_dependencies();
            status_t res = sExpr.evaluate(&value);
            if (res != STATUS_OK)
            {
                calc::destroy_value(&value);
                return 0.0f;
            }

            float fval;
            calc::cast_float(&value);
            fval = (value.type == calc::VT_FLOAT) ? value.v_float : 0.0f;
            calc::destroy_value(&value);
            return fval;
        }

        float CtlExpression::evaluate(size_t idx)
        {
            calc::value_t value;
            calc::init_value(&value);

            sVars.clear();
            drop_dependencies();
            status_t res = sExpr.evaluate(idx, &value);
            if (res != STATUS_OK)
            {
                calc::destroy_value(&value);
                return 0.0f;
            }

            float fval;
            calc::cast_float(&value);
            fval = (value.type == calc::VT_FLOAT) ? value.v_float : 0.0f;
            calc::destroy_value(&value);
            return fval;
        }

        float CtlExpression::result(size_t idx)
        {
            calc::value_t value;
            calc::init_value(&value);

            status_t res = sExpr.result(&value, idx);
            if (res != STATUS_OK)
            {
                calc::destroy_value(&value);
                return 0.0f;
            }

            float fval;
            calc::cast_float(&value);
            fval = (value.type == calc::VT_FLOAT) ? value.v_float : 0.0f;
            calc::destroy_value(&value);
            return fval;
        }

        bool CtlExpression::parse(const char *expr, size_t flags)
        {
            sVars.clear();
            drop_dependencies();

            LSPString tmp;
            if (!tmp.set_utf8(expr))
                return false;
            if (sExpr.parse(&tmp, flags) != STATUS_OK)
                return false;

            status_t res = sExpr.evaluate();
            #ifdef LSP_TRACE
                if (res == STATUS_OK)
                    sText.swap(&tmp);
            #endif
            return res;
        }

        bool CtlExpression::parse(const LSPString *expr, size_t flags)
        {
            sVars.clear();
            drop_dependencies();

            if (sExpr.parse(expr, flags) != STATUS_OK)
                return false;

            status_t res = sExpr.evaluate();
            #ifdef LSP_TRACE
                if (res == STATUS_OK)
                    res = (sText.set(expr) ? STATUS_OK : STATUS_NO_MEM);
            #endif
            return res;
        }

        bool CtlExpression::parse(io::IInSequence *expr, size_t flags)
        {
            if (sExpr.parse(expr, flags) != STATUS_OK)
                return false;
            return evaluate() == STATUS_OK;
        }

        status_t CtlExpression::on_resolved(const LSPString *name, CtlPort *p)
        {
            // lsp_trace("[%s] resolved %s -> %s = %f", sText.get_utf8(), name->get_utf8(), p->id(), p->get_value());
            // Already subscribed?
            if (vDependencies.index_of(p) >= 0)
                return STATUS_OK;

            if (!vDependencies.add(p))
                return STATUS_NO_MEM;
            // lsp_trace("bind to %s", p->id());
            p->bind(this);
            return STATUS_OK;
        }

    } /* namespace tk */
} /* namespace lsp */
