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
            protected:
                float           fValue;
                CtlPort        *pPort;
                CtlColor        sColor;
                CtlColor        sBgColor;
                CtlColor        sTextColor;

            public:
                CtlIndicator(CtlRegistry *src, LSPIndicator *widget);
                virtual ~CtlIndicator();

            protected:
                void        commit_value(float value);

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

                /** Notify controller about one of port bindings has changed
                 *
                 * @param port port triggered change
                 */
                virtual void notify(CtlPort *port);

        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLINDICATOR_H_ */
