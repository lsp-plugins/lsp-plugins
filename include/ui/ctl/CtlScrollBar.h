/*
 * CtlScrollBar.h
 *
 *  Created on: 4 авг. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLSCROLLBAR_H_
#define UI_CTL_CTLSCROLLBAR_H_

namespace lsp
{
    namespace ctl
    {
        
        class CtlScrollBar: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            public:
                explicit CtlScrollBar(CtlRegistry *src, LSPScrollBar *widget);
                virtual ~CtlScrollBar();
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLSCROLLBAR_H_ */
