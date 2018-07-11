/*
 * CtlButton.h
 *
 *  Created on: 28 июн. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLBUTTON_H_
#define UI_CTL_CTLBUTTON_H_

namespace lsp
{
    namespace ctl
    {
        class CtlButton: public CtlWidget
        {
            protected:
                float           fValue;
                CtlPort        *pPort;
                CtlColor        sBgColor;
                CtlColor        sColor;

            protected:
                static status_t    slot_change(LSPWidget *sender, void *ptr, void *data);

                void        commit_value(float value);
                void        submit_value();
                float       next_value(bool down);

            public:
                CtlButton(CtlRegistry *src, LSPButton *widget);
                virtual ~CtlButton();

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

                /** Complete initialization
                 *
                 */
                virtual void end();
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLBUTTON_H_ */
