/*
 * CtlScrollBox.h
 *
 *  Created on: 13 окт. 2019 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLSCROLLBOX_H_
#define UI_CTL_CTLSCROLLBOX_H_

namespace lsp
{
    namespace ctl
    {
        class CtlScrollBox: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                ssize_t     nOrientation;

            public:
                explicit CtlScrollBox(CtlRegistry *src, LSPScrollBox *widget, ssize_t orientation = -1);
                virtual ~CtlScrollBox();

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

#endif /* UI_CTL_CTLSCROLLBOX_H_ */
