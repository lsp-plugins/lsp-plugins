/*
 * CtlComboBox.h
 *
 *  Created on: 21 авг. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLCOMBOBOX_H_
#define UI_CTL_CTLCOMBOBOX_H_

namespace lsp
{
    namespace ctl
    {
        class CtlComboBox: public CtlWidget
        {
            protected:
                CtlPort        *pPort;
                CtlColor        sBgColor;
                CtlColor        sColor;
                float           fMin;
                float           fMax;
                float           fStep;

                ui_handler_id_t idChange;

            protected:
                static status_t    slot_change(void *ptr, void *data);

                void submit_value();
                void do_destroy();

            public:
                CtlComboBox(CtlRegistry *src, LSPComboBox *widget);
                virtual ~CtlComboBox();

                virtual void init();

                virtual void destroy();

            public:
                virtual void set(widget_attribute_t att, const char *value);

                virtual void notify(CtlPort *port);

                virtual void end();
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLCOMBOBOX_H_ */
