/*
 * CtlIndicator.h
 *
 *  Created on: 7 июл. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLINDICATOR_H_
#define UI_CTL_CTLINDICATOR_H_

namespace lsp
{
    namespace ctl
    {
        
        class CtlIndicator: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                float           fValue;
                CtlPort        *pPort;
                CtlColor        sColor;
                CtlColor        sTextColor;

            public:
                explicit CtlIndicator(CtlRegistry *src, LSPIndicator *widget);
                virtual ~CtlIndicator();

            protected:
                void        commit_value(float value);

            public:
                virtual void init();

                virtual void end();

                virtual void set(widget_attribute_t att, const char *value);

                virtual void notify(CtlPort *port);

        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLINDICATOR_H_ */
