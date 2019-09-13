/*
 * CtlSwitch.h
 *
 *  Created on: 1 июл. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLSWITCH_H_
#define UI_CTL_CTLSWITCH_H_

namespace lsp
{
    namespace ctl
    {
        
        class CtlSwitch: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                float           fValue;
                bool            bInvert;
                CtlPort        *pPort;

                CtlColor        sColor;
                CtlColor        sBorderColor;
                CtlColor        sTextColor;

            protected:
                static status_t    slot_change(LSPWidget *sender, void *ptr, void *data);

                void        commit_value(float value);
                void        submit_value();

            public:
                explicit CtlSwitch(CtlRegistry *src, LSPSwitch *widget);
                virtual ~CtlSwitch();

            public:
                /** Initialize the controller
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

                /** Complete initialization
                 *
                 */
                virtual void end();
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLSWITCH_H_ */
