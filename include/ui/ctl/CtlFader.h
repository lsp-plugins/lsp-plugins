/*
 * CtlFader.h
 *
 *  Created on: 19 нояб. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLFADER_H_
#define UI_CTL_CTLFADER_H_

namespace lsp
{
    namespace ctl
    {
        
        class CtlFader: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                bool                bLog;
                CtlPort            *pPort;

            protected:
                static status_t    slot_change(LSPWidget *sender, void *ptr, void *data);
                void        submit_value();
                void        commit_value(float value);

            public:
                explicit CtlFader(CtlRegistry *src, LSPFader *widget);
                virtual ~CtlFader();

            public:
                virtual void init();

                virtual void set(widget_attribute_t att, const char *value);

                virtual void notify(CtlPort *port);

                virtual void end();
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLFADER_H_ */
