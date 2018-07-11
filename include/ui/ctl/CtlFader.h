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
            protected:
                bool                bLog;
                CtlPort            *pPort;

            protected:
                static status_t    slot_change(void *ptr, void *data);
                void        submit_value();
                void        commit_value(float value);

            public:
                CtlFader(CtlRegistry *src, LSPFader *widget);
                virtual ~CtlFader();

            public:
                /** Set attribute
                 *
                 * @param att widget attribute
                 * @param value widget value
                 */
                virtual void set(widget_attribute_t att, const char *value);

                virtual void notify(CtlPort *port);

                virtual void end();
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLFADER_H_ */
