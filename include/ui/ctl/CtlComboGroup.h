/*
 * ComboGroup.h
 *
 *  Created on: 29 апр. 2018 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLCOMBOGROUP_H_
#define UI_CTL_CTLCOMBOGROUP_H_

namespace lsp
{
    namespace ctl
    {
        
        class CtlComboGroup: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                CtlPort        *pPort;
                float           fMin;
                float           fMax;
                float           fStep;
                char           *pText;

                ui_handler_id_t idChange;

                CtlColor        sColor;
                CtlColor        sTextColor;
                CtlExpression   sEmbed;

            protected:
                static status_t slot_change(LSPWidget *sender, void *ptr, void *data);

                void submit_value();
                void do_destroy();

            public:
                explicit CtlComboGroup(CtlRegistry *src, LSPComboGroup *widget);
                virtual ~CtlComboGroup();

            public:
                virtual void init();

                virtual void destroy();

                virtual void set(widget_attribute_t att, const char *value);

                virtual status_t add(CtlWidget *child);

                virtual void notify(CtlPort *port);

                virtual void end();
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_COMBOGROUP_H_ */
