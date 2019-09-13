/*
 * CtlLed.h
 *
 *  Created on: 10 июл. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLLED_H_
#define UI_CTL_CTLLED_H_

namespace lsp
{
    namespace ctl
    {
        
        class CtlLed: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                CtlColor            sColor;
                CtlPort            *pPort;
                float               fValue;
                float               fKey;
                CtlExpression       sActivity;
                bool                bActivitySet;
                bool                bInvert;

            protected:
                void        update_value();

            public:
                explicit CtlLed(CtlRegistry *src, LSPLed *widget);
                virtual ~CtlLed();

            public:
                /** Begin initialization of controller
                 *
                 */
                virtual void init();

                virtual void destroy();

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

                /** Complete initialization
                 *
                 */
                virtual void end();
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLLED_H_ */
