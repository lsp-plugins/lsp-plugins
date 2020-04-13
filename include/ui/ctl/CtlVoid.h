/*
 * CtlVoid.h
 *
 *  Created on: 11 апр. 2020 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLVOID_H_
#define UI_CTL_CTLVOID_H_

namespace lsp
{
    namespace ctl
    {
        class CtlVoid: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            public:
                explicit CtlVoid(CtlRegistry *src, LSPVoid *widget);
                virtual ~CtlVoid();

            public:
                virtual void set(widget_attribute_t att, const char *value);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLVOID_H_ */
