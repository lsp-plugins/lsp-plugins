/*
 * CtlBasis.h
 *
 *  Created on: 27 июл. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLBASIS_H_
#define UI_CTL_CTLBASIS_H_

namespace lsp
{
    namespace ctl
    {
        class CtlBasis: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            public:
                explicit CtlBasis(CtlRegistry *src, LSPBasis *base);
                virtual ~CtlBasis();

            public:
                virtual void set(widget_attribute_t att, const char *value);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLBASIS_H_ */
