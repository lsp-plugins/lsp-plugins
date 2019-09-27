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
        
        CtlExpression::CtlExpression(): CtlPortListener()
        {
            pCtl        = NULL;
            pListener   = NULL;
            sExpr.set_resolver(&sVars);
        }

        CtlExpression::~CtlExpression()
        {
            do_destroy();
        }

        void CtlExpression::init(CtlRegistry *ctl, CtlPortListener *listener)
        {
            pCtl        = ctl;
            pListener   = listener;
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
            drop_dependencies();
            LSPString tmp;
            if (!tmp.set_utf8(expr))
                return false;
            if (sExpr.parse(&tmp, flags) != STATUS_OK)
                return false;
            return build_dependencies();
        }

        bool CtlExpression::parse(const LSPString *expr, size_t flags)
        {
            drop_dependencies();
            if (sExpr.parse(expr, flags) != STATUS_OK)
                return false;
            return build_dependencies();
        }

        bool CtlExpression::parse(io::IInSequence *expr, size_t flags)
        {
            drop_dependencies();
            if (sExpr.parse(expr, flags) != STATUS_OK)
                return false;
            return build_dependencies();
        }

        bool CtlExpression::build_dependencies()
        {
            // TODO
            return true;
        }
    
    } /* namespace tk */
} /* namespace lsp */
