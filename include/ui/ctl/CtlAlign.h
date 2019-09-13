/*
 * CtlAlign.h
 *
 *  Created on: 17 июл. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLALIGN_H_
#define UI_CTL_CTLALIGN_H_

namespace lsp
{
    namespace ctl
    {
        class CtlAlign: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            public:
                explicit CtlAlign(CtlRegistry *src, LSPAlign *widget);
                virtual ~CtlAlign();

            public:
                virtual void init();

                virtual void set(widget_attribute_t att, const char *value);

                virtual status_t add(CtlWidget *child);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLALIGN_H_ */
