/*
 * CtlCenter.h
 *
 *  Created on: 27 июл. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLCENTER_H_
#define UI_CTL_CTLCENTER_H_

namespace lsp
{
    namespace ctl
    {
        
        class CtlCenter: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                CtlColor        sColor;

            public:
                explicit CtlCenter(CtlRegistry *src, LSPCenter *cnt);
                virtual ~CtlCenter();

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
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLCENTER_H_ */
