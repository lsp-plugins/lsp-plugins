/*
 * CtlHyperlink.h
 *
 *  Created on: 23 окт. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLHYPERLINK_H_
#define UI_CTL_CTLHYPERLINK_H_

namespace lsp
{
    namespace ctl
    {
        
        class CtlHyperlink: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                CtlColor            sColor;
                CtlColor            sHoverColor;

            protected:
                void commit_value();

            public:
                explicit CtlHyperlink(CtlRegistry *src, LSPHyperlink *widget, ctl_label_type_t type);
                virtual ~CtlHyperlink();

            public:
                /** Begin initialization of controller
                 *
                 */
                virtual void init();

                /** Set attribute to widget
                 *
                 * @param att attribute identifier
                 * @param value attribute value
                 */
                virtual void set(widget_attribute_t att, const char *value);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLHYPERLINK_H_ */
