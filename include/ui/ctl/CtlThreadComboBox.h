/*
 * CtlThreadBox.h
 *
 *  Created on: 2 июл. 2019 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLTHREADBOX_H_
#define UI_CTL_CTLTHREADBOX_H_

namespace lsp
{
    namespace ctl
    {
        class CtlThreadComboBox: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                CtlPort        *pPort;
                CtlColor        sColor;
                ui_handler_id_t idChange;

            protected:
                static status_t    slot_change(LSPWidget *sender, void *ptr, void *data);

                void submit_value();
                void do_destroy();

            public:
                explicit CtlThreadComboBox(CtlRegistry *src, LSPComboBox *widget);
                virtual ~CtlThreadComboBox();

                virtual void init();

                virtual void destroy();

            public:
                virtual void set(widget_attribute_t att, const char *value);

                virtual void notify(CtlPort *port);

                virtual void end();

        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLTHREADBOX_H_ */
