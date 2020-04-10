/*
 * CtlMarker.h
 *
 *  Created on: 27 июл. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLMARKER_H_
#define UI_CTL_CTLMARKER_H_

namespace lsp
{
    namespace ctl
    {
        class CtlMarker: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                CtlPort        *pPort;
                CtlColor        sColor;
                CtlExpression   sAngle;
                CtlExpression   sDX;
                CtlExpression   sDY;
                float           fTransparency;

            protected:
                static status_t     slot_change(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_graph_resize(LSPWidget *sender, void *ptr, void *data);

                void                submit_values();
                void                trigger_expr();
                float               eval_expr(CtlExpression *expr);

            public:
                explicit CtlMarker(CtlRegistry *src, LSPMarker *mark);
                virtual ~CtlMarker();

            public:
                /** Begin initialization of controller
                 *
                 */
                virtual void init();

                /** Set attribute
                 *
                 * @param att widget attribute
                 * @param value widget value
                 */
                virtual void set(widget_attribute_t att, const char *value);

                virtual void end();

                virtual void notify(CtlPort *port);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLMARKER_H_ */
