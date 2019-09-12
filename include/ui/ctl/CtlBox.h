/*
 * CtlBox.h
 *
 *  Created on: 28 июн. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLBOX_H_
#define UI_CTL_CTLBOX_H_

namespace lsp
{
    namespace ctl
    {
        class CtlBox: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                ssize_t     nOrientation;

            public:
                explicit CtlBox(CtlRegistry *src, LSPBox *widget, ssize_t orientation = -1);
                virtual ~CtlBox();

            public:
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

#endif /* UI_CTL_CTLBOX_H_ */
