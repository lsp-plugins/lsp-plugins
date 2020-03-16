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
            public:
                static const ctl_class_t metadata;

            protected:
                float           fValue;
                CtlPort        *pPort;
                CtlColor        sColor;
                CtlColor        sTextColor;

            protected:
                static status_t    slot_change(LSPWidget *sender, void *ptr, void *data);

                void        commit_value(float value);
                void        submit_value();
                float       next_value(bool down);

            public:
                explicit CtlButton(CtlRegistry *src, LSPButton *widget);
                virtual ~CtlButton();

            public:
                virtual void init();

                virtual void set(const char *name, const char *value);

                virtual void set(widget_attribute_t att, const char *value);

                virtual void notify(CtlPort *port);

                virtual void end();
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLBUTTON_H_ */
