/*
 * CtlGraph.h
 *
 *  Created on: 26 июл. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLGRAPH_H_
#define UI_CTL_CTLGRAPH_H_

namespace lsp
{
    namespace ctl
    {
        class CtlGraph: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                CtlColor        sColor;
                CtlPadding      sPadding;

            public:
                explicit CtlGraph(CtlRegistry *src, LSPGraph *graph);
                virtual ~CtlGraph();

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

                /** Add child widget
                 *
                 * @param child child widget to add
                 */
                virtual status_t add(CtlWidget *child);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLGRAPH_H_ */
