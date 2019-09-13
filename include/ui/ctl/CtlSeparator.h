/*
 * CtlSeparator.h
 *
 *  Created on: 9 июл. 2017 г.
 *      Author: sadko
 */

#ifndef INCLUDE_UI_CTL_CTLSEPARATOR_H_
#define INCLUDE_UI_CTL_CTLSEPARATOR_H_

namespace lsp
{
    namespace ctl
    {
        class CtlSeparator: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                CtlColor        sColor;
                ssize_t         nOrientation;

            public:
                explicit CtlSeparator(CtlRegistry *src, LSPSeparator *widget, ssize_t orientation = -1);
                virtual ~CtlSeparator();

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

#endif /* INCLUDE_UI_CTL_CTLSEPARATOR_H_ */
